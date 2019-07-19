#pragma once

#include "AudioSessionClient.g.h"
#include "AudioSessionCaptureClient.h"

namespace winrt::Wasapi::implementation
{
	struct AudioSessionClient : AudioSessionClientT<AudioSessionClient>
	{
		winrt::com_ptr<::IAudioClient> _audioClient;
		uint32_t _sampleLength{ 0 };
		uint32_t _sampleRate{ 0 };
		AudioSessionClient(winrt::com_ptr<::IAudioClient> const &AudioSessionClient);

		static Windows::Foundation::IAsyncOperation<Wasapi::AudioSessionClient> CreateAsync(hstring deviceId);
		void Start();
		void Stop();
		void Reset();
		uint32_t BufferSize();
		uint32_t Padding();
		Windows::Foundation::TimeSpan DefaultPeriod();
		Windows::Foundation::TimeSpan MinimumPeriod();
		Windows::Foundation::TimeSpan Latency();
		Windows::Media::MediaProperties::AudioEncodingProperties Format();
		void Initialize(Windows::Foundation::TimeSpan bufferDuration,Windows::Media::MediaProperties::AudioEncodingProperties format);
		void InitializeLoopback(Windows::Foundation::TimeSpan bufferDuration, Windows::Media::MediaProperties::AudioEncodingProperties format);
		bool IsFormatSupported(Windows::Media::MediaProperties::AudioEncodingProperties const &format);
		Wasapi::AudioSessionCaptureClient CreateCaptureClient();

		HRESULT Initialize(AUDCLNT_SHAREMODE shareMode, DWORD flags, REFERENCE_TIME bufferDuration, REFERENCE_TIME periodicity, Windows::Media::MediaProperties::AudioEncodingProperties const& format);
		HRESULT MediaPropertiesToWaveFormatEx(Windows::Media::MediaProperties::AudioEncodingProperties const& format, WAVEFORMATEX** ppFormat);

	};
}
namespace winrt::Wasapi::factory_implementation
{
	struct AudioSessionClient : AudioSessionClientT<AudioSessionClient, implementation::AudioSessionClient>
	{
	};
}
