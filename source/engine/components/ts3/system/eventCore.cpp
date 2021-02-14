
#include "eventCore.h"

namespace ts3
{

	SysEventController::SysEventController( SysContextHandle pSysContext ) noexcept
	: SysBaseObject( pSysContext )
	, _activeDispatcher( nullptr )
	{}

	SysEventController::~SysEventController() noexcept
	{
		_sysRelease();
	}

	SysEventControllerHandle SysEventController::create( SysContextHandle pSysContext )
	{
		auto eventController = sysCreateObject<SysEventController>( pSysContext );

		eventController->_defaultDispatcher = sysCreateObject<SysEventDispatcher>( eventController );
		eventController->_activeDispatcher = eventController->_defaultDispatcher.get();

		eventController->_sysInitialize();

		return eventController;
	}

	SysEventDispatcherHandle SysEventController::createEventDispatcher()
	{
		return SysEventDispatcher::create( getHandle<SysEventController>() );
	}

	void SysEventController::dispatchEvent( SysEvent & pEvent )
	{
		_activeDispatcher->internalPutEvent( pEvent );
	}

	void SysEventController::processEvent()
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

	void SysEventController::processQueue()
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

	void SysEventController::dispatchNextEvent()
	{
		ts3DebugAssert( _activeDispatcher );
		_sysDispatchNextEvent();
	}

	void SysEventController::dispatchNextEventWait()
	{
		ts3DebugAssert( _activeDispatcher );
		_sysDispatchNextEventWait();
	}

	void SysEventController::dispatchQueuedEvents()
	{
		ts3DebugAssert( _activeDispatcher );
		_sysDispatchQueuedEvents();
	}

	void SysEventController::dispatchQueuedEventsWait()
	{
		ts3DebugAssert( _activeDispatcher );
		_sysDispatchQueuedEventsWait();
	}

	void SysEventController::addEventSource( SysEventSource & pEventSource )
	{
		_sysAddEventSource( pEventSource );
	}

	void SysEventController::removeEventSource( SysEventSource & pEventSource )
	{
		_sysRemoveEventSource( pEventSource );
	}

	void SysEventController::setActiveDispatcher( SysEventDispatcher & pDispatcher )
	{
		_activeDispatcher = &pDispatcher;
	}

	void SysEventController::resetActiveDispatcher()
	{
		_activeDispatcher = _defaultDispatcher.get();
	}

	SysEventDispatcher & SysEventController::getActiveDispatcher() const
	{
		return *_activeDispatcher;
	}

	SysEventDispatcher & SysEventController::getDefaultDispatcher() const
	{
		return *_defaultDispatcher;
	}


	SysEventDispatcher::SysEventDispatcher( SysEventControllerHandle pEventController ) noexcept
	: SysBaseObject( pEventController->mSysContext )
	, mEventController( pEventController )
	{
		_inputState.scNativeData = &( mEventController->mSysContext->mNativeData );
	}

	SysEventDispatcher::~SysEventDispatcher() noexcept = default;

	SysEventDispatcherHandle SysEventDispatcher::create( SysEventControllerHandle pEventController )
	{
		auto eventDispatcher = sysCreateObject<SysEventDispatcher>( pEventController );
		return eventDispatcher;
	}

	void SysEventDispatcher::bindEventHandler( ESysEventBaseType pBaseType, SysEventHandler pHandler )
	{
		// EventBaseType enum is numbered from zero to have constant access time to its entry.
		auto baseTypeValue = static_cast<size_t>( pBaseType );
		// Check for possible violation attempt.
		if ( baseTypeValue >= cvSysEnumEventBaseTypeCount )
		{
			throw 0;
		}

		_handlerMapByBaseType[baseTypeValue] = std::move( pHandler );
	}

	void SysEventDispatcher::bindEventHandler( ESysEventCategory pCategory, SysEventHandler pHandler )
	{
		// EventCategory enum is numbered from zero to have constant access time to its entry.
		auto categoryValue = static_cast<size_t>( pCategory );
		// Check for possible violation attempt.
		if ( categoryValue >= cvSysEnumEventCategoryCount )
		{
			throw 0;
		}

		_handlerMapByCategory[categoryValue] = std::move( pHandler );
	}

	void SysEventDispatcher::bindEventHandler( ESysEventCodeIndex pCodeIndex, SysEventHandler pHandler )
	{
		// EventCodeIndex enum is numbered from zero to have constant access time to its entry.
		auto codeIndexValue = static_cast<size_t>( pCodeIndex );
		// Check for possible violation attempt.
		if ( codeIndexValue >= cvSysEnumEventCodeIndexCount )
		{
			throw 0;
		}

		_handlerMapByCodeIndex[codeIndexValue] = std::move( pHandler );
	}

