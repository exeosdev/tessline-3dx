
#include "eventSystemNative.h"
#include <ts3/stdext/mapUtils.h>

namespace ts3::system
{

    EventSource::EventSource( system::SysContextHandle pSysContext )
    : SysObject( std::move( pSysContext ) )
    {}

    EventSource::~EventSource() noexcept
    {
        if( _eventControllerActiveRef )
        {
            _eventControllerActiveRef->onEventSourceDestroy( *this );
            _eventControllerActiveRef = nullptr;
        }
    }

    void EventSource::setEventController( EventControllerHandle pEventController )
    {
        _eventControllerActiveRef = pEventController;
    }

    void EventSource::setPrivateData( void * pPrivateData )
    {
        _privateData = pPrivateData;
    }

    EventSourceNativeData * EventSource::getEventSourceNativeData() const
    {
        return _eventSourceNativeData;
    }

    void * EventSource::getPrivateData() const
    {
        return _privateData;
    }

    void EventSource::setEventSourceNativeData( EventSourceNativeData & pNativeData )
    {
        _eventSourceNativeData = &pNativeData;
    }

    void EventSource::resetEventSourceNativeData()
    {
        _eventSourceNativeData = nullptr;
    }


    void _internalEventPreProcess( EventObject & pEvent, EventController & pEventController );
    void _internalEventOnInputMouseButton( EventObject & pEvent, const EventSystemInternalConfig & pConfig, EventInputState & pInputState );
    void _internalEventOnInputMouseButtonMultiClick( EvtInputMouseButton & pMouseButtonEvent, const EventSystemInternalConfig & pConfig, EventInputState & pInputState );

    EventDispatcher::EventDispatcher( EventControllerHandle pEventController )
    : SysObject( pEventController->mSysContext )
    , mEventController( pEventController.get() )
    , mInternal( std::make_unique<ObjectInternalData>( this ) )
    , _eventControllerActiveRef( std::move( pEventController ) )
    {}

    EventDispatcher::~EventDispatcher() noexcept
    {
        if( _eventControllerActiveRef )
        {
            _eventControllerActiveRef->onEventDispatcherDestroy( *this );
            _eventControllerActiveRef = nullptr;
        }
    }

