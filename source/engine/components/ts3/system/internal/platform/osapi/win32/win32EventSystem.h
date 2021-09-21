
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_EVENT_SYSTEM_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_EVENT_SYSTEM_H__

#include "win32Common.h"
#include <windowsx.h> // For GET_X_LPARAM/GET_Y_LPARAM

namespace ts3::system
{

    class EventController;
    class EventDispatcher;
    class EventSource;

    using NativeEvent = MSG;

    struct EventSourceNativeData
    {
        HWND hwnd;
    };

    struct Win32EventSourceState
    {
        EventController * eventController = nullptr;

        EventSource * eventSource = nullptr;

        LONG_PTR savedEventCallback = 0;
    };

    bool nativeWin32RegisterEventSource( EventController & pEventController, EventSource & pEventSource );

} // namespace ts3::system

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_EVENT_SYSTEM_H__
