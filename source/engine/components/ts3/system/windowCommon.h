
#ifndef __TS3_SYSTEM_WINDOW_DEFS_H__
#define __TS3_SYSTEM_WINDOW_DEFS_H__

#include "gfxVisual.h"
#include <ts3/math/vector.h>

namespace ts3
{
namespace system
{

	ts3DeclareHandle( WmWindow );
	ts3DeclareHandle( WmWindowManager );

	using WmWindowPos = math::Pos2i;
	using WmWindowSize = math::Size2u;
	using WindowNativeHandle = uint64;

	/// @brief Predefined window position: auto. When used, window is spawned centered.
	constexpr WmWindowPos cvWindowPositionAuto { -1, -1 };

	/// @brief Predefined window position: origin. When used, window is spawned at the origin point (0,0).
	constexpr WmWindowPos cvWindowPositionOrigin { 0, 0 };

	/// @brief Predefined window size: auto. When used, window size is selected by the implementation.
	constexpr WmWindowSize cvWindowSizeAuto { 0, 0 };

	/// @brief Predefined window size: max. When used, window size is the maximum size allowed by the system.
	/// Max size usually means the current size of the screen, but selected frame style may affect this (Win32).
	constexpr WmWindowSize cvWindowSizeMax { cxUint32Max, cxUint32Max };

	/// @brief Window styles supported by the system.
	enum class WmWindowFrameStyle : enum_default_value_t
	{
		// Title-bar only window, no system menu, non-resizeable, no close or minimize button.
		Caption,
		// Fixed window, with system menu, non-resizeable.
		Fixed,
		//
		Overlay,
		// "Normal" window with title bar, minimize/maximize buttons (if applicable), resizeable.
		Resizeable,
		// Default style is the one which is most convenient for rendering - fixed frame.
		Default = static_cast< enum_default_value_t >( Fixed )
	};

	/// @brief
	struct WmWindowGeometry
	{
		WmWindowPos position;
		WmWindowSize size;
		WmWindowFrameStyle frameStyle;
	};

	/// @brief
	struct WmWindowProperties
	{
		WmWindowGeometry geometry;
		std::string title;
	};

} // namespace system
} // namespace ts3

#endif // __TS3_SYSTEM_WINDOW_DEFS_H__
