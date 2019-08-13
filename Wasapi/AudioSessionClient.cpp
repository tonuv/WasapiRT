#include "pch.h"
#include "AudioSessionClient.h"
#include "AudioSessionClient.g.cpp"
#include <pplawait.h>
#include <windows.media.mediaproperties.h>
#include <mfidl.h>
#include <mfapi.h>
#include "AudioInterfaceActivator.h"
#include "AudioSessionRenderClient.h"
#include <winrt/Windows.Media.Devices.h>
#include "trace.h"

using namespace winrt;

namespace winrt::Wasapi::implementation
{

	Windows::Foundation::IAsyncOperation<Wasapi::AudioSessionRenderClient> AudioSessionClient::CreateRenderClientAsync(Windows::Devices::Enumeration::DeviceInformation  device)
	{
		trace::create_render_client(device.Id());
		auto audioClient = co_await ::Wasapi::AudioInterfaceActivator::ActivateAudioInterfaceAsync(device.Id().c_str());
		return make_self<AudioSessionRenderClient>(audioClient).as<Wasapi::AudioSessionRenderClient>();
	}

	Windows::Foundation::IAsyncOperation<Wasapi::AudioSessionRenderClient> AudioSessionClient::CreateRenderClientAsync()
	{
		auto deviceId = Windows::Media::Devices::MediaDevice::GetDefaultAudioRenderId(Windows::Media::Devices::AudioDeviceRole::Default);
		trace::create_render_client(deviceId);
		auto audioClient = co_await ::Wasapi::AudioInterfaceActivator::ActivateAudioInterfaceAsync(deviceId.c_str());
		return make_self<AudioSessionRenderClient>(audioClient).as<Wasapi::AudioSessionRenderClient>();
	}

	Windows::Foundation::IAsyncOperation<Wasapi::AudioSessionCaptureClient> AudioSessionClient::CreateCaptureClientAsync(Windows::Devices::Enumeration::DeviceInformation device)
	{
		trace::create_capture_client(device.Id());
		auto audioClient = co_await ::Wasapi::AudioInterfaceActivator::ActivateAudioInterfaceAsync(device.Id().c_str());
		return make_self<AudioSessionCaptureClient>(audioClient).as<Wasapi::AudioSessionCaptureClient>();
	}
	Windows::Foundation::IAsyncOperation<Wasapi::AudioSessionCaptureClient> AudioSessionClient::CreateCaptureClientAsync()
	{
		auto deviceId = Windows::Media::Devices::MediaDevice::GetDefaultAudioCaptureId(Windows::Media::Devices::AudioDeviceRole::Default);
		trace::create_capture_client(deviceId);
		auto audioClient = co_await ::Wasapi::AudioInterfaceActivator::ActivateAudioInterfaceAsync(deviceId.c_str());
		return make_self<AudioSessionCaptureClient>(audioClient).as<Wasapi::AudioSessionCaptureClient>();
	}


}
