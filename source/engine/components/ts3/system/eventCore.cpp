
#include "internal/eventCorePrivate.h"
#include <ts3/math/vectorOps.h>

namespace ts3::system
{

	EventController::EventController( system::SysContextHandle pSysContext )
	: SysObject( std::move( pSysContext ) )
	, _privateData( std::make_unique<EventControllerPrivateData>() )
	{}

	EventController::~EventController() noexcept = default;

	EventDispatcherHandle EventController::createEventDispatcher()
	{
		auto dispatcher = createSysObject<EventDispatcher>( getHandle<EventController>() );

		_privateData->eventDispatcherList.push_back( dispatcher.get() );

		return dispatcher;
	}

	void EventController::registerEventSource( EventSource & pEventSource )
	{
		_nativeRegisterEventSource( pEventSource );

		auto eventSourceRef = _privateData->findEventSourceInternal( &pEventSource );
		if( eventSourceRef.first )
		{
			ts3Throw( E_EXC_DEBUG_PLACEHOLDER );
		}

		_privateData->eventSourceList.push_back( &pEventSource );

		pEventSource.setEventController( getHandle<EventController>() );
	}

	bool EventController::registerPrimaryEventSource( EventSource & pEventSource )
	{
		if( _privateData->primaryEventSource )
		{
			// If the primary event source has been already set, we abort the process.
			// This is an all-or-nothing transaction - there should be no automatic overwriting here.
			return false;
		}

		_privateData->primaryEventSource = &pEventSource;

		registerEventSource( pEventSource );

		return true;
	}

	void EventController::unregisterEventSource( EventSource & pEventSource )
	{
		auto eventSourceRef = _privateData->findEventSourceInternal( &pEventSource );
		if( !eventSourceRef.first )
		{
			ts3DebugInterrupt();
			return;
		}

		_privateData->eventSourceList.erase( eventSourceRef.second );

		_nativeUnregisterEventSource( pEventSource );

		if( &pEventSource == _privateData->primaryEventSource )
		{
			_privateData->primaryEventSource = nullptr;
		}

		pEventSource.setEventController( nullptr );
	}

	void EventController::setPrimaryEventSource( EventSource * pEventSource )
	{
		if( pEventSource )
		{
			// If the specified event source is not NULL, it should be a valid object,
			// already registered within this event controller. Check if that is the case.
			auto eventSourceRef = _privateData->findEventSourceInternal( pEventSource );

			if( !eventSourceRef.first )
			{
				// Unknown event source. Most likely it has not been registered yet.
				ts3Throw( E_EXC_DEBUG_PLACEHOLDER );
			}
		}

		_privateData->primaryEventSource = pEventSource;
	}

	bool EventController::setActiveEventDispatcher( EventDispatcher & pEventDispatcher )
	{
		if( &pEventDispatcher != _privateData->activeEventDispatcher )
		{
			_onActiveDispatcherChange( &pEventDispatcher );
			return true;
		}
		return false;
	}

	bool EventController::resetActiveEventDispatcher()
	{
		if( _privateData->activeEventDispatcher  )
		{
			_onActiveDispatcherChange( nullptr );
			return true;
		}
		return false;
	}

	bool EventController::dispatchEvent( EventObject pEvent )
	{
		validateActiveDispatcherState();

		const auto dispatchResult = _privateData->activeEventDispatcher->postEvent( pEvent );

		if( pEvent.code == E_EVENT_CODE_WINDOW_UPDATE_DESTROY )
		{
			//
			if( auto * eventSource = pEvent.eWindowUpdateDestroy.eventSource )
			{
				//
				_checkAndPostAppAutoQuitEvent( E_EVENT_CODE_WINDOW_UPDATE_DESTROY, *eventSource );
			}
		}

		return dispatchResult;
	}

	void EventController::pushUserEvent( EventObject pEvent )
	{
		_privateData->userEventQueue.push_back(  pEvent );
	}

	EventObject & EventController::emplaceUserEvent()
	{
		return _privateData->userEventQueue.emplace_back();
	}

	void EventController::pushPriorityEvent( EventObject pEvent )
	{
		_privateData->priorityEventQueue.push_back(  pEvent );
	}

	EventObject & EventController::emplacePriorityEvent()
	{
		return _privateData->priorityEventQueue.emplace_back();
	}

