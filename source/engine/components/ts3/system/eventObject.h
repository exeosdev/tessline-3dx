
#ifndef __TS3_SYSTEM_EVENT_OBJECT_H__
#define __TS3_SYSTEM_EVENT_OBJECT_H__

#include "eventTypes.h"

namespace ts3::system
{

	struct Event
	{
		union
		{
			EEventCode                 code;
			EvtBase                    commonData;
			EvtAppActivityDisplayReady eAppActivityDisplayReady;
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
			EvtInputKeyboardKey        eInputKeyboardKey;
			EvtInputMouseButton        eInputMouseButton;
			EvtInputMouseMove          eInputMouseMove;
			EvtInputMouseScroll        eInputMouseScroll;
			EvtInputTouchDown          eInputTouchDown;
			EvtInputTouchMove          eInputTouchMove;
			EvtInputTouchUp            eInputTouchUp;
			EvtWindowUpdateClose       eWindowUpdateClose;
			EvtWindowUpdateFullscreen  eWindowUpdateFullscreen;
			EvtWindowUpdateResize      eWindowUpdateResize;
			EvtWindowUpdateVisibility  eWindowUpdateVisibility;
		};

		Event()
		: code{ E_EVENT_CODE_UNDEFINED }
		{}

		explicit operator bool() const
		{
			return code != E_EVENT_CODE_UNDEFINED;
		}
	};

} // namespace ts3::system

#endif // __TS3_SYSTEM_EVENT_OBJECT_H__
