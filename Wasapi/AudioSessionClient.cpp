#include "pch.h"
#include "AudioSessionClient.h"
#include "AudioSessionClient.g.cpp"
#include <pplawait.h>
#include <windows.media.mediaproperties.h>
#include <mfidl.h>
#include "AudioInterfaceActivator.h"

using namespace winrt;


namespace winrt::Wasapi::implementation
{
	AudioSessionClient::AudioSessionClient(winrt::com_ptr<::IAudioClient> const &audioClient)
	{
		_audioClient = audioClient;
	}
	winrt::Windows::Foundation::IAsyncOperation<Wasapi::AudioSessionClient> AudioSessionClient::CreateAsync(hstring deviceId)
	{
		auto audioClient = co_await ::Wasapi::AudioInterfaceActivator::ActivateAudioInterfaceAsync(deviceId.c_str());
		return make<AudioSessionClient>(audioClient);
	}
	void AudioSessionClient::Start()
	{
		check_hresult(_audioClient->Start());
	}
	void AudioSessionClient::Stop()
	{
		check_hresult(_audioClient->Stop());
	}

	void AudioSessionClient::Reset() {
		check_hresult(_audioClient->Reset());
	}
	uint32_t AudioSessionClient::BufferSize()
	{
		UINT32 bufferSize = 0;
		check_hresult(_audioClient->GetBufferSize(&bufferSize));
		return bufferSize;
	}
	uint32_t AudioSessionClient::Padding()
	{
		UINT32 padding = 0;
		check_hresult(_audioClient->GetBufferSize(&padding));
		return padding;
	}
	winrt::Windows::Foundation::TimeSpan AudioSessionClient::DefaultPeriod()
	{
		REFERENCE_TIME period = 0;
		check_hresult(_audioClient->GetDevicePeriod(&period, nullptr));
		return Windows::Foundation::TimeSpan(period);
	}
	winrt::Windows::Foundation::TimeSpan AudioSessionClient::MinimumPeriod()
	{
		REFERENCE_TIME period = 0;
		check_hresult(_audioClient->GetDevicePeriod(nullptr,&period));
		return Windows::Foundation::TimeSpan(period);
	}
	Windows::Foundation::TimeSpan AudioSessionClient::Latency()
	{
		REFERENCE_TIME latency = 0;
		check_hresult(_audioClient->GetStreamLatency(&latency));
		return Windows::Foundation::TimeSpan(latency);
	}
	winrt::Windows::Media::MediaProperties::AudioEncodingProperties AudioSessionClient::Format()
	{
		WAVEFORMATEX *pFormat = nullptr;
		check_hresult(_audioClient->GetMixFormat(&pFormat));
		com_ptr<IMFMediaType> mediaType;
		check_hresult(MFCreateMediaType(mediaType.put()));
		check_hresult(MFInitMediaTypeFromWaveFormatEx(mediaType.get(), pFormat, sizeof(WAVEFORMATEXTENSIBLE)));
		CoTaskMemFree(pFormat);

		com_ptr<ABI::Windows::Media::MediaProperties::IMediaEncodingProperties> abiMediaProps;
		check_hresult(MFCreatePropertiesFromMediaType(mediaType.get(), IID_PPV_ARGS(abiMediaProps.put())));
		
		winrt::Windows::Media::MediaProperties::AudioEncodingProperties encodingProperties{ nullptr };
		winrt::copy_from_abi(encodingProperties, abiMediaProps.get());

		return encodingProperties;
	}

	void AudioSessionClient::Initialize(Windows::Foundation::TimeSpan bufferDuration, Windows::Media::MediaProperties::AudioEncodingProperties format)
	{
		check_hresult(Initialize(AUDCLNT_SHAREMODE::AUDCLNT_SHAREMODE_SHARED, 0, bufferDuration.count(), 0, format));
	}

	void AudioSessionClient::InitializeLoopback(Windows::Foundation::TimeSpan bufferDuration, Windows::Media::MediaProperties::AudioEncodingProperties format)
	{
		check_hresult(Initialize(AUDCLNT_SHAREMODE::AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_LOOPBACK, bufferDuration.count(), 0, format));
	}

	bool AudioSessionClient::IsFormatSupported(Windows::Media::MediaProperties::AudioEncodingProperties const & format)
	{
		WAVEFORMATEX* pFormat = nullptr, *pClosestMatch = nullptr;
		MediaPropertiesToWaveFormatEx(format, &pFormat);
		HRESULT hr = _audioClient->IsFormatSupported(AUDCLNT_SHAREMODE::AUDCLNT_SHAREMODE_SHARED, pFormat, &pClosestMatch);
		CoTaskMemFree(pClosestMatch);
		CoTaskMemFree(pFormat);
		return hr == S_OK;
	}

	Wasapi::AudioSessionCaptureClient AudioSessionClient::CreateCaptureClient()
	{
		winrt::com_ptr<::IAudioCaptureClient> captureClient;
		check_hresult(_audioClient->GetService(_uuidof(::IAudioCaptureClient), captureClient.put_void()));
		return make_self<AudioSessionCaptureClient>(captureClient, _sampleLength, _sampleRate).as<Wasapi::AudioSessionCaptureClient>();;
	}

	HRESULT AudioSessionClient::Initialize(AUDCLNT_SHAREMODE shareMode,DWORD flags,REFERENCE_TIME bufferDuration,REFERENCE_TIME periodicity,Windows::Media::MediaProperties::AudioEncodingProperties const & format)
	{
		com_ptr<IMFMediaType> mediaType;
		HRESULT hr = MFCreateMediaTypeFromProperties(reinterpret_cast<IUnknown *>(winrt::get_abi(format)), mediaType.put());
		if (FAILED(hr))
			return hr;
		WAVEFORMATEX* pFormat = nullptr;
		hr = MediaPropertiesToWaveFormatEx(format, &pFormat);
		if (FAILED(hr))
			return hr;
		hr = _audioClient->Initialize(shareMode, flags, bufferDuration, periodicity, pFormat, nullptr);
		_sampleRate = pFormat->nSamplesPerSec;
		_sampleLength = pFormat->wBitsPerSample >> 3;
		CoTaskMemFree(pFormat);
		return hr;
	}
	HRESULT AudioSessionClient::MediaPropertiesToWaveFormatEx(Windows::Media::MediaProperties::AudioEncodingProperties const& format, WAVEFORMATEX** ppFormat)
	{
		com_ptr<IMFMediaType> mediaType;
		HRESULT hr = MFCreateMediaTypeFromProperties(reinterpret_cast<IUnknown*>(winrt::get_abi(format)), mediaType.put());
		if (FAILED(hr))
			return hr;
		UINT32 size = 0;
		hr = MFCreateWaveFormatExFromMFMediaType(mediaType.get(), ppFormat, &size);
		return hr;		
	}
}
