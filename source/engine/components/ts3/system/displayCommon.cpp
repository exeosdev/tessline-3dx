
#include "displayDriverNative.h"
#include <sstream>

namespace ts3::system
{

    std::string DisplayAdapterDesc::toString() const
    {
        return {};
    }

    std::string DisplayOutputDesc::toString() const
    {
        return {};
    }

    std::string DisplayVideoModeDesc::toString() const
    {
        return {};
    }

    dsm_video_settings_hash_t dsmComputeVideoSettingsHash( ColorFormat pFormat, const DisplayVideoSettings & pSettings )
	{
		DisplayVideoSettingsHash hashValueGen;
		hashValueGen.uResWidth = static_cast<uint16>( pSettings.resolution.x );
		hashValueGen.uResHeight = static_cast<uint16>( pSettings.resolution.x );
		hashValueGen.uRefreshRate = static_cast<uint16>( pSettings.refreshRate );
		hashValueGen.uFlags = static_cast<uint8>( pSettings.flags );
		hashValueGen.uColorFormatIndex = static_cast<uint8>( pFormat );
		return hashValueGen.hashValue;
	}

	std::string dsmGetVideoSettingsString( ColorFormat pFormat, const DisplayVideoSettings & pSettings )
	{
        auto & colorFormatDesc = vsxGetDescForColorFormat( pFormat );

        std::stringstream strStream;
        strStream << pSettings.resolution.x << "x" << pSettings.resolution.y;
        strStream << ":" << colorFormatDesc.size << "bpp@" << pSettings.refreshRate << "Hz";
        return strStream.str();
	}

	ColorFormat dsmResolveSystemColorFormat( ColorFormat pColorFormat )
	{
        return ColorFormat::B8G8R8A8;
	}

} // namespace ts3::system
