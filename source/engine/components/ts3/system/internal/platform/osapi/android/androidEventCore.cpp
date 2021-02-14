
#include <ts3/system/internal/internalEventCore.h>
#include <android/input.h>
#include <android/keycodes.h>

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_ANDROID )
namespace ts3
{

    void _androidOnAppCommand( AndroidAppState * pAppState, int32_t pCommand );
    int32_t _androidOnInputEvent( AndroidAppState * pAppState, AInputEvent * pInputEvent );
	bool _androidTranslateAndDispatch( SysEventController & pEventController, ANativeEvent * pANativeEvent );
    bool _androidTranslateAppCommand( AndroidAppState * pAppState, int32_t pCommand, SysEvent & pEvent );
    bool _androidTranslateInputEvent( AndroidAppState * pAppState, AInputEvent * pInputEvent, SysEvent & pEvent );
    bool _androidTranslateInputEventKey( AndroidAppState * pAppState, AInputEvent * pInputEvent, SysEvent & pEvent );
    bool _androidTranslateInputEventTouch( AndroidAppState * pAppState, AInputEvent * pInputEvent, SysEvent & pEvent );


	void SysEventController::_sysInitialize()
	{
		auto * appState = mSystemContext->nativeData.androidAppState;
		appState->onAppCmd = _androidOnAppCommand;
		appState->onInputEvent = _androidOnInputEvent;
		appState->setUserData( SYS_ANDROID_APP_STATE_USER_DATA_INDEX_EVENT_CONTROLLER, &pEventController );
	}

	void SysEventController::_sysRelease()
	{}

	void SysEventController::_sysAddEventSource( SysEventSource & pEventSource )
	{}

	void SysEventController::_sysRemoveEventSource( SysEventSource & pEventSource )
	{}

	void SysEventController::_sysDispatchNextEvent()
	{
		// Note for Android event dispatching:
		// We use a modified native_app_glue code from the NDK to handle the Activity <--> C++ flow.
		// AndroidAppState (modified android_app struct) is the basic entity used to store any internal
		// data (to avoid introducing our custom types into the low-level android stuff).

		// AndroidAppState is always obtainable through SysContext - which is accessible everywhere.
		auto * appState = pEventController.systemContext->nativeData->androidAppState;

		int events = 0;
		AndroidPollSource * source = nullptr;

		// Poll single event without any wait ("peek mode"). We wil get the source in case of success.
		if( ALooper_pollOnce( cvAndroidLooperWaitTimeoutImmediate, nullptr, &events, reinterpret_cast<void**>( &source ) ) >= 0 )
		{
			if( source != nullptr )
			{
				// Process the dispatched event. This will be handled by the internal Android layer.
				// It uses two separate callbacks - one for general app commands and one for input
				// events (see nativeInitializeEventController() above, where these are set).
				source->process( appState, source );
			}
		}

		if( appState->destroyRequested != 0 )
		{
			// A bit of a hack, but we want to support a destroy request explicitly. There is no
			// dedicated event for that, but is signaled through the 'destroyRequested' variable.
			// So we use our own additional APP_CMD_USER_DESTROY_REQUESTED event code and artificially
			// emit it when the destroyRequested is set to true (the value changes *after* process()
			// is executed on a source).
			_androidOnAppCommand( appState, APP_CMD_USER_DESTROY_REQUESTED );
		}
	}

	void SysEventController::_sysDispatchNextEventWait()
	{
		auto * appState = pEventController.systemContext->nativeData->androidAppState;
		int events = 0;
		AndroidPollSource * source = nullptr;

		// Same thing as inside nativeFetchNextEvent, but here we block if there is no event in the queue.
		if( ALooper_pollOnce( cvAndroidLooperWaitTimeoutInfinity, nullptr, &events, reinterpret_cast<void**>( &source ) ) >= 0 )
		{
			if( source != nullptr )
			{
				source->process( appState, source );
			}
		}

		if( appState->destroyRequested != 0 )
		{
			_androidOnAppCommand( appState, APP_CMD_USER_DESTROY_REQUESTED );
		}
	}

	void SysEventController::_sysDispatchQueuedEvents()
	{
		auto * appState = pEventController.systemContext->nativeData->androidAppState;
		int events = 0;
		AndroidPollSource * source = nullptr;

		// Same thing as inside nativeFetchNextEvent, but we poll all events instead of only the next one.
		if( ALooper_pollAll( cvAndroidLooperWaitTimeoutImmediate, nullptr, &events, reinterpret_cast<void**>( &source ) ) >= 0 )
		{
			if( source != nullptr )
			{
				source->process( appState, source );
			}
		}

		if( appState->destroyRequested != 0 )
		{
			_androidOnAppCommand( appState, APP_CMD_USER_DESTROY_REQUESTED );
		}
	}

	void SysEventController::_sysDispatchQueuedEventsWait()
	{
		auto * appState = pEventController.systemContext->nativeData->androidAppState;
		int events = 0;
		AndroidPollSource * source = nullptr;

		// Same thing as inside nativeFetchQueue, but here we block if there are no events in the queue.
		if( ALooper_pollAll( cvAndroidLooperWaitTimeoutInfinity, nullptr, &events, reinterpret_cast<void**>( &source ) ) >= 0 )
		{
			if( source != nullptr )
			{
				source->process( appState, source );
			}
		}

		if( appState->destroyRequested != 0 )
		{
			_androidOnAppCommand( appState, APP_CMD_USER_DESTROY_REQUESTED );
		}
	}