	uint32 EventController::dispatchPendingEventsAuto()
	{
		validateActiveDispatcherState();

		uint32 eventCounter = 0;

		while( _processLocalQueues() )
		{
			++eventCounter;
		}

		while( _nativeDispatchPendingEvents() )
		{
			++eventCounter;
		}

		if( eventCounter == 0 )
		{
			if( checkEventSystemConfigFlags( E_EVENT_SYSTEM_CONFIG_FLAG_IDLE_PROCESSING_MODE_BIT ) )
			{
				_nativeDispatchPendingEventsWait();
			}
		}

		return eventCounter;
	}

	uint32 EventController::dispatchPendingEventsPeek( uint32 pLimit )
	{
		validateActiveDispatcherState();

		uint32 eventCounter = 0;

		while( _processLocalQueues() && ( eventCounter <= pLimit ) )
		{
			++eventCounter;
		}

		while( _nativeDispatchPendingEvents() && ( eventCounter <= pLimit ) )
		{
			++eventCounter;
		}

		return eventCounter;
	}

	uint32 EventController::dispatchPendingEventsWait( uint32 pLimit )
	{
		validateActiveDispatcherState();

		uint32 eventCounter = 0;

		while( _processLocalQueues() && ( eventCounter <= pLimit ) )
		{
			++eventCounter;
		}

		while( _nativeDispatchPendingEvents() && ( eventCounter <= pLimit ) )
		{
			++eventCounter;
		}

		if( eventCounter == 0 )
		{
			_nativeDispatchPendingEventsWait();
		}

		return eventCounter;
	}

	uint32 EventController::dispatchPendingEventsWaitTimeout( const Microseconds & pTimeout, uint32 pLimit )
	{
		validateActiveDispatcherState();

		uint32 eventCounter = 0;

		while( _processLocalQueues() && ( eventCounter <= pLimit ) )
		{
			++eventCounter;
		}

		while( _nativeDispatchPendingEvents() && ( eventCounter <= pLimit ) )
		{
			++eventCounter;
		}

		if( eventCounter == 0 )
		{
			_nativeDispatchPendingEventsWaitTimeout( pTimeout );
		}

		return eventCounter;
	}

	void EventController::validateActiveDispatcherState() const
	{
		if( !isActiveDispatcherSet() )
		{
			ts3Throw( E_EXC_DEBUG_PLACEHOLDER );
		}
	}

	EventSystemSharedState & EventController::getEventSystemSharedState() noexcept
	{
		return _privateData->sharedEventSystemState;
	}

	bool EventController::checkEventSystemConfigFlags( Bitmask<EEventSystemConfigFlags> pFlags ) const
	{
		if( !isActiveDispatcherSet() )
		{
			return false;
		}
		return _privateData->activeEventDispatcher->checkEventSystemConfigFlags( pFlags );
	}

	EventSource * EventController::findEventSource( const EventSourceFindPredicate & pPredicate ) const
	{
		for( auto * eventSource : _privateData->eventSourceList )
		{
			if( eventSource && pPredicate( *eventSource ) )
			{
				return eventSource;
			}
		}
		return nullptr;
	}

	EventSource * EventController::findEventSource( const EventSourceNativeDataFindPredicate & pPredicate ) const
	{
		for( auto * eventSource : _privateData->eventSourceList )
		{
			if( pPredicate( eventSource->getEventSourceNativeData() ) )
			{
				return eventSource;
			}
		}
		return nullptr;
	}

	bool EventController::isActiveDispatcherSet() const noexcept
	{
		if( !_privateData->activeEventDispatcher )
		{
			// No active dispatcher set. This basically makes the event system useless, as no events
			// can be forwarded to the respective handlers. Since there can be multiple dispatchers
			// switched at runtime according to needs, this is a crucial check to make.

			return false;
		}

		if( !_privateData->sharedEventSystemState.currentEventSystemConfig )
		{
			// Missing per-dispatcher data. Configuration and input state belong to a dispatcher,
			// so the system can properly adjust when they are changed.
			// Currently, this situation is rather impossible to occur (config and inputState are
			// stored by value and set/cleared inside _onActiveDispatcherChange()), but if we ever
			// change this, this check will help to catch potential issues much faster.

			return false;
		}

		return true;
	}

