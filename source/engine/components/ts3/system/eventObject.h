
#ifndef __TS3_SYSTEM_EVENT_OBJECT_H__
#define __TS3_SYSTEM_EVENT_OBJECT_H__

#include "eventCoreTypes.h"

namespace ts3::system
{

	struct EventObject
	{
		union
		{
			EEventCode                 code;
			EvtBase                    commonData;
			EvtApp<0>                  eApp;
			EvtAppActivityDisplayReady eAppActivityDisplayReady;
			EvtAppActivityFocusGained  eAppActivityFocusGained;
			EvtAppActivityFocusLost    eAppActivityFocusLost;
			EvtAppActivityPause        eAppActivityPause;
			EvtAppActivityResume       eAppActivityResume;
			EvtAppActivityStart        eAppActivityStart;
			EvtAppActivityStop         eAppActivityStop;
			EvtAppActivityQuit         eAppActivityQuit;
			EvtAppActivityTerminate    eAppActivityTerminate;
			EvtInput<0>                eInput;
			EvtInputGamepadAxis        eInputGamepadAxis;
			EvtInputGamepadButton      eInputGamepadButton;
			EvtInputGamepadState       eInputGamepadState;
			EvtInputKeyboardKey        eInputKeyboardKey;
			EvtInputMouseButton        eInputMouseButton;
			EvtInputMouseMove          eInputMouseMove;
			EvtInputMouseScroll        eInputMouseScroll;
			EvtInputTouchDown          eInputTouchDown;
			EvtInputTouchMove          eInputTouchMove;
			EvtInputTouchUp            eInputTouchUp;
			EvtWindow<0>               eWindow;
			EvtWindowUpdateClose       eWindowUpdateClose;
			EvtWindowUpdateFullscreen  eWindowUpdateFullscreen;
			EvtWindowUpdateResize      eWindowUpdateResize;
			EvtWindowUpdateVisibility  eWindowUpdateVisibility;
		};

		constexpr EventObject()
		: code{ E_EVENT_CODE_UNDEFINED }
		{}

		constexpr explicit EventObject( EEventCode pEventCode )
		: code{ pEventCode }
		{}

		constexpr explicit EventObject( event_code_value_t pEventCode )
		: code{ static_cast<EEventCode>( pEventCode ) }
		{}

		constexpr explicit operator bool() const
		{
		    return ( code != E_EVENT_CODE_UNDEFINED ) && ecValidateEventCode( code );
		}
	};

} // namespace ts3::system

#endif // __TS3_SYSTEM_EVENT_OBJECT_H__
