#pragma once
#include "AudioSessionRenderClient.g.h"
#include "AudioClientBase.h"


namespace winrt::Wasapi::implementation
{
    struct AudioSessionRenderClient : AudioSessionRenderClientT<AudioSessionRenderClient>, public AudioClientBase
    {
		winrt::com_ptr<::IAudioRenderClient> _renderClient;
		IAudioSessionRenderCallback _renderCallback{ nullptr };

		AudioSessionRenderClient(winrt::com_ptr<::IAudioClient3> const& client) : AudioClientBase(client) {

		}
		uint32_t FramesNeeded();
		void Initialize();	// initialize non-event driven, default format and default buffer size
		void Initialize(IAudioSessionRenderCallback const& callback);
		void AddFrames(Windows::Media::AudioBuffer const& buffer);
		virtual void OnEventCallback();
    };
}
