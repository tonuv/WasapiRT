#pragma once
#include <Windows.h>
#include <chrono>
namespace win32
{
	class handle {
	private:
		HANDLE _handle = INVALID_HANDLE_VALUE;
		void close_handle() {
			if (_handle != INVALID_HANDLE_VALUE) {
				CloseHandle(_handle);
			}
		}
	public:
		handle(std::nullptr_t) {}
		handle(HANDLE handle) {
			_handle = handle;
		}
		handle(handle const&) = delete;	// Do not allow copy
		handle(handle && other) noexcept {		// Move handle value
			_handle = other._handle;
			other._handle = INVALID_HANDLE_VALUE;
		}
		~handle() {
			close_handle();
		}
		void operator=(HANDLE hOther) {
			close_handle();
			_handle = hOther;
		}
		operator HANDLE() const {
			return _handle;
		}
	};

	class object {
		protected:
			handle _hobject{ nullptr };
	public:
		operator HANDLE() const {
			return _hobject;
		}
	};

	class event : public object {
	public:
		event()
		{
			_hobject = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS);
		}
	};

	class critical_section {
		CRITICAL_SECTION _criticalSection;
	public:
		critical_section(critical_section const&) = delete;	// Do not allow copy
		critical_section(critical_section&& other) = delete;

		critical_section() {
			InitializeCriticalSection(&_criticalSection);
		}
		~critical_section() {
			DeleteCriticalSection(&_criticalSection);
		}
		void enter() {
			EnterCriticalSection(&_criticalSection);
		}
		bool try_enter() {
			TryEnterCriticalSection(&_criticalSection);
		}
		void leave() {
			LeaveCriticalSection(&_criticalSection);
		}
	};

}