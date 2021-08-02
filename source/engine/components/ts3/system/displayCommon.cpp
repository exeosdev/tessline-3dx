
#include "displayCommon.h"
#include <sstream>

namespace ts3
{

	dsm_video_settings_hash_t sysDsmComputeVideoSettingsHash( GfxColorFormat pFormat, const DsmVideoSettings & pSettings )
	{
		DsmVideoModeHash modeHashGen;
		modeHashGen.uResWidth = static_cast<uint16>( pSettings.resolution.x );
		modeHashGen.uResHeight = static_cast<uint16>( pSettings.resolution.x );
		modeHashGen.uRefreshRate = static_cast<uint16>( pSettings.refreshRate );
		modeHashGen.uFlags = static_cast<uint8>( pSettings.flags );
		modeHashGen.uColorFormatIndex = static_cast<uint8>( pFormat );
		return modeHashGen.modeHash;;
	}

	std::string sysDsmGetVideoSettingsString( GfxColorFormat pFormat, const DsmVideoSettings & pSettings )
	{
		return {};
	}

}
