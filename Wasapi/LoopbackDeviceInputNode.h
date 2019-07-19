#pragma once
#include "LoopbackDeviceInputNode.g.h"
#include <ppltasks.h>
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <pplawait.h>
#include "RingBuffer.h"
/*
namespace winrt::WASAPI::implementation
{
	struct AudioInterfaceActivator : implements<AudioInterfaceActivator,::IActivateAudioInterfaceCompletionHandler>
	{
		concurrency::task_completion_event<winrt::com_ptr<::IAudioClient2>> activationCompleted;
		HRESULT STDMETHODCALLTYPE ActivateCompleted(::IActivateAudioInterfaceAsyncOperation* activateOperation);
		static concurrency::task<winrt::com_ptr<::IAudioClient2>> ActivateAudioInterfaceAsync(LPCWCHAR szDeviceId);
	};

    struct LoopbackDeviceInputNode : LoopbackDeviceInputNodeT<LoopbackDeviceInputNode, IMFAsyncCallback>
    {
		winrt::com_ptr<::IAudioClient2> _audioClient;
		winrt::com_ptr<::IAudioCaptureClient> _audioCaptureClient;
		HANDLE evtSamplesReady = INVALID_HANDLE_VALUE;
		DWORD dwWorkQueueId = 0;
		DWORD dwTaskId = 0;
		winrt::com_ptr<IMFAsyncResult> _asyncResult;
		MFWORKITEM_KEY _workItemKey = 0;
		util::RingBuffer _buffer = util::RingBuffer(192000u);

		Windows::Media::Audio::AudioFrameInputNode _audioNode{ nullptr };

		// Async callback methods
		HRESULT STDMETHODCALLTYPE GetParameters(DWORD* pdwFlags,DWORD* pdwQueue);
		HRESULT STDMETHODCALLTYPE Invoke(IMFAsyncResult* pAsyncResult);

		HRESULT ScheduleWaitForSamples();

		void OnQuantumStarted(winrt::Windows::Media::Audio::AudioFrameInputNode const& node, winrt::Windows::Media::Audio::FrameInputNodeQuantumStartedEventArgs const& args);

		LoopbackDeviceInputNode(Windows::Media::Audio::AudioGraph const &graph,winrt::com_ptr<IAudioClient2> const& audioClient);


        static Windows::Foundation::IAsyncOperation<WASAPI::LoopbackDeviceInputNode> CreateAsync(Windows::Media::Audio::AudioGraph graph,Windows::Devices::Enumeration::DeviceInformation renderDevice);
        void Close();
        Windows::Foundation::Collections::IVector<Windows::Media::Effects::IAudioEffectDefinition> EffectDefinitions();
        void OutgoingGain(double value);
        double OutgoingGain();
        Windows::Media::MediaProperties::AudioEncodingProperties EncodingProperties();
        bool ConsumeInput();
        void ConsumeInput(bool value);
        void Start();
        void Stop();
        void Reset();
        void DisableEffectsByDefinition(Windows::Media::Effects::IAudioEffectDefinition const& definition);
        void EnableEffectsByDefinition(Windows::Media::Effects::IAudioEffectDefinition const& definition);
        Windows::Foundation::Collections::IVectorView<Windows::Media::Audio::AudioGraphConnection> OutgoingConnections();
        void AddOutgoingConnection(Windows::Media::Audio::IAudioNode const& destination);
        void AddOutgoingConnection(Windows::Media::Audio::IAudioNode const& destination, double gain);
        void RemoveOutgoingConnection(Windows::Media::Audio::IAudioNode const& destination);
    };
}
namespace winrt::WASAPI::factory_implementation
{
    struct LoopbackDeviceInputNode : LoopbackDeviceInputNodeT<LoopbackDeviceInputNode, implementation::LoopbackDeviceInputNode>
    {
    };
}*/