	void SysEventDispatcher::bindDefaultEventHandler( SysEventHandler pHandler )
	{
		_defaultHandler = std::move( pHandler );
	}

	void SysEventDispatcher::postEvent( SysEvent pEvent )
	{
		internalPutEvent( pEvent );
	}

	void SysEventDispatcher::postSimpleEvent( sys_event_code_value_t pEventCode )
	{
		SysEvent evtObject;
		evtObject.code = static_cast<ESysEventCode>( pEventCode );
		internalPutEvent( evtObject );
	}

	void SysEventDispatcher::postEventAppQuit()
	{
		return postSimpleEvent( E_SYS_EVENT_CODE_APP_ACTIVITY_QUIT );
	}

	void SysEventDispatcher::postEventAppTerminate()
	{
		return postSimpleEvent( E_SYS_EVENT_CODE_APP_ACTIVITY_TERMINATE );
	}

	void SysEventDispatcher::setIdleProcessingMode( bool pIdle )
	{
		_inputState.flags.setOrUnset( pIdle, E_SYS_EVENT_CONFIG_FLAG_IDLE_PROCESSING_MODE_BIT );
	}

	bool SysEventDispatcher::checkIdleProcessingMode() const
	{
		return _inputState.flags.isSet( E_SYS_EVENT_CONFIG_FLAG_IDLE_PROCESSING_MODE_BIT );
	}

	SysEventInputState & SysEventDispatcher::getInputState()
	{
		return _inputState;
	}

	void SysEventDispatcher::internalPutEvent( SysEvent & pEvent )
	{
		_preProcessEvent( pEvent );
		_putEvent( pEvent );
	}

	void _internalEventOnInputMouseButton( SysEvent & pEvent, SysEventInputState & pInputState );
	void _internalEventOnInputMouseButtonMulticlick( SysEvtInputMouseButton & pMouseButtonEvent, SysEventInputState & pInputState );

	void SysEventDispatcher::_preProcessEvent( SysEvent & pEvent )
	{
		// For INPUT_MOUSE_BUTTON events, we may want to modify event's data depending on the input configuration.
		// _evtImplOnInputMouseButton() checks the input config and handles, for example, multi-click sequences.
		if ( pEvent.code == E_SYS_EVENT_CODE_INPUT_MOUSE_BUTTON )
		{
			_internalEventOnInputMouseButton( pEvent, _inputState );

		}
	}

	void SysEventDispatcher::_putEvent( SysEvent & pEvent )
	{
		pEvent.commonData.timeStamp = SysPerfCounter::queryCurrentStamp();
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

	void _internalEventOnInputMouseButton( SysEvent & pEvent, SysEventInputState & pInputState )
	{
		if ( pInputState.flags.isSet( E_SYS_EVENT_CONFIG_FLAG_ENABLE_MOUSE_DOUBLE_CLICK_BIT ) )
		{
			auto & eInputMouseButton = pEvent.eInputMouseButton;
			if ( eInputMouseButton.buttonAction == SysMouseButtonActionType::Click )
			{
				_internalEventOnInputMouseButtonMulticlick( eInputMouseButton, pInputState );
			}
		}
	}

	void _internalEventOnInputMouseButtonMulticlick( SysEvtInputMouseButton & pMouseButtonEvent, SysEventInputState & pInputState )
	{
		bool multiClickEventSet = false;

		if ( pInputState.mouseLastPressButton == pMouseButtonEvent.buttonID )
		{
			auto lastClickDiff = pMouseButtonEvent.timeStamp - pInputState.mouseLastPressTimestamp;
			auto lastClickDiffMs = SysPerfCounter::convertToDuration<DurationPeriod::Millisecond>( lastClickDiff );
			if ( lastClickDiffMs <= pInputState.mouseClickSequenceTimeoutMs )
			{
				if( pInputState.mouseClickSequenceLength == 1 )
				{
					pMouseButtonEvent.buttonAction = SysMouseButtonActionType::DoubleClick;
					pInputState.mouseClickSequenceLength = 2;
					multiClickEventSet = true;
				}
				else if ( pInputState.flags.isSet( E_SYS_EVENT_CONFIG_FLAG_ENABLE_MOUSE_MULTI_CLICK_BIT ) )
				{
					pMouseButtonEvent.buttonAction = SysMouseButtonActionType::MultiClick;
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
