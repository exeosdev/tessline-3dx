
#ifndef __TS3_SYSTEM_EVENT_SYSTEM_NATIVE_H__
#define __TS3_SYSTEM_EVENT_SYSTEM_NATIVE_H__

#include "eventSystem.h"
#include "eventObject.h"

#include <deque>
#include <list>
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

    class Window;
    class WindowManager;

    // Represents invalid mouse position. Used as a default value for last position registered.
    constexpr math::Vec2i32 CX_EVENT_MOUSE_POS_INVALID { CX_INT32_MAX, CX_INT32_MAX };

    enum EEventSystemConfigFlags : uint32
    {
        E_EVENT_SYSTEM_CONFIG_FLAG_ENABLE_MOUSE_DOUBLE_CLICK_BIT = 0x0001,
        E_EVENT_SYSTEM_CONFIG_FLAG_ENABLE_MOUSE_MULTI_CLICK_BIT = 0x0002 | E_EVENT_SYSTEM_CONFIG_FLAG_ENABLE_MOUSE_DOUBLE_CLICK_BIT,
        E_EVENT_SYSTEM_CONFIG_FLAG_IDLE_PROCESSING_MODE_BIT = 0x1000
    };

    struct EventSystemInternalConfig
    {
        // Configuration flags. Allow controlling aspects like mouse behaviour.
        Bitmask<EEventSystemConfigFlags> configFlags = 0u;

        // Timeout (in milliseconds) after which mouse click sequence is reset.
        duration_value_t mouseClickSequenceTimeoutMs = 100;
    };

    struct EventInputState
    {
        // Current state of the keyboard.
        KeyboardState keyboardState;

        // Last cursor position registered by the event system.
        math::Vec2i32 mouseLastRegPos = CX_EVENT_MOUSE_POS_INVALID;

        // State of the mouse buttons. Used for motion events on systems which do not have reliable states (X11).
        Bitmask<EMouseButtonFlagBits> mouseButtonStateMask = 0;

        // Last mouse button pressed. Used to detect multi-click sequences.
        EMouseButtonID mouseLastPressButton = EMouseButtonID::Unknown;

        // Current sequence length, i.e. number of clicks of the same button in a row.
        uint32 mouseClickSequenceLength = 1;

        // Timestamp of last registered mouse button press.
        perf_counter_value_t mouseLastPressTimestamp = 0u;
    };

    /// @brief Private, implementation-specific data of the EventController class.
    struct EventController::ObjectInternalData
    {
        // Pointer to the object which owns this private data.
        EventController * parentController = nullptr;

        // Default event dispatcher. It can be bound by either calling setDefaultActiveDispatcher() on the controller
        // or by getting its pointer with getEventDispatcher( CX_EVENT_DISPATCHER_ID_DEFAULT ) and making it active.
        EventDispatcherHandle defaultEventDispatcher = nullptr;

        // Current active dispatcher used to forward all events. Initially NULL and can be reset to this state.
        EventDispatcher * activeEventDispatcher = nullptr;

        // Container for all dispatchers created in the system.
        std::list<EventDispatcherHandle> dispatcherList;

        // ID -> Dispatcher map.
        std::unordered_map<event_dispatcher_id_t, EventDispatcher *> dispatcherMap;

        //
        std::vector<EventSource *> eventSourceList;

        //
        std::unordered_map<EventSource *, void *> eventSourcePrivateDataMap;

        // Pointer to the configuration data from currently bound dispatcher.
        const EventSystemInternalConfig * currentInternalConfig = nullptr;

        // Pointer to the configuration data from currently bound dispatcher.
        EventInputState * currentInputState = nullptr;

        explicit ObjectInternalData( EventController * pController )
        : parentController( pController )
        {}

        const EventSystemInternalConfig & getCurrentInternalConfig() const
        {
            ts3DebugAssert( currentInternalConfig != nullptr );
            return *currentInternalConfig;
        }

        EventInputState & getCurrentInputState() const
        {
            ts3DebugAssert( currentInputState != nullptr );
            return *currentInputState;
        }
    };

    /// @brief Private, implementation-specific data of the EventDispatcher class.
    struct EventDispatcher::ObjectInternalData
    {
        using EventBaseTypeHandlerMap = std::array<EventHandler, CX_ENUM_EVENT_BASE_TYPE_COUNT>;
        using EventCategoryHandlerMap = std::array<EventHandler, CX_ENUM_EVENT_CATEGORY_COUNT>;
        using EventCodeIndexHandlerMap = std::array<EventHandler, CX_ENUM_EVENT_CODE_INDEX_COUNT>;

        // Pointer to the object which owns this private data.
        EventDispatcher * parentDispatcher = nullptr;

        //
        std::list<EventDispatcherHandle>::iterator parentListRef;

        // Internal configuration of the event system. The configuration is stored per-dispatcher, so that in case
        // of multiple instances, configuration is properly restored each time a dispatcher is set as an active one.
        EventSystemInternalConfig internalConfig;

        // Input state
        EventInputState inputState;

        // A default handler. If set, it is called if there is no handler registered for a given code/category/base type.
        EventHandler defaultHandler;

        // Array of handlers registered for EventBaseType.
        EventBaseTypeHandlerMap handlerMapByBaseType;

        // Array of handlers registered for EventCategory.
        EventCategoryHandlerMap handlerMapByCategory;

        // Array of handlers registered for EventCodeIndex (i.e. event code itself).
        EventCodeIndexHandlerMap handlerMapByCodeIndex;

        explicit ObjectInternalData( EventDispatcher * pDispatcher )
        : parentDispatcher( pDispatcher )
        {}
    };

    /// @brief Translates a native, OS-specific event into an internal representation.
    /// This is an internal function, implemented at the OS API level. Technically, it could be purely OS-specific,
    /// but having it here gives us the ability to write the dispatching helper function below and avoid duplicating.
    TS3_SYSTEM_API bool nativeEventTranslate( EventController & pEventController, const NativeEvent & pNativeEvent, EventObject & pOutEvent );

    /// @brief Helper function for translating and dispatching a native event.
    inline bool nativeEventDispatch( EventController & pEventController, const NativeEvent & pNativeEvent )
    {
        EventObject eventObject;

        // Translate the input event and store the output in the temporary auto event object.
        // The boolean result indicates whether the translation was successful (event is known).
        if( nativeEventTranslate( pEventController, pNativeEvent, eventObject ) )
        {
            // Dispatch the event through the provided controller object.
            // The boolean result indicates whether there was a handler for this event.
            // False means, that the event has not been processed.
            if( pEventController.dispatchEvent( eventObject ) )
            {
                return true;
            }
        }
        return false;
    }

} // namespace ts3::system

#endif // __TS3_SYSTEM_EVENT_SYSTEM_NATIVE_H__
