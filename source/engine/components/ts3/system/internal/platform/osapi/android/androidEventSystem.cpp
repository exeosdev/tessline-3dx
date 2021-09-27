
#include <ts3/system/eventSystemNative.h>
#include <ts3/system/sysContextNative.h>
#include <android/input.h>
#include <android/keycodes.h>

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_ANDROID )
namespace ts3::system
{

    void _androidOnAppCommand( AndroidAppState * pAppState, int32_t pCommand );
    int32_t _androidOnInputEvent( AndroidAppState * pAppState, AInputEvent * pInputEvent );
    bool _androidTranslateAppCommand( AndroidAppState * pAppState, int32_t pCommand, EventObject & pOutEvent );
    bool _androidTranslateInputEvent( AndroidAppState * pAppState, AInputEvent * pInputEvent, EventObject & pOutEvent );
    bool _androidTranslateInputEventKey( AndroidAppState * pAppState, AInputEvent * pInputEvent, EventObject & pOutEvent );
    bool _androidTranslateInputEventTouch( AndroidAppState * pAppState, AInputEvent * pInputEvent, EventObject & pOutEvent );


    void EventController::_nativeConstructor()
    {
        auto & aSessionData = nativeAndroidGetASessionData( *mSysContext );

        auto * aCommonAppState = aSessionData.aCommonAppState;
        aCommonAppState->onAppCmd = _androidOnAppCommand;
        aCommonAppState->onInputEvent = _androidOnInputEvent;
        aCommonAppState->ts3SetUserData( E_ANDROID_APP_STATE_USER_DATA_INDEX_EVENT_CONTROLLER, this );
    }

    void EventController::_nativeDestructor() noexcept
    {}

    void EventController::_nativeRegisterEventSource( EventSource & /* pEventSource */ )
    {}

    void EventController::_nativeUnregisterEventSource( EventSource & /* pEventSource */ )
    {}

    void EventController::_nativeDestroyEventSourcePrivateData( EventSource & /* pEventSource */, void * /* pData */ )
    {}

    bool EventController::_nativeUpdateSysQueue()
	{
		// Note for Android event dispatching:
		// We use a modified native_app_glue code from the NDK to handle the Activity <--> C++ flow.
		// AndroidAppState (modified android_app struct) is the basic entity used to store any internal
		// data (to avoid introducing our custom types into the low-level android stuff).

        auto & aSessionData = nativeAndroidGetASessionData( *mSysContext );
        auto * aCommonAppState = aSessionData.aCommonAppState;

		int events = 0;
		AndroidPollSource * source = nullptr;

		// Poll single event without any wait ("peek mode"). We will get the source in case of success.
		auto pollResult = ALooper_pollOnce( CX_ANDROID_EVENT_LOOPER_WAIT_TIMEOUT_IMMEDIATE,
                                            nullptr,
                                            &events,
                                            reinterpret_cast<void**>( &source ) );

		if( pollResult >= 0 )
		{
			if( source != nullptr )
			{
				// Process the dispatched event. This will be handled by the internal Android layer.
				// It uses two separate callbacks - one for general app commands and one for input
				// events (see nativeInitializeEventController() above, where these are set).
				source->process( aCommonAppState, source );

                if( aCommonAppState->destroyRequested != 0 )
                {
                    // A bit of a hack, but we want to support a destroy request explicitly. There is no
                    // dedicated event for that, but is signaled through the 'destroyRequested' variable.
                    // So we use our own additional E_ANDROID_EVT_CMD_USER_DESTROY_REQUESTED event code
                    // and artificially emit it when the destroyRequested is set to true (the value changes
                    // *after* process() is executed on a source).
                    _androidOnAppCommand( aCommonAppState, E_ANDROID_EVT_CMD_USER_DESTROY_REQUESTED );
                }

                return true;
			}
		}

		return false;
	}

    bool EventController::_nativeUpdateSysQueueWait()
	{
        auto & aSessionData = nativeAndroidGetASessionData( *mSysContext );
        auto * aCommonAppState = aSessionData.aCommonAppState;

		int events = 0;
		AndroidPollSource * source = nullptr;

		// Same thing as inside nativeFetchNextEvent, but here we block if there is no event in the queue.
        auto pollResult = ALooper_pollOnce( CX_ANDROID_EVENT_LOOPER_WAIT_TIMEOUT_INFINITY,
                                            nullptr,
                                            &events,
                                            reinterpret_cast<void**>( &source ) );

        if( pollResult >= 0 )
		{
			if( source != nullptr )
			{
				source->process( aCommonAppState, source );

                if( aCommonAppState->destroyRequested != 0 )
                {
                    _androidOnAppCommand( aCommonAppState, E_ANDROID_EVT_CMD_USER_DESTROY_REQUESTED );
                }

                return true;
			}
		}

        return false;
	}

    void EventController::_nativeOnActiveDispatcherChange( EventDispatcher * /* pDispatcher */ )
    {}


    void _androidOnAppCommand( AndroidAppState * pAppState, int32_t pCommand )
    {
        ANativeEvent androidEvent {};
        androidEvent.type = ANativeEventType::AppCommand;
        androidEvent.eAppCommand = pCommand;

        // Event controller is always accessible through the user data within AndroidAppState.
        // We set it inside nativeInitializeEventController(). Events may be emitted after the
        // framework state is released, so the null-check is necessary.
        if( auto * eventController = pAppState->ts3GetUserDataAs<EventController>( E_ANDROID_APP_STATE_USER_DATA_INDEX_EVENT_CONTROLLER ) )
        {
            nativeEventDispatch( *eventController, androidEvent );
        }
    }

