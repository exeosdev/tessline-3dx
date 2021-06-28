
#pragma once

#ifndef __TS3_GPUAPI_DISPLAY_COMMON_H__
#define __TS3_GPUAPI_DISPLAY_COMMON_H__

#include "prerequisites.h"
#include <ts3/system/displayCommon.h>

namespace ts3::gpuapi
{

	using display_system_id_t = uint64;

	constexpr display_system_id_t cvDisplaySystemIDDefault = ts3::Limits<display_system_id_t>::maxValue;
	constexpr display_system_id_t cvDisplaySystemIDInvalid = cvDisplaySystemIDDefault - 1;

	enum EAdapterFlags : uint32
	{
		E_ADAPTER_FLAG_PRIMARY_BIT = static_cast<uint32>( ts3::E_SYS_DSM_ADAPTER_FLAG_PRIMARY_BIT ),
		E_ADAPTER_FLAG_MULTI_NODE_BIT = static_cast<uint32>( ts3::E_SYS_DSM_ADAPTER_FLAG_MULTI_NODE_BIT ),
		E_ADAPTER_FLAG_SOFTWARE_BIT = static_cast<uint32>( ts3::E_SYS_DSM_ADAPTER_FLAG_SOFTWARE_BIT ),
	};

	enum EOutputFlags : uint32
	{
		E_OUTPUT_FLAG_PRIMARY_BIT = static_cast<uint32>( ts3::E_SYS_DSM_OUTPUT_FLAG_PRIMARY_BIT )
	};

	enum EVideoSettingsFlags : uint16
	{
		E_VIDEO_SETTINGS_FLAG_SCAN_INTERLACED_BIT = static_cast<uint32>( ts3::E_SYS_DSM_VIDEO_SETTINGS_FLAG_SCAN_INTERLACED_BIT ),
		E_VIDEO_SETTINGS_FLAG_SCAN_PROGRESSIVE_BIT = static_cast<uint32>( ts3::E_SYS_DSM_VIDEO_SETTINGS_FLAG_SCAN_PROGRESSIVE_BIT ),
		E_VIDEO_SETTINGS_FLAG_STEREO_BIT = static_cast<uint32>( ts3::E_SYS_DSM_VIDEO_SETTINGS_FLAG_STEREO_BIT )
	};

	enum EDisplayConfigurationFlags : uint32
	{
		E_DISPLAY_CONFIGURATION_FLAG_FULLSCREEN_BIT = 0x0001,
		E_DISPLAY_CONFIGURATION_FLAG_SYNC_MODE_ADAPTIVE_BIT = 0x0100,
		E_DISPLAY_CONFIGURATION_FLAG_SYNC_MODE_VERTICAL_BIT = 0x0200,
	};

	struct ScreenRect
	{
		ts3::math::Vec2i32 offset;
		ts3::math::Vec2u32 size;
	};

	/// @brief
	struct VideoSettings
	{
		ts3::math::Size2u resolution;
		uint16 refreshRate = 0u;
		Bitmask<EVideoSettingsFlags> flags = 0u;
	};

	struct AdapterDesc
	{
		display_system_id_t id = cvDisplaySystemIDInvalid;
		std::string name;
		Bitmask<EAdapterFlags> flags = 0u;
	};

	struct OutputDesc
	{
		display_system_id_t id = cvDisplaySystemIDInvalid;
		std::string name;
		ts3::SysDsmScreenRect screenRect;
		Bitmask<EOutputFlags> flags = 0u;
	};

	struct VideoModeDesc
	{
		display_system_id_t id = cvDisplaySystemIDInvalid;
		ts3::SysColorFormat format = ts3::SysColorFormat::Unknown;
		VideoSettings settings;
	};

	inline EAdapterFlags translateSysAdapterFlags( Bitmask<ts3::ESysDsmAdapterFlags> pSysAdapterFlags )
	{
		Bitmask<EAdapterFlags> result = 0;
		if( pSysAdapterFlags.isSet( ts3::E_SYS_DSM_ADAPTER_FLAG_PRIMARY_BIT ) )
		{
			result.set( E_ADAPTER_FLAG_PRIMARY_BIT );
		}
		if( pSysAdapterFlags.isSet( ts3::E_SYS_DSM_ADAPTER_FLAG_MULTI_NODE_BIT ) )
		{
			result.set( E_ADAPTER_FLAG_MULTI_NODE_BIT );
		}
		if( pSysAdapterFlags.isSet( ts3::E_SYS_DSM_ADAPTER_FLAG_SOFTWARE_BIT ) )
		{
			result.set( E_ADAPTER_FLAG_SOFTWARE_BIT );
		}
		return result;
	}

	inline EOutputFlags translateSysOutputFlags( Bitmask<ts3::ESysDsmOutputFlags> pSysOutputFlags )
	{
		Bitmask<EOutputFlags> result = 0;
		if( pSysOutputFlags.isSet( ts3::E_SYS_DSM_OUTPUT_FLAG_PRIMARY_BIT ) )
		{
			result.set( E_OUTPUT_FLAG_PRIMARY_BIT );
		}
		return result;
	}

	inline EVideoSettingsFlags translateSysVideoSettingsFlags( Bitmask<ts3::ESysDsmVideoSettingsFlags> pSysVideoSettingsFlags )
	{
		Bitmask<EVideoSettingsFlags> result = 0;
		if( pSysVideoSettingsFlags.isSet( ts3::E_SYS_DSM_VIDEO_SETTINGS_FLAG_SCAN_INTERLACED_BIT ) )
		{
			result.set( E_VIDEO_SETTINGS_FLAG_SCAN_INTERLACED_BIT );
		}
		if( pSysVideoSettingsFlags.isSet( ts3::E_SYS_DSM_VIDEO_SETTINGS_FLAG_SCAN_PROGRESSIVE_BIT ) )
		{
			result.set( E_VIDEO_SETTINGS_FLAG_SCAN_PROGRESSIVE_BIT );
		}
		if( pSysVideoSettingsFlags.isSet( ts3::E_SYS_DSM_VIDEO_SETTINGS_FLAG_STEREO_BIT ) )
		{
			result.set( E_VIDEO_SETTINGS_FLAG_STEREO_BIT );
		}
		return result;
	}

}

#endif // __TS3_GPUAPI_DISPLAY_COMMON_H__
