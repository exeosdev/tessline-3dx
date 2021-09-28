
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
        HWND hwnd = nullptr;
    };

    struct Win32EventSourceState
    {
        LONG_PTR savedEventCallback = 0;
        LONG_PTR savedEventCallbackUserData = 0;
        EventController * eventController = nullptr;
    };

} // namespace ts3::system

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_EVENT_SYSTEM_H__
