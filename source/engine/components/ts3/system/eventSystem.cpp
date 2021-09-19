
#include "eventSystemNative.h"
#include <ts3/stdext/mapUtils.h>

namespace ts3::system
{

    void _internalEventPreProcess( EventObject & pEvent, EventController & pEventController );
    void _internalEventOnInputMouseButton( EventObject & pEvent, const EventSystemInternalConfig & pConfig, EventInputState & pInputState );
    void _internalEventOnInputMouseButtonMultiClick( EvtInputMouseButton & pMouseButtonEvent, const EventSystemInternalConfig & pConfig, EventInputState & pInputState );


    EventController::EventController( SysContextHandle pSysContext )
    : SysObject( pSysContext )
    , mPrivate( std::make_unique<ObjectPrivateData>( this ) )
	{}

	EventController::~EventController() noexcept = default;

    void EventController::dispatchEvent( EventObject pEvent )
    {
        _checkActiveDispatcherState();

        mPrivate->activeDispatcher->postEvent( pEvent );
    }

    uint32 EventController::dispatchSysEventAuto()
    {
        _checkActiveDispatcherState();

        uint32 eventCounter = 0;
        while( _nativeDispatchNext() )
        {
            ++eventCounter;
        }
        if( ( eventCounter == 0 ) && mPrivate->currentInternalConfig->configFlags.isSet( E_EVENT_SYSTEM_CONFIG_FLAG_IDLE_PROCESSING_MODE_BIT ) )
        {
            _nativeDispatchNextWait();
        }
        return eventCounter;
    }

    uint32 EventController::dispatchSysEventPeek( uint32 pLimit )
    {
        _checkActiveDispatcherState();

        uint32 eventCounter = 0;
        while( _nativeDispatchNext() && ( eventCounter <= pLimit ) )
        {
            ++eventCounter;
        }
        return eventCounter;
    }

    uint32 EventController::dispatchSysEventWait( uint32 pLimit )
    {
        _checkActiveDispatcherState();

        uint32 eventCounter = 0;
        while( _nativeDispatchNext() && ( eventCounter <= pLimit ) )
        {
            ++eventCounter;
        }
        if( eventCounter == 0 )
        {
            _nativeDispatchNextWait();
        }
        return eventCounter;
    }

    bool EventController::setActiveDispatcher( EventDispatcher & pDispatcher )
    {
        if( &pDispatcher != mPrivate->activeDispatcher )
        {
            _onActiveDispatcherChange( &pDispatcher );
            mPrivate->activeDispatcher = &pDispatcher;
            return true;
        }

        return false;
    }

    bool EventController::resetActiveDispatcher()
    {
        if( mPrivate->activeDispatcher  )
        {
            _onActiveDispatcherChange( nullptr );
            mPrivate->activeDispatcher = nullptr;
            return true;
        }

        return false;
    }

    EventDispatcher * EventController::createEventDispatcher( event_dispatcher_id_t pDispatcherID )
    {
        if( pDispatcherID == CX_EVENT_DISPATCHER_ID_DEFAULT )
        {
            return nullptr;
        }

        auto existingDispatcherRef = mPrivate->dispatcherMap.find( pDispatcherID );
        if( existingDispatcherRef != mPrivate->dispatcherMap.end() )
        {
            return existingDispatcherRef->second;
        }

        auto & dispatcher = mPrivate->dispatcherInternalStorage.emplace_back( this, pDispatcherID );
        mPrivate->dispatcherMap[pDispatcherID] = &dispatcher;

        return &dispatcher;

    }

    EventDispatcher * EventController::getEventDispatcher( event_dispatcher_id_t pDispatcherID )
    {
        if( pDispatcherID == CX_EVENT_DISPATCHER_ID_DEFAULT )
        {
            return &( mPrivate->defaultEventDispatcher );
        }
        return getMapValueOrDefault( mPrivate->dispatcherMap, pDispatcherID, nullptr );
    }

    bool EventController::hasActiveDispatcher() const
    {
        return mPrivate->activeDispatcher != nullptr;
    }

    void EventController::_checkActiveDispatcherState()
    {
        if( !hasActiveDispatcher() )
        {
            ts3DebugInterruptOnce();
            throw 0;
        }

        ts3DebugAssert( mPrivate->currentInternalConfig != nullptr );
    }
    
    void EventController::_onActiveDispatcherChange( EventDispatcher * pDispatcher )
    {
        mPrivate->inputState.mouseClickSequenceLength = 0;
        mPrivate->inputState.mouseLastPressTimestamp = 0;
        mPrivate->inputState.mouseButtonStateMask = 0;
        mPrivate->inputState.mouseLastPressButton = EMouseButtonID::Unknown;
        mPrivate->inputState.mouseLastRegPos = CX_EVENT_MOUSE_POS_INVALID;

        if( pDispatcher )
        {
            mPrivate->currentInternalConfig = &( pDispatcher->mPrivate->internalConfig );
        }
        else
        {
            mPrivate->currentInternalConfig = nullptr;
        }

        _nativeOnActiveDispatcherChange( pDispatcher );
    }