	size_t EventController::getRegisteredEventSourcesNum() const noexcept
	{
		return _privateData->eventSourceList.size();
	}

	EventSource * EventController::getPrimaryEventSource() const noexcept
	{
		return _privateData->primaryEventSource;
	}

	EventSource * EventController::getRegisteredEventSourceByIndex( size_t pIndex ) const noexcept
	{
		return _privateData->eventSourceList[pIndex];
	}

	bool EventController::isEventSourceRegistered( const EventSource & pEventSource ) const noexcept
	{
		for( auto * eventSource : _privateData->eventSourceList )
		{
			if( &pEventSource == eventSource )
			{
				return true;
			}
		}
		return false;
	}

	void EventController::onEventSourceDestroy( EventSource & pEventSource ) noexcept
	{
		unregisterEventSource( pEventSource );
	}

	void EventController::onEventDispatcherDestroy( EventDispatcher & pEventDispatcher ) noexcept
	{
		auto eventDispatcherRef = _privateData->findEventDispatcherInternal( &pEventDispatcher );

		if( !eventDispatcherRef.first )
		{
			ts3DebugInterrupt();
			return;
		}

		_privateData->eventDispatcherList.erase( eventDispatcherRef.second );
	}

	void EventController::_nativeRegisterEventSource( EventSource & /* pEventSource */ )
	{}

	void EventController::_nativeUnregisterEventSource( EventSource & /* pEventSource */ )
	{}

	void EventController::_onActiveDispatcherChange( EventDispatcher * pEventDispatcher )
	{
		if( pEventDispatcher )
		{
			auto & eventSystemConfig = pEventDispatcher->getEventSystemConfig();
			_privateData->sharedEventSystemState.currentEventSystemConfig = &eventSystemConfig;
		}
		else
		{
			_privateData->sharedEventSystemState.currentEventSystemConfig = nullptr;
		}

		_privateData->activeEventDispatcher = pEventDispatcher;
	}

	bool EventController::_processLocalQueues()
	{
		auto & priorityQueue = _privateData->priorityEventQueue;

		if( !priorityQueue.empty() )
		{
			auto nextPriorityEvent = priorityQueue.front();
			priorityQueue.pop_front();

			if( dispatchEvent( nextPriorityEvent ) )
			{
				return true;
			}
		}

		auto & userQueue = _privateData->userEventQueue;

		if( !userQueue.empty() )
		{
			auto nextUserEvent = userQueue.front();
			userQueue.pop_front();

			if( dispatchEvent( nextUserEvent ) )
			{
				return true;
			}
		}

		return false;
	}

	bool EventController::_checkAndPostAppAutoQuitEvent( EEventCode pEvent, EventSource & pEventSource )
	{
		if( pEvent != E_EVENT_CODE_WINDOW_UPDATE_DESTROY )
		{
			return false;
		}

		bool postAutoQuit = false;

		if( pEventSource.isLastEventSource() )
		{
			if( checkEventSystemConfigFlags( E_EVENT_SYSTEM_CONFIG_FLAG_ENABLE_AUTO_QUIT_ON_LAST_SOURCE_DESTROY_BIT ) )
			{
				postAutoQuit = true;
			}
		}

		if( !postAutoQuit && pEventSource.isPrimaryEventSource() )
		{
			if( checkEventSystemConfigFlags( E_EVENT_SYSTEM_CONFIG_FLAG_ENABLE_AUTO_QUIT_ON_PRIMARY_SOURCE_DESTROY_BIT ) )
			{
				postAutoQuit = true;
			}
		}

		if( postAutoQuit )
		{
			auto internalStateMask = _getInternalStateFlags();

			// Dedicated mask for quick checking if any of the quit modes has been properly set.
			if( !internalStateMask.isSetAnyOf( E_EVENT_SYSTEM_CONFIG_MASK_SET_AUTO_QUIT_MODE ) )
			{
				// If not, we default to the one based on user message. This is probably the best one, as it allows
				// to still process pending important messages before breaking the whole event lopp.
				internalStateMask.set( E_EVENT_SYSTEM_CONFIG_FLAG_SET_AUTO_QUIT_MODE_POST_USER_EVENT_BIT );
			}

			if( internalStateMask.isSet( E_EVENT_SYSTEM_CONFIG_FLAG_SET_AUTO_QUIT_MODE_DIRECT_EVENT_DISPATCH_BIT ) )
			{
				// Dispatch event immediately. This basically causes the handler for the AppQuit to be executed here immediately.
				dispatchEvent( EventObject{ E_EVENT_CODE_APP_ACTIVITY_QUIT } );
			}
			else if( internalStateMask.isSet( E_EVENT_SYSTEM_CONFIG_FLAG_SET_AUTO_QUIT_MODE_POST_PRIORITY_EVENT_BIT ) )
			{
				// Push the AppQuit event to the end of the priority queue.
				pushPriorityEvent( EventObject{ E_EVENT_CODE_APP_ACTIVITY_QUIT } );
			}
			else if( internalStateMask.isSet( E_EVENT_SYSTEM_CONFIG_FLAG_SET_AUTO_QUIT_MODE_POST_USER_EVENT_BIT ) )
			{
				// Push the AppQuit event to the end of the user queue.
				pushUserEvent( EventObject{ E_EVENT_CODE_APP_ACTIVITY_QUIT } );
			}
			else if( internalStateMask.isSet( E_EVENT_SYSTEM_CONFIG_FLAG_SET_AUTO_QUIT_MODE_SET_INTERNAL_REQUEST_ONLY_BIT ) )
			{
				// Only set the internal flag to mark a quit request has been issued. The system will process it when there are no events.
				_setInternalStateFlags( E_EVENT_SYSTEM_INTERNAL_FLAG_APP_QUIT_REQUEST_SET_BIT, true );
			}
			else
			{
				ts3DebugInterrupt();
			}
		}

		return postAutoQuit;
	}

