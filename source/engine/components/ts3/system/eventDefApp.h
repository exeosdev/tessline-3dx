
#ifndef __TS3_SYSTEM_EVENT_DEF_APP_H__
#define __TS3_SYSTEM_EVENT_DEF_APP_H__

#include "eventCommon.h"

namespace ts3::system
{

	struct EvtApp : public EvtBase
	{
	};

	struct EvtAppActivityDisplayInit : public EvtApp
	{
	};

	struct EvtAppActivityDisplayTerm : public EvtApp
	{
	};

	struct EvtAppActivityFocusGained : public EvtApp
	{
	};

	struct EvtAppActivityFocusLost : public EvtApp
	{
	};

	struct EvtAppActivityPause : public EvtApp
	{
	};

	struct EvtAppActivityResume : public EvtApp
	{
	};

	struct EvtAppActivityStart : public EvtApp
	{
	};

	struct EvtAppActivityStop : public EvtApp
	{
	};

	struct EvtAppActivityQuit : public EvtApp
	{
	};

	struct EvtAppActivityTerminate : public EvtApp
	{
	};

} // namespace ts3::system

#endif // __TS3_SYSTEM_EVENT_DEF_APP_H__
