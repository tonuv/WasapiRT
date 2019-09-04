#pragma once
#include <Audioclient.h>
#include "AudioSessionClientCallback.h"
#include "winrt/Wasapi.h"

namespace winrt::Wasapi::implementation
{

	struct AudioClientBase
	{
		winrt::com_ptr<::IAudioClient3> _audioClient;
		winrt::com_ptr<AudioSessionClientCallback> _callback;
		HRESULT MediaPropertiesToWaveFormatEx(Windows::Media::MediaProperties::AudioEncodingProperties const& format, WAVEFORMATEX** ppFormat);
		Windows::Media::MediaProperties::AudioEncodingProperties WaveFormatExToMediaProperties(const WAVEFORMATEX* pFormat);
		HRESULT InitializeSharedClient(DWORD flags = 0, uint32_t periodInFrame = 0, const WAVEFORMATEX *pFormat=nullptr);
		uint32_t audioFrameSize = 0;
		uint32_t audioSampleRate = 0;
		Wasapi::AudioSessionClientState _state;
		AudioClientProperties _clientProperties;
		Windows::Media::MediaProperties::AudioEncodingProperties _format{ nullptr };
		uint32_t _periodInFrames{ 0u };

		// IMFAsyncCallback interface implementation
		HRESULT STDMETHODCALLTYPE GetParameters(DWORD* pdwFlags, DWORD* pdwQueue);
		HRESULT STDMETHODCALLTYPE Invoke(IMFAsyncResult* pAsyncResult);
		virtual void OnEventCallback()=0;
	public:
		AudioClientBase(winrt::com_ptr<::IAudioClient3> const& client);

		void Start();
		void Stop();
		void Reset();
		Wasapi::AudioSessionClientState State();
		Windows::Media::MediaProperties::AudioEncodingProperties Format();
		uint32_t Period();
		uint32_t BufferSize();
		uint32_t Padding();
		Windows::Foundation::TimeSpan Latency();
		Windows::Media::MediaProperties::AudioEncodingProperties GetDefaultFormat();
		bool IsFormatSupported(Windows::Media::MediaProperties::AudioEncodingProperties const& format);
		Wasapi::AudioClientEnginePeriods GetPeriods(Windows::Media::MediaProperties::AudioEncodingProperties const& format);
		bool IsRawStream();
		void IsRawStream(bool bIsRaw);

		void Close();
	};
};