
#include "visual.h"
#include <unordered_map>
#include <ts3/stdext/mapUtils.h>

namespace ts3
{

	// SysColorDesc representation of values in SysColorFormat enumeration.
	static const SysColorDesc sColorDescB8G8R8       { { 8,  8,  8,  0 }, 24, SysColorSpace::Linear };
    static const SysColorDesc sColorDescB8G8R8A8     { { 8,  8,  8,  8 }, 32, SysColorSpace::Linear };
    static const SysColorDesc sColorDescB8G8R8A8SRGB { { 8,  8,  8,  8 }, 24, SysColorSpace::SRGB };
    static const SysColorDesc sColorDescR8G8B8A8     { { 8,  8,  8,  8 }, 32, SysColorSpace::Linear };
	static const SysColorDesc sColorDescR8G8B8A8SRGB { { 8,  8,  8,  8 }, 32, SysColorSpace::SRGB };
	static const SysColorDesc sColorDescR10G10B10A2  { { 10, 10, 10, 2 }, 32, SysColorSpace::Linear };
	static const SysColorDesc sColorDescUnknown      { { 0, 0, 0, 0 }, 0 };

    const SysColorDesc & sysDsmGetDescForColorFormat( SysColorFormat pFormat )
    {
    	static const std::unordered_map<SysColorFormat, SysColorDesc> colorDescMap =
	    {
		    { SysColorFormat::B8G8R8       , sColorDescB8G8R8       },
		    { SysColorFormat::B8G8R8A8     , sColorDescB8G8R8A8     },
		    { SysColorFormat::B8G8R8A8SRGB , sColorDescB8G8R8A8SRGB },
		    { SysColorFormat::R8G8B8A8     , sColorDescR8G8B8A8     },
		    { SysColorFormat::R8G8B8A8SRGB , sColorDescR8G8B8A8SRGB },
		    { SysColorFormat::R10G10B10A2  , sColorDescR10G10B10A2  },
	    };
    	return getMapValueOrDefault( colorDescMap, pFormat, sColorDescUnknown );
    }

	// SysDepthStencilDesc representation of values in SysDepthStencilFormat enumeration.
	static const SysDepthStencilDesc sDepthStencilDescD16     { 16, 0 };
	static const SysDepthStencilDesc sDepthStencilDescD24S8   { 24, 8 };
	static const SysDepthStencilDesc sDepthStencilDescD24X8   { 24, 0 };
	static const SysDepthStencilDesc sDepthStencilDescD32F    { 32, 0 };
	static const SysDepthStencilDesc sDepthStencilDescD32FS8  { 32, 8 };
	static const SysDepthStencilDesc sDepthStencilDescUnknown { 0,  0 };

    const SysDepthStencilDesc & sysDsmGetDescForDepthStencilFormat( SysDepthStencilFormat pFormat )
    {
	    static const std::unordered_map<SysDepthStencilFormat, SysDepthStencilDesc> depthStencilDescMap =
	    {
		    { SysDepthStencilFormat::D16    , sDepthStencilDescD16    },
		    { SysDepthStencilFormat::D24S8  , sDepthStencilDescD24S8  },
		    { SysDepthStencilFormat::D24X8  , sDepthStencilDescD24X8  },
		    { SysDepthStencilFormat::D32F   , sDepthStencilDescD32F   },
		    { SysDepthStencilFormat::D32FS8 , sDepthStencilDescD32FS8 },
	    };
	    return getMapValueOrDefault( depthStencilDescMap, pFormat, sDepthStencilDescUnknown );
    }

	// SysMSAADesc representation of values in SysMSAAMode enumeration.
	static const SysMSAADesc sMSAADescNone { 0, 0  };
	static const SysMSAADesc sMSAADescX1   { 1, 1  };
	static const SysMSAADesc sMSAADescX2   { 1, 2  };
	static const SysMSAADesc sMSAADescX4   { 1, 4  };
	static const SysMSAADesc sMSAADescX8   { 1, 8  };
	static const SysMSAADesc sMSAADescX16  { 1, 16 };

    const SysMSAADesc & sysDsmGetDescForMSAAMode( SysMSAAMode pMode )
    {
	    static const std::unordered_map<SysMSAAMode, SysMSAADesc> msaaModeMap =
	    {
		    { SysMSAAMode::x1,  sMSAADescX1  },
		    { SysMSAAMode::x2,  sMSAADescX2  },
		    { SysMSAAMode::x4,  sMSAADescX4  },
		    { SysMSAAMode::x8,  sMSAADescX8  },
		    { SysMSAAMode::x16, sMSAADescX16 }
	    };
	    return getMapValueOrDefault( msaaModeMap, pMode, sMSAADescNone );
    }

	const SysVisualConfig & sysDsmGetDefaultVisualConfigForSystemWindow()
	{
		// Default SysVisualConfig for creating a system-level window.
		static const SysVisualConfig sVisualConfigWindowDefault
		{
			sColorDescB8G8R8,
			SysColorFormat::B8G8R8,
			sDepthStencilDescD24S8,
			SysDepthStencilFormat::D24S8,
			sMSAADescNone,
			SysMSAAMode::Unknown,
			SYS_VISUAL_ATTRIB_FLAG_DOUBLE_BUFFER_BIT
		};
		return sVisualConfigWindowDefault;
	}

	bool sysDsmCheckColorFormatCompatibility( SysColorFormat pFormat, uint8 pRed, uint8 pGreen, uint8 pBlue, uint8 pAlpha )
	{
		const auto & colorDesc = sysDsmGetDescForColorFormat( pFormat );
		const auto & rgba = colorDesc.rgba;
		// Format matches with a color spec if all channels have the exact same size.
		return ( rgba.u8Red == pRed ) && ( rgba.u8Green == pGreen ) && ( rgba.u8Blue == pBlue ) && ( rgba.u8Alpha == pAlpha );
	}

}
