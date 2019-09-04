#include "pch.h"
#include "AudioSessionCaptureClient.h"
#include "AudioSessionCaptureClient.g.cpp"
#include <mfapi.h>
#include <windows.media.h>


namespace winrt::Wasapi::implementation
{
	AudioSessionCaptureClient::AudioSessionCaptureClient(winrt::com_ptr<::IAudioClient3> const& client) : AudioClientBase(client)
	{
		MULTI_QI qiFactory[1] = { { &__uuidof(IAudioFrameNativeFactory),nullptr,S_OK } };
		check_hresult(CoCreateInstanceFromApp(CLSID_AudioFrameNativeFactory, nullptr, CLSCTX_INPROC_SERVER, nullptr, 1, qiFactory));
		check_hresult(qiFactory[0].hr);
		copy_from_abi(_audioFrameFactory, qiFactory[0].pItf);
	}
	void AudioSessionCaptureClient::Initialize()
	{
		if (State() != AudioSessionClientState::Uninitialized)
			throw hresult_error(E_NOT_VALID_STATE);
		
		InitializeSharedClient();
		check_hresult(_audioClient->GetService(__uuidof(::IAudioCaptureClient), _captureClient.put_void()));
	}
	void AudioSessionCaptureClient::Initialize(IAudioSessionCaptureCallback const& callback)
	{
		if (State() != AudioSessionClientState::Uninitialized)
			throw hresult_error(E_NOT_VALID_STATE);

		_captureCallback = callback;
		InitializeSharedClient(AUDCLNT_STREAMFLAGS_EVENTCALLBACK);
		check_hresult(_audioClient->GetService(__uuidof(::IAudioCaptureClient), _captureClient.put_void()));
	}
	void AudioSessionCaptureClient::InitializeLoopback()
	{
		if (State() != AudioSessionClientState::Uninitialized)
			throw hresult_error(E_NOT_VALID_STATE);

		InitializeSharedClient(AUDCLNT_STREAMFLAGS_LOOPBACK);
		check_hresult(_audioClient->GetService(__uuidof(::IAudioCaptureClient), _captureClient.put_void()));
	}
	void AudioSessionCaptureClient::InitializeLoopback(IAudioSessionCaptureCallback const& callback)
	{
		if (State() != AudioSessionClientState::Uninitialized)
			throw hresult_error(E_NOT_VALID_STATE);

		_captureCallback = callback;
		InitializeSharedClient(AUDCLNT_STREAMFLAGS_LOOPBACK | AUDCLNT_STREAMFLAGS_EVENTCALLBACK);
		check_hresult(_audioClient->GetService(__uuidof(::IAudioCaptureClient), _captureClient.put_void()));
	}
	uint32_t AudioSessionCaptureClient::NextPacketSize()
	{
		UINT32 framesInNextPacket = 0;
		_captureClient->GetNextPacketSize(&framesInNextPacket);
		return framesInNextPacket;
	}
	void AudioSessionCaptureClient::OnEventCallback()
	{
		if (_state == AudioSessionClientState::Running) {
			_captureCallback.OnFramesAvailable();
		}
	}

	
	Windows::Media::AudioFrame AudioSessionCaptureClient::GetFrame()
	{
		// Copy frames from buffer
		UINT32 samplesInBuffer{ 0 };
		DWORD flags{ 0 };
		UINT64 devicePosition{ 0 };
		UINT64 qpcPosition{ 0 };
		LPBYTE pSourceBuffer{ nullptr };
		_captureClient->GetNextPacketSize(&samplesInBuffer);

		HRESULT hr = _captureClient->GetBuffer(&pSourceBuffer, &samplesInBuffer, &flags, &devicePosition, &qpcPosition);

		if (hr == AUDCLNT_S_BUFFER_EMPTY) {
			return nullptr;
		}
		uint32_t bufferByteSize = samplesInBuffer * audioFrameSize;
		// Create a buffer
		winrt::com_ptr<IMFMediaBuffer> mediaBuffer;
		check_hresult(MFCreateMemoryBuffer(bufferByteSize, mediaBuffer.put()));
		LPBYTE pBuffer{ nullptr };
		DWORD maxLength{ 0 }, length{ 0 };
		check_hresult(mediaBuffer->Lock(&pBuffer, &maxLength, &length));
		memcpy_s(pBuffer, maxLength, pSourceBuffer, bufferByteSize);

		_captureClient->ReleaseBuffer(samplesInBuffer);
		mediaBuffer->Unlock();
		mediaBuffer->SetCurrentLength(samplesInBuffer);

		winrt::com_ptr<IMFSample> sample;
		check_hresult(MFCreateSample(sample.put()));
		sample->AddBuffer(mediaBuffer.get());
		sample->SetSampleTime(devicePosition);
		sample->SetSampleFlags(flags);
		sample->SetSampleDuration(long(samplesInBuffer) * 10000000L / long(audioSampleRate));

		com_ptr<ABI::Windows::Media::IAudioFrame> frame;
		check_hresult(_audioFrameFactory->CreateFromMFSample(sample.get(), false, IID_PPV_ARGS(frame.put())));

		Windows::Media::AudioFrame result{ nullptr };
		copy_from_abi(result, frame.get());
		return result;
	}
}
