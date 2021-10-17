
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_EVENT_CORE_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_EVENT_CORE_H__

#include <ts3/system/eventCore.h>
#include <ts3/system/eventObject.h>
#include "win32Common.h"
#include <windowsx.h> // For GET_X_LPARAM/GET_Y_LPARAM

namespace ts3::system
{

    namespace platform
    {

        struct NativeEvent : public MSG
        {};

        struct Win32EventSourceNativeData
        {
            HWND hwnd = nullptr;
        };

        struct Win32EventSourceState
        {
            LONG_PTR savedEventCallback = 0;
            LONG_PTR savedEventCallbackUserData = 0;
            EventController * eventController = nullptr;
        };

        struct Win32EventSourceStateDeleter
        {
            void operator()( void * pState )
            {
                auto * win32State = static_cast<Win32EventSourceState *>( pState );
                delete win32State;
            }
        };

        bool win32TranslateEvent( EventController & pEventController, const MSG & pMSG, EventObject & pOutEvent );

    }

    class Win32EventController : public EventController
    {
    public:
        Win32EventController( SysContextHandle pSysContext );
        virtual ~Win32EventController() noexcept;

        using EventController::getEventSystemInternalConfig;
        using EventController::getEventSystemSharedState;

    private:
        /// @override EventController::_nativeRegisterEventSource
        virtual std::shared_ptr<void> _nativeRegisterEventSource( EventSource & pEventSource ) override final;

        /// @override EventController::_nativeUnregisterEventSource
        virtual void _nativeUnregisterEventSource( EventSource & pEventSource ) override final;

        /// @override EventController::_nativeUpdateSysQueue
        virtual bool _nativeUpdateSysQueue() override final;

        /// @override EventController::_nativeUpdateSysQueueWait
        virtual bool _nativeUpdateSysQueueWait() override final;
    };

} // namespace ts3::system

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_EVENT_CORE_H__
