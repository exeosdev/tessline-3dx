
#ifndef __TS3_SYSTEM_EVENT_SYSTEM_NATIVE_H__
#define __TS3_SYSTEM_EVENT_SYSTEM_NATIVE_H__

#include "eventSystem.h"
#include "eventObject.h"
#include <deque>
#include <unordered_map>

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_ANDROID )
#  include "internal/platform/osapi/android/androidEventSystem.h"
#elif( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_WIN32 )
#  include "internal/platform/osapi/win32/win32EventSystem.h"
#elif( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_X11 )
#  include "internal/platform/osapi/x11/x11EventSystem.h"
#endif

namespace ts3::system
{

    // Represents invalid mouse position. Used as a default value for last position registered.
    constexpr math::Vec2i32 CX_EVENT_MOUDE_POINT_INVALID { CX_INT32_MAX, CX_INT32_MAX };

    enum EEventSystemConfigFlags : uint32
    {
        E_EVENT_SYSTEM_CONFIG_FLAG_ENABLE_MOUSE_DOUBLE_CLICK_BIT = 0x0001,
        E_EVENT_SYSTEM_CONFIG_FLAG_ENABLE_MOUSE_MULTI_CLICK_BIT = 0x0002 | E_EVENT_SYSTEM_CONFIG_FLAG_ENABLE_MOUSE_DOUBLE_CLICK_BIT,
        E_EVENT_SYSTEM_CONFIG_FLAG_IDLE_PROCESSING_MODE_BIT = 0x1000
    };

    struct EventSystemInternalConfig
    {
        //
        Bitmask<EEventSystemConfigFlags> configFlags = 0u;
        //
        duration_value_t mouseClickSequenceTimeoutMs = 100;
    };

    struct EventInputState
    {
        // Current state of the keyboard.
        KeyboardState keyboardState;
        // Last cursor position registered by the event system.
        math::Vec2i32 mouseLastRegPos = CX_EVENT_MOUDE_POINT_INVALID;
        // State of the mouse buttons. Used for motion events on systems which do not have reliable states (X11).
        Bitmask<MouseButtonFlagBits> mouseButtonStateMask = 0;
        // Last mouse button pressed. Used to detect multi-click sequences.
        MouseButtonID mouseLastPressButton = MouseButtonID::Unknown;
        // Current sequence length, i.e. number of clicks of the same button in a row.
        uint32 mouseClickSequenceLength = 1;
        // Timestamp of last registered mouse button press.
        perf_counter_value_t mouseLastPressTimestamp = 0u;
    };

    struct EventController::ObjectPrivateData
    {
        EventController * objectPtr = nullptr;
        EventDispatcher defaultEventDispatcher;
        std::deque<EventDispatcher> dispatcherInternalStorage;
        std::unordered_map<event_dispatcher_id_t, EventDispatcher *> dispatcherMap;
        EventDispatcher * activeDispatcher = nullptr;
        const EventSystemInternalConfig * currentInternalConfig = nullptr;
        EventInputState inputState;

        explicit ObjectPrivateData( EventController * pObjectPtr )
        : objectPtr( pObjectPtr )
        , defaultEventDispatcher( pObjectPtr, CX_EVENT_DISPATCHER_ID_DEFAULT )
        {}
    };

    struct EventDispatcher::ObjectPrivateData
    {
        using EventBaseTypeHandlerMap = std::array<EventHandler, CX_ENUM_EVENT_BASE_TYPE_COUNT>;
        using EventCategoryHandlerMap = std::array<EventHandler, CX_ENUM_EVENT_CATEGORY_COUNT>;
        using EventCodeIndexHandlerMap = std::array<EventHandler, CX_ENUM_EVENT_CODE_INDEX_COUNT>;

        EventDispatcher * objectPtr = nullptr;
        //
        EventSystemInternalConfig internalConfig;
        //
        EventHandler defaultHandler;
        // Array of handlers registered for EventBaseType.
        EventBaseTypeHandlerMap handlerMapByBaseType;
        // Array of handlers registered for EventCategory.
        EventCategoryHandlerMap handlerMapByCategory;
        // Array of handlers registered for EventCodeIndex (i.e. event code itself).
        EventCodeIndexHandlerMap handlerMapByCodeIndex;

        explicit ObjectPrivateData( EventDispatcher * pObjectPtr )
        : objectPtr( pObjectPtr )
        {}
    };

    TS3_SYSTEM_API bool nativeEventTranslate( EventController & pEventController, const NativeEvent & pNativeEvent, EventObject & pOutEvent );

    inline bool nativeEventDispatch( EventController & pEventController, const NativeEvent & pNativeEvent )
    {
        EventObject eventObject;
        if( nativeEventTranslate( pEventController, pNativeEvent, eventObject ) )
        {
            pEventController.dispatchEvent( eventObject );
            return true;
        }
        return false;
    }

} // namespace ts3::system

#endif // __TS3_SYSTEM_EVENT_SYSTEM_NATIVE_H__