	void EventController::_setInternalStateFlags( Bitmask<uint32> pFlags, bool pSetOrUnset )
	{
		_privateData->sharedEventSystemState.internalStateFlags.setOrUnset( pFlags, pSetOrUnset );
	}

	uint32 EventController::_checkInternalStateFlags( Bitmask<uint32> pFlags ) const
	{
		return ( _privateData->sharedEventSystemState.internalStateFlags & pFlags );
	}

	Bitmask<uint32> EventController::_getInternalStateFlags() const
	{
		return _privateData->sharedEventSystemState.internalStateFlags;
	}


	EventDispatcher::EventDispatcher( EventControllerHandle pEventController )
	: SysObject( pEventController->mSysContext )
	, _eventControllerActiveRef( std::move( pEventController ) )
	, _privateData( std::make_unique<EventDispatcherPrivateData>() )
	{}

	EventDispatcher::~EventDispatcher() noexcept
	{
		if( _eventControllerActiveRef )
		{
			_eventControllerActiveRef->onEventDispatcherDestroy( *this );
			_eventControllerActiveRef = nullptr;
		}
	}

	void EventDispatcher::setActive()
	{
		_eventControllerActiveRef->setActiveEventDispatcher( *this );
	}

	void EventDispatcher::setEventHandler( EEventBaseType pBaseType, EventHandler pHandler )
	{
		// EventBaseType enum is numbered from zero to have constant access time to its entry.
		auto baseTypeValue = static_cast<size_t>( pBaseType );
		// Check for possible violation attempt.
		if( baseTypeValue >= CX_ENUM_EVENT_BASE_TYPE_COUNT )
		{
			ts3Throw( E_EXC_DEBUG_PLACEHOLDER );
		}
		_privateData->handlerMapByBaseType[baseTypeValue] = std::move( pHandler );
	}

	void EventDispatcher::setEventHandler( EEventCategory pCategory, EventHandler pHandler )
	{
		// EventCategory enum is numbered from zero to have constant access time to its entry.
		auto categoryValue = static_cast<size_t>( pCategory );
		// Check for possible violation attempt.
		if( categoryValue >= CX_ENUM_EVENT_CATEGORY_COUNT )
		{
			ts3Throw( E_EXC_DEBUG_PLACEHOLDER );
		}
		_privateData->handlerMapByCategory[categoryValue] = std::move( pHandler );
	}

	void EventDispatcher::setEventHandler( EEventCodeIndex pCodeIndex, EventHandler pHandler )
	{
		// EventCodeIndex enum is numbered from zero to have constant access time to its entry.
		auto codeIndexValue = static_cast<size_t>( pCodeIndex );
		// Check for possible violation attempt.
		if( codeIndexValue >= CX_ENUM_EVENT_CODE_INDEX_COUNT )
		{
			ts3Throw( E_EXC_DEBUG_PLACEHOLDER );
		}
		_privateData->handlerMapByCodeIndex[codeIndexValue] = std::move( pHandler );
	}

