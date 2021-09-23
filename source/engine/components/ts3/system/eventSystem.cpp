
#include "eventSystemNative.h"
#include <ts3/stdext/mapUtils.h>

namespace ts3::system
{

    EventSource::EventSource( SysContextHandle pSysContext )
    : SysObject( pSysContext )
    {}

    EventSource::~EventSource() noexcept
    {
        if( _eventController )
        {
            _eventController->onEventSourceDestroy( *this );
            _eventController = nullptr;
        }
    }

    void EventSource::unregisterEventSourceAuto()
    {
        if( _eventController )
        {
            _eventController->unregisterEventSource( *this );
            _eventController = nullptr;
        }
    }

    void EventSource::setEventController( EventControllerHandle pEventController )
    {
        _eventController = pEventController;
    }

    void EventSource::setEventSourceNativeData( void * pData )
    {
        _eventSourceNativeData = pData;
    }


    EventController::EventController( SysContextHandle pSysContext )
    : SysObject( pSysContext )
    , mInternal( std::make_unique<ObjectInternalData>( this ) )
	{}

	EventController::~EventController() noexcept
	{
        releaseDispatcherObjects();
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

    EventDispatcher * EventController::createEventDispatcher( event_dispatcher_id_t pDispatcherID )
    {
        if( pDispatcherID == CX_EVENT_DISPATCHER_ID_DEFAULT )
        {
            return _getDefaultDispatcher();
        }

        auto existingDispatcherRef = mInternal->dispatcherMap.find( pDispatcherID );
        if( existingDispatcherRef != mInternal->dispatcherMap.end() )
        {
            return existingDispatcherRef->second;
        }

        auto dispatcher = createSysObject<EventDispatcher>( this, pDispatcherID );
        mInternal->dispatcherList.push_back( dispatcher );
        mInternal->dispatcherMap[pDispatcherID] = dispatcher.get();

        return dispatcher.get();
    }

    EventDispatcher * EventController::getEventDispatcher( event_dispatcher_id_t pDispatcherID )
    {
        if( pDispatcherID == CX_EVENT_DISPATCHER_ID_DEFAULT )
        {
            return _getDefaultDispatcher();
        }
        return getMapValueOrDefault( mInternal->dispatcherMap, pDispatcherID, nullptr );

    }

    bool EventController::setActiveEventDispatcher( event_dispatcher_id_t pDispatcherID )
    {
        if( auto * dispatcher = getEventDispatcher( pDispatcherID ) )
        {
            return setActiveEventDispatcher( dispatcher );
        }
        return false;
    }

    bool EventController::setActiveEventDispatcher( EventDispatcher * pDispatcher )
    {
        if( pDispatcher != mInternal->activeEventDispatcher )
        {
            auto * validatedPtr = _validateDispatcher( pDispatcher );
            if( validatedPtr == pDispatcher )
            {
                _onActiveDispatcherChange( pDispatcher );
                mInternal->activeEventDispatcher = pDispatcher;
                return true;
            }
        }
        return false;
    }

    bool EventController::setActiveEventDispatcherDefault()
    {
        auto defaultDispatcher = _getDefaultDispatcher();
        return setActiveEventDispatcher( defaultDispatcher );
    }

    bool EventController::resetActiveEventDispatcher()
    {
        if( mInternal->activeEventDispatcher  )
        {
            _onActiveDispatcherChange( nullptr );
            mInternal->activeEventDispatcher = nullptr;
            return true;
        }

        return false;
    }

    void EventController::releaseDispatcherObjects()
    {
        resetActiveEventDispatcher();

        mInternal->dispatcherMap.clear();
        mInternal->dispatcherList.clear();

        mInternal->activeEventDispatcher = nullptr;
        mInternal->defaultEventDispatcher = nullptr;
    }

    EventSource * EventController::findEventSource( EventSourceFindPredicate pPredicate ) const
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

    EventSource * EventController::findEventSource( EventSourceNativeDataFindPredicate pPredicate ) const
    {
        for( auto * eventSource : mInternal->eventSourceList )
        {
            const auto * eventSourceNativeData = eventSource->getEventSourceNativeData<EventSourceNativeData>();
            if( pPredicate( *eventSourceNativeData ) )
            {
                return eventSource;
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
        // Important note on destroying event sources: native-side unregistration should not be used here
        // (which means, to never call the native method _nativeUnregisterEventSource( pEventSource );
        //
        // onEventSourceDestroy is called when an EventSource is being destroyed (i.e. within its destructor), so the
        // sub-class part ( e.g. Window/GLSurface) is already gone. That will cause any native code to (most likely)
        // fail. For example, Win32 would not be able to reset the event procedure, since HWND has been already destroyed.
        // The purpose of this method is only to remove those sources, that have been destroyed as a consequence of a
        // system-generated action (e.g. close button of a window).

        try
        {
            _removeEventSource( pEventSource );

            if( auto * privateData = _removeEventSourcePrivateData( pEventSource ) )
            {
                _nativeDestroyEventSourcePrivateData( pEventSource, privateData );
            }
        }
        catch( const Exception & pException )
        {
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
            ts3ThrowAuto( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
        }

        ts3DebugAssert( mInternal->currentInputState != nullptr );
        ts3DebugAssert( mInternal->currentInternalConfig != nullptr );
    }
    
    void EventController::_onActiveDispatcherChange( EventDispatcher * pDispatcher )
    {
        if( pDispatcher )
        {
            mInternal->currentInputState = &( pDispatcher->mInternal->inputState );
            mInternal->currentInternalConfig = &( pDispatcher->mInternal->internalConfig );
        }
        else
        {
            mInternal->currentInputState = nullptr;
            mInternal->currentInternalConfig = nullptr;
        }

        _nativeOnActiveDispatcherChange( pDispatcher );
    }

    EventDispatcher * EventController::_getDefaultDispatcher()
    {
        if( !mInternal->defaultEventDispatcher )
        {
            auto defaultDispatcher = createSysObject<EventDispatcher>( this, CX_EVENT_DISPATCHER_ID_DEFAULT );
            mInternal->defaultEventDispatcher = defaultDispatcher;
        }
        return mInternal->defaultEventDispatcher.get();
    }

    EventDispatcher * EventController::_validateDispatcher( EventDispatcher * pDispatcher )
    {
        if( !pDispatcher )
        {
            return nullptr;
        }

        if( pDispatcher->mEventController != this )
        {
            return nullptr;
        }

        auto * internalDispatcherPtr = getEventDispatcher( pDispatcher->mID );
        if( pDispatcher != internalDispatcherPtr )
        {
            return nullptr;
        }

        return pDispatcher;
    }

    void EventController::_addEventSource( EventSource & pEventSource )
    {
        auto eventSourceIter = std::find( mInternal->eventSourceList.begin(), mInternal->eventSourceList.end(), &pEventSource );
        if( eventSourceIter != mInternal->eventSourceList.end() )
        {
            ts3ThrowAuto( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
        }

        mInternal->eventSourceList.push_back( &pEventSource );

        pEventSource.setEventController( getHandle<EventController>() );
    }

    void EventController::_removeEventSource( EventSource & pEventSource )
    {
        auto eventSourceIter = std::find( mInternal->eventSourceList.begin(), mInternal->eventSourceList.end(), &pEventSource );
        if( eventSourceIter == mInternal->eventSourceList.end() )
        {
            ts3ThrowAuto( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
        }

        mInternal->eventSourceList.erase( eventSourceIter );

        pEventSource.setEventController( nullptr );
    }

    void * EventController::_removeEventSourcePrivateData( EventSource & pEventSource )
    {
        auto dataIter = mInternal->eventSourcePrivateDataMap.find( &pEventSource );
        if( dataIter != mInternal->eventSourcePrivateDataMap.end() )
        {
            auto * privateData = dataIter->second;
            mInternal->eventSourcePrivateDataMap.erase( dataIter );
            return privateData;
        }
        return nullptr;
    }

    void EventController::_setEventSourcePrivateData( EventSource & pEventSource, void * pData )
    {
        mInternal->eventSourcePrivateDataMap[&pEventSource] = pData;
    }

    void * EventController::_getEventSourcePrivateData( EventSource & pEventSource ) const
    {
        auto dataIter = mInternal->eventSourcePrivateDataMap.find( &pEventSource );
        if( dataIter != mInternal->eventSourcePrivateDataMap.end() )
        {
            return dataIter->second;
        }
        return nullptr;
    }


    void _internalEventPreProcess( EventObject & pEvent, EventController & pEventController );
    void _internalEventOnInputMouseButton( EventObject & pEvent, const EventSystemInternalConfig & pConfig, EventInputState & pInputState );
    void _internalEventOnInputMouseButtonMultiClick( EvtInputMouseButton & pMouseButtonEvent, const EventSystemInternalConfig & pConfig, EventInputState & pInputState );

    EventDispatcher::EventDispatcher( EventController * pEventController,
                                      event_dispatcher_id_t pID )
    : SysObject( pEventController->mSysContext )
    , mEventController( pEventController )
	, mInternal( std::make_unique<ObjectInternalData>( this ) )
	, mID( ( pID != CX_EVENT_DISPATCHER_ID_AUTO ) ? pID : reinterpret_cast<event_dispatcher_id_t>( this ) )
	{}

	EventDispatcher::~EventDispatcher() noexcept = default;

	void EventDispatcher::bindEventHandler( EEventBaseType pBaseType, EventHandler pHandler )
	{
		// EventBaseType enum is numbered from zero to have constant access time to its entry.
		auto baseTypeValue = static_cast<size_t>( pBaseType );
		// Check for possible violation attempt.
		if ( baseTypeValue >= CX_ENUM_EVENT_BASE_TYPE_COUNT )
		{
			ts3ThrowAuto( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
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
			ts3ThrowAuto( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
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
			ts3ThrowAuto( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
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

} // namespace ts3::system
