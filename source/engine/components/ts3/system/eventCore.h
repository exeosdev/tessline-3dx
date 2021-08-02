
#ifndef __TS3_SYSTEM_EVENT_CORE_H__
#define __TS3_SYSTEM_EVENT_CORE_H__

#include "eventObject.h"
#include "systemContext.h"
#include <ts3/math/vector.h>

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_ANDROID )
#  include "internal/platform/osapi/android/androidEventCore.h"
#elif( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_WIN32 )
#  include "internal/platform/osapi/win32/win32EventCore.h"
#elif( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_X11 )
#  include "internal/platform/osapi/x11/x11EventCore.h"
#endif

namespace ts3
{
namespace system
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

	class EventSource
	{
		friend class EventController;

	public:
		EventSourceNativeData & mEvtSrcNativeData;

		explicit EventSource( EventSourceNativeData & pEvtSrcNativeData ) noexcept
		: mEvtSrcNativeData( pEvtSrcNativeData )
		{}
	};

	class EventController : public BaseObject
	{
		friend class EventDispatcher;

	public:
		explicit EventController( ContextHandle pContext ) noexcept;
		virtual ~EventController() noexcept;

		static EventControllerHandle create( ContextHandle pContext );

		EventDispatcherHandle createEventDispatcher();

		void dispatchEvent( Event & pEvent );

		void processEvent();
		void processQueue();
		void dispatchNextEvent();
		void dispatchNextEventWait();
		void dispatchQueuedEvents();
		void dispatchQueuedEventsWait();

		void addEventSource( EventSource & pEventSource );
		void removeEventSource( EventSource & pEventSource );

		void setActiveDispatcher( EventDispatcher & pDispatcher );
		void resetActiveDispatcher();

		TS3_PCL_ATTR_NO_DISCARD EventDispatcher & getActiveDispatcher() const;
		TS3_PCL_ATTR_NO_DISCARD EventDispatcher & getDefaultDispatcher() const;

	private:
		void _sysInitialize();
		void _sysRelease();
		void _sysAddEventSource( EventSource & pEventSource );
		void _sysRemoveEventSource( EventSource & pEventSource );
		void _sysDispatchNextEvent();
		void _sysDispatchNextEventWait();
		void _sysDispatchQueuedEvents();
		void _sysDispatchQueuedEventsWait();

	private:
		//
		EventDispatcherHandle _defaultDispatcher;
		//
		EventDispatcher * _activeDispatcher;
	};

	class EventDispatcher : public BaseObject
	{
		friend class EventController;

	public:
		EventControllerHandle const mEventController;

		EventDispatcher( EventControllerHandle pEventController ) noexcept;
		~EventDispatcher() noexcept;

		static EventDispatcherHandle create( EventControllerHandle pEventController );

		void bindEventHandler( EEventBaseType pBaseType, EventHandler pHandler );
		void bindEventHandler( EEventCategory pCategory, EventHandler pHandler );
		void bindEventHandler( EEventCodeIndex pCodeIndex, EventHandler pHandler );
		void bindDefaultEventHandler( EventHandler pHandler );

		void postEvent( Event pEvent );
		void postSimpleEvent( event_code_value_t pEventCode );
		void postEventAppQuit();
		void postEventAppTerminate();

		void setIdleProcessingMode( bool pIdle );

		bool checkIdleProcessingMode() const;

		EventInputState & getInputState();

	private:
		void internalPutEvent( Event & pEvent );

		void _preProcessEvent( Event & pEvent );
		void _putEvent( Event & pEvent );

	private:
		using EventBaseTypeHandlerMap = std::array<EventHandler, cvEnumEventBaseTypeCount>;
		using EventCategoryHandlerMap = std::array<EventHandler, cvEnumEventCategoryCount>;
		using EventCodeIndexHandlerMap = std::array<EventHandler, cvEnumEventCodeIndexCount>;

		//
		EventInputState _inputState;
		//
		EventHandler _defaultHandler;
		// Array of handlers registered for EventBaseType.
		EventBaseTypeHandlerMap _handlerMapByBaseType;
		// Array of handlers registered for EventCategory.
		EventCategoryHandlerMap _handlerMapByCategory;
		// Array of handlers registered for EventCodeIndex (i.e. event code itself).
		EventCodeIndexHandlerMap _handlerMapByCodeIndex;
	};

} // namespace system
} // namespace ts3

#endif // __TS3_SYSTEM_EVENT_CORE_H__
