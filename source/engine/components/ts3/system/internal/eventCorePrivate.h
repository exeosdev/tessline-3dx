
#ifndef __TS3_SYSTEM_INTERNAL_EVENT_CORE_PRIVATE_H__
#define __TS3_SYSTEM_INTERNAL_EVENT_CORE_PRIVATE_H__

#include <ts3/system/eventCore.h>
#include <ts3/system/eventObject.h>

#include <deque>
#include <list>
#include <unordered_map>

namespace ts3::system
{
	
	using LocalEventQueue = std::deque<EventObject>;

	enum EEventSystemInternalFlags : uint32
	{
		//
		E_EVENT_SYSTEM_INTERNAL_FLAG_APP_QUIT_REQUEST_SET_BIT = 0x010000,
	};

	struct EventDispatcherConfig
	{
		// Configuration flags. Allow controlling aspects like mouse or keyboard behaviour.
		Bitmask<EEventDispatcherConfigFlags> dispatcherConfigFlags = 0u;

		// Timeout (in milliseconds) after which mouse click sequence is reset.
		duration_value_t mouseClickSequenceTimeoutMs = 100;
	};

	struct EventDispatcherInputState
	{
		EvtSharedInputKeyboardState inputKeyboardState;

		EvtSharedInputMouseState inputMouseState;
	};

	/// @brief Private, implementation-specific data of the EventController class.
	struct EventController::EventControllerPrivateData
	{
		// Current active dispatcher used to forward all events. Initially NULL and can be reset to this state.
		EventDispatcher * activeEventDispatcher = nullptr;

		// Pointer to the primary event source. Set by the user, may be NULL at any given point.
		// Used primarily for the auto-quit behaviour (emitting quit event after last/primary event source is destroyed).
		EventSource * primaryEventSource = nullptr;

		// Container for all dispatchers created in the system.
		std::vector<EventDispatcher *> eventDispatcherList;

		// List of all registered event sources, i.e. all windows/surfaces/views currently observed by our event system.
		std::vector<EventSource *> eventSourceList;

		// Pointer to the configuration data from currently bound dispatcher.
		EventDispatcherInputState * currentDispatcherInputState = nullptr;

		// Pointer to the configuration data from currently bound dispatcher.
		const EventDispatcherConfig * currentDispatcherConfig = nullptr;

		Bitmask<uint32> internalContollerStateFlags = 0u;

		LocalEventQueue priorityEventQueue;

		LocalEventQueue userEventQueue;

		TS3_FUNC_NO_DISCARD std::pair<bool, std::vector<EventDispatcher *>::iterator> findEventDispatcherInternal( EventDispatcher * pEventDispatcher )
		{
			std::pair<bool, std::vector<EventDispatcher *>::iterator> result;
			result.second = std::find( eventDispatcherList.begin(), eventDispatcherList.end(), pEventDispatcher );
			result.first = ( result.second != eventDispatcherList.end() );
			return result;
		}

		TS3_FUNC_NO_DISCARD std::pair<bool, std::vector<EventSource *>::iterator> findEventSourceInternal( EventSource * pEventSource )
		{
			std::pair<bool, std::vector<EventSource *>::iterator> result;
			result.second = std::find( eventSourceList.begin(), eventSourceList.end(), pEventSource );
			result.first = ( result.second != eventSourceList.end() );
			return result;
		}

		TS3_FUNC_NO_DISCARD const EventDispatcherConfig & getCurrentDispatcherConfig() const
		{
			ts3DebugAssert( currentDispatcherConfig != nullptr );
			return *currentDispatcherConfig;
		}

		TS3_FUNC_NO_DISCARD EventDispatcherInputState & getCurrentDispatcherInputState() const
		{
			ts3DebugAssert( currentDispatcherInputState != nullptr );
			return *currentDispatcherInputState;
		}
	};

	/// @brief Private, implementation-specific data of the EventDispatcher class.
	struct EventDispatcher::EventDispatcherPrivateData
	{
		// Internal configuration of the event system. The configuration is stored per-dispatcher, so that in case
		// of multiple instances, configuration is properly restored each time a dispatcher is set as an active one.
		EventDispatcherConfig evtDispatcherConfig;

		// Shared state
		EventDispatcherInputState evtDispatcherInputState;

		// A default handler. If set, it is called if there is no handler registered for a given code/category/base type.
		EventHandler defaultHandler;

		// Array of handlers registered for EventBaseType.
		std::array<EventHandler, CX_ENUM_EVENT_BASE_TYPE_COUNT> handlerMapByBaseType;

		// Array of handlers registered for EventCategory.
		std::array<EventHandler, CX_ENUM_EVENT_CATEGORY_COUNT> handlerMapByCategory;

		// Array of handlers registered for EventCodeIndex (i.e. event code itself).
		std::array<EventHandler, CX_ENUM_EVENT_CODE_INDEX_COUNT> handlerMapByCodeIndex;
	};

	namespace platform
	{

		struct NativeEventType;

		/// @brief Translates a native, OS-specific event into an internal representation.
		/// This is an internal function, implemented at the OS API level. Technically, it could be purely OS-specific,
		/// but having it here gives us the ability to write the dispatching helper function below and avoid duplicating.
		TS3_SYSTEM_API bool nativeEventTranslate( EventController & pEventController,
		                                          const NativeEventType & pNativeEvent,
		                                          EventObject & pOutEvent );

		/// @brief Helper function for translating and dispatching a native event.
		inline bool nativeEventDispatch( EventController & pEventController, const NativeEventType & pNativeEvent )
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

	}

} // namespace ts3::system

#endif // __TS3_SYSTEM_INTERNAL_EVENT_CORE_PRIVATE_H__
