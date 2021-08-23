
#include "displayCommon.h"
#include <sstream>

namespace ts3
{
namespace system
{

    dsm_video_settings_hash_t dsmComputeVideoSettingsHash( ColorFormat pFormat, const DisplayVideoSettings & pSettings )
	{
		DisplayVideoModeHash modeHashGen;
		modeHashGen.uResWidth = static_cast<uint16>( pSettings.resolution.x );
		modeHashGen.uResHeight = static_cast<uint16>( pSettings.resolution.x );
		modeHashGen.uRefreshRate = static_cast<uint16>( pSettings.refreshRate );
		modeHashGen.uFlags = static_cast<uint8>( pSettings.flags );
		modeHashGen.uColorFormatIndex = static_cast<uint8>( pFormat );
		return modeHashGen.modeHash;
	}

	std::string dsmGetVideoSettingsString( ColorFormat pFormat, const DisplayVideoSettings & pSettings )
	{
		return {};
	}

} // namespace system
} // namespace ts3