    EventDispatcher::EventDispatcher( EventController * pEventController,
                                      event_dispatcher_id_t pID )
	: mEventController( pEventController )
	, mPrivate( std::make_unique<ObjectPrivateData>( this ) )
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
			throw 0;
		}
		mPrivate->handlerMapByBaseType[baseTypeValue] = std::move( pHandler );
	}

	void EventDispatcher::bindEventHandler( EEventCategory pCategory, EventHandler pHandler )
	{
		// EventCategory enum is numbered from zero to have constant access time to its entry.
		auto categoryValue = static_cast<size_t>( pCategory );
		// Check for possible violation attempt.
		if ( categoryValue >= CX_ENUM_EVENT_CATEGORY_COUNT )
		{
			throw 0;
		}
		mPrivate->handlerMapByCategory[categoryValue] = std::move( pHandler );
	}

	void EventDispatcher::bindEventHandler( EEventCodeIndex pCodeIndex, EventHandler pHandler )
	{
		// EventCodeIndex enum is numbered from zero to have constant access time to its entry.
		auto codeIndexValue = static_cast<size_t>( pCodeIndex );
		// Check for possible violation attempt.
		if ( codeIndexValue >= CX_ENUM_EVENT_CODE_INDEX_COUNT )
		{
			throw 0;
		}
		mPrivate->handlerMapByCodeIndex[codeIndexValue] = std::move( pHandler );
	}

	void EventDispatcher::bindDefaultEventHandler( EventHandler pHandler )
	{
	    mPrivate->defaultHandler = std::move( pHandler );
	}

	void EventDispatcher::setIdleProcessingMode( bool pIdle )
	{
	    mPrivate->internalConfig.configFlags.setOrUnset( pIdle, E_EVENT_SYSTEM_CONFIG_FLAG_IDLE_PROCESSING_MODE_BIT );
	}

	void EventDispatcher::postEvent( EventObject pEvent )
	{
	    _internalEventPreProcess( pEvent, *mEventController );

	    pEvent.commonData.timeStamp = PerfCounter::queryCurrentStamp();

	    {
	        auto codeIndexValue = static_cast<size_t>( ecGetEventCodeCodeIndex( pEvent.code ) );
	        auto & eventHandler = mPrivate->handlerMapByCodeIndex[codeIndexValue];
	        if ( eventHandler && eventHandler( pEvent ) )
	        {
	            return;
	        }
	    }
	    {
	        auto categoryValue = static_cast<size_t>( ecGetEventCodeCategory( pEvent.code ) );
	        auto & eventHandler = mPrivate->handlerMapByCategory[categoryValue];
	        if ( eventHandler && eventHandler( pEvent ) )
	        {
	            return;
	        }
	    }
	    {
	        auto baseTypeValue = static_cast<size_t>( ecGetEventCodeBaseType( pEvent.code ) );
	        auto & eventHandler = mPrivate->handlerMapByCategory[baseTypeValue];
	        if ( eventHandler && eventHandler( pEvent ) )
	        {
	            return;
	        }
	    }
	    {
	        auto & defaultHandler = mPrivate->defaultHandler;
	        if ( defaultHandler && defaultHandler( pEvent ) )
	        {
	            return;
	        }
	    }

	    // warn?
	}

	void EventDispatcher::postEvent( event_code_value_t pEventCode )
	{
	    EventObject eventObject;
	    eventObject.code = static_cast<EEventCode>( pEventCode );
	    postEvent( eventObject );
	}

	void EventDispatcher::postEventAppQuit()
	{
	    postEvent( E_EVENT_CODE_APP_ACTIVITY_QUIT );
	}

	void EventDispatcher::postEventAppTerminate()
	{
	    postEvent( E_EVENT_CODE_APP_ACTIVITY_TERMINATE );
	}


	void _internalEventPreProcess( EventObject & pEvent, EventController & pEventController )
	{
	    // For INPUT_MOUSE_BUTTON events, we may want to modify event's data depending on the input configuration.
	    // _evtImplOnInputMouseButton() checks the input config and handles, for example, multi-click sequences.
	    if ( pEvent.code == E_EVENT_CODE_INPUT_MOUSE_BUTTON )
	    {
	        _internalEventOnInputMouseButton( pEvent, *( pEventController.mPrivate->currentInternalConfig ), pEventController.mPrivate->inputState );

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
