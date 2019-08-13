#pragma once
#include <mfapi.h>
#include <Audioclient.h>
#include "wrappers.h"

namespace winrt::Wasapi::implementation
{
	struct AudioClientBase;
	// Helper class to implement media foundation async callbacks
	struct AudioSessionClientCallback : implements<AudioSessionClientCallback, IMFAsyncCallback>
	{
		DWORD workQueueTaskId = 0;
		DWORD workQueueId = 0;
		winrt::com_ptr<IMFAsyncResult> _asyncResult;
		MFWORKITEM_KEY _workItemKey = 0;
		win32::event _eventCallback;
		win32::critical_section _csCallback;
		winrt::hresult _result = S_OK;

		AudioClientBase* _client;
		AudioSessionClientCallback(AudioClientBase* pClient);
		
		HRESULT ScheduleWorkItemWait();
		HRESULT CancelWorkItemWait();
		HRESULT SetupWorkQueue();

		HRESULT STDMETHODCALLTYPE GetParameters(DWORD* pdwFlags, DWORD* pdwQueue);
		HRESULT STDMETHODCALLTYPE Invoke(IMFAsyncResult* pAsyncResult);
	};
}

