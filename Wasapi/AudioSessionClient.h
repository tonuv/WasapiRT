#pragma once

#include "AudioSessionClient.g.h"
#include "AudioSessionCaptureClient.h"
#include "AudioSessionRenderClient.h"
#include "wrappers.h"

namespace winrt::Wasapi::implementation
{
	struct AudioSessionClient : AudioSessionClientT<AudioSessionClient>
	{
		AudioSessionClient() = default;
		static Windows::Foundation::IAsyncOperation<Wasapi::AudioSessionRenderClient> CreateRenderClientAsync(winrt::Windows::Devices::Enumeration::DeviceInformation const &device);
		static Windows::Foundation::IAsyncOperation<Wasapi::AudioSessionRenderClient> CreateRenderClientAsync();
	};

}

namespace winrt::Wasapi::factory_implementation
{
	struct AudioSessionClient : AudioSessionClientT<AudioSessionClient, implementation::AudioSessionClient>
	{
	};
}
