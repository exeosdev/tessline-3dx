
#include "visual.h"
#include <unordered_map>
#include <ts3/stdext/mapUtils.h>

namespace ts3::system
{

	// ColorDesc representation of values in ColorFormat enumeration.
	static const ColorDesc sColorDescB8G8R8 {
	    math::RGBAColorU8{ 8,  8,  8,  0 }, 24, ColorSpace::Linear };

    static const ColorDesc sColorDescB8G8R8A8 {
        math::RGBAColorU8{ 8,  8,  8,  8 }, 32, ColorSpace::Linear };

    static const ColorDesc sColorDescB8G8R8A8SRGB {
        math::RGBAColorU8{ 8,  8,  8,  8 }, 24, ColorSpace::SRGB };

    static const ColorDesc sColorDescR8G8B8A8 {
        math::RGBAColorU8{ 8,  8,  8,  8 }, 32, ColorSpace::Linear };

	static const ColorDesc sColorDescR8G8B8A8SRGB {
	    math::RGBAColorU8{ 8,  8,  8,  8 }, 32, ColorSpace::SRGB };

	static const ColorDesc sColorDescR10G10B10A2 {
	    math::RGBAColorU8{ 10, 10, 10, 2 }, 32, ColorSpace::Linear };

	static const ColorDesc sColorDescUnknown {
	    math::RGBAColorU8{ 0, 0, 0, 0 }, 0, ColorSpace::Unknown };

	const std::string & vsxQueryColorFormatStr( ColorFormat pFormat )
	{
	    static const std::unordered_map<ColorFormat, std::string> colorFormatStrMap =
        {
            { ColorFormat::B8G8R8       , "B8G8R8"       },
            { ColorFormat::B8G8R8A8     , "B8G8R8A8"     },
            { ColorFormat::B8G8R8A8SRGB , "B8G8R8A8SRGB" },
            { ColorFormat::R8G8B8A8     , "R8G8B8A8"     },
            { ColorFormat::R8G8B8A8SRGB , "R8G8B8A8SRGB" },
            { ColorFormat::R10G10B10A2  , "R10G10B10A2"  },
        };
	    static const std::string defaultColorFormatStr = "UNKNOWN";
	    return getMapValueOrDefault( colorFormatStrMap, pFormat, defaultColorFormatStr );
	}

	const ColorDesc & vsxGetDescForColorFormat( ColorFormat pFormat )
    {
    	static const std::unordered_map<ColorFormat, ColorDesc> colorDescMap =
	    {
		    { ColorFormat::B8G8R8       , sColorDescB8G8R8       },
		    { ColorFormat::B8G8R8A8     , sColorDescB8G8R8A8     },
		    { ColorFormat::B8G8R8A8SRGB , sColorDescB8G8R8A8SRGB },
		    { ColorFormat::R8G8B8A8     , sColorDescR8G8B8A8     },
		    { ColorFormat::R8G8B8A8SRGB , sColorDescR8G8B8A8SRGB },
		    { ColorFormat::R10G10B10A2  , sColorDescR10G10B10A2  },
	    };
    	return getMapValueOrDefault( colorDescMap, pFormat, sColorDescUnknown );
    }

	// DepthStencilDesc representation of values in DepthStencilFormat enumeration.
	static const DepthStencilDesc sDepthStencilDescD16     { 16 , 0 };
	static const DepthStencilDesc sDepthStencilDescD24S8   { 24 , 8 };
	static const DepthStencilDesc sDepthStencilDescD24X8   { 24 , 0 };
	static const DepthStencilDesc sDepthStencilDescD32F    { 32 , 0 };
	static const DepthStencilDesc sDepthStencilDescD32FS8  { 32 , 8 };
	static const DepthStencilDesc sDepthStencilDescUnknown { 0  , 0 };

	const DepthStencilDesc & vsxGetDescForDepthStencilFormat( DepthStencilFormat pFormat )
    {
	    static const std::unordered_map<DepthStencilFormat, DepthStencilDesc> depthStencilDescMap =
	    {
		    { DepthStencilFormat::D16    , sDepthStencilDescD16    },
		    { DepthStencilFormat::D24S8  , sDepthStencilDescD24S8  },
		    { DepthStencilFormat::D24X8  , sDepthStencilDescD24X8  },
		    { DepthStencilFormat::D32F   , sDepthStencilDescD32F   },
		    { DepthStencilFormat::D32FS8 , sDepthStencilDescD32FS8 },
	    };
	    return getMapValueOrDefault( depthStencilDescMap, pFormat, sDepthStencilDescUnknown );
    }

	// MSAADesc representation of values in MSAAMode enumeration.
	static const MSAADesc sMSAADescNone { 0, 0  };
	static const MSAADesc sMSAADescX1   { 1, 1  };
	static const MSAADesc sMSAADescX2   { 1, 2  };
	static const MSAADesc sMSAADescX4   { 1, 4  };
	static const MSAADesc sMSAADescX8   { 1, 8  };
	static const MSAADesc sMSAADescX16  { 1, 16 };

	const MSAADesc & vsxGetDescForMSAAMode( MSAAMode pMode )
    {
	    static const std::unordered_map<MSAAMode, MSAADesc> msaaModeMap =
	    {
		    { MSAAMode::x1,  sMSAADescX1  },
		    { MSAAMode::x2,  sMSAADescX2  },
		    { MSAAMode::x4,  sMSAADescX4  },
		    { MSAAMode::x8,  sMSAADescX8  },
		    { MSAAMode::x16, sMSAADescX16 }
	    };
	    return getMapValueOrDefault( msaaModeMap, pMode, sMSAADescNone );
    }

    const VisualConfig & vsxGetDefaultVisualConfigForSysWindow()
	{
		// Default VisualConfig for creating a system-level window.
		static const VisualConfig sVisualConfigWindowDefault
		{
			sColorDescB8G8R8A8,
			ColorFormat::B8G8R8A8,
			sDepthStencilDescD24S8,
			DepthStencilFormat::D24S8,
			sMSAADescNone,
			MSAAMode::Unknown,
			VISUAL_ATTRIB_FLAG_DOUBLE_BUFFER_BIT
		};
		return sVisualConfigWindowDefault;
	}

	bool vsxCheckColorFormatCompatibility( ColorFormat pFormat, uint8 pRed, uint8 pGreen, uint8 pBlue, uint8 pAlpha )
	{
	    const auto & colorDesc = vsxGetDescForColorFormat( pFormat );
		const auto & rgba = colorDesc.rgba;
		// Format matches with a color spec if all channels have the exact same size.
		return ( rgba.u8Red == pRed ) && ( rgba.u8Green == pGreen ) && ( rgba.u8Blue == pBlue ) && ( rgba.u8Alpha == pAlpha );
	}

} // namespace ts3::system
