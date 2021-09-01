
#ifndef __TS3_SYSTEM_EVENT_SYSTEM_NATIVE_H__
#define __TS3_SYSTEM_EVENT_SYSTEM_NATIVE_H__

#include "eventSystem.h"
#include "eventObject.h"

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_ANDROID )
#  include "internal/platform/osapi/android/androidEventCore.h"
#elif( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_WIN32 )
#  include "internal/platform/osapi/win32/win32EventCore.h"
#elif( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_X11 )
#  include "internal/platform/osapi/x11/x11EventCore.h"
#endif

namespace ts3::system
{

    // Represents invalid mouse position. Used as a default value for last position registered.
    constexpr math::Vec2i32 cvMousePosInvalid { cxInt32Max, cxInt32Max };

    enum EEventConfigFlags : uint32
    {
        E_EVENT_CONFIG_FLAG_ENABLE_MOUSE_DOUBLE_CLICK_BIT = 0x0001,
        E_EVENT_CONFIG_FLAG_ENABLE_MOUSE_MULTI_CLICK_BIT = 0x0002 | E_EVENT_CONFIG_FLAG_ENABLE_MOUSE_DOUBLE_CLICK_BIT,
        E_EVENT_CONFIG_FLAG_IDLE_PROCESSING_MODE_BIT = 0x1000
    };

    struct EventInputState
    {
        //
        ContextNativeData * scNativeData = nullptr;
        //
        Bitmask<EEventConfigFlags> flags = 0u;
        //
        duration_value_t mouseClickSequenceTimeoutMs = 100;
        // Current state of the keyboard.
        KeyboardState keyboardState;
        // Last cursor position registered by the event system.
        math::Vec2i32 mouseLastRegPos = cvMousePosInvalid;
        // State of the mouse buttons. Used for motion events on systems which do not have reliable states (X11).
        Bitmask<MouseButtonFlagBits> mouseButtonStateMask = 0;
        // Last mouse button pressed. Used to detect multi-click sequences.
        MouseButtonID mouseLastPressButton = MouseButtonID::Unknown;
        // Current sequence length, i.e. number of clicks of the same button in a row.
        uint32 mouseClickSequenceLength = 1;
        // Timestamp of last registered mouse button press.
        perf_counter_value_t mouseLastPressTimestamp = 0u;
    };

} // namespace ts3::system

#endif // __TS3_SYSTEM_EVENT_SYSTEM_NATIVE_H__
