
#include "displayDriverNative.h"
#include <ts3/stdext/stringUtils.h>
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

        strStream << ", Origin:[" << screenRect.offset.x << "x" << screenRect.offset.y << "]";
        strStream << ", Size:[" << screenRect.size.x << "x" << screenRect.size.y << "]";

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

    dsm_output_id_t dsmCreateDisplayOutputID( dsm_index_t pAdapterIndex, dsm_index_t pOutputIndex )
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
        auto settingsHash = dsmComputeVideoSettingsHash( pFormat, pSettings );

        std::stringstream strStream;
        strStream << pSettings.resolution.x << 'x' << pSettings.resolution.y;

        if( pSettings.flags.isSet( E_DISPLAY_VIDEO_SETTINGS_FLAG_SCAN_PROGRESSIVE_BIT ) )
        {
            strStream << "p";
        }
        else if( pSettings.flags.isSet( E_DISPLAY_VIDEO_SETTINGS_FLAG_SCAN_INTERLACED_BIT ) )
        {
            strStream << "i";
        }

        strStream << ", " << colorFormatDesc.size << "bit, " << pSettings.refreshRate << "Hz";
        strStream << ", //0x" << std::hex << settingsHash << std::dec;

        return strStream.str();
	}

	EDisplayAdapterVendorID dsmResolveAdapterVendorID( const std::string & pAdapterName )
	{
        auto adapterVendorID = EDisplayAdapterVendorID::Unknown;
        auto adapterString = strUtils::makeUpper( pAdapterName );

        if( ( adapterString.find( "AMD" ) != std::string::npos ) || ( adapterString.find( "ATI" ) != std::string::npos ) )
        {
            adapterVendorID = EDisplayAdapterVendorID::AMD;
        }
        else if( adapterString.find( "ARM" ) != std::string::npos )
        {
            adapterVendorID = EDisplayAdapterVendorID::ARM;
        }
        else if( adapterString.find( "GOOGLE" ) != std::string::npos )
        {
            adapterVendorID = EDisplayAdapterVendorID::Google;
        }
        else if( adapterString.find( "INTEL" ) != std::string::npos )
        {
            adapterVendorID = EDisplayAdapterVendorID::Intel;
        }
        else if( adapterString.find( "MICROSOFT" ) != std::string::npos )
        {
            adapterVendorID = EDisplayAdapterVendorID::Microsoft;
        }
        else if( adapterString.find( "NVIDIA" ) != std::string::npos )
        {
            adapterVendorID = EDisplayAdapterVendorID::Nvidia;
        }
        else if( adapterString.find( "QUALCOMM" ) != std::string::npos )
        {
            adapterVendorID = EDisplayAdapterVendorID::Qualcomm;
        }

        return adapterVendorID;
	}

	bool dsmCheckSettingsFilterMatch( const DisplayVideoSettingsFilter & pFilter, const DisplayVideoSettings & pSettings )
	{
        return true;
	}

} // namespace ts3::system
