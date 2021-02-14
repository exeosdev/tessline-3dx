
#ifndef __TS3_SYSTEM_EVENT_OBJECT_H__
#define __TS3_SYSTEM_EVENT_OBJECT_H__

#include "eventTypesDef.h"

namespace ts3
{

	struct SysEvent
	{
		union
		{
			ESysEventCode                 code;
			SysEvtBase                    commonData;
			SysEvtAppActivityDisplayReady eAppActivityDisplayReady;
			SysEvtAppActivityFocusGained  eAppActivityFocusGained;
			SysEvtAppActivityFocusLost    eAppActivityFocusLost;
			SysEvtAppActivityPause        eAppActivityPause;
			SysEvtAppActivityResume       eAppActivityResume;
			SysEvtAppActivityStart        eAppActivityStart;
			SysEvtAppActivityStop         eAppActivityStop;
			SysEvtAppActivityQuit         eAppActivityQuit;
			SysEvtAppActivityTerminate    eAppActivityTerminate;
			SysEvtInputGamepadAxis        eInputGamepadAxis;
			SysEvtInputGamepadButton      eInputGamepadButton;
			SysEvtInputGamepadState       eInputGamepadState;
			SysEvtInputKeyboardKey        eInputKeyboardKey;
			SysEvtInputMouseButton        eInputMouseButton;
			SysEvtInputMouseMove          eInputMouseMove;
			SysEvtInputMouseScroll        eInputMouseScroll;
			SysEvtInputTouchDown          eInputTouchDown;
			SysEvtInputTouchMove          eInputTouchMove;
			SysEvtInputTouchUp            eInputTouchUp;
			SysEvtWindowUpdateClose       eWindowUpdateClose;
			SysEvtWindowUpdateFullscreen  eWindowUpdateFullscreen;
			SysEvtWindowUpdateResize      eWindowUpdateResize;
			SysEvtWindowUpdateVisibility  eWindowUpdateVisibility;
		};

		SysEvent()
		: code{ E_SYS_EVENT_CODE_UNDEFINED }
		{}

		explicit operator bool() const
		{
			return code != E_SYS_EVENT_CODE_UNDEFINED;
		}
	};

}

#endif // __TS3_SYSTEM_EVENT_OBJECT_H__