	void EventDispatcher::setDefaultEventHandler( EventHandler pHandler )
	{
		_privateData->defaultHandler = std::move( pHandler );
	}

	void EventDispatcher::resetEventHandler( EEventBaseType pBaseType )
	{
		setEventHandler( pBaseType, nullptr );
	}

	void EventDispatcher::resetEventHandler( EEventCategory pCategory )
	{
		setEventHandler( pCategory, nullptr );
	}

	void EventDispatcher::resetEventHandler( EEventCodeIndex pCodeIndex )
	{
		setEventHandler( pCodeIndex, nullptr );
	}

	void EventDispatcher::resetDefaultEventHandler()
	{
		setDefaultEventHandler( nullptr );
	}

	void EventDispatcher::setEventSystemConfigFlags( Bitmask<EEventSystemConfigFlags> pFlags, bool pSetOrUnset )
	{
		const auto configFlagsOnly = ( pFlags & E_EVENT_SYSTEM_CONFIG_MASK_ALL );
		ts3DebugAssert( configFlagsOnly == pFlags );
		_privateData->currentEventSystemConfig.configFlags.setOrUnset( pFlags, pSetOrUnset );
	}

	void EventDispatcher::setIdleProcessingMode( bool pIdle )
	{
		_privateData->currentEventSystemConfig.configFlags.setOrUnset( E_EVENT_SYSTEM_CONFIG_FLAG_IDLE_PROCESSING_MODE_BIT, pIdle );
	}

	bool EventDispatcher::postEvent( EventObject pEvent )
	{
		if( !ecValidateEventCode( pEvent.code ) )
		{
			return false;
		}

		// _preProcessEvent( pEvent );

		pEvent.commonData.timeStamp = PerfCounter::queryCurrentStamp();

		{
			auto codeIndexValue = static_cast<size_t>( ecGetEventCodeCodeIndex( pEvent.code ) );
			auto & eventHandler = _privateData->handlerMapByCodeIndex[codeIndexValue];
			if( eventHandler && eventHandler( pEvent ) )
			{
				return true;
			}
		}
		{
			auto categoryValue = static_cast<size_t>( ecGetEventCodeCategory( pEvent.code ) );
			auto & eventHandler = _privateData->handlerMapByCategory[categoryValue];
			if( eventHandler && eventHandler( pEvent ) )
			{
				return true;
			}
		}
		{
			auto baseTypeValue = static_cast<size_t>( ecGetEventCodeBaseType( pEvent.code ) );
			auto & eventHandler = _privateData->handlerMapByBaseType[baseTypeValue];
			if( eventHandler && eventHandler( pEvent ) )
			{
				return true;
			}
		}
		{
			auto & defaultHandler = _privateData->defaultHandler;
			if( defaultHandler && defaultHandler( pEvent ) )
			{
				return true;
			}
		}

		return false;
	}

	bool EventDispatcher::postEvent( event_code_value_t pEventCode )
	{
		return postEvent( EventObject( pEventCode ) );
	}

	bool EventDispatcher::postEventAppQuit()
	{
		return postEvent( E_EVENT_CODE_APP_ACTIVITY_QUIT );
	}

	bool EventDispatcher::postEventAppTerminate()
	{
		return postEvent( E_EVENT_CODE_APP_ACTIVITY_TERMINATE );
	}

	bool EventDispatcher::checkEventSystemConfigFlags( Bitmask<EEventSystemConfigFlags> pFlags ) const
	{
		const auto configFlagsOnly = ( pFlags & E_EVENT_SYSTEM_CONFIG_MASK_ALL );
		ts3DebugAssert( configFlagsOnly == pFlags );
		return _privateData->currentEventSystemConfig.configFlags.isSet( configFlagsOnly );
	}

	Bitmask<EEventSystemConfigFlags> EventDispatcher::getEventSystemConfigFlags() const
	{
		return _privateData->currentEventSystemConfig.configFlags;
	}

	const EventSystemConfig & EventDispatcher::getEventSystemConfig() const
	{
		return _privateData->currentEventSystemConfig;
	}

