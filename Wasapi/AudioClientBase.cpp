#include "pch.h"
#include "AudioClientBase.h"
#include <windows.media.mediaproperties.h>
#include <winrt/Windows.Media.MediaProperties.h>
#include <mfidl.h>
#include <mfapi.h>
#include <trace.h>

namespace winrt::Wasapi::implementation
{
	AudioClientBase::AudioClientBase(winrt::com_ptr<::IAudioClient3> const& client)
	{
		_state = Wasapi::AudioSessionClientState::Uninitialized;
		_audioClient = client;
		_clientProperties.cbSize = sizeof(_clientProperties);
		_clientProperties.bIsOffload = false;
		_clientProperties.eCategory = AudioCategory_Other;
		_clientProperties.Options = AUDCLNT_STREAMOPTIONS_NONE;
		_audioClient->SetClientProperties(&_clientProperties);
	}

	void AudioClientBase::Close()
	{
		_audioClient = nullptr;
	}

	void AudioClientBase::Start()
	{
		auto a = trace::start(this);
		if (_callback) {
			check_hresult(_callback->ScheduleWorkItemWait());
		}
		check_hresult(_audioClient->Start());
		_state = AudioSessionClientState::Running;
	}
	void AudioClientBase::Stop()
	{
		auto a = trace::stop(this);
		_state = AudioSessionClientState::Stopped;
		if (_callback) {
			_callback->CancelWorkItemWait();
		}
		check_hresult(_audioClient->Stop());
	}

	void AudioClientBase::Reset() {
		auto a = trace::reset(this);
		if (_callback) {
			_callback->CancelWorkItemWait();
		}
		check_hresult(_audioClient->Reset());
	}
	Wasapi::AudioSessionClientState AudioClientBase::State()
	{
		return _state;
	}
	Windows::Media::MediaProperties::AudioEncodingProperties AudioClientBase::Format()
	{
		return _format;
	}
	uint32_t AudioClientBase::Period()
	{
		return _periodInFrames;
	}
	uint32_t AudioClientBase::BufferSize()
	{
		UINT32 bufferSize = 0;
		check_hresult(_audioClient->GetBufferSize(&bufferSize));
		return bufferSize;
	}
	uint32_t AudioClientBase::Padding()
	{
		UINT32 padding = 0;
		check_hresult(_audioClient->GetCurrentPadding(&padding));
		return padding;
	}
	Windows::Foundation::TimeSpan AudioClientBase::Latency()
	{
		REFERENCE_TIME latency = 0;
		check_hresult(_audioClient->GetStreamLatency(&latency));
		return Windows::Foundation::TimeSpan(latency);
	}
	winrt::Windows::Media::MediaProperties::AudioEncodingProperties AudioClientBase::GetDefaultFormat()
	{
		WAVEFORMATEX* pFormat = nullptr;
		check_hresult(_audioClient->GetMixFormat(&pFormat));

		auto encoding = WaveFormatExToMediaProperties(pFormat);
		CoTaskMemFree(pFormat);

		trace::get_format(encoding);
		return encoding;
	}

	bool AudioClientBase::IsFormatSupported(Windows::Media::MediaProperties::AudioEncodingProperties const& format)
	{
		WAVEFORMATEX* pFormat = nullptr, * pClosestMatch = nullptr;
		MediaPropertiesToWaveFormatEx(format, &pFormat);
		HRESULT hr = _audioClient->IsFormatSupported(AUDCLNT_SHAREMODE::AUDCLNT_SHAREMODE_SHARED, pFormat, &pClosestMatch);
		CoTaskMemFree(pClosestMatch);
		CoTaskMemFree(pFormat);
		return hr == S_OK;
	}

	Wasapi::AudioClientEnginePeriods AudioClientBase::GetPeriods(Windows::Media::MediaProperties::AudioEncodingProperties const& format)
	{
		WAVEFORMATEX* pFormat{ nullptr };
		check_hresult(MediaPropertiesToWaveFormatEx(format, &pFormat));
		Wasapi::AudioClientEnginePeriods result { 0,0,0,0 };
		check_hresult(_audioClient->GetSharedModeEnginePeriod(pFormat,&result.DefaultPeriodFrames,&result.FundamentalPeriodFrames,&result.MinimumPeriodFrames,&result.MaximumPeriodFrames));
		CoTaskMemFree(pFormat);
		return result;
	}

