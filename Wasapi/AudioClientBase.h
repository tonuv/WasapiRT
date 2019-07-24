#pragma once
#include <Audioclient.h>
#include "wrappers.h"
#include "AudioSessionClientCallback.h"

namespace winrt::Wasapi::implementation
{

	struct AudioClientBase
	{
		winrt::com_ptr<::IAudioClient> _audioClient;
		winrt::com_ptr<AudioSessionClientCallback> _callback;
		HRESULT MediaPropertiesToWaveFormatEx(Windows::Media::MediaProperties::AudioEncodingProperties const& format, WAVEFORMATEX** ppFormat);
		void InitializeWithDefaults(DWORD flags = 0);
		void InitializeEventDriven();
		// IMFAsyncCallback interface implementation
		HRESULT STDMETHODCALLTYPE GetParameters(DWORD* pdwFlags, DWORD* pdwQueue);
		HRESULT STDMETHODCALLTYPE Invoke(IMFAsyncResult* pAsyncResult);
		virtual void OnEventCallback()=0;
	public:
		AudioClientBase(winrt::com_ptr<::IAudioClient> const& client);

		void Start();
		void Stop();
		void Reset();
		uint32_t BufferSize();
		uint32_t Padding();
		Windows::Foundation::TimeSpan DefaultPeriod();
		Windows::Foundation::TimeSpan MinimumPeriod();
		Windows::Foundation::TimeSpan Latency();
		Windows::Media::MediaProperties::AudioEncodingProperties Format();
		bool IsFormatSupported(Windows::Media::MediaProperties::AudioEncodingProperties const& format);


		void Close();
	};
};