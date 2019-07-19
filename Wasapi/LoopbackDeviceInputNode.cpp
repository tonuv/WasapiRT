#include "pch.h"
#include "LoopbackDeviceInputNode.h"
#include "LoopbackDeviceInputNode.g.cpp"
#include "winrt/Windows.Media.MediaProperties.h"

using namespace winrt::Windows::Media::MediaProperties;
using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Media::Audio;
using namespace winrt::Windows::Media;
using namespace util;

/*
namespace winrt::WASAPI::implementation
{
	HRESULT __stdcall LoopbackDeviceInputNode::GetParameters(DWORD* pdwFlags, DWORD* pdwQueue)
	{
		if (!pdwFlags)
			return E_POINTER;
		if (!pdwQueue)
			return E_POINTER;
		*pdwFlags = 0;
		*pdwQueue = dwWorkQueueId;

		return S_OK;
	}
	HRESULT __stdcall LoopbackDeviceInputNode::Invoke(IMFAsyncResult* pAsyncResult)
	{
		// Samples are available
		LPBYTE pBuffer{ nullptr };
		UINT32 frameCount{ 0 };
		DWORD dwFlags{ 0 };
		UINT64 devicePosition{ 0 };
		UINT64 qpcPosition{ 0 };
		HRESULT hr = _audioCaptureClient->GetBuffer(&pBuffer, &frameCount, &dwFlags, &devicePosition, &qpcPosition);
		// Copy frames to buffer
		_buffer.append(reinterpret_cast<const float*>(pBuffer), frameCount);
		
		_audioCaptureClient->ReleaseBuffer(frameCount);

		ScheduleWaitForSamples();

		return hr;
	}
	HRESULT LoopbackDeviceInputNode::ScheduleWaitForSamples()
	{
		return MFPutWaitingWorkItem(evtSamplesReady, 0, _asyncResult.get(), &_workItemKey);
	}

	void LoopbackDeviceInputNode::OnQuantumStarted(winrt::Windows::Media::Audio::AudioFrameInputNode const& node, winrt::Windows::Media::Audio::FrameInputNodeQuantumStartedEventArgs const& args)
	{
		auto samplesNeeded = args.RequiredSamples();
		auto frame = AudioFrame(samplesNeeded * sizeof(float));
		node.AddFrame(frame);
	}

	LoopbackDeviceInputNode::LoopbackDeviceInputNode(Windows::Media::Audio::AudioGraph const& graph,winrt::com_ptr<IAudioClient2> const& audioClient)
	{
		MFStartup(MF_VERSION);
		_audioClient = audioClient;
		WAVEFORMATEX* pFormat{ nullptr };
		check_hresult(_audioClient->GetMixFormat(&pFormat));
		auto encoding = AudioEncodingProperties::CreatePcm(pFormat->nSamplesPerSec, pFormat->nChannels, pFormat->wBitsPerSample);
		encoding.Subtype(L"Float");

		_audioNode = graph.CreateFrameInputNode(encoding);

		_audioNode.QuantumStarted(TypedEventHandler<AudioFrameInputNode, FrameInputNodeQuantumStartedEventArgs>(this, &LoopbackDeviceInputNode::OnQuantumStarted));

		REFERENCE_TIME quantumDuration = REFERENCE_TIME(graph.SamplesPerQuantum()) * 10000000L / REFERENCE_TIME(pFormat->nSamplesPerSec);

		check_hresult(_audioClient->Initialize(
			AUDCLNT_SHAREMODE::AUDCLNT_SHAREMODE_SHARED,
			AUDCLNT_STREAMFLAGS_LOOPBACK | AUDCLNT_STREAMFLAGS_EVENTCALLBACK,
			quantumDuration,
			quantumDuration,
			pFormat,
			NULL
		));

		CoTaskMemFree(pFormat);

		evtSamplesReady = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS);

		check_hresult(_audioClient->SetEventHandle(evtSamplesReady));
	
		check_hresult(_audioClient->GetService(__uuidof(IAudioCaptureClient), _audioCaptureClient.put_void()));

		check_hresult(MFLockSharedWorkQueue(L"Pro Audio", 0, &dwTaskId, &dwWorkQueueId));

		check_hresult(MFCreateAsyncResult(nullptr,this,nullptr,_asyncResult.put()));

		check_hresult(ScheduleWaitForSamples());

	}

	Windows::Foundation::IAsyncOperation<WASAPI::LoopbackDeviceInputNode> LoopbackDeviceInputNode::CreateAsync(Windows::Media::Audio::AudioGraph graph,Windows::Devices::Enumeration::DeviceInformation device)
    {
		auto audioClient = co_await AudioInterfaceActivator::ActivateAudioInterfaceAsync(device.Id().c_str());
		return make<LoopbackDeviceInputNode>(graph,audioClient);
    }

    void LoopbackDeviceInputNode::Close()
    {
		_audioNode.Close();
		_audioClient = nullptr;
		_audioCaptureClient = nullptr;
		MFUnlockWorkQueue(dwWorkQueueId);
		CloseHandle(evtSamplesReady);
    }
    Windows::Foundation::Collections::IVector<Windows::Media::Effects::IAudioEffectDefinition> LoopbackDeviceInputNode::EffectDefinitions()
    {
		return _audioNode.EffectDefinitions();
    }
    void LoopbackDeviceInputNode::OutgoingGain(double value)
    {
		_audioNode.OutgoingGain(value);
    }
    double LoopbackDeviceInputNode::OutgoingGain()
    {
        return _audioNode.OutgoingGain();
    }
    Windows::Media::MediaProperties::AudioEncodingProperties LoopbackDeviceInputNode::EncodingProperties()
    {
		return _audioNode.EncodingProperties();
    }
    bool LoopbackDeviceInputNode::ConsumeInput()
    {
		return _audioNode.ConsumeInput();
    }
    void LoopbackDeviceInputNode::ConsumeInput(bool value)
    {
		_audioNode.ConsumeInput(value);
    }
    void LoopbackDeviceInputNode::Start()
    {
		_audioClient->Start();
		_audioNode.Start();
    }

    void LoopbackDeviceInputNode::Stop()
    {
		_audioClient->Stop();
		MFCancelWorkItem(_workItemKey);
		_audioNode.Stop();
    }

    void LoopbackDeviceInputNode::Reset()
    {
		_audioNode.Reset();
    }

    void LoopbackDeviceInputNode::DisableEffectsByDefinition(Windows::Media::Effects::IAudioEffectDefinition const& definition)
    {
		_audioNode.DisableEffectsByDefinition(definition);
    }
    void LoopbackDeviceInputNode::EnableEffectsByDefinition(Windows::Media::Effects::IAudioEffectDefinition const& definition)
    {
		_audioNode.EnableEffectsByDefinition(definition);
    }
    Windows::Foundation::Collections::IVectorView<Windows::Media::Audio::AudioGraphConnection> LoopbackDeviceInputNode::OutgoingConnections()
    {
		return _audioNode.OutgoingConnections();
    }
    void LoopbackDeviceInputNode::AddOutgoingConnection(Windows::Media::Audio::IAudioNode const& destination)
    {
		_audioNode.AddOutgoingConnection(destination);
    }
    void LoopbackDeviceInputNode::AddOutgoingConnection(Windows::Media::Audio::IAudioNode const& destination, double gain)
    {
		_audioNode.AddOutgoingConnection(destination, gain);
    }
    void LoopbackDeviceInputNode::RemoveOutgoingConnection(Windows::Media::Audio::IAudioNode const& destination)
    {
		_audioNode.RemoveOutgoingConnection(destination);
    }

	HRESULT __stdcall AudioInterfaceActivator::ActivateCompleted(IActivateAudioInterfaceAsyncOperation* pActivateOperation)
	{
		HRESULT hrActivate = E_FAIL;
		com_ptr<IUnknown> activatedInterface;
		HRESULT hr = pActivateOperation->GetActivateResult(&hrActivate, activatedInterface.put());
		if (FAILED(hrActivate)) {
			activationCompleted.set_exception(winrt::hresult_error(hrActivate));
		}
		else if (FAILED(hr)) {
			activationCompleted.set_exception(winrt::hresult_error(hr));
		}
		else {
			activationCompleted.set(activatedInterface.as<IAudioClient2>());
		}
		return S_OK;
	}
	concurrency::task<winrt::com_ptr<IAudioClient2>> AudioInterfaceActivator::ActivateAudioInterfaceAsync(LPCWCHAR szDeviceId)
	{
		auto activator = winrt::make_self<AudioInterfaceActivator>();
		winrt::com_ptr<IActivateAudioInterfaceAsyncOperation> op;
		check_hresult( ::ActivateAudioInterfaceAsync(
			szDeviceId,
			__uuidof(IAudioClient2),
			nullptr,
			activator.as<IActivateAudioInterfaceCompletionHandler>().get(),
			op.put()));
		
		return concurrency::create_task(activator->activationCompleted);
	}
}*/
