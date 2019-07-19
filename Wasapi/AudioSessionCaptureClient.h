#pragma once
#include "AudioSessionCaptureClient.g.h"
#include <Audioclient.h>
#include <windows.media.core.interop.h>
#include <windows.media.h>

namespace winrt::Wasapi::implementation
{
    struct AudioSessionCaptureClient : AudioSessionCaptureClientT<AudioSessionCaptureClient>
    {
		winrt::com_ptr<::IAudioCaptureClient> _captureClient;
		uint32_t _sampleSize;
		uint32_t _sampleRate;
		winrt::com_ptr<IAudioFrameNativeFactory> _audioFrameFactory;

        AudioSessionCaptureClient(winrt::com_ptr<::IAudioCaptureClient> const & captureClient,uint32_t sampleSize,uint32_t sampleRate);

        uint32_t GetNextPacketSize();
        Windows::Media::AudioFrame GetBuffer();
    };
}
