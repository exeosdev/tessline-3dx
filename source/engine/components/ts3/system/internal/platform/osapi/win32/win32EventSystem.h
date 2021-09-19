
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_EVENT_SYSTEM_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_EVENT_SYSTEM_H__

#include "win32Common.h"
#include <windowsx.h> // For GET_X_LPARAM/GET_Y_LPARAM

namespace ts3::system
{

    class EventController;
    class EventDispatcher;
    class Window;

    using NativeEvent = MSG;

    struct Win32WindowEventState
    {
        Window * windowObject;
        EventController * eventController;
    };

} // namespace ts3::system

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_EVENT_SYSTEM_H__
