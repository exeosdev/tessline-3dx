
#ifndef __TS3_SYSTEM_EVENT_DEF_WINDOW_H__
#define __TS3_SYSTEM_EVENT_DEF_WINDOW_H__

#include "eventCommon.h"
#include "windowCommon.h"

namespace ts3::system
{

	enum class EWindowVisibilityState : uint32
	{
		Unknown,
		Hidden,
		Visible
	};

	struct EvtWindow : public EvtBase
	{
	public:
		EventSource * eventSource = nullptr;

	public:
		template <typename TpObject>
		bool checkEventSource( const TpObject * pSource ) const
		{
			return static_cast<const void *>( pSource ) == eventSource;
		}
	};

	struct EvtWindowUpdateCreate : public EvtWindow
	{
	};

	struct EvtWindowUpdateDestroy : public EvtWindow
	{
	};

	struct EvtWindowUpdateFullscreen : public EvtWindow
	{
		EActiveState fullscreenState = EActiveState::Unknown;
	};

	struct EvtWindowUpdateResize : public EvtWindow
	{
		FrameSize newSize;
	};

	struct EvtWindowUpdateVisibility : public EvtWindow
	{
		EWindowVisibilityState newVisibilityState = EWindowVisibilityState::Unknown;
	};

} // namespace ts3::system

#endif // __TS3_SYSTEM_EVENT_DEF_WINDOW_H__
