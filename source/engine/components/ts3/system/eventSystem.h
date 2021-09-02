
#ifndef __TS3_SYSTEM_EVENT_SYSTEM_H__
#define __TS3_SYSTEM_EVENT_SYSTEM_H__

#include "eventCommon.h"
#include "sysObject.h"
#include <ts3/math/vector.h>

namespace ts3::system
{

    using event_dispatcher_id_t = native_uint;

    constexpr event_dispatcher_id_t CX_EVENT_DISPATCHER_ID_AUTO = 0u;

    constexpr event_dispatcher_id_t CX_EVENT_DISPATCHER_ID_DEFAULT = Limits<event_dispatcher_id_t>::maxValue;

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

		void dispatchEvent( EventObject pEvent );

		uint32 dispatchSysEventAuto();
		uint32 dispatchSysEventPeek( uint32 pLimit = CX_INT32_MAX );
		uint32 dispatchSysEventWait( uint32 pLimit = CX_INT32_MAX );

		bool setActiveDispatcher( EventDispatcher & pDispatcher );
		bool resetActiveDispatcher();

		EventDispatcher * createEventDispatcher( event_dispatcher_id_t pDispatcherID );

		EventDispatcher * getEventDispatcher( event_dispatcher_id_t pDispatcherID );

	private:
	    void _onActiveDispatcherChange( EventDispatcher * pDispatcher );

	    bool _nativeDispatchNext();
	    bool _nativeDispatchNextWait();
	    void _nativeOnActiveDispatcherChange( EventDispatcher * pDispatcher );
	};

	class EventDispatcher
	{
		friend class EventController;

	public:
	    struct ObjectPrivateData;
		EventController * const mEventController;
		std::unique_ptr<ObjectPrivateData> const mPrivate;
		event_dispatcher_id_t const mID;

		EventDispatcher( EventController * pEventController,
                         event_dispatcher_id_t pID );

		~EventDispatcher() noexcept;

		void bindEventHandler( EEventBaseType pBaseType, EventHandler pHandler );
		void bindEventHandler( EEventCategory pCategory, EventHandler pHandler );
		void bindEventHandler( EEventCodeIndex pCodeIndex, EventHandler pHandler );
		void bindDefaultEventHandler( EventHandler pHandler );

		void resetHandlers();

		void setIdleProcessingMode( bool pIdle );

		void postEvent( EventObject pEvent );
		void postEvent( event_code_value_t pEventCode );
		void postEventAppQuit();
		void postEventAppTerminate();

	private:
		void _preProcessEvent( EventObject & pEvent );
		void _putEvent( EventObject & pEvent );
	};

} // namespace ts3::system

#endif // __TS3_SYSTEM_EVENT_SYSTEM_H__
