
#ifndef __TS3_SYSTEM_EVENT_SYSTEM_H__
#define __TS3_SYSTEM_EVENT_SYSTEM_H__

#include "eventCommon.h"
#include "sysObject.h"
#include <ts3/math/vector.h>
#include <functional>

namespace ts3::system
{

    struct EventSourceNativeData;

    using event_dispatcher_id_t = native_uint;

    using EventSourceFindPredicate = std::function<bool( const EventSource & )>;
    using EventSourceNativeDataFindPredicate = std::function<bool( const EventSourceNativeData & )>;

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

    class EventSource : public SysObject
    {
    public:
        EventController * mEventController = nullptr;

    public:
        explicit EventSource( SysContextHandle pSysContext );
        virtual ~EventSource();

        virtual const EventSourceNativeData * getEventSourceNativeData() const = 0;

        void unregisterEventSourceAuto();
    };

	class EventController : public SysObject
	{
	    friend class EventSource;

	public:
	    struct ObjectPrivateData;
	    std::unique_ptr<ObjectPrivateData> const mPrivate;

	public:
	    explicit EventController( SysContextHandle pSysContext );
		virtual ~EventController() noexcept;

		void registerEventSource( EventSource & pEventSource );
		void unregisterEventSource( EventSource & pEventSource );

		bool dispatchEvent( EventObject pEvent );

		uint32 updateSysQueueAuto();
		uint32 updateSysQueuePeek( uint32 pLimit = CX_INT32_MAX );
		uint32 updateSysQueueWait( uint32 pLimit = CX_INT32_MAX );

        bool setActiveDispatcher( EventDispatcher & pDispatcher );
        bool setDefaultActiveDispatcher();
		bool resetActiveDispatcher();

		TS3_PCL_ATTR_NO_DISCARD EventDispatcher * createEventDispatcher( event_dispatcher_id_t pDispatcherID );

		TS3_PCL_ATTR_NO_DISCARD EventDispatcher * getEventDispatcher( event_dispatcher_id_t pDispatcherID );

        TS3_PCL_ATTR_NO_DISCARD EventSource * findEventSource( EventSourceFindPredicate pPredicate ) const;

        TS3_PCL_ATTR_NO_DISCARD EventSource * findEventSource( EventSourceNativeDataFindPredicate pPredicate ) const;

        TS3_PCL_ATTR_NO_DISCARD bool isEventSourceRegistered( EventSource & pEventSource ) const;

		TS3_PCL_ATTR_NO_DISCARD bool hasActiveDispatcher() const;

	friendapi: // EventSource
	    void onEventSourceDestroy( EventSource & pEventSource );

	private:
	    void _checkActiveDispatcherState();
	    void _onActiveDispatcherChange( EventDispatcher * pDispatcher );
	    void _setEventSourcePrivateData( EventSource & pEventSource, void * pData );
	    void * _getEventSourcePrivateData( EventSource & pEventSource ) const;

	    void _nativeRegisterEventSource( EventSource & pEventSource );
	    void _nativeUnregisterEventSource( EventSource & pEventSource );
	    void _nativeDestroyEventSourcePrivateData( EventSource & pEventSource, void * pData );
	    bool _nativeUpdateSysQueue();
	    bool _nativeUpdateSysQueueWait();
	    void _nativeOnActiveDispatcherChange( EventDispatcher * pDispatcher );
	};

	class EventDispatcher
	{
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

		bool postEvent( EventObject pEvent );
		bool postEvent( event_code_value_t pEventCode );
        bool postEventAppQuit();
        bool postEventAppTerminate();

	private:
		void _preProcessEvent( EventObject & pEvent );
		void _putEvent( EventObject & pEvent );
	};

} // namespace ts3::system

#endif // __TS3_SYSTEM_EVENT_SYSTEM_H__
