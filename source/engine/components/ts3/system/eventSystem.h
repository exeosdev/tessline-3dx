
#ifndef __TS3_SYSTEM_EVENT_SYSTEM_H__
#define __TS3_SYSTEM_EVENT_SYSTEM_H__

#include "eventCommon.h"
#include "sysObject.h"
#include <ts3/math/vector.h>
#include <functional>

namespace ts3::system
{

    ts3SysDeclareHandle( EventController );
    ts3SysDeclareHandle( EventDispatcher );

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
        friend class EventController;

    public:
        explicit EventSource( SysContextHandle pSysContext );

        virtual ~EventSource() noexcept;

        template <typename TpNativeData>
        const TpNativeData * getEventSourceNativeData() const
        {
            return static_cast<TpNativeData *>( _eventSourceNativeData );
        }

        void unregisterEventSourceAuto();

    friendapi: // EventController
        void setEventController( EventControllerHandle pEventController );

    protected:
        void setEventSourceNativeData( void * pData );

    private:
        // EventController which handles events generated by this source. A single EventSource may be registered
        // only in one controller at any given time.
        // NOTE: this is an active reference, so the controller is always accessible to a source object.
        EventControllerHandle _eventController = nullptr;

        // Pointer to a native data (EventSourceNativeData). Defined as void* because we don't bring any *Native*
        // definitions into public namespace. Each specific Source sub-class (a window, a surface, etc.) can have
        // its own way to provide this.
        void * _eventSourceNativeData = nullptr;
    };

    class EventDispatcher : public SysObject
    {
    public:
        struct ObjectInternalData;
        EventController * const mEventController = nullptr;
        std::unique_ptr<ObjectInternalData> const mInternal;
        event_dispatcher_id_t const mID;

        EventDispatcher( EventController * pEventController, event_dispatcher_id_t pID );

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

	class EventController : public SysObject
	{
	    friend class EventDispatcher;
	    friend class EventSource;

	public:
	    struct ObjectInternalData;
	    std::unique_ptr<ObjectInternalData> const mInternal;

	public:
	    explicit EventController( SysContextHandle pSysContext );
		virtual ~EventController() noexcept;

		void registerEventSource( EventSource & pEventSource );
		void unregisterEventSource( EventSource & pEventSource );

		bool dispatchEvent( EventObject pEvent );

		uint32 updateSysQueueAuto();
		uint32 updateSysQueuePeek( uint32 pLimit = CX_INT32_MAX );
		uint32 updateSysQueueWait( uint32 pLimit = CX_INT32_MAX );

		TS3_PCL_ATTR_NO_DISCARD EventDispatcher * createEventDispatcher( event_dispatcher_id_t pDispatcherID );

		TS3_PCL_ATTR_NO_DISCARD EventDispatcher * getEventDispatcher( event_dispatcher_id_t pDispatcherID );

		bool setActiveEventDispatcher( event_dispatcher_id_t pDispatcherID );

		bool setActiveEventDispatcher( EventDispatcher * pDispatcher );

		bool setActiveEventDispatcherDefault();

		bool resetActiveEventDispatcher();

		void releaseDispatcherObjects();

        TS3_PCL_ATTR_NO_DISCARD EventSource * findEventSource( EventSourceFindPredicate pPredicate ) const;

        TS3_PCL_ATTR_NO_DISCARD EventSource * findEventSource( EventSourceNativeDataFindPredicate pPredicate ) const;

        TS3_PCL_ATTR_NO_DISCARD bool isEventSourceRegistered( EventSource & pEventSource ) const;

		TS3_PCL_ATTR_NO_DISCARD bool hasActiveDispatcher() const;

	friendapi:
	    // Used by EventSource class. It is called inside its destructor.
	    void onEventSourceDestroy( EventSource & pEventSource ) noexcept;

	private:
	    void _checkActiveDispatcherState();
	    void _onActiveDispatcherChange( EventDispatcher * pDispatcher );
	    EventDispatcher * _getDefaultDispatcher();
	    EventDispatcher * _validateDispatcher( EventDispatcher * pDispatcher );
	    void _addEventSource( EventSource & pEventSource );
	    void _removeEventSource( EventSource & pEventSource );
	    void * _removeEventSourcePrivateData( EventSource & pEventSource );
	    void _setEventSourcePrivateData( EventSource & pEventSource, void * pData );
	    void * _getEventSourcePrivateData( EventSource & pEventSource ) const;

	    void _nativeRegisterEventSource( EventSource & pEventSource );
	    void _nativeUnregisterEventSource( EventSource & pEventSource );
	    void _nativeDestroyEventSourcePrivateData( EventSource & pEventSource, void * pData );
	    bool _nativeUpdateSysQueue();
	    bool _nativeUpdateSysQueueWait();
	    void _nativeOnActiveDispatcherChange( EventDispatcher * pDispatcher );
	};

} // namespace ts3::system

#endif // __TS3_SYSTEM_EVENT_SYSTEM_H__
