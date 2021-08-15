
#include "eventCore.h"

namespace ts3
{

	EventController::EventController( ContextHandle pContext ) noexcept
	: BaseObject( pContext )
	, _activeDispatcher( nullptr )
	{}

	EventController::~EventController() noexcept
	{
		_sysRelease();
	}

	EventControllerHandle EventController::create( ContextHandle pContext )
	{
		auto eventController =  createSysObject<EventController>( pContext );

		eventController->_defaultDispatcher =  createSysObject<EventDispatcher>( eventController );
		eventController->_activeDispatcher = eventController->_defaultDispatcher.get();

		eventController->_sysInitialize();

		return eventController;
	}

	EventDispatcherHandle EventController::createEventDispatcher()
	{
		return EventDispatcher::create( getHandle<EventController>() );
	}

	void EventController::dispatchEvent( Event & pEvent )
	{
		_activeDispatcher->internalPutEvent( pEvent );
	}

	void EventController::processEvent()
	{
		ts3DebugAssert( _activeDispatcher );
		if( _activeDispatcher->checkIdleProcessingMode() )
		{
			_sysDispatchNextEvent();
		}
		else
		{
			_sysDispatchNextEventWait();
		}
	}

	void EventController::processQueue()
	{
		ts3DebugAssert( _activeDispatcher );
		if( _activeDispatcher->checkIdleProcessingMode() )
		{
			_sysDispatchQueuedEvents();
		}
		else
		{
			_sysDispatchQueuedEventsWait();
		}
	}

	void EventController::dispatchNextEvent()
	{
		ts3DebugAssert( _activeDispatcher );
		_sysDispatchNextEvent();
	}

	void EventController::dispatchNextEventWait()
	{
		ts3DebugAssert( _activeDispatcher );
		_sysDispatchNextEventWait();
	}

	void EventController::dispatchQueuedEvents()
	{
		ts3DebugAssert( _activeDispatcher );
		_sysDispatchQueuedEvents();
	}

	void EventController::dispatchQueuedEventsWait()
	{
		ts3DebugAssert( _activeDispatcher );
		_sysDispatchQueuedEventsWait();
	}

	void EventController::addEventSource( EventSource & pEventSource )
	{
		_sysAddEventSource( pEventSource );
	}

	void EventController::removeEventSource( EventSource & pEventSource )
	{
		_sysRemoveEventSource( pEventSource );
	}

	void EventController::setActiveDispatcher( EventDispatcher & pDispatcher )
	{
		_activeDispatcher = &pDispatcher;
	}

	void EventController::resetActiveDispatcher()
	{
		_activeDispatcher = _defaultDispatcher.get();
	}

	EventDispatcher & EventController::getActiveDispatcher() const
	{
		return *_activeDispatcher;
	}

	EventDispatcher & EventController::getDefaultDispatcher() const
	{
		return *_defaultDispatcher;
	}


	EventDispatcher::EventDispatcher( EventControllerHandle pEventController ) noexcept
	: BaseObject( pEventController->mContext )
	, mEventController( pEventController )
	{
		_inputState.scNativeData = &( mEventController->mContext->mNativeData );
	}

	EventDispatcher::~EventDispatcher() noexcept = default;

	EventDispatcherHandle EventDispatcher::create( EventControllerHandle pEventController )
	{
		auto eventDispatcher =  createSysObject<EventDispatcher>( pEventController );
		return eventDispatcher;
	}

	void EventDispatcher::bindEventHandler( EEventBaseType pBaseType, EventHandler pHandler )
	{
		// EventBaseType enum is numbered from zero to have constant access time to its entry.
		auto baseTypeValue = static_cast<size_t>( pBaseType );
		// Check for possible violation attempt.
		if ( baseTypeValue >= cvEnumEventBaseTypeCount )
		{
			throw 0;
		}

		_handlerMapByBaseType[baseTypeValue] = std::move( pHandler );
	}

	void EventDispatcher::bindEventHandler( EEventCategory pCategory, EventHandler pHandler )
	{
		// EventCategory enum is numbered from zero to have constant access time to its entry.
		auto categoryValue = static_cast<size_t>( pCategory );
		// Check for possible violation attempt.
		if ( categoryValue >= cvEnumEventCategoryCount )
		{
			throw 0;
		}

		_handlerMapByCategory[categoryValue] = std::move( pHandler );
	}

	void EventDispatcher::bindEventHandler( EEventCodeIndex pCodeIndex, EventHandler pHandler )
	{
		// EventCodeIndex enum is numbered from zero to have constant access time to its entry.
		auto codeIndexValue = static_cast<size_t>( pCodeIndex );
		// Check for possible violation attempt.
		if ( codeIndexValue >= cvEnumEventCodeIndexCount )
		{
			throw 0;
		}

		_handlerMapByCodeIndex[codeIndexValue] = std::move( pHandler );
	}

	void EventDispatcher::bindDefaultEventHandler( EventHandler pHandler )
	{
		_defaultHandler = std::move( pHandler );
	}

