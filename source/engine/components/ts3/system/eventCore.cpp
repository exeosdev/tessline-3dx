
#include "internal/eventCorePrivate.h"

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
			auto & dispatcherConfig = _privateData->getCurrentDispatcherConfig();
			if( dispatcherConfig.dispatcherConfigFlags.isSet( E_EVENT_DISPATCHER_CONFIG_FLAG_IDLE_PROCESSING_MODE_BIT ) )
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

	void EventController::setEventSystemConfigFlags( Bitmask<EEventSystemConfigFlags> pFlags, bool pSetOrUnset )
	{
		const auto configFlagsOnly = ( pFlags & E_EVENT_SYSTEM_CONFIG_MASK_ALL );
		ts3DebugAssert( configFlagsOnly == pFlags );
		_setInternalStateFlags( configFlagsOnly, pSetOrUnset );
	}

	void EventController::validateActiveDispatcherState() const
	{
		if( !checkActiveDispatcherState() )
		{
			ts3Throw( E_EXC_DEBUG_PLACEHOLDER );
		}
	}

	bool EventController::checkSystemConfigFlags( Bitmask<EEventSystemConfigFlags> pFlags ) const
	{
		const auto configFlagsOnly = ( pFlags & E_EVENT_SYSTEM_CONFIG_MASK_ALL );
		ts3DebugAssert( configFlagsOnly == pFlags );
		return _checkInternalStateFlags( configFlagsOnly ) == configFlagsOnly;
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

	bool EventController::checkActiveDispatcherState() const noexcept
	{
		if( !_privateData->activeEventDispatcher )
		{
			// No active dispatcher set. This basically makes the event system useless, as no events
			// can be forwarded to the respective handlers. Since there can be multiple dispatchers
			// switched at runtime according to needs, this is a crucial check to make.

			return false;
		}

		if( !_privateData->currentDispatcherConfig || !_privateData->currentDispatcherInputState )
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

	EventDispatcherInputState & EventController::getEventDispatcherInputState()
	{
		ts3DebugAssert( _privateData->currentDispatcherInputState );
		return *( _privateData->currentDispatcherInputState );
	}

	const EventDispatcherConfig & EventController::getEventDispatcherConfig() const
	{
		ts3DebugAssert( _privateData->currentDispatcherConfig );
		return *( _privateData->currentDispatcherConfig );
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
			auto & dispatcherConfig = pEventDispatcher->getConfig();
			_privateData->currentDispatcherConfig = &dispatcherConfig;

			auto & dispatcherInputState = pEventDispatcher->getInputState();
			_privateData->currentDispatcherInputState = &dispatcherInputState;
		}
		else
		{
			_privateData->currentDispatcherConfig = nullptr;
			_privateData->currentDispatcherInputState = nullptr;
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
			if( checkSystemConfigFlags( E_EVENT_SYSTEM_CONFIG_FLAG_ENABLE_QUIT_ON_LAST_SOURCE_DESTROY_BIT ) )
			{
				postAutoQuit = true;
			}
		}

		if( !postAutoQuit && pEventSource.isPrimaryEventSource() )
		{
			if( checkSystemConfigFlags( E_EVENT_SYSTEM_CONFIG_FLAG_ENABLE_QUIT_ON_PRIMARY_SOURCE_DESTROY_BIT ) )
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
				// Only set the internal flag to mark a quit resuest has been issued. The system will process it when there are no events.
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
		_privateData->internalContollerStateFlags.setOrUnset( pFlags, pSetOrUnset );
	}

	uint32 EventController::_checkInternalStateFlags( Bitmask<uint32> pFlags ) const
	{
		return ( _privateData->internalContollerStateFlags & pFlags );
	}

	Bitmask<uint32> EventController::_getInternalStateFlags() const
	{
		return _privateData->internalContollerStateFlags;
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

	void EventDispatcher::setDispatcherConfigFlags( Bitmask<EEventDispatcherConfigFlags> pFlags, bool pSetOrUnset )
	{
		_privateData->evtDispatcherConfig.dispatcherConfigFlags.setOrUnset( pFlags, pSetOrUnset );
	}

	void EventDispatcher::setIdleProcessingMode( bool pIdle )
	{
		_privateData->evtDispatcherConfig.dispatcherConfigFlags.setOrUnset( E_EVENT_DISPATCHER_CONFIG_FLAG_IDLE_PROCESSING_MODE_BIT, pIdle );
	}

	bool EventDispatcher::postEvent( EventObject pEvent )
	{
		if( !ecValidateEventCode( pEvent.code ) )
		{
			return false;
		}

		_preProcessEvent( pEvent );

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

	EventDispatcherInputState & EventDispatcher::getInputState()
	{
		return _privateData->evtDispatcherInputState;
	}

	const EventDispatcherConfig & EventDispatcher::getConfig() const
	{
		return _privateData->evtDispatcherConfig;
	}

	void _preProcessEventOnInputMouseButton( EvtInputMouseButton & pMouseButtonEvent,
	                                         const EventDispatcherConfig & pDispatcherConfig,
	                                         EvtSharedInputMouseState & pInputMouseState );

	void _preProcessEventOnInputMouseButtonMultiClick( EvtInputMouseButton & pMouseButtonEvent,
	                                                   const EventDispatcherConfig & pDispatcherConfig,
	                                                   EvtSharedInputMouseState & pInputMouseState );

	void EventDispatcher::_preProcessEvent( EventObject & pEvent )
	{
		// For INPUT_MOUSE_BUTTON events, we may want to modify event's data depending on the input configuration.
		// _evtImplOnInputMouseButton() checks the input config and handles, for example, multi-click sequences.
		if( pEvent.code == E_EVENT_CODE_INPUT_MOUSE_BUTTON )
		{
			_preProcessEventOnInputMouseButton( pEvent.eInputMouseButton,
			                                    _privateData->evtDispatcherConfig,
			                                    _privateData->evtDispatcherInputState.inputMouseState );
		}
		else if( pEvent.code == E_EVENT_CODE_INPUT_KEYBOARD_KEY )
		{
			auto & keyboardState = _privateData->evtDispatcherInputState.inputKeyboardState;
			if( pEvent.eInputKeyboardKey.keyAction == EKeyActionType::Press )
			{
				keyboardState.keyStateMap[pEvent.eInputKeyboardKey.keyCode] = true;
			}
			else if( pEvent.eInputKeyboardKey.keyAction == EKeyActionType::Release )
			{
				keyboardState.keyStateMap[pEvent.eInputKeyboardKey.keyCode] = false;
			}
		}
	}

	void _preProcessEventOnInputMouseButton( EvtInputMouseButton & pMouseButtonEvent,
	                                         const EventDispatcherConfig & pDispatcherConfig,
	                                         EvtSharedInputMouseState & pInputMouseState )
	{
		if( pDispatcherConfig.dispatcherConfigFlags.isSet( E_EVENT_DISPATCHER_CONFIG_FLAG_ENABLE_MOUSE_DOUBLE_CLICK_BIT ) )
		{
			if( pMouseButtonEvent.buttonAction == EMouseButtonActionType::Click )
			{
				_preProcessEventOnInputMouseButtonMultiClick( pMouseButtonEvent, pDispatcherConfig, pInputMouseState );
			}
		}
	}

	void _preProcessEventOnInputMouseButtonMultiClick( EvtInputMouseButton & pMouseButtonEvent,
	                                                   const EventDispatcherConfig & pDispatcherConfig,
	                                                   EvtSharedInputMouseState & pInputMouseState )
	{
		bool multiClickEventSet = false;

		if( pInputMouseState.lastPressButtonID == pMouseButtonEvent.buttonID )
		{
			auto lastClickDiff = pMouseButtonEvent.timeStamp - pInputMouseState.lastPressTimestamp;
			auto lastClickDiffMs = PerfCounter::convertToDuration<EDurationPeriod::Millisecond>( lastClickDiff );
			if( lastClickDiffMs <= pDispatcherConfig.mouseClickSequenceTimeoutMs )
			{
				if( pInputMouseState.multiClickSequenceLength == 1 )
				{
					pMouseButtonEvent.buttonAction = EMouseButtonActionType::DoubleClick;
					pInputMouseState.multiClickSequenceLength = 2;
					multiClickEventSet = true;
				}
				else if( pDispatcherConfig.dispatcherConfigFlags.isSet( E_EVENT_DISPATCHER_CONFIG_FLAG_ENABLE_MOUSE_MULTI_CLICK_BIT ) )
				{
					pMouseButtonEvent.buttonAction = EMouseButtonActionType::MultiClick;
					pInputMouseState.multiClickSequenceLength += 1;
					multiClickEventSet = true;
				}
			}
		}

		if( !multiClickEventSet )
		{
			// Multi-click has not been detected/updated. Possible reasons: different button ID,
			// click timeout (mouse button clicked too slow), multi-click support is disabled.
			pInputMouseState.lastPressButtonID = pMouseButtonEvent.buttonID;
			pInputMouseState.multiClickSequenceLength = 1;
		}

		pInputMouseState.lastPressTimestamp = pMouseButtonEvent.timeStamp;
		pInputMouseState.lastCursorPos = pMouseButtonEvent.cursorPos;
	}

} // namespace ts3::system
