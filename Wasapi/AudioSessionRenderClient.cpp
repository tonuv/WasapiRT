#include "pch.h"
#include "AudioSessionRenderClient.h"
#include "AudioSessionRenderClient.g.cpp"
#include <MemoryBuffer.h>

namespace winrt::Wasapi::implementation
{
	uint32_t AudioSessionRenderClient::FramesNeeded()
	{
		UINT32 bufferSize = 0, padding = 0;
		check_hresult(_audioClient->GetBufferSize(&bufferSize));
		check_hresult(_audioClient->GetCurrentPadding(&padding));
		return bufferSize - padding;
	}
	void AudioSessionRenderClient::Initialize()
	{
		if (State() != AudioSessionClientState::Uninitialized)
			throw hresult_error(E_NOT_VALID_STATE);

		InitializeClient();
		check_hresult(_audioClient->GetService(__uuidof(::IAudioRenderClient), _renderClient.put_void()));
	}
	void AudioSessionRenderClient::Initialize(IAudioSessionRenderCallback const& callback)
	{
		if (State() != AudioSessionClientState::Uninitialized)
			throw hresult_error(E_NOT_VALID_STATE);

		_renderCallback = callback;
		InitializeClient(AUDCLNT_STREAMFLAGS_EVENTCALLBACK);
		check_hresult(_audioClient->GetService(__uuidof(::IAudioRenderClient), _renderClient.put_void()));
	}
	void AudioSessionRenderClient::AddFrames(Windows::Media::AudioBuffer const& buffer)
	{
		auto bufferReference = buffer.CreateReference();
		LPBYTE pSourceBuffer = nullptr;
		UINT32 capacity = 0;
		check_hresult(bufferReference.as<::Windows::Foundation::IMemoryBufferByteAccess>()->GetBuffer(&pSourceBuffer, &capacity));

		LPBYTE pDestBuffer = nullptr;
		HRESULT hr = _renderClient->GetBuffer(capacity / audioFrameSize, &pDestBuffer);
		if (SUCCEEDED(hr)) {
			memcpy_s(pDestBuffer,capacity, pSourceBuffer, capacity);
			hr = _renderClient->ReleaseBuffer(capacity / audioFrameSize, 0);
		}
		bufferReference.Close();
		check_hresult(hr);
	}
	void AudioSessionRenderClient::OnEventCallback()
	{
		if (_state == AudioSessionClientState::Running) {
			_renderCallback.OnFramesNeeded();
		}
	}
}
