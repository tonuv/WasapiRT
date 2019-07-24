#include "pch.h"
#include "AudioClientBase.h"
#include <windows.media.mediaproperties.h>
#include <mfidl.h>
#include <mfapi.h>


namespace winrt::Wasapi::implementation
{
	AudioClientBase::AudioClientBase(winrt::com_ptr<::IAudioClient> const& client)
	{
		_audioClient = client;
	}

	void AudioClientBase::Close()
	{
		_audioClient = nullptr;
	}

	void AudioClientBase::Start()
	{
		_callback->ScheduleWorkItemWait();
		check_hresult(_audioClient->Start());
	}
	void AudioClientBase::Stop()
	{
		_callback->CancelWorkItemWait();
		check_hresult(_audioClient->Stop());
	}

	void AudioClientBase::Reset() {
		_callback->CancelWorkItemWait();
		check_hresult(_audioClient->Reset());
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
		check_hresult(_audioClient->GetBufferSize(&padding));
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
	winrt::Windows::Media::MediaProperties::AudioEncodingProperties AudioClientBase::Format()
	{
		WAVEFORMATEX* pFormat = nullptr;
		check_hresult(_audioClient->GetMixFormat(&pFormat));
		com_ptr<IMFMediaType> mediaType;
		check_hresult(MFCreateMediaType(mediaType.put()));
		check_hresult(MFInitMediaTypeFromWaveFormatEx(mediaType.get(), pFormat,sizeof(WAVEFORMATEX) + pFormat->cbSize));
		CoTaskMemFree(pFormat);

		com_ptr<ABI::Windows::Media::MediaProperties::IMediaEncodingProperties> abiMediaProps;
		check_hresult(MFCreatePropertiesFromMediaType(mediaType.get(), IID_PPV_ARGS(abiMediaProps.put())));

		winrt::Windows::Media::MediaProperties::AudioEncodingProperties encodingProperties{ nullptr };
		winrt::copy_from_abi(encodingProperties, abiMediaProps.get());

		return encodingProperties;
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

	void AudioClientBase::InitializeWithDefaults(DWORD flags) {
		REFERENCE_TIME defaultBufferSize = 0;
		check_hresult(_audioClient->GetDevicePeriod(&defaultBufferSize, nullptr));
		WAVEFORMATEX* pFormat = nullptr;
		check_hresult(_audioClient->GetMixFormat(&pFormat));
		check_hresult(_audioClient->Initialize(AUDCLNT_SHAREMODE::AUDCLNT_SHAREMODE_SHARED, flags, defaultBufferSize, 0, pFormat, nullptr));
		CoTaskMemFree(pFormat);

	}

	void AudioClientBase::InitializeEventDriven()
	{
		InitializeWithDefaults(AUDCLNT_STREAMFLAGS_EVENTCALLBACK);
		_callback = make_self<AudioSessionClientCallback>(this);
	}


}