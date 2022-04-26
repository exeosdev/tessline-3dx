
#ifndef __TS3_SYSTEM_VISUAL_H__
#define __TS3_SYSTEM_VISUAL_H__

#include "prerequisites.h"
#include <ts3/math/color.h>

namespace ts3::system
{

	enum class EColorFormat : uint32
	{
		Unknown,
		B8G8R8,
		B8G8R8A8,
		B8G8R8A8SRGB,
		B8G8R8X8,
		R8G8B8A8,
		R8G8B8A8SRGB,
		R8G8B8X8,
		R10G10B10A2,

		// Sentinel value, used to determine the number of possible values.
		// New color formats should be all added above this line so that the
		// implementation can automatically adjust to the new definitions.
		_Reserved,
	};

	inline constexpr auto CX_ENUM_COLOR_FORMAT_COUNT = static_cast<uint32>( EColorFormat::_Reserved ) - 1;

	enum class EColorSpace : uint32
	{
		Unknown,
		Linear,
		SRGB
	};

	enum class EPixelOrder : uint32
	{
		Unknown,
		BGR,
		RGB
	};

	enum class EDepthStencilFormat : uint16
	{
		Unknown,
		D16,
		D24S8,
		D24X8,
		D32F,
		D32FS8
	};

	enum class EMSAAMode : uint16
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
	///
	/// Note: some flags are mutually exclusive. If more than one is set (e.g. inside visual
	/// configuration for a surface), they are chosen by the following priority rules:
	/// - Buffering: TRIPLE_BUFFER > DOUBLE_BUFFER > SINGLE_BUFFER.
	/// - If StereoDisplay and MonoDisplay are both set, MonoDisplay is used. If none is set, MonoDisplay is used.
	enum EVisualAttribFlags : uint32
	{
		/// Visual supports legacy formats. Used forcreation of temporary GL surfaces.
		E_VISUAL_ATTRIB_FLAG_LEGACY_BIT = 0x0001,

		/// Visual supports double buffering.
		E_VISUAL_ATTRIB_FLAG_DOUBLE_BUFFER_BIT = 0x0100,

		/// Visual does not support double buffering.
		E_VISUAL_ATTRIB_FLAG_SINGLE_BUFFER_BIT = 0x0200,

		/// Visual supports triple buffering.
		E_VISUAL_ATTRIB_FLAG_TRIPLE_BUFFER_BIT = 0x0400,

		/// Visual is sRGB-capable.
		E_VISUAL_ATTRIB_FLAG_SRGB_CAPABLE_BIT = 0x1000,

		/// Visual has the stereoscopic display mode.
		E_VISUAL_ATTRIB_FLAG_STEREO_DISPLAY_BIT = 0x0400,

		/// Visual has the classic, monoscopic display mode.
		E_VISUAL_ATTRIB_FLAG_MONO_DISPLAY_BIT = 0x0800,
	};

	struct ColorDesc
	{
		math::RGBAColorU8 rgba;
		uint32 size = 0;
		EPixelOrder pixelOrder = EPixelOrder::Unknown;
		EColorSpace colorSpace = EColorSpace::Unknown;
	};

	struct DepthStencilDesc
	{
		uint8 depthBufferSize;
		uint8 stencilBufferSize;
	};

	struct MSAADesc
	{
		uint8 bufferCount;
		uint8 quality;
	};

	/// @brief
	struct VisualConfig
	{
		// Struct representation of the color format.
		ColorDesc colorDesc;
		// Enum ID of the color format.
		EColorFormat colorFormat;
		// Struct representation of a depth/stencil buffer description.
		DepthStencilDesc depthStencilDesc;
		// Enum ID of the depth/stencil buffer format.
		EDepthStencilFormat depthStencilFormat;
		// Struct representation of the MSAA mode.
		MSAADesc msaaDesc;
		// Enum ID of the MSAA mode.
		EMSAAMode msaaMode;
		// Additional visual config flags.
		Bitmask<EVisualAttribFlags> flags;
	};

	/// @brief Returns a ColorDesc structure representation of a specified EColorFormat.
	TS3_SYSTEM_API const std::string & vsxQueryColorFormatStr( EColorFormat pFormat );

	/// @brief Returns a ColorDesc structure representation of a specified EColorFormat.
	TS3_SYSTEM_API const ColorDesc & vsxGetDescForColorFormat( EColorFormat pFormat );

	/// @brief Returns a DepthStencilDesc structure representation of a specified EDepthStencilFormat.
	TS3_SYSTEM_API const DepthStencilDesc & vsxGetDescForDepthStencilFormat( EDepthStencilFormat pFormat );

	/// @brief Returns an MSAADesc structure representation of a specified EMSAAMode.
	TS3_SYSTEM_API const MSAADesc & vsxGetDescForMSAAMode( EMSAAMode pMode );

	/// @brief Returns a ColorDesc structure representation of a specified EColorFormat.
	TS3_SYSTEM_API const VisualConfig & vsxGetDefaultVisualConfigForSysWindow();

	///
	TS3_SYSTEM_API bool vsxCheckColorFormatCompatibility( EColorFormat pFormat, uint8 pRed, uint8 pGreen, uint8 pBlue, uint8 pAlpha );

} // namespace ts3::system

#endif // __TS3_SYSTEM_VISUAL_H__
