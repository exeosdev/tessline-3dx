
#ifndef __TS3_SYSTEM_EVENT_CORE_H__
#define __TS3_SYSTEM_EVENT_CORE_H__

#include "eventSource.h"
#include <functional>

namespace ts3::system
{

	ts3SysDeclareHandle( EventController );
	ts3SysDeclareHandle( EventDispatcher );

	struct EventDispatcherConfig;
	struct EventDispatcherInputState;

	using EventSourceFindPredicate = std::function<bool( const EventSource & )>;
	using EventSourceNativeDataFindPredicate = std::function<bool( const void * )>;

	/// @brief A set of flags which modify the behaviour of the event system. Used in EventController::setEventSystemConfigFlags().
	enum EEventSystemConfigFlags : uint32
	{
		// If set, an AppQuit event will be posted after the last event source is destroyed.
		E_EVENT_SYSTEM_CONFIG_FLAG_ENABLE_QUIT_ON_LAST_SOURCE_DESTROY_BIT = 0x10,

		// If set, an AppQuit event will be posted after the primary event source is destroyed.
		// See EventController::registerPrimaryEventSource() and EventController::setPrimaryEventSource().
		E_EVENT_SYSTEM_CONFIG_FLAG_ENABLE_QUIT_ON_PRIMARY_SOURCE_DESTROY_BIT = 0x20,

		//
		E_EVENT_SYSTEM_CONFIG_MASK_ENABLE_AUTO_QUIT = 0x30,

		// The flags below control what approach is taken to handle automatic app quit
		// in case one of the situations above happens (last source/primary source is gone).

		// When an auto-quit is triggerred, the even is immediately dispatched to the target dispatcher and executed.
		// Since the whole logic happens during event pre-processing, this flag essentially causes the event source's
		// "Close" event to be replaced with the "AppQuit". This is a very aggressive setup and should be used carefully.
		E_EVENT_SYSTEM_CONFIG_FLAG_SET_AUTO_QUIT_MODE_DIRECT_EVENT_DISPATCH_BIT = 0x0100,

		// When an auto-quit is triggerred, the AppQuit event is pushed to the front of the priority event queue.
		// That means it will be the next event dispatched. The event source's "Close" event is delivered as usual.
		E_EVENT_SYSTEM_CONFIG_FLAG_SET_AUTO_QUIT_MODE_POST_PRIORITY_EVENT_BIT = 0x0200,

		// When an auto-quit is triggerred, the AppQuit event is pushed to the back of the priority event queue.
		// That means it will MOST LIKELY be the next event dispatched, UNLESS the user code heavily uses the priority
		// queue and there are other events already in it. The event source's "Close" event is delivered as usual.
		E_EVENT_SYSTEM_CONFIG_FLAG_SET_AUTO_QUIT_MODE_POST_USER_EVENT_BIT = 0x0400,

		// When an auto-quit is triggerred, the AppQuit event is not triggerred immediately. Instead, an internal flag
		// is set instead. The controller keeps fetching the priority events and peeking the system queue, but no new
		// events are allowed. When all pending events are processed, system automatically generates the AppQuit event.
		E_EVENT_SYSTEM_CONFIG_FLAG_SET_AUTO_QUIT_MODE_SET_INTERNAL_REQUEST_ONLY_BIT = 0x0800,

		//
		E_EVENT_SYSTEM_CONFIG_MASK_SET_AUTO_QUIT_MODE = 0x0F00,

		E_EVENT_SYSTEM_CONFIG_MASK_ALL = 0xFFFF
	};
	
	enum EEventDispatcherConfigFlags : uint32
	{
		E_EVENT_DISPATCHER_CONFIG_FLAG_ENABLE_MOUSE_DOUBLE_CLICK_BIT = 0x0001,

		E_EVENT_DISPATCHER_CONFIG_FLAG_ENABLE_MOUSE_MULTI_CLICK_BIT = 0x0002 | E_EVENT_DISPATCHER_CONFIG_FLAG_ENABLE_MOUSE_DOUBLE_CLICK_BIT,

		E_EVENT_DISPATCHER_CONFIG_FLAG_IDLE_PROCESSING_MODE_BIT = 0x0004
	};

	/// @brief
	class EventController : public SysObject
	{
		friend class EventDispatcher;
		friend class EventSource;

	public:
		explicit EventController( SysContextHandle pSysContext );
		virtual ~EventController() noexcept;

		TS3_FUNC_NO_DISCARD EventDispatcherHandle createEventDispatcher();

		bool setActiveEventDispatcher( EventDispatcher & pEventDispatcher );

		bool resetActiveEventDispatcher();

		void registerEventSource( EventSource & pEventSource );

		bool registerPrimaryEventSource( EventSource & pEventSource );

		void unregisterEventSource( EventSource & pEventSource );

		void setPrimaryEventSource( EventSource * pEventSource );

		bool dispatchEvent( EventObject pEvent );

		void pushUserEvent( EventObject pEvent );

