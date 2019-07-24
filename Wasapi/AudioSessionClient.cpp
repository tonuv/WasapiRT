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

using namespace winrt;

namespace winrt::Wasapi::implementation
{

	Windows::Foundation::IAsyncOperation<Wasapi::AudioSessionRenderClient> AudioSessionClient::CreateRenderClientAsync(winrt::Windows::Devices::Enumeration::DeviceInformation const& device)
	{
		auto audioClient = co_await ::Wasapi::AudioInterfaceActivator::ActivateAudioInterfaceAsync(device.Id().c_str());
		return make_self<AudioSessionRenderClient>(audioClient).as<Wasapi::AudioSessionRenderClient>();
	}

	Windows::Foundation::IAsyncOperation<Wasapi::AudioSessionRenderClient> AudioSessionClient::CreateRenderClientAsync()
	{
		auto deviceId = Windows::Media::Devices::MediaDevice::GetDefaultAudioRenderId(Windows::Media::Devices::AudioDeviceRole::Default);
		auto audioClient = co_await ::Wasapi::AudioInterfaceActivator::ActivateAudioInterfaceAsync(deviceId.c_str());
		return make_self<AudioSessionRenderClient>(audioClient).as<Wasapi::AudioSessionRenderClient>();
	}

}
