
#include "internal/eventCorePrivate.h"

namespace ts3::system
{

    EventController::EventController( system::SysContextHandle pSysContext )
    : SysObject( pSysContext )
    , _privateData( std::make_unique<EventControllerPrivateData>() )
	{}

	EventController::~EventController() noexcept = default;

	EventDispatcherHandle EventController::createEventDispatcher()
	{
        auto dispatcher = createSysObject<EventDispatcher>( getHandle<EventController>() );
        _addEventDispatcher( *dispatcher );
        return dispatcher;
	}

	void EventController::registerEventSource( EventSource & pEventSource )
	{
	    _nativeRegisterEventSource( pEventSource );
	    _addEventSource( pEventSource );

	    pEventSource.setEventController( getHandle<EventController>() );
	}

	void EventController::unregisterEventSource( EventSource & pEventSource )
	{
	    _removeEventSource( pEventSource );
	    _nativeUnregisterEventSource( pEventSource );

	    pEventSource.setEventController( nullptr );
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
        _checkActiveDispatcherState();

        return _privateData->activeEventDispatcher->postEvent( pEvent );
	}

	uint32 EventController::updateSysQueueAuto()
	{
        _checkActiveDispatcherState();

        uint32 eventCounter = 0;
        while( _nativeUpdateSysQueue() )
        {
            ++eventCounter;
        }

        if( eventCounter == 0 )
        {
            auto & esInternalConfig = _privateData->getCurrentInternalConfig();
            if( esInternalConfig.configFlags.isSet( E_EVENT_SYSTEM_CONFIG_FLAG_IDLE_PROCESSING_MODE_BIT ) )
            {
                _nativeUpdateSysQueueWait();
            }
        }

        return eventCounter;
	}

	uint32 EventController::updateSysQueuePeek( uint32 pLimit )
	{
        _checkActiveDispatcherState();

        uint32 eventCounter = 0;
        while( _nativeUpdateSysQueue() && ( eventCounter <= pLimit ) )
        {
            ++eventCounter;
        }

        return eventCounter;
	}