		EventObject & emplaceUserEvent();

		void pushPriorityEvent( EventObject pEvent );

		EventObject & emplacePriorityEvent();

		uint32 processEventsAuto();

		uint32 processEventsPeek( uint32 pLimit = CX_INT32_MAX );

		uint32 processEventsWait( uint32 pLimit = CX_INT32_MAX );

		void setEventSystemConfigFlags( Bitmask<EEventSystemConfigFlags> pFlags, bool pSetOrUnset = true );

		void validateActiveDispatcherState() const;

		TS3_FUNC_NO_DISCARD bool checkSystemConfigFlags( Bitmask<EEventSystemConfigFlags> pFlags ) const;

		TS3_FUNC_NO_DISCARD EventSource * findEventSource( const EventSourceFindPredicate & pPredicate ) const;

		TS3_FUNC_NO_DISCARD EventSource * findEventSource( const EventSourceNativeDataFindPredicate & pPredicate ) const;

		TS3_FUNC_NO_DISCARD bool checkActiveDispatcherState() const noexcept;

		TS3_FUNC_NO_DISCARD size_t getRegisteredEventSourcesNum() const noexcept;

		TS3_FUNC_NO_DISCARD EventSource * getPrimaryEventSource() const noexcept;

		TS3_FUNC_NO_DISCARD EventSource * getRegisteredEventSourceByIndex( size_t pIndex ) const noexcept;

		TS3_FUNC_NO_DISCARD bool isEventSourceRegistered( const EventSource & pEventSource ) const noexcept;

	protected:
		EventDispatcherInputState & getEventDispatcherInputState();

		const EventDispatcherConfig & getEventDispatcherConfig() const;

	friendapi:
		// Used by the EventSource class. It is called inside its destructor.
		void onEventSourceDestroy( EventSource & pEventSource ) noexcept;

		// Used by the EventDispatcher class. It is called inside its destructor.
		void onEventDispatcherDestroy( EventDispatcher & pEventDispatcher ) noexcept;

	private:
		// System-level call. Pulls events from the system queue.
		// If the queue is empty, returns immediately.
		virtual bool _nativeUpdateSysQueue() = 0;

		// System-level call. Pulls events from the system queue.
		// If the queue is empty, waits until an event arrives (infinite timeout).
		virtual bool _nativeUpdateSysQueueWait() = 0;

		// System-level call. Pulls events from the system queue.
		// If the queue is empty, waits until an event arrives or the specified timeout occurs.
		// A separate function because on some OSes timeout-based event fetching may be tricky (yes, you again, Win32).
		virtual bool _nativeUpdateSysQueueWaitTimeout( const Microseconds & pTimeout ) { return false; }

		// Registers the event source at the system level. Empty for most OSes.
		// Win32: replaces WNDPROC with a custom one and allocates extra window data.
		virtual void _nativeRegisterEventSource( EventSource & pEventSource );

		// Unregisters the event source at the system level. Empty for most OSes.
		// Win32: restores the original (default) WNDPROC and releases the allocated data.
		virtual void _nativeUnregisterEventSource( EventSource & pEventSource );

		// Private utility function. Handles internal details of replacing active dispatcher with another one (or NULL).
		void _onActiveDispatcherChange( EventDispatcher * pEventDispatcher );

		// Private utility function. Pulls events from the priority queue, which comes before the system one.
		bool _processLocalQueues();

		// Private utility function. Sends an "AppQuit" event as a reaction to removal of the last/priority event source.
		// See setEventSystemConfigFlags() and EEventSystemConfigFlags::E_EVENT_SYSTEM_CONFIG_FLAG_ENABLE_QUIT_ON_xxx.
		bool _checkAndPostAppAutoQuitEvent( EEventCode pEvent, EventSource & pEventSource );

		//
		void _setInternalStateFlags( Bitmask<uint32> pFlags, bool pSetOrUnset );

		//
		TS3_FUNC_NO_DISCARD uint32 _checkInternalStateFlags( Bitmask<uint32> pFlags ) const;

		//
		TS3_FUNC_NO_DISCARD Bitmask<uint32> _getInternalStateFlags() const;

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

		void setDispatcherConfigFlags( Bitmask<EEventDispatcherConfigFlags> pFlags, bool pSetOrUnset = true );
		void setIdleProcessingMode( bool pIdle );

		bool postEvent( EventObject pEvent );
		bool postEvent( event_code_value_t pEventCode );
		bool postEventAppQuit();
		bool postEventAppTerminate();

	friendapi:
		EventDispatcherInputState & getInputState();

		const EventDispatcherConfig & getConfig() const;

	private:
		void _preProcessEvent( EventObject & pEvent );

	protected:
		struct EventDispatcherPrivateData;
		EventControllerHandle _eventControllerActiveRef;
		std::unique_ptr<EventDispatcherPrivateData> _privateData;
	};

} // namespace ts3::system

#endif // __TS3_SYSTEM_EVENT_CORE_H__
