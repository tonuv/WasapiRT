#include "pch.h"
#include "AudioSessionClientCallback.h"
#include "AudioClientBase.h"
#include "Trace.h"

namespace winrt::Wasapi::implementation {
	AudioSessionClientCallback::AudioSessionClientCallback(AudioClientBase* pClient)
	{
		_callbackEventHandle = winrt::handle(CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS));
		_client = pClient;
		_result = _client->_audioClient->SetEventHandle(_callbackEventHandle.get());
		if (SUCCEEDED(_result)) {
			_result = SetupWorkQueue();
		}
	}

	HRESULT __stdcall AudioSessionClientCallback::GetParameters(DWORD* pdwFlags, DWORD* pdwQueue)
	{
		if (!pdwFlags || !pdwQueue) {
			return E_POINTER;
		}
		*pdwFlags = 0;
		*pdwQueue = workQueueId;
		return S_OK;
	}
	HRESULT __stdcall AudioSessionClientCallback::Invoke(IMFAsyncResult *)
	{
		auto activity = trace::begin_client_callback();
		_client->OnEventCallback();
		activity.end();
		ScheduleWorkItemWait();
		return S_OK;
	}

	HRESULT AudioSessionClientCallback::ScheduleWorkItemWait()
	{
		trace::schedule_callback();
		return MFPutWaitingWorkItem(_callbackEventHandle.get(), 0, _asyncResult.get(), &_workItemKey);
	}
	HRESULT AudioSessionClientCallback::CancelWorkItemWait()
	{
		trace::cancel_callback();
		return MFCancelWorkItem(_workItemKey);
	}

	static bool mfInitialized = false;

	HRESULT AudioSessionClientCallback::SetupWorkQueue()
	{
		HRESULT hr = S_OK;
		if (!mfInitialized) {
			hr = MFStartup(MF_VERSION);
			if FAILED(hr) {
				return hr;
			}
			mfInitialized = true;
		}

		hr = MFLockSharedWorkQueue(L"Pro Audio", 0, &workQueueTaskId, &workQueueId);
		if FAILED(hr) {
			return hr;
		}
		// Create async result that can be shared across wait operations as there is only one active at a time
		hr = MFCreateAsyncResult(nullptr, (IMFAsyncCallback*)(this), nullptr, _asyncResult.put());
		return hr;
	}
}
