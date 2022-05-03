
#ifndef __TS3_SYSTEM_WINDOW_COMMON_H__
#define __TS3_SYSTEM_WINDOW_COMMON_H__

#include "frame.h"
#include "visual.h"

namespace ts3::system
{

    ts3SysDeclareHandle( Window );
    ts3SysDeclareHandle( WindowManager );

	struct WindowCreateInfo
	{
		FrameGeometry frameGeometry = CX_FRAME_GEOMETRY_DEFAULT;

		std::string title;

		/// Visual config for the window. Describes primary rendering properties of the surface.
		VisualConfig visualConfig;
	};

} // namespace ts3::system

#endif // __TS3_SYSTEM_WINDOW_COMMON_H__
