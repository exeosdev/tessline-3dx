
#ifndef __TS3_SYSTEM_VISUAL_H__
#define __TS3_SYSTEM_VISUAL_H__

#include "prerequisites.h"
#include <ts3/math/color.h>

namespace ts3
{
namespace system
{

	enum class GfxColorFormat : uint32
	{
		Unknown,
		B8G8R8,
		B8G8R8A8,
		B8G8R8A8SRGB,
		R8G8B8A8,
		R8G8B8A8SRGB,
		R10G10B10A2
	};

	enum class GfxColorSpace : uint32
	{
		Unknown,
		Linear,
		SRGB
	};

	enum class GfxDepthStencilFormat : uint16
	{
		Unknown,
		D16,
		D24S8,
		D24X8,
		D32F,
		D32FS8
	};

	enum class GfxMSAAMode : uint16
	{
		Unknown,
		x1,
		x2,
		x4,
		x8,
		x16,
		Best = static_cast< uint16 >( x16 ),
	};

	/// @brief Flags representing attributes (properties) of visuals.
	enum GfxVisualAttribFlags : uint32
	{
		// Note: some flags are mutually exclusive. If both are set (e.g. inside visual
		// configuration for a surface), they are chosen by the following priority rules:
		// - If DoubleBuffer and SingleBuffer are both set, DoubleBuffer is used. If none is set, DoubleBuffer is used.
		// - If StereoDisplay and MonoDisplay are both set, MonoDisplay is used. If none is set, MonoDisplay is used.

		//
		SYS_GFX_VISUAL_ATTRIB_FLAG_LEGACY_BIT = 0x0001,
		// Visual supports double buffering.
		SYS_GFX_VISUAL_ATTRIB_FLAG_DOUBLE_BUFFER_BIT = 0x0100,
		// Visual does not support double buffering.
		SYS_GFX_VISUAL_ATTRIB_FLAG_SINGLE_BUFFER_BIT = 0x0200,
		// Visual is sRGB-capable.
		SYS_GFX_VISUAL_ATTRIB_FLAG_SRGB_CAPABLE_BIT = 0x1000,
		// Visual has the stereoscopic display mode.
		SYS_GFX_VISUAL_ATTRIB_FLAG_STEREO_DISPLAY_BIT = 0x0400,
		// Visual has the classic, monoscopic display mode.
		SYS_GFX_VISUAL_ATTRIB_FLAG_MONO_DISPLAY_BIT = 0x0800,
	};

	struct GfxColorDesc
	{
		math::RGBAColorU8 rgba;
		uint32 size = 0;
		GfxColorSpace colorSpace = GfxColorSpace::Linear;
	};

	struct GfxDepthStencilDesc
	{
		uint8 depthBufferSize;
		uint8 stencilBufferSize;
	};

	struct GfxMSAADesc
	{
		uint8 bufferCount;
		uint8 quality;
	};

	/// @brief
	struct GfxVisualConfig
	{
		// Struct representation of the color format.
		GfxColorDesc colorDesc;
		// Enum ID of the color format.
		GfxColorFormat colorFormat;
		// Struct representation of a depth/stencil buffer description.
		GfxDepthStencilDesc depthStencilDesc;
		// Enum ID of the depth/stencil buffer format.
		GfxDepthStencilFormat depthStencilFormat;
		// Struct representation of the MSAA mode.
		GfxMSAADesc msaaDesc;
		// Enum ID of the MSAA mode.
		GfxMSAAMode msaaMode;
		// Additional visual config flags.
		Bitmask<GfxVisualAttribFlags> flags;
	};

    /// @brief Returns a ColorDesc structure representation of a specified ColorFormat.
	TS3_SYSTEM_API const GfxColorDesc & sysGfxGetDescForColorFormat( GfxColorFormat pFormat );

    /// @brief Returns a DepthStencilDesc structure representation of a specified DepthStencilFormat.
	TS3_SYSTEM_API const GfxDepthStencilDesc & sysGfxGetDescForDepthStencilFormat( GfxDepthStencilFormat pFormat );

    /// @brief Returns an MSAADesc structure representation of a specified MSAAMode.
	TS3_SYSTEM_API const GfxMSAADesc & sysGfxGetDescForMSAAMode( GfxMSAAMode pMode );

    /// @brief Returns a ColorDesc structure representation of a specified ColorFormat.
	TS3_SYSTEM_API const GfxVisualConfig & sysGfxGetDefaultVisualConfigFortemWindow();

    ///
	TS3_SYSTEM_API bool sysGfxCheckColorFormatCompatibility( GfxColorFormat pFormat, uint8 pRed, uint8 pGreen, uint8 pBlue, uint8 pAlpha );

} // namespace system
} // namespace ts3

#endif // __TS3_SYSTEM_VISUAL_H__