	void EventDispatcher::postEvent( Event pEvent )
	{
		internalPutEvent( pEvent );
	}

	void EventDispatcher::postSimpleEvent( event_code_value_t pEventCode )
	{
		Event evtObject;
		evtObject.code = static_cast<EEventCode>( pEventCode );
		internalPutEvent( evtObject );
	}

	void EventDispatcher::postEventAppQuit()
	{
		return postSimpleEvent( E_EVENT_CODE_APP_ACTIVITY_QUIT );
	}

	void EventDispatcher::postEventAppTerminate()
	{
		return postSimpleEvent( E_EVENT_CODE_APP_ACTIVITY_TERMINATE );
	}

	void EventDispatcher::setIdleProcessingMode( bool pIdle )
	{
		_inputState.flags.setOrUnset( pIdle, E_EVENT_CONFIG_FLAG_IDLE_PROCESSING_MODE_BIT );
	}

	bool EventDispatcher::checkIdleProcessingMode() const
	{
		return _inputState.flags.isSet( E_EVENT_CONFIG_FLAG_IDLE_PROCESSING_MODE_BIT );
	}

	EventInputState & EventDispatcher::getInputState()
	{
		return _inputState;
	}

	void EventDispatcher::internalPutEvent( Event & pEvent )
	{
		_preProcessEvent( pEvent );
		_putEvent( pEvent );
	}

	void _internalEventOnInputMouseButton( Event & pEvent, EventInputState & pInputState );
	void _internalEventOnInputMouseButtonMulticlick( EvtInputMouseButton & pMouseButtonEvent, EventInputState & pInputState );

	void EventDispatcher::_preProcessEvent( Event & pEvent )
	{
		// For INPUT_MOUSE_BUTTON events, we may want to modify event's data depending on the input configuration.
		// _evtImplOnInputMouseButton() checks the input config and handles, for example, multi-click sequences.
		if ( pEvent.code == E_EVENT_CODE_INPUT_MOUSE_BUTTON )
		{
			_internalEventOnInputMouseButton( pEvent, _inputState );

		}
	}

	void EventDispatcher::_putEvent( Event & pEvent )
	{
		pEvent.commonData.timeStamp = PerfCounter::queryCurrentStamp();
		{
			auto codeIndexValue = static_cast<size_t>( ExfEnumGetEventCodeIndex( pEvent.code ) );
			auto & eventHandler = _handlerMapByCodeIndex[codeIndexValue];
			if ( eventHandler && eventHandler( pEvent ) )
			{
				return;
			}
		}
		{
			auto categoryValue = static_cast<size_t>( ExfEnumGetEventCodeCategory( pEvent.code ) );
			auto & eventHandler = _handlerMapByCategory[categoryValue];
			if ( eventHandler && eventHandler( pEvent ) )
			{
				return;
			}
		}
		{
			auto baseTypeValue = static_cast<size_t>( ExfEnumGetEventCodeBaseType( pEvent.code ) );
			auto & eventHandler = _handlerMapByCategory[baseTypeValue];
			if ( eventHandler && eventHandler( pEvent ) )
			{
				return;
			}
		}
		{
			auto & defaultHandler = _defaultHandler;
			if ( defaultHandler && defaultHandler( pEvent ) )
			{
				return;
			}
		}
	}

	void _internalEventOnInputMouseButton( Event & pEvent, EventInputState & pInputState )
	{
		if ( pInputState.flags.isSet( E_EVENT_CONFIG_FLAG_ENABLE_MOUSE_DOUBLE_CLICK_BIT ) )
		{
			auto & eInputMouseButton = pEvent.eInputMouseButton;
			if ( eInputMouseButton.buttonAction == MouseButtonActionType::Click )
			{
				_internalEventOnInputMouseButtonMulticlick( eInputMouseButton, pInputState );
			}
		}
	}

	void _internalEventOnInputMouseButtonMulticlick( EvtInputMouseButton & pMouseButtonEvent, EventInputState & pInputState )
	{
		bool multiClickEventSet = false;

		if ( pInputState.mouseLastPressButton == pMouseButtonEvent.buttonID )
		{
			auto lastClickDiff = pMouseButtonEvent.timeStamp - pInputState.mouseLastPressTimestamp;
			auto lastClickDiffMs = PerfCounter::convertToDuration<DurationPeriod::Millisecond>( lastClickDiff );
			if ( lastClickDiffMs <= pInputState.mouseClickSequenceTimeoutMs )
			{
				if( pInputState.mouseClickSequenceLength == 1 )
				{
					pMouseButtonEvent.buttonAction = MouseButtonActionType::DoubleClick;
					pInputState.mouseClickSequenceLength = 2;
					multiClickEventSet = true;
				}
				else if ( pInputState.flags.isSet( E_EVENT_CONFIG_FLAG_ENABLE_MOUSE_MULTI_CLICK_BIT ) )
				{
					pMouseButtonEvent.buttonAction = MouseButtonActionType::MultiClick;
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

}
