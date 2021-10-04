
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

    struct EventSystemSharedState
    {
        ESSharedInputKeyboardState inputKeyboardState;
        ESSharedInputMouseState inputMouseState;
    };

    /// @brief Private, implementation-specific data of the EventController class.
    struct EventController::ObjectInternalData
    {
        // Pointer to the object which owns this private data.
        EventController * parentController = nullptr;

        // Current active dispatcher used to forward all events. Initially NULL and can be reset to this state.
        EventDispatcher * activeEventDispatcher = nullptr;

        // Container for all dispatchers created in the system.
        std::vector<EventDispatcher *> eventDispatcherList;

        //
        std::vector<EventSource *> eventSourceList;

        //
        std::unordered_map<EventSource *, void *> eventSourcePrivateDataMap;

        // Pointer to the configuration data from currently bound dispatcher.
        const EventSystemInternalConfig * currentInternalConfig = nullptr;

        // Pointer to the configuration data from currently bound dispatcher.
        EventSystemSharedState * currentSharedState = nullptr;

        explicit ObjectInternalData( EventController * pController )
        : parentController( pController )
        {}

        std::pair<bool, std::vector<EventDispatcher *>::iterator> findEventDispatcherInternal( EventDispatcher * pEventDispatcher )
        {
            std::pair<bool, std::vector<EventDispatcher *>::iterator> result;
            result.second = std::find( eventDispatcherList.begin(), eventDispatcherList.end(), pEventDispatcher );
            result.first = ( result.second != eventDispatcherList.end() );
            return result;
        }

        std::pair<bool, std::vector<EventSource *>::iterator> findEventSourceInternal( EventSource * pEventSource )
        {
            std::pair<bool, std::vector<EventSource *>::iterator> result;
            result.second = std::find( eventSourceList.begin(), eventSourceList.end(), pEventSource );
            result.first = ( result.second != eventSourceList.end() );
            return result;
        }

        const EventSystemInternalConfig & getCurrentInternalConfig() const
        {
            ts3DebugAssert( currentInternalConfig != nullptr );
            return *currentInternalConfig;
        }

        EventSystemSharedState & getCurrentSharedState() const
        {
            ts3DebugAssert( currentSharedState != nullptr );
            return *currentSharedState;
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
        EventSystemInternalConfig evtSysInternalConfig;

        // Shared state
        EventSystemSharedState evtSysSharedState;

        // A default handler. If set, it is called if there is no handler registered for a given code/category/base type.
        EventHandler defaultHandler;

        // Array of handlers registered for EventBaseType.
        EventBaseTypeHandlerMap handlerMapByBaseType;

        // Array of handlers registered for EventCategory.
        EventCategoryHandlerMap handlerMapByCategory;

        // Array of handlers registered for EventCodeIndex (i.e. event code itself).
        EventCodeIndexHandlerMap handlerMapByCodeIndex;

        explicit ObjectInternalData( EventDispatcher * pEventDispatcher )
        : parentDispatcher( pEventDispatcher )
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
