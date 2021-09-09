
#include "displayDriverNative.h"
#include <sstream>

namespace ts3::system
{

    std::string DisplayAdapterDesc::toString() const
    {
        std::stringstream strStream;
        strStream << "Adapter <" << adapterIndex << "> (" << name << ")";

        if( flags.isSet( E_DISPLAY_ADAPTER_FLAG_ACTIVE_BIT ) )
        {
            strStream << ", Active";
        }

        if( flags.isSet( E_DISPLAY_ADAPTER_FLAG_PRIMARY_BIT ) )
        {
            strStream << ", Primary";
        }

        if( flags.isSet( E_DISPLAY_ADAPTER_FLAG_HARDWARE_BIT ) )
        {
            strStream << ", Hardware";
        }
        else if( flags.isSet( E_DISPLAY_ADAPTER_FLAG_SOFTWARE_BIT ) )
        {
            strStream << ", Software";
        }

        return strStream.str();
    }

    std::string DisplayOutputDesc::toString() const
    {
        std::stringstream strStream;
        strStream << "Output <" << outputIndex << "> (" << name << ")";

        if( flags.isSet( E_DISPLAY_OUTPUT_FLAG_ACTIVE_BIT ) )
        {
            strStream << ", Active";
        }

        if( flags.isSet( E_DISPLAY_OUTPUT_FLAG_PRIMARY_BIT ) )
        {
            strStream << ", Primary";
        }

        strStream << ", O:[" << screenRect.offset.x << "x" << screenRect.offset.y << "]";
        strStream << ", S:[" << screenRect.size.x << "x" << screenRect.size.y << "]";

        return strStream.str();
    }

    std::string DisplayVideoModeDesc::toString() const
    {
        std::stringstream strStream;
        strStream << "DisplayMode <" << videoModeIndex << "> ";

        const auto settingsText = dsmGetVideoSettingsString( colorFormat, settings );
        strStream << settingsText;

        return strStream.str();
    }

    dsm_output_id_t dsmCreateDisplayOutputIDGen( dsm_index_t pAdapterIndex, dsm_index_t pOutputIndex )
    {
        DisplayOutputIDGen outputIDGen;
        outputIDGen.uAdapterIndex = pAdapterIndex;
        outputIDGen.uOutputIndex = pOutputIndex;
        return outputIDGen.outputID;
    }

    dsm_video_settings_hash_t dsmComputeVideoSettingsHash( ColorFormat pFormat, const DisplayVideoSettings & pSettings )
	{
		DisplayVideoSettingsHashGen hashValueGen;
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

} // namespace ts3::system