    void _androidOnAppCommand( AndroidAppState * pAppState, int32_t pCommand )
    {
        ANativeEvent androidEvent {};
        androidEvent.type = ANativeEventType::AppCommand;
        androidEvent.eAppCommand = pCommand;

        // Event controller is always accessible through the user data within AndroidAppState.
        // We set it inside nativeInitializeEventController(). Events may be emitted after the
        // framework state is released, so the null-check is necessary.
        if( auto * eventController = pAppState->getUserDataAs<SysEventController>( SYS_ANDROID_APP_STATE_USER_DATA_INDEX_EVENT_CONTROLLER ) )
        {
            SysEventCoreImplProxy::internalTranslateAndDispatch( *eventController, androidEvent );
        }
    }

    int32_t _androidOnInputEvent( AndroidAppState * pAppState, AInputEvent * pInputEvent )
    {
        ANativeEvent androidEvent {};
        androidEvent.type = ANativeEventType::Input;
        androidEvent.eInputEvent = pInputEvent;

        if( auto * eventController = pAppState->getUserDataAs<SysEventController>( SYS_ANDROID_APP_STATE_USER_DATA_INDEX_EVENT_CONTROLLER ) )
        {
            if( SysEventCoreImplProxy::internalTranslateAndDispatch( *eventController, androidEvent ) )
            {
                return 1;
            }
        }

        return 0;
    }

	bool _androidTranslateAndDispatch( SysEventController & pEventController, ANativeEvent * pANativeEvent )
	{
		auto * androidAppState = pEventController.systemContext->nativeData->androidAppState;

		SysEvent sysEvent;

		if( pNativeEvent.type == ANativeEventType::AppCommand )
		{
			if( !_androidTranslateAppCommand( androidAppState, pNativeEvent.eAppCommand, sysEvent ) )
			{
				return false;
			}
		}
		else if( pNativeEvent.type == ANativeEventType::Input )
		{
			if( !_androidTranslateInputEvent( androidAppState, pNativeEvent.eInputEvent, sysEvent ) )
			{
				return false;
			}
		}

		pEventController.dispatchEvent( sysEvent );

		return false;
	}

    bool _androidTranslateAppCommand( AndroidAppState * pAppState, int32_t pCommand, SysEvent & pEvent )
    {
        pEvent.code = E_SYS_EVENT_CODE_UNDEFINED;

        switch( pCommand )
        {
            case APP_CMD_INPUT_CHANGED:
            {
                break;
            }
            case APP_CMD_INIT_WINDOW:
            {
                if( pAppState->window != nullptr )
                {
                    pEvent.code = E_SYS_EVENT_CODE_APP_ACTIVITY_DISPLAY_READY;

                }
                break;
            }
            case APP_CMD_TERM_WINDOW:
            {
                pEvent.code = E_SYS_EVENT_CODE_WINDOW_UPDATE_CLOSE;
                break;
            }
            case APP_CMD_WINDOW_RESIZED:
            {
                pEvent.code = E_SYS_EVENT_CODE_WINDOW_UPDATE_RESIZE;
                break;
            }
            case APP_CMD_WINDOW_REDRAW_NEEDED:
            case APP_CMD_CONTENT_RECT_CHANGED:
            {
                break;
            }
            case APP_CMD_GAINED_FOCUS:
            {
                pEvent.code = E_SYS_EVENT_CODE_APP_ACTIVITY_FOCUS_GAINED;
                break;
            }
            case APP_CMD_LOST_FOCUS:
            {
                pEvent.code = E_SYS_EVENT_CODE_APP_ACTIVITY_FOCUS_LOST;
                break;
            }
            case APP_CMD_CONFIG_CHANGED:
            case APP_CMD_LOW_MEMORY:
            {
                break;
            }
            case APP_CMD_START:
            {
                pEvent.code = E_SYS_EVENT_CODE_APP_ACTIVITY_START;
                break;
            }
            case APP_CMD_RESUME:
            {
                pEvent.code = E_SYS_EVENT_CODE_APP_ACTIVITY_RESUME;
                break;
            }
            case APP_CMD_SAVE_STATE:
            {
                break;
            }
            case APP_CMD_PAUSE:
            {
                pEvent.code = E_SYS_EVENT_CODE_APP_ACTIVITY_PAUSE;
                break;
            }
            case APP_CMD_STOP:
            {
                pEvent.code = E_SYS_EVENT_CODE_APP_ACTIVITY_STOP;
                break;
            }
            case APP_CMD_DESTROY:
            {
                pEvent.code = E_SYS_EVENT_CODE_APP_ACTIVITY_QUIT;
                break;
            }
            case APP_CMD_USER_DESTROY_REQUESTED:
            {
                pEvent.code = E_SYS_EVENT_CODE_APP_ACTIVITY_TERMINATE;
                break;
            }
            default:
            {
                break;
            }
        }

        return false;
    }

    bool _androidTranslateInputEvent( AndroidAppState * pAppState, AInputEvent * pInputEvent, SysEvent & pEvent )
    {
        auto eventType = AInputEvent_getType( pInputEvent );

        if( eventType == AINPUT_EVENT_TYPE_KEY )
        {
            return _androidTranslateInputEventKey( pAppState, pInputEvent, pEvent );
        }
        else if( eventType == AINPUT_EVENT_TYPE_MOTION )
        {
            return _androidTranslateInputEventTouch( pAppState, pInputEvent, pEvent );
        }

        return false;
    }

    bool _androidTranslateInputEventKey( AndroidAppState * pAppState, AInputEvent * pInputEvent, SysEvent & pEvent )
    {
        return false;
    }

    bool _androidTranslateInputEventTouch( AndroidAppState * pAppState, AInputEvent * pInputEvent, SysEvent & pEvent )
    {
        return false;
    }
    
}
#endif