	void EventDispatcher::_preProcessEvent( EventObject & pEvent )
	{
		switch( pEvent.category() )
		{
			case EEventCategory::InputMouse:
			{
				//_preProcessInputMouseEvent( pEvent.eInputMouse );
				break;
			}
			case EEventCategory::InputKeyboard:
			{
				//_preProcessInputKeyboardEvent( pEvent.eInputKeyboard );
				break;
			}
			default:
			{
				break;
			}
		}
	}

	void evtUpdateEventInputMouse( EvtInputMouse & pMouseEvent, EventSystemSharedState & pEventSystemSharedState )
	{
		if( pEventSystemSharedState.inputMouseState.lastCursorPos == CX_EVENT_MOUSE_POS_INVALID )
		{
			pEventSystemSharedState.inputMouseState.lastCursorPos = pMouseEvent.cursorPos;
		}
	}

	void evtUpdateEventInputMouseButton( EvtInputMouseButton & pMouseButtonEvent, EventSystemSharedState & pEventSystemSharedState )
	{
		evtUpdateEventInputMouse( pMouseButtonEvent, pEventSystemSharedState );

		const auto & eventSystemConfig = pEventSystemSharedState.getEventSystemConfig();
		auto & inputMouseState = pEventSystemSharedState.inputMouseState;

		if( eventSystemConfig.configFlags.isSet( E_EVENT_SYSTEM_CONFIG_FLAG_ENABLE_MOUSE_MULTI_CLICK_BIT ) )
		{
			if( pMouseButtonEvent.buttonAction == EMouseButtonActionType::Click )
			{
				bool multiClickEventSet = false;

				if( inputMouseState.lastPressButtonID == pMouseButtonEvent.buttonID )
				{
					auto lastClickDiff = pMouseButtonEvent.timeStamp - inputMouseState.lastPressTimestamp;
					auto lastClickDiffMs = PerfCounter::convertToDuration<EDurationPeriod::Millisecond>( lastClickDiff );
					if( lastClickDiffMs <= eventSystemConfig.mouseClickSequenceTimeoutMs )
					{
						if( inputMouseState.currentMultiClickSequenceLength == 1 )
						{
							pMouseButtonEvent.buttonAction = EMouseButtonActionType::DoubleClick;
							inputMouseState.currentMultiClickSequenceLength = 2;
							multiClickEventSet = true;
						}
						else if( eventSystemConfig.configFlags.isSet( E_EVENT_SYSTEM_CONFIG_FLAG_ENABLE_MOUSE_MULTI_CLICK_BIT ) )
						{
							pMouseButtonEvent.buttonAction = EMouseButtonActionType::MultiClick;
							inputMouseState.currentMultiClickSequenceLength += 1;
							multiClickEventSet = true;
						}
					}
				}

				if( !multiClickEventSet )
				{
					// Multi-click has not been detected/updated. Possible reasons: different button ID,
					// click timeout (mouse button clicked too slow), multi-click support is disabled.
					inputMouseState.lastPressButtonID = pMouseButtonEvent.buttonID;
					inputMouseState.currentMultiClickSequenceLength = 1;
				}

				inputMouseState.lastPressTimestamp = pMouseButtonEvent.timeStamp;
			}
		}

		const auto buttonMask = ecGetMouseButtonFlagFromButtonID( pMouseButtonEvent.buttonID );
		if( pMouseButtonEvent.buttonAction == EMouseButtonActionType::Click )
		{
			pMouseButtonEvent.buttonStateMask.set( buttonMask );
		}
		else if( pMouseButtonEvent.buttonAction == EMouseButtonActionType::Release )
		{
			pMouseButtonEvent.buttonStateMask.unset( buttonMask );
		}
	}

	void evtUpdateEventInputMouseMove( EvtInputMouseMove & pMouseMoveEvent, EventSystemSharedState & pEventSystemSharedState )
	{
		evtUpdateEventInputMouse( pMouseMoveEvent, pEventSystemSharedState );

		auto & inputMouseState = pEventSystemSharedState.inputMouseState;

		pMouseMoveEvent.movementDelta = pMouseMoveEvent.cursorPos - inputMouseState.lastCursorPos;

		inputMouseState.lastCursorPos = pMouseMoveEvent.cursorPos;
	}

} // namespace ts3::system