    void EventDispatcher::bindEventHandler( EEventBaseType pBaseType, EventHandler pHandler )
    {
        // EventBaseType enum is numbered from zero to have constant access time to its entry.
        auto baseTypeValue = static_cast<size_t>( pBaseType );
        // Check for possible violation attempt.
        if ( baseTypeValue >= CX_ENUM_EVENT_BASE_TYPE_COUNT )
        {
            ts3Throw( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
        }
        mInternal->handlerMapByBaseType[baseTypeValue] = std::move( pHandler );
    }

    void EventDispatcher::bindEventHandler( EEventCategory pCategory, EventHandler pHandler )
    {
        // EventCategory enum is numbered from zero to have constant access time to its entry.
        auto categoryValue = static_cast<size_t>( pCategory );
        // Check for possible violation attempt.
        if ( categoryValue >= CX_ENUM_EVENT_CATEGORY_COUNT )
        {
            ts3Throw( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
        }
        mInternal->handlerMapByCategory[categoryValue] = std::move( pHandler );
    }

    void EventDispatcher::bindEventHandler( EEventCodeIndex pCodeIndex, EventHandler pHandler )
    {
        // EventCodeIndex enum is numbered from zero to have constant access time to its entry.
        auto codeIndexValue = static_cast<size_t>( pCodeIndex );
        // Check for possible violation attempt.
        if ( codeIndexValue >= CX_ENUM_EVENT_CODE_INDEX_COUNT )
        {
            ts3Throw( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
        }
        mInternal->handlerMapByCodeIndex[codeIndexValue] = std::move( pHandler );
    }

    void EventDispatcher::bindDefaultEventHandler( EventHandler pHandler )
    {
        mInternal->defaultHandler = std::move( pHandler );
    }

    void EventDispatcher::setIdleProcessingMode( bool pIdle )
    {
        mInternal->internalConfig.configFlags.setOrUnset( pIdle, E_EVENT_SYSTEM_CONFIG_FLAG_IDLE_PROCESSING_MODE_BIT );
    }

    bool EventDispatcher::postEvent( EventObject pEvent )
    {
        if( !ecValidateEventCode( pEvent.code ) )
        {
            return false;
        }

        _internalEventPreProcess( pEvent, *mEventController );

        pEvent.commonData.timeStamp = PerfCounter::queryCurrentStamp();

        {
            auto codeIndexValue = static_cast<size_t>( ecGetEventCodeCodeIndex( pEvent.code ) );
            auto & eventHandler = mInternal->handlerMapByCodeIndex[codeIndexValue];
            if ( eventHandler && eventHandler( pEvent ) )
            {
                return true;
            }
        }
        {
            auto categoryValue = static_cast<size_t>( ecGetEventCodeCategory( pEvent.code ) );
            auto & eventHandler = mInternal->handlerMapByCategory[categoryValue];
            if ( eventHandler && eventHandler( pEvent ) )
            {
                return true;
            }
        }
        {
            auto baseTypeValue = static_cast<size_t>( ecGetEventCodeBaseType( pEvent.code ) );
            auto & eventHandler = mInternal->handlerMapByBaseType[baseTypeValue];
            if ( eventHandler && eventHandler( pEvent ) )
            {
                return true;
            }
        }
        {
            auto & defaultHandler = mInternal->defaultHandler;
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


    void _internalEventPreProcess( EventObject & pEvent, EventController & pEventController )
    {
        // For INPUT_MOUSE_BUTTON events, we may want to modify event's data depending on the input configuration.
        // _evtImplOnInputMouseButton() checks the input config and handles, for example, multi-click sequences.
        if ( pEvent.code == E_EVENT_CODE_INPUT_MOUSE_BUTTON )
        {
            _internalEventOnInputMouseButton( pEvent,
                                              pEventController.mInternal->getCurrentInternalConfig(),
                                              pEventController.mInternal->getCurrentInputState() );
        }
    }

    void _internalEventOnInputMouseButton( EventObject & pEvent, const EventSystemInternalConfig & pConfig, EventInputState & pInputState )
    {
        if ( pConfig.configFlags.isSet( E_EVENT_SYSTEM_CONFIG_FLAG_ENABLE_MOUSE_DOUBLE_CLICK_BIT ) )
        {
            auto & eInputMouseButton = pEvent.eInputMouseButton;
            if ( eInputMouseButton.buttonAction == EMouseButtonActionType::Click )
            {
                _internalEventOnInputMouseButtonMultiClick( eInputMouseButton, pConfig, pInputState );
            }
        }
    }

    void _internalEventOnInputMouseButtonMultiClick( EvtInputMouseButton & pMouseButtonEvent, const EventSystemInternalConfig & pConfig, EventInputState & pInputState )
    {
        bool multiClickEventSet = false;

        if ( pInputState.mouseLastPressButton == pMouseButtonEvent.buttonID )
        {
            auto lastClickDiff = pMouseButtonEvent.timeStamp - pInputState.mouseLastPressTimestamp;
            auto lastClickDiffMs = PerfCounter::convertToDuration<DurationPeriod::Millisecond>( lastClickDiff );
            if ( lastClickDiffMs <= pConfig.mouseClickSequenceTimeoutMs )
            {
                if( pInputState.mouseClickSequenceLength == 1 )
                {
                    pMouseButtonEvent.buttonAction = EMouseButtonActionType::DoubleClick;
                    pInputState.mouseClickSequenceLength = 2;
                    multiClickEventSet = true;
                }
                else if ( pConfig.configFlags.isSet( E_EVENT_SYSTEM_CONFIG_FLAG_ENABLE_MOUSE_MULTI_CLICK_BIT ) )
                {
                    pMouseButtonEvent.buttonAction = EMouseButtonActionType::MultiClick;
                    pInputState.mouseClickSequenceLength += 1;
                    multiClickEventSet = true;
                }
            }
        }

        if( !multiClickEventSet )
        {
            // Multi-click has not been detected/updated. Possible reasons: different button ID,
            // click timeout (mouse button clicked too slow), multi-click support is disabled.
            pInputState.mouseLastPressButton = pMouseButtonEvent.buttonID;
            pInputState.mouseClickSequenceLength = 1;
        }

        pInputState.mouseLastPressTimestamp = pMouseButtonEvent.timeStamp;
        pInputState.mouseLastRegPos = pMouseButtonEvent.cursorPos;
    }


    EventController::EventController( system::SysContextHandle pSysContext )
    : SysObject( pSysContext )
    , mInternal( std::make_unique<ObjectInternalData>( this ) )
	{
        _nativeConstructor();
	}

	EventController::~EventController() noexcept
	{
        _resetEventSourceList();

        _resetEventDispatcherList();

        _nativeDestructor();
	}

	bool EventController::dispatchEvent( EventObject pEvent )
	{
        _checkActiveDispatcherState();

        return mInternal->activeEventDispatcher->postEvent( pEvent );
	}

	uint32 EventController::updateSysQueueAuto()
	{
        _checkActiveDispatcherState();

        uint32 eventCounter = 0;
        while( _nativeUpdateSysQueue() )
        {
            ++eventCounter;
        }
        if( ( eventCounter == 0 ) && mInternal->currentInternalConfig->configFlags.isSet( E_EVENT_SYSTEM_CONFIG_FLAG_IDLE_PROCESSING_MODE_BIT ) )
        {
            _nativeUpdateSysQueueWait();
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

	EventDispatcherHandle EventController::createEventDispatcher()
	{
        auto eventDispatcher = createSysObject<EventDispatcher>( getHandle<EventController>() );

        _addEventDispatcher( *eventDispatcher );

        return eventDispatcher;
	}

	bool EventController::setActiveEventDispatcher( EventDispatcher & pEventDispatcher )
	{
        if( &pEventDispatcher != mInternal->activeEventDispatcher )
        {
            _validateDispatcher( pEventDispatcher );
            _onActiveDispatcherChange( &pEventDispatcher );
            return true;
        }
        return false;
	}

	bool EventController::resetActiveEventDispatcher()
	{
        if( mInternal->activeEventDispatcher  )
        {
            _onActiveDispatcherChange( nullptr );
            return true;
        }
        return false;
	}

    void EventController::registerEventSource( EventSource & pEventSource )
    {
        _nativeRegisterEventSource( pEventSource );

        _addEventSource( pEventSource );
    }

    void EventController::unregisterEventSource( EventSource & pEventSource )
    {
        _removeEventSource( pEventSource );

        _nativeUnregisterEventSource( pEventSource );
    }

    EventSource * EventController::findEventSource( const EventSourceFindPredicate & pPredicate ) const
    {
        for( auto * eventSource : mInternal->eventSourceList )
        {
            if( pPredicate( *eventSource ) )
            {
                return eventSource;
            }
        }
        return nullptr;
    }

    EventSource * EventController::findEventSource( const EventSourceNativeDataFindPredicate & pPredicate ) const
    {
        for( auto * eventSource : mInternal->eventSourceList )
        {
            if( const auto * eventSourceNativeData = eventSource->getEventSourceNativeData() )
            {
                if( pPredicate( *eventSourceNativeData ) )
                {
                    return eventSource;
                }
            }
        }
        return nullptr;
    }

    bool EventController::isEventSourceRegistered( EventSource & pEventSource ) const
    {
        for( auto * eventSource : mInternal->eventSourceList )
        {
            if( &pEventSource == eventSource )
            {
                return true;
            }
        }
        return false;
    }

    bool EventController::hasActiveDispatcher() const
    {
        return mInternal->activeEventDispatcher != nullptr;
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
        if( !hasActiveDispatcher() )
        {
            ts3Throw( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
        }

        ts3DebugAssert( mInternal->currentInputState != nullptr );
        ts3DebugAssert( mInternal->currentInternalConfig != nullptr );
    }
    
    void EventController::_onActiveDispatcherChange( EventDispatcher * pEventDispatcher )
    {
        _nativeOnActiveDispatcherChange( pEventDispatcher );

        if( pEventDispatcher )
        {
            mInternal->currentInputState = &( pEventDispatcher->mInternal->inputState );
            mInternal->currentInternalConfig = &( pEventDispatcher->mInternal->internalConfig );
        }
        else
        {
            mInternal->currentInputState = nullptr;
            mInternal->currentInternalConfig = nullptr;
        }

        mInternal->activeEventDispatcher = pEventDispatcher;
    }

    void EventController::_validateDispatcher( EventDispatcher & pEventDispatcher )
    {
        if( pEventDispatcher.mEventController != this )
        {
            ts3Throw( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
        }

        auto eventDispatcherRef = mInternal->findEventDispatcherInternal( &pEventDispatcher );
        if( !eventDispatcherRef.first || ( *( eventDispatcherRef.second ) != &pEventDispatcher ) )
        {
            ts3Throw( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
        }
    }

    void EventController::_addEventDispatcher( EventDispatcher & pEventDispatcher )
    {
        auto eventDispatcherRef = mInternal->findEventDispatcherInternal( &pEventDispatcher );
        if( eventDispatcherRef.first )
        {
            ts3Throw( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
        }

        mInternal->eventDispatcherList.push_back( &pEventDispatcher );
    }

    void EventController::_removeEventDispatcher( EventDispatcher & pEventDispatcher )
    {
        auto eventDispatcherRef = mInternal->findEventDispatcherInternal( &pEventDispatcher );
        if( !eventDispatcherRef.first )
        {
            ts3Throw( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
        }

        mInternal->eventDispatcherList.erase( eventDispatcherRef.second );
    }

    void EventController::_addEventSource( EventSource & pEventSource )
    {
        auto eventSourceRef = mInternal->findEventSourceInternal( &pEventSource );
        if( eventSourceRef.first )
        {
            ts3Throw( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
        }

        mInternal->eventSourceList.push_back( &pEventSource );

        pEventSource.setEventController( getHandle<EventController>() );
    }

    void EventController::_removeEventSource( EventSource & pEventSource )
    {
        auto eventSourceRef = mInternal->findEventSourceInternal( &pEventSource );
        if( !eventSourceRef.first )
        {
            ts3Throw( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
        }

        mInternal->eventSourceList.erase( eventSourceRef.second );
    }

    void EventController::_resetEventDispatcherList()
    {
        while( !mInternal->eventDispatcherList.empty() )
        {
            auto * eventDispatcher = mInternal->eventDispatcherList.back();

            _removeEventDispatcher( *eventDispatcher );
        }
    }

    void EventController::_resetEventSourceList()
    {
        while( !mInternal->eventSourceList.empty() )
        {
            auto * eventSource = mInternal->eventSourceList.back();

            _removeEventSource( *eventSource );

            _nativeUnregisterEventSource( *eventSource );
        }
    }

} // namespace ts3::system
