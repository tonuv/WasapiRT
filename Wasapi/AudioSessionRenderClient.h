#pragma once
#include "AudioSessionRenderClient.g.h"
#include "AudioClientBase.h"


namespace winrt::Wasapi::implementation
{
    struct AudioSessionRenderClient : AudioSessionRenderClientT<AudioSessionRenderClient>, public AudioClientBase
    {
		winrt::com_ptr<::IAudioRenderClient> _renderClient;
		IAudioSessionRenderCallback _renderCallback{ nullptr };

		AudioSessionRenderClient(winrt::com_ptr<::IAudioClient> const& client) : AudioClientBase(client) {

		}
		void Initialize();	// initialize non-event driven, default format and default buffer size
		void Initialize(IAudioSessionRenderCallback const& callback);

		virtual void OnEventCallback();
    };
}
