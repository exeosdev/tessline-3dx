
#ifndef __TS3_SYSTEM_EVENT_OBJECT_H__
#define __TS3_SYSTEM_EVENT_OBJECT_H__

#include "eventDefApp.h"
#include "eventDefInputKeyboard.h"
#include "eventDefInputMouse.h"
#include "eventDefInputTouch.h"
#include "eventDefWindow.h"

namespace ts3::system
{

	struct EventObject
	{
		union
		{
			EEventCode                 code;
			EvtBase                    commonData;
			EvtAppActivityDisplayInit  eAppActivityDisplayInit;
			EvtAppActivityDisplayTerm  eAppActivityDisplayTerm;
			EvtAppActivityFocusGained  eAppActivityFocusGained;
			EvtAppActivityFocusLost    eAppActivityFocusLost;
			EvtAppActivityPause        eAppActivityPause;
			EvtAppActivityResume       eAppActivityResume;
			EvtAppActivityStart        eAppActivityStart;
			EvtAppActivityStop         eAppActivityStop;
			EvtAppActivityQuit         eAppActivityQuit;
			EvtAppActivityTerminate    eAppActivityTerminate;
			EvtInputGamepadAxis        eInputGamepadAxis;
			EvtInputGamepadButton      eInputGamepadButton;
			EvtInputGamepadState       eInputGamepadState;
			EvtInputKeyboard           eInputKeyboard;
			EvtInputMouse              eInputMouse;
			EvtInputMouseButton        eInputMouseButton;
			EvtInputMouseMove          eInputMouseMove;
			EvtInputMouseScroll        eInputMouseScroll;
			EvtInputTouchDown          eInputTouchDown;
			EvtInputTouchMove          eInputTouchMove;
			EvtInputTouchUp            eInputTouchUp;
			EvtWindowUpdateCreate      eWindowUpdateCreate;
			EvtWindowUpdateDestroy     eWindowUpdateDestroy;
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

		 TS3_FUNC_NO_DISCARD constexpr EEventCategory category() const
		{
			return ecGetEventCodeCategory( code );
		}
	};

} // namespace ts3::system

#endif // __TS3_SYSTEM_EVENT_OBJECT_H__
