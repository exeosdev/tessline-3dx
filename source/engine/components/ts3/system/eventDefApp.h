
#ifndef __TS3_SYSTEM_EVENT_DEF_APP_H__
#define __TS3_SYSTEM_EVENT_DEF_APP_H__

#include "eventCommon.h"

namespace ts3::system
{

	template <event_code_value_t tpEventCode>
	struct EvtApp : public EvtBase
	{
	public:
		constexpr EvtApp()
		: EvtBase( tpEventCode )
		{}
	};

	struct EvtAppActivityDisplayInit : public EvtApp<E_EVENT_CODE_APP_ACTIVITY_DISPLAY_INIT>
	{
	};

	struct EvtAppActivityDisplayTerm : public EvtApp<E_EVENT_CODE_APP_ACTIVITY_DISPLAY_TERM>
	{
	};

	struct EvtAppActivityFocusGained : public EvtApp<E_EVENT_CODE_APP_ACTIVITY_FOCUS_GAINED>
	{
	};

	struct EvtAppActivityFocusLost : public EvtApp<E_EVENT_CODE_APP_ACTIVITY_FOCUS_LOST>
	{
	};

	struct EvtAppActivityPause : public EvtApp<E_EVENT_CODE_APP_ACTIVITY_PAUSE>
	{
	};

	struct EvtAppActivityResume : public EvtApp<E_EVENT_CODE_APP_ACTIVITY_RESUME>
	{
	};

	struct EvtAppActivityStart : public EvtApp<E_EVENT_CODE_APP_ACTIVITY_START>
	{
	};

	struct EvtAppActivityStop : public EvtApp<E_EVENT_CODE_APP_ACTIVITY_STOP>
	{
	};

	struct EvtAppActivityQuit : public EvtApp<E_EVENT_CODE_APP_ACTIVITY_QUIT>
	{
	};

	struct EvtAppActivityTerminate : public EvtApp<E_EVENT_CODE_APP_ACTIVITY_TERMINATE>
	{
	};

} // namespace ts3::system

#endif // __TS3_SYSTEM_EVENT_DEF_APP_H__
