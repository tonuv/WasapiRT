#include "pch.h"
#include "AudioClientBase.h"
#include <windows.media.mediaproperties.h>
#include <winrt/Windows.Media.MediaProperties.h>
#include <mfidl.h>
#include <mfapi.h>
#include <trace.h>

namespace winrt::Wasapi::implementation
{
	AudioClientBase::AudioClientBase(winrt::com_ptr<::IAudioClient> const& client)
	{
		_state = Wasapi::AudioSessionClientState::Uninitialized;
		_audioClient = client;
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
	winrt::Windows::Foundation::TimeSpan AudioClientBase::DefaultPeriod()
	{
		REFERENCE_TIME period = 0;
		check_hresult(_audioClient->GetDevicePeriod(&period, nullptr));
		return Windows::Foundation::TimeSpan(period);
	}
	winrt::Windows::Foundation::TimeSpan AudioClientBase::MinimumPeriod()
	{
		REFERENCE_TIME period = 0;
		check_hresult(_audioClient->GetDevicePeriod(nullptr, &period));
		return Windows::Foundation::TimeSpan(period);
	}
	Windows::Foundation::TimeSpan AudioClientBase::Latency()
	{
		REFERENCE_TIME latency = 0;
		check_hresult(_audioClient->GetStreamLatency(&latency));
		return Windows::Foundation::TimeSpan(latency);
	}
	winrt::Windows::Media::MediaProperties::AudioEncodingProperties AudioClientBase::GetFormat()
	{
		WAVEFORMATEX* pFormat = nullptr;
		check_hresult(_audioClient->GetMixFormat(&pFormat));
		com_ptr<IMFMediaType> mediaType;
		check_hresult(MFCreateMediaType(mediaType.put()));
		check_hresult(MFInitMediaTypeFromWaveFormatEx(mediaType.get(), pFormat,sizeof(WAVEFORMATEX) + pFormat->cbSize));
		CoTaskMemFree(pFormat);

		com_ptr<ABI::Windows::Media::MediaProperties::IAudioEncodingProperties> abiMediaProps;
		check_hresult(MFCreatePropertiesFromMediaType(mediaType.get(), IID_PPV_ARGS(abiMediaProps.put())));

		auto encoding =  abiMediaProps.as<winrt::Windows::Media::MediaProperties::AudioEncodingProperties>();
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

	// Passing in 0 as bufferSize or nullptr for format will use default values
	HRESULT AudioClientBase::InitializeClient(DWORD flags, REFERENCE_TIME bufferSize, const WAVEFORMATEX* pFormat)
	{
		REFERENCE_TIME initializeBufferSize = bufferSize;
		if (initializeBufferSize == 0) {
			check_hresult(_audioClient->GetDevicePeriod(&initializeBufferSize, nullptr));
		}
		WAVEFORMATEX* pInitializeFormat = (WAVEFORMATEX*) pFormat;
		if (!pInitializeFormat) {
			check_hresult(_audioClient->GetMixFormat(&pInitializeFormat));
		}
		auto a = trace::begin_initialize(flags,bufferSize, pInitializeFormat);
		HRESULT hr = _audioClient->Initialize(AUDCLNT_SHAREMODE::AUDCLNT_SHAREMODE_SHARED, flags, initializeBufferSize, 0, pInitializeFormat, nullptr);
		trace::end_initialize(a,hr);

		if SUCCEEDED(hr) {
			_state = AudioSessionClientState::Stopped;
			audioFrameSize = pInitializeFormat->wBitsPerSample >> 3;
			audioSampleRate = pInitializeFormat->nSamplesPerSec;
			if (flags & AUDCLNT_STREAMFLAGS_EVENTCALLBACK) {
				_callback = make_self<AudioSessionClientCallback>(this);

			}
		}
		if (!pFormat) {
			CoTaskMemFree(pInitializeFormat);
		}
		return hr;
	}
}