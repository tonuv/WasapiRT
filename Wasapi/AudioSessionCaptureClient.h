#pragma once
#include "AudioSessionCaptureClient.g.h"
#include <Audioclient.h>
#include <windows.media.core.interop.h>
#include <windows.media.h>
#include "AudioClientBase.h"

namespace winrt::Wasapi::implementation
{
	struct AudioSessionCaptureClient : AudioSessionCaptureClientT<AudioSessionCaptureClient>, public AudioClientBase
	{
		winrt::com_ptr<::IAudioCaptureClient> _captureClient;
		IAudioSessionCaptureCallback _captureCallback{ nullptr };
		winrt::com_ptr<IAudioFrameNativeFactory> _audioFrameFactory;

		AudioSessionCaptureClient(winrt::com_ptr<::IAudioClient3> const& client);
		void Initialize();	// initialize non-event driven, default format and default buffer size
		void Initialize(IAudioSessionCaptureCallback const& callback);
		void InitializeLoopback();	// initialize non-event driven, default format and default buffer size
		void InitializeLoopback(IAudioSessionCaptureCallback const& callback);
		uint32_t NextPacketSize();
		Windows::Media::AudioFrame GetFrame();
		virtual void OnEventCallback();
	};

}
