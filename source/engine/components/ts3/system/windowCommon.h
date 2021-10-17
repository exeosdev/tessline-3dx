
#ifndef __TS3_SYSTEM_WINDOW_DEFS_H__
#define __TS3_SYSTEM_WINDOW_DEFS_H__

#include "frame.h"
#include "visual.h"

namespace ts3::system
{

    ts3SysDeclareHandle( Window );
    ts3SysDeclareHandle( WindowManager );

	/// @brief
	struct WindowProperties
	{
		FrameGeometry frameGeometry;
		std::string title;
	};

} // namespace ts3::system

#endif // __TS3_SYSTEM_WINDOW_DEFS_H__
