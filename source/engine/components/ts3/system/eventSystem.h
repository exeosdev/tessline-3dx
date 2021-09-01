
#ifndef __TS3_SYSTEM_EVENT_SYSTEM_H__
#define __TS3_SYSTEM_EVENT_SYSTEM_H__

#include "eventCommon.h"
#include "sysObject.h"
#include <ts3/math/vector.h>

namespace ts3::system
{

    /// @brief
    enum EventControllerConfigFlagBits : uint32
    {
        E_EVENT_CONTROLLER_CONFIG_FLAG_ENABLE_MOUSE_DOUBLE_CLICK_BIT = 0x0001,
        E_EVENT_CONTROLLER_CONFIG_FLAG_ENABLE_MOUSE_MULTI_CLICK_BIT = 0x0002 | E_EVENT_CONTROLLER_CONFIG_FLAG_ENABLE_MOUSE_DOUBLE_CLICK_BIT,
        E_EVENT_CONTROLLER_CONFIG_FLAGS_DEFAULT = E_EVENT_CONTROLLER_CONFIG_FLAG_ENABLE_MOUSE_MULTI_CLICK_BIT
    };

    /// @brief
    struct EventControllerConfig
    {
        //
        Bitmask<EventControllerConfigFlagBits> flags = E_EVENT_CONTROLLER_CONFIG_FLAGS_DEFAULT;
        //
        event_code_value_t mouseClickSequenceTimeoutMs = 100;
    };

	class EventController : public SysObject
	{
	public:
	    struct ObjectPrivateData;
	    std::unique_ptr<ObjectPrivateData> const mPrivate;

	public:
	    explicit EventController( SysContextHandle pSysContext );
		virtual ~EventController() noexcept;

		EventDispatcher * createEventDispatcher();

		void dispatchEvent( Event & pEvent );

		void processEvent();
		void processQueue();
		void dispatchNextEvent();
		void dispatchNextEventWait();
		void dispatchQueuedEvents();
		void dispatchQueuedEventsWait();

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

} // namespace ts3::system

#endif // __TS3_SYSTEM_EVENT_SYSTEM_H__
