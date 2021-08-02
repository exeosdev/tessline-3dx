
#include "visual.h"
#include <unordered_map>
#include <ts3/stdext/mapUtils.h>

namespace ts3
{

	// GfxColorDesc representation of values in GfxColorFormat enumeration.
	static const GfxColorDesc sColorDescB8G8R8       { { 8,  8,  8,  0 }, 24, GfxColorSpace::Linear };
    static const GfxColorDesc sColorDescB8G8R8A8     { { 8,  8,  8,  8 }, 32, GfxColorSpace::Linear };
    static const GfxColorDesc sColorDescB8G8R8A8SRGB { { 8,  8,  8,  8 }, 24, GfxColorSpace::SRGB };
    static const GfxColorDesc sColorDescR8G8B8A8     { { 8,  8,  8,  8 }, 32, GfxColorSpace::Linear };
	static const GfxColorDesc sColorDescR8G8B8A8SRGB { { 8,  8,  8,  8 }, 32, GfxColorSpace::SRGB };
	static const GfxColorDesc sColorDescR10G10B10A2  { { 10, 10, 10, 2 }, 32, GfxColorSpace::Linear };
	static const GfxColorDesc sColorDescUnknown      { { 0, 0, 0, 0 }, 0 };

    const GfxColorDesc & sysGfxGetDescForColorFormat( GfxColorFormat pFormat )
    {
    	static const std::unordered_map<GfxColorFormat, GfxColorDesc> colorDescMap =
	    {
		    { GfxColorFormat::B8G8R8       , sColorDescB8G8R8       },
		    { GfxColorFormat::B8G8R8A8     , sColorDescB8G8R8A8     },
		    { GfxColorFormat::B8G8R8A8SRGB , sColorDescB8G8R8A8SRGB },
		    { GfxColorFormat::R8G8B8A8     , sColorDescR8G8B8A8     },
		    { GfxColorFormat::R8G8B8A8SRGB , sColorDescR8G8B8A8SRGB },
		    { GfxColorFormat::R10G10B10A2  , sColorDescR10G10B10A2  },
	    };
    	return getMapValueOrDefault( colorDescMap, pFormat, sColorDescUnknown );
    }

	// GfxDepthStencilDesc representation of values in GfxDepthStencilFormat enumeration.
	static const GfxDepthStencilDesc sDepthStencilDescD16     { 16, 0 };
	static const GfxDepthStencilDesc sDepthStencilDescD24S8   { 24, 8 };
	static const GfxDepthStencilDesc sDepthStencilDescD24X8   { 24, 0 };
	static const GfxDepthStencilDesc sDepthStencilDescD32F    { 32, 0 };
	static const GfxDepthStencilDesc sDepthStencilDescD32FS8  { 32, 8 };
	static const GfxDepthStencilDesc sDepthStencilDescUnknown { 0,  0 };

    const GfxDepthStencilDesc & sysGfxGetDescForDepthStencilFormat( GfxDepthStencilFormat pFormat )
    {
	    static const std::unordered_map<GfxDepthStencilFormat, GfxDepthStencilDesc> depthStencilDescMap =
	    {
		    { GfxDepthStencilFormat::D16    , sDepthStencilDescD16    },
		    { GfxDepthStencilFormat::D24S8  , sDepthStencilDescD24S8  },
		    { GfxDepthStencilFormat::D24X8  , sDepthStencilDescD24X8  },
		    { GfxDepthStencilFormat::D32F   , sDepthStencilDescD32F   },
		    { GfxDepthStencilFormat::D32FS8 , sDepthStencilDescD32FS8 },
	    };
	    return getMapValueOrDefault( depthStencilDescMap, pFormat, sDepthStencilDescUnknown );
    }

	// GfxMSAADesc representation of values in GfxMSAAMode enumeration.
	static const GfxMSAADesc sMSAADescNone { 0, 0  };
	static const GfxMSAADesc sMSAADescX1   { 1, 1  };
	static const GfxMSAADesc sMSAADescX2   { 1, 2  };
	static const GfxMSAADesc sMSAADescX4   { 1, 4  };
	static const GfxMSAADesc sMSAADescX8   { 1, 8  };
	static const GfxMSAADesc sMSAADescX16  { 1, 16 };

    const GfxMSAADesc & sysGfxGetDescForMSAAMode( GfxMSAAMode pMode )
    {
	    static const std::unordered_map<GfxMSAAMode, GfxMSAADesc> msaaModeMap =
	    {
		    { GfxMSAAMode::x1,  sMSAADescX1  },
		    { GfxMSAAMode::x2,  sMSAADescX2  },
		    { GfxMSAAMode::x4,  sMSAADescX4  },
		    { GfxMSAAMode::x8,  sMSAADescX8  },
		    { GfxMSAAMode::x16, sMSAADescX16 }
	    };
	    return getMapValueOrDefault( msaaModeMap, pMode, sMSAADescNone );
    }

	const GfxVisualConfig & sysGfxGetDefaultVisualConfigFortemWindow()
	{
		// Default GfxVisualConfig for creating a system-level window.
		static const GfxVisualConfig sVisualConfigWindowDefault
		{
			sColorDescB8G8R8,
			GfxColorFormat::B8G8R8,
			sDepthStencilDescD24S8,
			GfxDepthStencilFormat::D24S8,
			sMSAADescNone,
			GfxMSAAMode::Unknown,
			SYS_GFX_VISUAL_ATTRIB_FLAG_DOUBLE_BUFFER_BIT
		};
		return sVisualConfigWindowDefault;
	}

	bool sysDsmCheckColorFormatCompatibility( GfxColorFormat pFormat, uint8 pRed, uint8 pGreen, uint8 pBlue, uint8 pAlpha )
	{
		const auto & colorDesc = sysGfxGetDescForColorFormat( pFormat );
		const auto & rgba = colorDesc.rgba;
		// Format matches with a color spec if all channels have the exact same size.
		return ( rgba.u8Red == pRed ) && ( rgba.u8Green == pGreen ) && ( rgba.u8Blue == pBlue ) && ( rgba.u8Alpha == pAlpha );
	}

}
