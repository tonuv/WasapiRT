#include "pch.h"
#include "AudioSessionClientCallback.h"
#include "AudioClientBase.h"

namespace winrt::Wasapi::implementation {
	AudioSessionClientCallback::AudioSessionClientCallback(AudioClientBase* pClient)
	{
		_client = pClient;
		_client->_audioClient->SetEventHandle(_eventCallback);
		SetupWorkQueue();
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
	HRESULT __stdcall AudioSessionClientCallback::Invoke(IMFAsyncResult*)
	{
		_csCallback.enter();
		_client->OnEventCallback();
		ScheduleWorkItemWait();
		_csCallback.leave();
		return S_OK;
	}

	HRESULT AudioSessionClientCallback::ScheduleWorkItemWait()
	{
		return MFPutWaitingWorkItem(_eventCallback, 0, _asyncResult.get(), &_workItemKey);
	}
	HRESULT AudioSessionClientCallback::CancelWorkItemWait()
	{
		return MFCancelWorkItem(_workItemKey);
	}

	static bool mfInitialized = false;

	HRESULT AudioSessionClientCallback::SetupWorkQueue()
	{
		if (!mfInitialized) {
			MFStartup(MF_VERSION);
			mfInitialized = true;
		}
		HRESULT hr = S_OK;

		hr = MFLockSharedWorkQueue(L"Pro Audio", 0, &workQueueTaskId, &workQueueId);
		if FAILED(hr) {
			return hr;
		}
		// Create async result that can be shared across wait operations as there is only one active at a time
		hr = MFCreateAsyncResult(nullptr, (IMFAsyncCallback*)(this), nullptr, _asyncResult.put());
		return hr;
	}
}
