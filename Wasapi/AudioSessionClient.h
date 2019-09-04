#pragma once

#include "AudioSessionClient.g.h"
#include "AudioSessionCaptureClient.h"
#include "AudioSessionRenderClient.h"

namespace winrt::Wasapi::implementation
{
	struct AudioSessionClient : AudioSessionClientT<AudioSessionClient>
	{
		AudioSessionClient() = default;
		static Windows::Foundation::IAsyncOperation<Wasapi::AudioSessionRenderClient> CreateRenderClientAsync(Windows::Devices::Enumeration::DeviceInformation device);
		static Windows::Foundation::IAsyncOperation<Wasapi::AudioSessionRenderClient> CreateRenderClientAsync();
		static Windows::Foundation::IAsyncOperation<Wasapi::AudioSessionCaptureClient> CreateCaptureClientAsync(Windows::Devices::Enumeration::DeviceInformation device);
		static Windows::Foundation::IAsyncOperation<Wasapi::AudioSessionCaptureClient> CreateCaptureClientAsync();
	};

}

namespace winrt::Wasapi::factory_implementation
{
	struct AudioSessionClient : AudioSessionClientT<AudioSessionClient, implementation::AudioSessionClient>
	{
	};
}
