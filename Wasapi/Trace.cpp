#include "pch.h"
#include "trace.h"
#include <winrt/Windows.Foundation.Diagnostics.h>

using namespace winrt::Windows::Foundation::Diagnostics;
using namespace winrt;

namespace trace {
	LoggingChannel g_Log = LoggingChannel(L"WasapiRT",nullptr);
	
	void AddPointerField(LoggingFields const& fields, winrt::hstring const& name,const void* ptr) {
#ifdef _WIN64
		fields.AddInt64(name, (int64_t)(ptr), LoggingFieldFormat::Hexadecimal);
#else
		fields.AddInt32(name, (int32_t)(ptr), LoggingFieldFormat::Hexadecimal);
#endif
	}

	void create_capture_client(winrt::hstring const& id)
	{
		LoggingFields fields = LoggingFields();
		fields.AddString(L"Type", L"Capture");
		fields.AddString(L"Device", id);
		g_Log.LogEvent(L"Create", fields);
	}

	void create_render_client(winrt::hstring const& id)
	{
		LoggingFields fields = LoggingFields();
		fields.AddString(L"Type", L"Render");
		fields.AddString(L"Device", id);
		g_Log.LogEvent(L"Create", fields);
	}

	void audio_activate_completed(winrt::hresult hr1, winrt::hresult hr2)
	{
		LoggingFields fields = LoggingFields();
		fields.AddInt32(L"ActivateResult",hr1,LoggingFieldFormat::HResult);
		fields.AddInt32(L"GetResult", hr2, LoggingFieldFormat::HResult);
		g_Log.LogEvent(L"CreateCompleted", fields);
	}

	activity begin_client_callback()
	{
		LoggingActivity a = g_Log.StartActivity(L"ClientCallback");
		return activity(a);
	}

	void schedule_callback()
	{
		g_Log.LogEvent(L"ScheduleCallback");
	}

	void cancel_callback()
	{
		g_Log.LogEvent(L"CancelCallback");
	}

	activity start(const void *pClient)
	{
		LoggingFields fields = LoggingFields();
		AddPointerField(fields,L"pClient", pClient);
		return g_Log.StartActivity(L"Start", fields);
	}

	activity stop(const void* pClient)
	{
		LoggingFields fields = LoggingFields();
		AddPointerField(fields,L"pClient", pClient);
		return g_Log.StartActivity(L"Stop", fields);
	}

	activity reset(const void* pClient)
	{
		LoggingFields fields = LoggingFields();
		AddPointerField(fields,L"pClient", pClient);
		return g_Log.StartActivity(L"Reset", fields);
	}

	void get_format(winrt::Windows::Media::MediaProperties::AudioEncodingProperties const& format)
	{
		LoggingFields fields = LoggingFields();
		fields.AddString(L"Type", format.Subtype());
		fields.AddUInt32(L"SampleRate", format.SampleRate());
		fields.AddUInt32(L"Channels", format.ChannelCount());
		fields.AddUInt32(L"BitsPerSample", format.BitsPerSample());
		g_Log.LogEvent(L"GetFormat", fields);
	}

	activity begin_initialize(DWORD flags, REFERENCE_TIME bufferSize, const WAVEFORMATEX* pFormat)
	{
		LoggingFields fields = LoggingFields();
		fields.AddUInt32(L"Flags", flags, LoggingFieldFormat::Hexadecimal);
		fields.AddTimeSpan(L"BufferSize", winrt::Windows::Foundation::TimeSpan(bufferSize));

		fields.AddUInt32(L"SampleRate", pFormat->nSamplesPerSec);
		fields.AddUInt32(L"Channels", pFormat->nChannels);
		fields.AddUInt32(L"BitsPerSample", pFormat->wBitsPerSample);
		return g_Log.StartActivity(L"Initialize", fields);
	}

	void end_initialize(activity& a, HRESULT hr)
	{
		LoggingFields fields = LoggingFields();
		fields.AddInt32(L"HResult", hr, LoggingFieldFormat::HResult);
		a.end(fields);
	}


	activity::activity(winrt::Windows::Foundation::Diagnostics::LoggingActivity const& activity)
	{
		_activity = activity;
	}

	activity::~activity()
	{
		end();
	}

	void activity::end() {
		if (_activity) {
			_activity.StopActivity(_activity.Name());
			_activity = nullptr;
		}
	}

	void activity::end(winrt::Windows::Foundation::Diagnostics::LoggingFields const& fields)
	{
		if (_activity) {
			_activity.StopActivity(_activity.Name(),fields);
			_activity = nullptr;
		}
	}

};
