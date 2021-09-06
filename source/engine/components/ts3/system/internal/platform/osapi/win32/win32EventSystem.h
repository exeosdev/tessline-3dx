
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_EVENT_CORE_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_EVENT_CORE_H__

#include "win32Common.h"
#include <windowsx.h> // For GET_X_LPARAM/GET_Y_LPARAM

namespace ts3::system
{

    class EventController;
    class EventDispatcher;

    using NativeEvent = MSG;

    void nativeWin32EnableWindowEventSupport( HWND pHWND, EventController * pEventController );

} // namespace ts3::system

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_EVENT_CORE_H__