
#ifndef __TS3_SYSTEM_EVENT_CORE_H__
#define __TS3_SYSTEM_EVENT_CORE_H__

#include "eventSource.h"
#include <functional>

namespace ts3::system
{

    ts3SysDeclareHandle( EventController );
    ts3SysDeclareHandle( EventDispatcher );

    struct EventSystemInternalConfig;
    struct EventSystemSharedState;

    using EventSourceFindPredicate = std::function<bool( const EventSource & )>;
    using EventSourceNativeDataFindPredicate = std::function<bool( const void * )>;

    /// @brief
    enum EventControllerConfigFlagBits : uint32
    {
        E_EVENT_CONTROLLER_CONFIG_FLAG_ENABLE_MOUSE_DOUBLE_CLICK_BIT = 0x0001,
        E_EVENT_CONTROLLER_CONFIG_FLAG_ENABLE_MOUSE_MULTI_CLICK_BIT = 0x0002 | E_EVENT_CONTROLLER_CONFIG_FLAG_ENABLE_MOUSE_DOUBLE_CLICK_BIT,
        E_EVENT_CONTROLLER_CONFIG_FLAGS_DEFAULT = E_EVENT_CONTROLLER_CONFIG_FLAG_ENABLE_MOUSE_MULTI_CLICK_BIT
    };

    /// @brief
	class EventController : public SysObject
	{
	    friend class EventDispatcher;
	    friend class EventSource;

	public:
		virtual ~EventController() noexcept;

		TS3_FUNC_NO_DISCARD EventDispatcherHandle createEventDispatcher();

		bool setActiveEventDispatcher( EventDispatcher & pEventDispatcher );

		bool resetActiveEventDispatcher();

		void registerEventSource( EventSource & pEventSource );

		void unregisterEventSource( EventSource & pEventSource );

		bool dispatchEvent( EventObject pEvent );

		uint32 updateSysQueueAuto();

		uint32 updateSysQueuePeek( uint32 pLimit = CX_INT32_MAX );

		uint32 updateSysQueueWait( uint32 pLimit = CX_INT32_MAX );

		TS3_FUNC_NO_DISCARD EventSource * findEventSource( const EventSourceFindPredicate & pPredicate ) const;

		TS3_FUNC_NO_DISCARD EventSource * findEventSource( const EventSourceNativeDataFindPredicate & pPredicate ) const;

		TS3_FUNC_NO_DISCARD bool isEventSourceRegistered( EventSource & pEventSource ) const;

	protected:
	    explicit EventController( SysContextHandle pSysContext );

	    void setEventSourcePlatformData( EventSource & pEventSource, EventSourceInternalDataPtr pPlatformDataPtr );

	    void setEventSourcePlatformData( EventSource & pEventSource, void * pPlatformData, EventSourceInternalDataDeleter pDeleter = {} );

	    void resetEventSourcePlatformData( EventSource & pEventSource );

	    EventSystemSharedState & getEventSystemSharedState();

	    const EventSystemInternalConfig & getEventSystemInternalConfig() const;

	friendapi:
		// Used by the EventSource class. It is called inside its destructor.
		void onEventSourceDestroy( EventSource & pEventSource ) noexcept;

		// Used by the EventDispatcher class. It is called inside its destructor.
		void onEventDispatcherDestroy( EventDispatcher & pEventDispatcher ) noexcept;

	private:
	    ///
	    virtual std::shared_ptr<void> _nativeRegisterEventSource( EventSource & pEventSource ) = 0;

	    ///
	    virtual void _nativeUnregisterEventSource( EventSource & pEventSource ) = 0;

	    ///
	    virtual bool _nativeUpdateSysQueue() = 0;

	    ///
	    virtual bool _nativeUpdateSysQueueWait() = 0;

	    void _checkActiveDispatcherState();
	    void _onActiveDispatcherChange( EventDispatcher * pEventDispatcher );
	    void _addEventDispatcher( EventDispatcher & pEventDispatcher );
	    void _removeEventDispatcher( EventDispatcher & pEventDispatcher );
	    void _addEventSource( EventSource & pEventSource );
	    void _removeEventSource( EventSource & pEventSource );

	protected:
	    struct EventControllerPrivateData;
	    std::unique_ptr<EventControllerPrivateData> _privateData;
	};

	/// @brief
	class EventDispatcher : public SysObject
    {
	    friend class EventController;

    public:
        explicit EventDispatcher( EventControllerHandle pEventController );
        virtual ~EventDispatcher() noexcept;

        void setActive();

        void setEventHandler( EEventBaseType pBaseType, EventHandler pHandler );
        void setEventHandler( EEventCategory pCategory, EventHandler pHandler );
        void setEventHandler( EEventCodeIndex pCodeIndex, EventHandler pHandler );
        void setDefaultEventHandler( EventHandler pHandler );

        void resetEventHandler( EEventBaseType pBaseType );
        void resetEventHandler( EEventCategory pCategory );
        void resetEventHandler( EEventCodeIndex pCodeIndex );
        void resetDefaultEventHandler();

        void setIdleProcessingMode( bool pIdle );

        bool postEvent( EventObject pEvent );
        bool postEvent( event_code_value_t pEventCode );
        bool postEventAppQuit();
        bool postEventAppTerminate();

    friendapi:
        EventSystemSharedState & getEventSystemSharedState();
        const EventSystemInternalConfig & getEventSystemInternalConfig() const;

    private:
        void _preProcessEvent( EventObject & pEvent );

    protected:
        struct EventDispatcherPrivateData;
        EventControllerHandle _eventControllerActiveRef;
        std::unique_ptr<EventDispatcherPrivateData> _privateData;
    };

} // namespace ts3::system

#endif // __TS3_SYSTEM_EVENT_CORE_H__
