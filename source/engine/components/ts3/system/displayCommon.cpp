
#include "displayCommon.h"
#include <sstream>

namespace ts3
{

	sys_dsm_video_settings_hash_t sysDsmComputeVideoSettingsHash( SysColorFormat pFormat, const SysDsmVideoSettings & pSettings )
	{
		SysDsmVideoModeHash modeHashGen;
		modeHashGen.uResWidth = static_cast<uint16>( pSettings.resolution.x );
		modeHashGen.uResHeight = static_cast<uint16>( pSettings.resolution.x );
		modeHashGen.uRefreshRate = static_cast<uint16>( pSettings.refreshRate );
		modeHashGen.uFlags = static_cast<uint8>( pSettings.flags );
		modeHashGen.uColorFormatIndex = static_cast<uint8>( pFormat );
		return modeHashGen.modeHash;;
	}

	std::string sysDsmGetVideoSettingsString( SysColorFormat pFormat, const SysDsmVideoSettings & pSettings )
	{
		return {};
	}

}
