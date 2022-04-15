
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_X11_EVENT_CORE_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_X11_EVENT_CORE_H__

#include "x11Common.h"
#include <ts3/system/eventCore.h>
#include <ts3/system/eventObject.h>

namespace ts3::system
{

    namespace platform
    {

        enum class EX11MouseButtonID : enum_default_value_t
        {
            Unknown,
            Left,
            Middle,
            Right,
            VWheelUp,
            VWheelDown,
            HWheelLeft,
            HWheelRight,
            XBT1,
            XBT2
        };

        struct X11EventSourceNativeData : public X11NativeDataCommon
        {
            XWindow windowXID = E_X11_XID_NONE;
        };

        struct NativeEventType
        {
            XEvent xEvent;
        };

        using X11NativeEvent = NativeEventType;

    }

    class X11EventController : public X11NativeObject<EventController, platform::X11NativeDataCommon>
    {
    public:
        X11EventController( SysContextHandle pSysContext );
        virtual ~X11EventController() noexcept;

        using EventController::getEventDispatcherInputState;
        using EventController::getEventDispatcherConfig;

    private:
        /// @override EventController::_nativeRegisterEventSource
        virtual void _nativeRegisterEventSource( EventSource & pEventSource ) override final;

        /// @override EventController::_nativeUnregisterEventSource
        virtual void _nativeUnregisterEventSource( EventSource & pEventSource ) override final;

        /// @override EventController::_nativeUpdateSysQueue
        virtual bool _nativeUpdateSysQueue() override final;

        /// @override EventController::_nativeUpdateSysQueueWait
        virtual bool _nativeUpdateSysQueueWait() override final;
    };

} // namespace ts3::system

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_X11_EVENT_CORE_H__
