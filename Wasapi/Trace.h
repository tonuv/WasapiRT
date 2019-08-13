#pragma once
#include <winrt/Windows.Foundation.Diagnostics.h>
#include <winrt/Windows.Media.MediaProperties.h>
namespace trace
{
	struct activity {
		winrt::Windows::Foundation::Diagnostics::LoggingActivity _activity {nullptr};

		activity(winrt::Windows::Foundation::Diagnostics::LoggingActivity const& activity);
		~activity();
		void end();
		void end(winrt::Windows::Foundation::Diagnostics::LoggingFields const& fields);
	};

	void create_capture_client(winrt::hstring const & id);
	void create_render_client(winrt::hstring const & id);
	void audio_activate_completed(winrt::hresult hr1, winrt::hresult hr2);
	activity begin_client_callback();
	void schedule_callback();
	void cancel_callback();
	activity start(const void *pClient);
	activity stop(const void* pClient);
	activity reset(const void* pClient);
	void get_format(winrt::Windows::Media::MediaProperties::AudioEncodingProperties const& format);
	activity begin_initialize(DWORD flags, REFERENCE_TIME bufferSize, const WAVEFORMATEX* pFormat);
	void end_initialize(activity& a, HRESULT hr);
};