
#ifndef __TS3_SYSTEM_WINDOW_DEFS_H__
#define __TS3_SYSTEM_WINDOW_DEFS_H__

#include "visual.h"
#include <ts3/math/vector.h>

namespace ts3::system
{

    class Window;
    class WindowManager;

	using WindowPos = math::Pos2i;
	using WindowSize = math::Size2u;
	using WindowNativeHandle = uint64;

	/// @brief Predefined window position: auto. When used, window is spawned centered.
	constexpr WindowPos cvWindowPositionAuto { -1, -1 };

	/// @brief Predefined window position: origin. When used, window is spawned at the origin point (0,0).
	constexpr WindowPos cvWindowPositionOrigin { 0, 0 };

	/// @brief Predefined window size: auto. When used, window size is selected by the implementation.
	constexpr WindowSize cvWindowSizeAuto { 0, 0 };

	/// @brief Predefined window size: max. When used, window size is the maximum size allowed by the system.
	/// Max size usually means the current size of the screen, but selected frame style may affect this (Win32).
	constexpr WindowSize cvWindowSizeMax { CX_UINT32_MAX, CX_UINT32_MAX };

	/// @brief Window styles supported by the system.
	enum class WindowFrameStyle : enum_default_value_t
	{
	    //
	    Unspecified,
		// Title-bar only window, no system menu, non-resizeable, no close or minimize button.
		Caption,
		// Fixed window, with system menu, non-resizeable.
		Fixed,
		//
		Overlay,
		// "Normal" window with title bar, minimize/maximize buttons (if applicable), resizeable.
		Resizeable,
		// Default style is the one which is most convenient for rendering - fixed frame.
		Default = static_cast< enum_default_value_t >( Fixed ),
	};

	/// @brief
	enum class WindowSizeMode : enum_default_value_t
    {
	    ClientArea,
	    FrameRect
    };

	/// @brief
	enum class WindowVisibilityMode : enum_default_value_t
    {
	    Hidden,
	    VisibleDefault,
	    Visible
    };

	/// @brief
	struct WindowGeometry
	{
		WindowPos position;
		WindowSize size;
		WindowFrameStyle frameStyle;
	};

	/// @brief
	struct WindowProperties
	{
		WindowGeometry geometry;
		std::string title;
	};

} // namespace ts3::system

#endif // __TS3_SYSTEM_WINDOW_DEFS_H__
