
#ifndef __TS3_SYSTEM_METAL_COMMON_H__
#define __TS3_SYSTEM_METAL_COMMON_H__

#include "visual.h"
#include "windowCommon.h"

namespace ts3::system
{

	class MetalDisplaySurface;
	class MetalSystemDriver;

	/// @brief
	enum EMetalSurfaceCreateFlags : uint32
	{
		/// Surface is created as a fullscreen layer/window, adjusted to the selected display dimensions.
		E_METAL_DISPLAY_SURFACE_CREATE_FLAG_FULLSCREEN_BIT = 0x1000,

		/// Enables adaptive sync for the surface: v-sync is performed only when the frame rate exceeds vertical frequency.
		E_METAL_DISPLAY_SURFACE_CREATE_FLAG_SYNC_ADAPTIVE_BIT = 0x2000,

		/// Disables vertical sync for the surface: buffers are swapped immediately upon request.
		E_METAL_DISPLAY_SURFACE_CREATE_FLAG_SYNC_DISABLED_BIT = 0x4000,

		/// Enables vertical sync for the surface: swap is performed during a v-blank.
		E_METAL_DISPLAY_SURFACE_CREATE_FLAG_SYNC_VERTICAL_BIT = 0x8000,
	};

}

#endif // __TS3_SYSTEM_METAL_COMMON_H__