	bool AudioClientBase::IsRawStream()
	{
		return (_clientProperties.Options & AUDCLNT_STREAMOPTIONS_RAW) != 0;
	}

	void AudioClientBase::IsRawStream(bool bIsRaw)
	{
		if (bIsRaw != IsRawStream()) {
			if (bIsRaw) {
				_clientProperties.Options |= AUDCLNT_STREAMOPTIONS_RAW;
			}
			else {
				_clientProperties.Options &= ~AUDCLNT_STREAMOPTIONS_RAW;
			}
			check_hresult(_audioClient->SetClientProperties(&_clientProperties));
		}
	}

	HRESULT AudioClientBase::MediaPropertiesToWaveFormatEx(Windows::Media::MediaProperties::AudioEncodingProperties const& format, WAVEFORMATEX** ppFormat)
	{
		com_ptr<IMFMediaType> mediaType;
		HRESULT hr = MFCreateMediaTypeFromProperties(reinterpret_cast<IUnknown*>(winrt::get_abi(format)), mediaType.put());
		if (FAILED(hr))
			return hr;
		UINT32 size = 0;
		hr = MFCreateWaveFormatExFromMFMediaType(mediaType.get(), ppFormat, &size);
		return hr;
	}

	Windows::Media::MediaProperties::AudioEncodingProperties AudioClientBase::WaveFormatExToMediaProperties(const WAVEFORMATEX* pFormat)
	{
		com_ptr<IMFMediaType> mediaType;
		check_hresult(MFCreateMediaType(mediaType.put()));
		check_hresult(MFInitMediaTypeFromWaveFormatEx(mediaType.get(), pFormat, sizeof(WAVEFORMATEX) + pFormat->cbSize));

		com_ptr<ABI::Windows::Media::MediaProperties::IAudioEncodingProperties> abiMediaProps;
		check_hresult(MFCreatePropertiesFromMediaType(mediaType.get(), IID_PPV_ARGS(abiMediaProps.put())));

		return abiMediaProps.as<winrt::Windows::Media::MediaProperties::AudioEncodingProperties>();
	}


	HRESULT AudioClientBase::InitializeSharedClient(DWORD flags,uint32_t periodInFrames, const WAVEFORMATEX *pFormat)
	{

		WAVEFORMATEX* pInitializeFormat = (WAVEFORMATEX*)pFormat;
		if (!pInitializeFormat) {
			check_hresult(_audioClient->GetMixFormat(&pInitializeFormat));
		}

		uint32_t initEnginePeriod = periodInFrames;
		if (initEnginePeriod == 0) {
			uint32_t notused1, notused2, notused3;
			check_hresult(_audioClient->GetSharedModeEnginePeriod(pInitializeFormat, &initEnginePeriod, &notused1, &notused2, &notused3));
		}

		HRESULT hr = S_OK;
		// InitializeSharedAudioStream does not allow loopback flag. Maybe a bug - work around that by calling Initialize for loopback stream
		if (flags & AUDCLNT_STREAMFLAGS_LOOPBACK) {
			// (1+20000000L...) >> 1 construct for rounding
			REFERENCE_TIME streamPeriod = (1 + 20000000L * long(initEnginePeriod) / long(pInitializeFormat->nSamplesPerSec))>>1;
			hr = _audioClient->Initialize(AUDCLNT_SHAREMODE::AUDCLNT_SHAREMODE_SHARED, flags, 0, streamPeriod, pInitializeFormat, nullptr);
		}
		else {
			hr = _audioClient->InitializeSharedAudioStream(flags, initEnginePeriod, pInitializeFormat, nullptr);
		}
		if SUCCEEDED(hr) {
			_state = AudioSessionClientState::Stopped;
			audioFrameSize = pInitializeFormat->wBitsPerSample >> 3;
			audioSampleRate = pInitializeFormat->nSamplesPerSec;
			if (flags & AUDCLNT_STREAMFLAGS_EVENTCALLBACK) {
				_callback = make_self<AudioSessionClientCallback>(this);

			}
			WAVEFORMATEX* pCurrentFormat = nullptr;
			check_hresult(_audioClient->GetCurrentSharedModeEnginePeriod(&pCurrentFormat, &_periodInFrames));
			_format = WaveFormatExToMediaProperties(pCurrentFormat);
			CoTaskMemFree(pCurrentFormat);
			
		}
		if (!pFormat) {
			CoTaskMemFree(pInitializeFormat);
		}

		return hr;
	}
}