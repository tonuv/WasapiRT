#include "pch.h"
#include "AudioSessionRenderClient.h"
#include "AudioSessionRenderClient.g.cpp"

namespace winrt::Wasapi::implementation
{
	void AudioSessionRenderClient::Initialize()
	{
		InitializeWithDefaults();
		check_hresult(_audioClient->GetService(__uuidof(::IAudioRenderClient), _renderClient.put_void()));
	}
	void AudioSessionRenderClient::Initialize(IAudioSessionRenderCallback const& callback)
	{
		_renderCallback = callback;
		InitializeEventDriven();
		check_hresult(_audioClient->GetService(__uuidof(::IAudioRenderClient), _renderClient.put_void()));
	}
	void AudioSessionRenderClient::OnEventCallback()
	{
		_renderCallback.OnFramesNeeded();
	}
}