	uint32 EventController::updateSysQueueWait( uint32 pLimit )
	{
        _checkActiveDispatcherState();

        uint32 eventCounter = 0;
        while( _nativeUpdateSysQueue() && ( eventCounter <= pLimit ) )
        {
            ++eventCounter;
        }

        if( eventCounter == 0 )
        {
            _nativeUpdateSysQueueWait();
        }

        return eventCounter;
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

    bool EventController::isEventSourceRegistered( EventSource & pEventSource ) const
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

    void EventController::setEventSourcePlatformData( EventSource & pEventSource, EventSourceInternalDataPtr pPlatformDataPtr )
    {
	    pEventSource.setEventSourcePlatformData( std::move( pPlatformDataPtr ) );
    }

    void EventController::setEventSourcePlatformData( EventSource & pEventSource, void * pPlatformData, EventSourceInternalDataDeleter pDeleter )
    {
	    pEventSource.setEventSourcePlatformData( pPlatformData, std::move( pDeleter ) );
    }

	void EventController::resetEventSourcePlatformData( EventSource & pEventSource )
    {
	    pEventSource.resetEventSourcePlatformData();
    }

	EventSystemSharedState & EventController::getEventSystemSharedState()
    {
	    ts3DebugAssert( _privateData->currentSharedState );
	    return *( _privateData->currentSharedState );
    }

    const EventSystemInternalConfig & EventController::getEventSystemInternalConfig() const
    {
	    ts3DebugAssert( _privateData->currentInternalConfig );
	    return *( _privateData->currentInternalConfig );
    }

    void EventController::onEventSourceDestroy( EventSource & pEventSource ) noexcept
    {
        try
        {
            _removeEventSource( pEventSource );

            _nativeUnregisterEventSource( pEventSource );
        }
        catch( const Exception & pException )
        {
            ( pException );
            ts3DebugInterrupt();
        }
        catch( ... )
        {
            ts3DebugInterrupt();
        }
    }

    void EventController::onEventDispatcherDestroy( EventDispatcher & pEventDispatcher ) noexcept
    {
        try
        {
            _removeEventDispatcher( pEventDispatcher );
        }
        catch( const Exception & pException )
        {
            ( pException );
            ts3DebugInterrupt();
        }
        catch( ... )
        {
            ts3DebugInterrupt();
        }
    }

    void EventController::_checkActiveDispatcherState()
    {
        if( !_privateData->activeEventDispatcher )
        {
            ts3Throw( E_EXC_DEBUG_PLACEHOLDER );
        }
        if( !_privateData->currentInternalConfig || !_privateData->currentSharedState )
        {
            ts3Throw( E_EXC_DEBUG_PLACEHOLDER );
        }
    }

    void EventController::_onActiveDispatcherChange( EventDispatcher * pEventDispatcher )
    {
        if( pEventDispatcher )
        {
            auto & esInternalConfig = pEventDispatcher->getEventSystemInternalConfig();
            _privateData->currentInternalConfig = &esInternalConfig;

            auto & esSharedState = pEventDispatcher->getEventSystemSharedState();
            _privateData->currentSharedState = &esSharedState;
        }
        else
        {
            _privateData->currentSharedState = nullptr;
            _privateData->currentInternalConfig = nullptr;
        }

        _privateData->activeEventDispatcher = pEventDispatcher;
    }

    void EventController::_addEventDispatcher( EventDispatcher & pEventDispatcher )
    {
        auto eventDispatcherRef = _privateData->findEventDispatcherInternal( &pEventDispatcher );
        if( eventDispatcherRef.first )
        {
            ts3Throw( E_EXC_DEBUG_PLACEHOLDER );
        }

        _privateData->eventDispatcherList.push_back( &pEventDispatcher );
    }

    void EventController::_removeEventDispatcher( EventDispatcher & pEventDispatcher )
    {
        auto eventDispatcherRef = _privateData->findEventDispatcherInternal( &pEventDispatcher );
        if( !eventDispatcherRef.first )
        {
            ts3Throw( E_EXC_DEBUG_PLACEHOLDER );
        }

        _privateData->eventDispatcherList.erase( eventDispatcherRef.second );
    }

    void EventController::_addEventSource( EventSource & pEventSource )
    {
        auto eventSourceRef = _privateData->findEventSourceInternal( &pEventSource );
        if( eventSourceRef.first )
        {
            ts3Throw( E_EXC_DEBUG_PLACEHOLDER );
        }

        _privateData->eventSourceList.push_back( &pEventSource );
    }

    void EventController::_removeEventSource( EventSource & pEventSource )
    {
        auto eventSourceRef = _privateData->findEventSourceInternal( &pEventSource );
        if( !eventSourceRef.first )
        {
            ts3Throw( E_EXC_DEBUG_PLACEHOLDER );
        }

        _privateData->eventSourceList.erase( eventSourceRef.second );
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
	    if ( baseTypeValue >= CX_ENUM_EVENT_BASE_TYPE_COUNT )
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
	    if ( categoryValue >= CX_ENUM_EVENT_CATEGORY_COUNT )
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
	    if ( codeIndexValue >= CX_ENUM_EVENT_CODE_INDEX_COUNT )
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

    void EventDispatcher::setIdleProcessingMode( bool pIdle )
    {
	    _privateData->evtSysInternalConfig.configFlags.setOrUnset( pIdle, E_EVENT_SYSTEM_CONFIG_FLAG_IDLE_PROCESSING_MODE_BIT );
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
	        if ( eventHandler && eventHandler( pEvent ) )
	        {
	            return true;
	        }
	    }
	    {
	        auto categoryValue = static_cast<size_t>( ecGetEventCodeCategory( pEvent.code ) );
	        auto & eventHandler = _privateData->handlerMapByCategory[categoryValue];
	        if ( eventHandler && eventHandler( pEvent ) )
	        {
	            return true;
	        }
	    }
	    {
	        auto baseTypeValue = static_cast<size_t>( ecGetEventCodeBaseType( pEvent.code ) );
	        auto & eventHandler = _privateData->handlerMapByBaseType[baseTypeValue];
	        if ( eventHandler && eventHandler( pEvent ) )
	        {
	            return true;
	        }
	    }
	    {
	        auto & defaultHandler = _privateData->defaultHandler;
	        if ( defaultHandler && defaultHandler( pEvent ) )
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

	EventSystemSharedState & EventDispatcher::getEventSystemSharedState()
	{
	    return _privateData->evtSysSharedState;
	}

	const EventSystemInternalConfig & EventDispatcher::getEventSystemInternalConfig() const
    {
        return _privateData->evtSysInternalConfig;
    }

    void _preProcessEventOnInputMouseButton( EvtInputMouseButton & pMouseButtonEvent,
                                             const EventSystemInternalConfig & pEvtSysConfig,
                                             ESSharedInputMouseState & pInputMouseState );

	void _preProcessEventOnInputMouseButtonMultiClick( EvtInputMouseButton & pMouseButtonEvent,
                                                       const EventSystemInternalConfig & pEvtSysConfig,
                                                       ESSharedInputMouseState & pInputMouseState );

    void EventDispatcher::_preProcessEvent( EventObject & pEvent )
    {
	    // For INPUT_MOUSE_BUTTON events, we may want to modify event's data depending on the input configuration.
	    // _evtImplOnInputMouseButton() checks the input config and handles, for example, multi-click sequences.
	    if ( pEvent.code == E_EVENT_CODE_INPUT_MOUSE_BUTTON )
	    {
	        _preProcessEventOnInputMouseButton( pEvent.eInputMouseButton,
                                                _privateData->evtSysInternalConfig,
                                                _privateData->evtSysSharedState.inputMouseState );
	    }
    }

    void _preProcessEventOnInputMouseButton( EvtInputMouseButton & pMouseButtonEvent,
                                             const EventSystemInternalConfig & pEvtSysConfig,
                                             ESSharedInputMouseState & pInputMouseState )
    {
        if ( pEvtSysConfig.configFlags.isSet( E_EVENT_SYSTEM_CONFIG_FLAG_ENABLE_MOUSE_DOUBLE_CLICK_BIT ) )
	    {
            if ( pMouseButtonEvent.buttonAction == EMouseButtonActionType::Click )
	        {
                _preProcessEventOnInputMouseButtonMultiClick( pMouseButtonEvent, pEvtSysConfig, pInputMouseState );
	        }
	    }
    }

    void _preProcessEventOnInputMouseButtonMultiClick( EvtInputMouseButton & pMouseButtonEvent,
                                                       const EventSystemInternalConfig & pEvtSysConfig,
                                                       ESSharedInputMouseState & pInputMouseState )
    {
	    bool multiClickEventSet = false;

	    if ( pInputMouseState.lastPressButtonID == pMouseButtonEvent.buttonID )
	    {
	        auto lastClickDiff = pMouseButtonEvent.timeStamp - pInputMouseState.lastPressTimestamp;
	        auto lastClickDiffMs = PerfCounter::convertToDuration<EDurationPeriod::Millisecond>( lastClickDiff );
	        if ( lastClickDiffMs <= pEvtSysConfig.mouseClickSequenceTimeoutMs )
	        {
	            if( pInputMouseState.multiClickSequenceLength == 1 )
	            {
	                pMouseButtonEvent.buttonAction = EMouseButtonActionType::DoubleClick;
	                pInputMouseState.multiClickSequenceLength = 2;
	                multiClickEventSet = true;
	            }
	            else if ( pEvtSysConfig.configFlags.isSet( E_EVENT_SYSTEM_CONFIG_FLAG_ENABLE_MOUSE_MULTI_CLICK_BIT ) )
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