    int32_t _androidOnInputEvent( AndroidAppState * pAppState, AInputEvent * pInputEvent )
    {
        ANativeEvent androidEvent {};
        androidEvent.type = ANativeEventType::Input;
        androidEvent.eInputEvent = pInputEvent;

        if( auto * eventController = pAppState->ts3GetUserDataAs<EventController>( E_ANDROID_APP_STATE_USER_DATA_INDEX_EVENT_CONTROLLER ) )
        {
            if( nativeEventDispatch( *eventController, androidEvent ) )
            {
                return 1;
            }
        }

        return 0;
    }

    bool nativeEventTranslate( EventController & pEventController, const NativeEvent & pNativeEvent, EventObject & pOutEvent )
    {
        auto & aSessionData = nativeAndroidGetASessionData( *( pEventController.mSysContext ) );
        auto * aCommonAppState = aSessionData.aCommonAppState;

		if( pNativeEvent.type == ANativeEventType::AppCommand )
		{
			if( _androidTranslateAppCommand( aCommonAppState, pNativeEvent.eAppCommand, pOutEvent ) )
			{
				return true;
			}
		}
		else if( pNativeEvent.type == ANativeEventType::Input )
		{
			if( _androidTranslateInputEvent( aCommonAppState, pNativeEvent.eInputEvent, pOutEvent ) )
			{
				return true;
			}
		}

		return false;
	}

    bool _androidTranslateAppCommand( AndroidAppState * pAppState, int32_t pCommand, EventObject & pOutEvent )
    {
        pOutEvent.code = E_EVENT_CODE_UNDEFINED;

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
                    auto * sysContext = pAppState->ts3GetUserDataAs<SysContext>( E_ANDROID_APP_STATE_USER_DATA_INDEX_SYS_CONTEXT );
                    sysContext->mInternal->nativeDataPriv.aSessionData.aNativeWindow = pAppState->window;
                    pOutEvent.code = E_EVENT_CODE_APP_ACTIVITY_DISPLAY_READY;
                }
                break;
            }
            case APP_CMD_TERM_WINDOW:
            {
                pOutEvent.code = E_EVENT_CODE_WINDOW_UPDATE_CLOSE;
                break;
            }
            case APP_CMD_WINDOW_RESIZED:
            {
                pOutEvent.code = E_EVENT_CODE_WINDOW_UPDATE_RESIZE;
                break;
            }
            case APP_CMD_WINDOW_REDRAW_NEEDED:
            case APP_CMD_CONTENT_RECT_CHANGED:
            {
                break;
            }
            case APP_CMD_GAINED_FOCUS:
            {
                pOutEvent.code = E_EVENT_CODE_APP_ACTIVITY_FOCUS_GAINED;
                break;
            }
            case APP_CMD_LOST_FOCUS:
            {
                pOutEvent.code = E_EVENT_CODE_APP_ACTIVITY_FOCUS_LOST;
                break;
            }
            case APP_CMD_CONFIG_CHANGED:
            case APP_CMD_LOW_MEMORY:
            {
                break;
            }
            case APP_CMD_START:
            {
                pOutEvent.code = E_EVENT_CODE_APP_ACTIVITY_START;
                break;
            }
            case APP_CMD_RESUME:
            {
                pOutEvent.code = E_EVENT_CODE_APP_ACTIVITY_RESUME;
                break;
            }
            case APP_CMD_SAVE_STATE:
            {
                break;
            }
            case APP_CMD_PAUSE:
            {
                pOutEvent.code = E_EVENT_CODE_APP_ACTIVITY_PAUSE;
                break;
            }
            case APP_CMD_STOP:
            {
                pOutEvent.code = E_EVENT_CODE_APP_ACTIVITY_STOP;
                break;
            }
            case APP_CMD_DESTROY:
            {
                pOutEvent.code = E_EVENT_CODE_APP_ACTIVITY_QUIT;
                break;
            }
            case E_ANDROID_EVT_CMD_USER_DESTROY_REQUESTED:
            {
                pOutEvent.code = E_EVENT_CODE_APP_ACTIVITY_TERMINATE;
                break;
            }
            default:
            {
                break;
            }
        }

        return pOutEvent.code != E_EVENT_CODE_UNDEFINED;
    }

    bool _androidTranslateInputEvent( AndroidAppState * pAppState, AInputEvent * pInputEvent, EventObject & pOutEvent )
    {
        auto eventType = AInputEvent_getType( pInputEvent );

        if( eventType == AINPUT_EVENT_TYPE_KEY )
        {
            return _androidTranslateInputEventKey( pAppState, pInputEvent, pOutEvent );
        }
        else if( eventType == AINPUT_EVENT_TYPE_MOTION )
        {
            return _androidTranslateInputEventTouch( pAppState, pInputEvent, pOutEvent );
        }

        return false;
    }

    bool _androidTranslateInputEventKey( AndroidAppState * pAppState, AInputEvent * pInputEvent, EventObject & pOutEvent )
    {
        return false;
    }

    bool _androidTranslateInputEventTouch( AndroidAppState * pAppState, AInputEvent * pInputEvent, EventObject & pOutEvent )
    {
        return false;
    }
    
}
#endif