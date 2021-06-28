
#ifndef __TS3_SYSTEM_DISPLAY_COMMON_H__
#define __TS3_SYSTEM_DISPLAY_COMMON_H__

#include "visual.h"
#include <ts3/math/vectorOps.h>
#include <ts3/stdext/bitmask.h>
#include <ts3/stdext/uuid.h>

#if( TS3_PCL_TARGET_OS & TS3_PCL_TARGET_FLAG_OS_WINFAMILY )
#  define TS3_SYSTEM_DISPLAY_DRIVER_SUPPORT_DXGI 1
#else
#  define TS3_SYSTEM_DISPLAY_DRIVER_SUPPORT_DXGI 0
#endif

#if( TS3_PCL_COMPILER & TS3_PCL_COMPILER_CLANG )
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wgnu-anonymous-struct"
#  pragma clang diagnostic ignored "-Wnested-anon-types"
#elif( TS3_PCL_COMPILER & TS3_PCL_COMPILER_GCC )
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wpedantic"
#elif( TS3_PCL_COMPILER & TS3_PCL_COMPILER_MSVC )
#  pragma warning( push )
#  pragma warning( disable: 4201 )  // 'Nonstandard extension used: nameless struct/union'
#endif

namespace ts3
{

	ts3DeclareSysHandle( SysDisplayDriver );
	ts3DeclareSysHandle( SysDisplayManager );

	struct SysDsmAdapter;
	struct SysDsmOutput;
	struct SysDsmVideoMode;
	struct SysWindowGeometry;

	using sys_dsm_index_t = uint16;
	using sys_dsm_output_id_t = uint32;
	using sys_dsm_video_mode_id_t = uint64;
	using sys_dsm_video_settings_hash_t = uint64;
	using SysDisplaySize = math::Size2u;

	/// @brief Represents invalid display system index (of an adapter or an output, for example).
	constexpr sys_dsm_index_t cvSysDsmIndexInvalid = Limits<sys_dsm_index_t>::maxValue;

	/// @brief Represents invalid display settings hash. Used to identify/report invalid and/or empty configurations.
	constexpr sys_dsm_video_settings_hash_t cvSysDsmVideoSettingsHashInvalid = Limits<sys_dsm_video_settings_hash_t>::maxValue;

	enum class ESysDisplayDriverType : enum_default_value_t
	{
		// Unknown driver. Cannot be used as an input.
		// Reported for an uninitialized driver object.
		Unknown,
		// Native, platform-specific driver. Implemented either with dedicated
		// libraries (like SDL) or low-level system API (XF86, XRR, Cocoa).
		// Available on all platforms.
		Generic,
		// Microsoft DXGI driver. Specific DXGI version depends on the platform.
		// Platform availability: Win32, UWP
		DXGI,
		// Default driver. Actual type is platform-dependent:
		// - Win32/UWP: DXGI
		// - Linux/Android/MacOS/iOS: Generic
		Default,
	};

	/// @brief Represents ID of the vendor of the graphics adapter/device.
	enum class ESysDsmAdapterVendorID : enum_default_value_t
	{
		AMD = 1,
		ARM,
		Google,
		Intel,
		Microsoft,
		Nvidia,
		Qualcomm,
		Unknown = 0,
	};

	/// @brief
	enum ESysDsmAdapterFlags : uint32
	{
		// Adapter is reported by system as the primary one.
		E_SYS_DSM_ADAPTER_FLAG_PRIMARY_BIT = 0x1,
		// Adapter is a multi-node adapter (e.g. classic CF- or SLI-configured adapter set)
		E_SYS_DSM_ADAPTER_FLAG_MULTI_NODE_BIT = 0x2,
		// Adapter is a software adapter. May indicate WARP-capable adapter in case of DXGI backend.
		E_SYS_DSM_ADAPTER_FLAG_SOFTWARE_BIT = 0x4,
	};

	/// @brief
	enum ESysDsmOutputFlags : uint32
	{
		// Output is the primary output of an adapter
		E_SYS_DSM_OUTPUT_FLAG_PRIMARY_BIT = 0x1
	};

	/// @brief
	enum ESysDsmVideoSettingsFlags : uint16
	{
		E_SYS_DSM_VIDEO_SETTINGS_FLAG_SCAN_INTERLACED_BIT = 0x1,
		E_SYS_DSM_VIDEO_SETTINGS_FLAG_SCAN_PROGRESSIVE_BIT = 0x2,
		E_SYS_DSM_VIDEO_SETTINGS_FLAG_STEREO_BIT = 0x4
	};

	enum ESysDsmVideoSettingsFilterFlags : uint32
	{
		// If set, resolution is not used as a filter criterion.
		E_SYS_DSM_VIDEO_MODE_FILTER_FLAG_RESOLUTION_IGNORE_BIT = 0x1,
		// If set, resolution must match exactly for the mode to be included.
		E_SYS_DSM_VIDEO_MODE_FILTER_FLAG_RESOLUTION_MATCH_EXACT_BIT = 0x2,
		// If set, refresh rate is not used as a filter criterion.
		E_SYS_DSM_VIDEO_MODE_FILTER_FLAG_REFRESH_RATE_IGNORE_BIT = 0x4,
		// If set, refresh rate must match exactly for the mode to be included.
		E_SYS_DSM_VIDEO_MODE_FILTER_FLAG_REFRESH_RATE_MATCH_EXACT_BIT = 0x8,
		// If set, additional flags are not used as a filter criterion.
		E_SYS_DSM_VIDEO_MODE_FILTER_FLAG_FLAGS_IGNORE_BIT = 0x40,
	};

	struct SysDsmScreenRect
	{
		math::Vec2i32 offset;
		math::Vec2u32 size;
	};

	/// @brief
	struct SysDsmVideoSettings
	{
		math::Size2u resolution;
		uint16 refreshRate = 0u;
		Bitmask<ESysDsmVideoSettingsFlags> flags = 0u;
	};

	constexpr SysDsmVideoSettings cvSysDsmVideoSettingsEmpty { { 0U, 0U }, 0U, 0U };

	/// @brief
	struct SysDsmVideoSettingsFilter
	{
		// Reference settings used to filter video modes. By default, each property in a mode
		// must be greater than or equal to the reference property (note for resolutions: both
		// dimensions must be >=, i.e. 1024x768 satisfies 1024x600, but 1280x720 does not satisfy
		// 1152x864). This does not apply to flags, which must have exact same bits set.
		// Each property can be ignored by using its respective IGNORE flag. Also, "greater than
		// or equal" condition can be changed to "equal" for a given property by setting MATCH_EXACT
		// flag for that property.
		SysDsmVideoSettings refSettings;
		// Filter flags used to control the process. Flags set by default: none.
		Bitmask<ESysDsmVideoSettingsFilterFlags> flags = 0;
	};

	constexpr SysDsmVideoSettingsFilter cvSysDsmVideoSettingsFilterNone { cvSysDsmVideoSettingsEmpty, 0U };

	struct SysDsmAdapterDesc
	{
		ESysDisplayDriverType driverType = ESysDisplayDriverType::Unknown;
		sys_dsm_index_t index = cvSysDsmIndexInvalid;
		std::string name;
		ESysDsmAdapterVendorID vendorID;
		Bitmask<ESysDsmAdapterFlags> flags = 0u;
		void * driverReserved = nullptr;
	};

	struct SysDsmOutputDesc
	{
		ESysDisplayDriverType driverType = ESysDisplayDriverType::Unknown;
		sys_dsm_index_t index = cvSysDsmIndexInvalid;
		sys_dsm_output_id_t id = 0u;
		Bitmask<ESysDsmOutputFlags> flags = 0u;
		std::string name;
		SysDsmScreenRect screenRect;
		void * driverReserved = nullptr;
	};

	struct SysDsmVideoModeDesc
	{
		ESysDisplayDriverType driverType = ESysDisplayDriverType::Unknown;
		sys_dsm_index_t index = cvSysDsmIndexInvalid;
		sys_dsm_video_mode_id_t id = 0u;
		sys_dsm_video_settings_hash_t settingsHash = 0u;
		SysColorFormat format = SysColorFormat::Unknown;
		SysDsmVideoSettings settings;
		void * driverReserved = nullptr;
	};

	union SysDsmOutputID
	{
		struct
		{
			sys_dsm_index_t uAdapterIndex;
			sys_dsm_index_t uOutputIndex;
		};
		sys_dsm_output_id_t outputID = 0u;
	};

	union SysDsmVideoModeID
	{
		struct
		{
			sys_dsm_output_id_t uOutputID;
			sys_dsm_index_t uColorFormatIndex;
			sys_dsm_index_t uModeIndex;
		};
		sys_dsm_video_mode_id_t modeID = 0u;
	};

	union SysDsmVideoModeHash
	{
		struct
		{
			uint16 uResWidth;
			uint16 uResHeight;
			uint16 uRefreshRate;
			uint8 uFlags;
			uint8 uColorFormatIndex;
		};
		sys_dsm_video_settings_hash_t modeHash = 0u;
	};

	inline bool operator==( const SysDsmVideoSettings & pLhs, const SysDsmVideoSettings & pRhs )
	{
		return ( pLhs.resolution == pRhs.resolution ) && ( pLhs.refreshRate == pRhs.refreshRate ) && ( pLhs.flags == pRhs.flags );
	}

	inline bool operator!=( const SysDsmVideoSettings & pLhs, const SysDsmVideoSettings & pRhs )
	{
		return ( pLhs.resolution != pRhs.resolution ) || ( pLhs.refreshRate != pRhs.refreshRate ) || ( pLhs.flags != pRhs.flags );
	}

	inline bool operator==( const SysDsmVideoSettingsFilter & pLhs, const SysDsmVideoSettingsFilter & pRhs )
	{
		return ( pLhs.refSettings == pRhs.refSettings ) && ( pLhs.flags == pRhs.flags );
	}

	inline bool operator!=( const SysDsmVideoSettingsFilter & pLhs, const SysDsmVideoSettingsFilter & pRhs )
	{
		return ( pLhs.refSettings != pRhs.refSettings ) || ( pLhs.flags != pRhs.flags );
	}

	sys_dsm_video_settings_hash_t sysDsmComputeVideoSettingsHash( SysColorFormat pFormat, const SysDsmVideoSettings & pSettings );

	std::string sysDsmGetVideoSettingsString( SysColorFormat pFormat, const SysDsmVideoSettings & pSettings );

}

#if( TS3_PCL_COMPILER & TS3_PCL_COMPILER_CLANG )
#  pragma clang diagnostic pop
#elif( TS3_PCL_COMPILER & TS3_PCL_COMPILER_GCC )
#  pragma GCC diagnostic pop
#elif( TS3_PCL_COMPILER & TS3_PCL_COMPILER_MSVC )
#  pragma warning( pop )
#endif

#endif // __TS3_SYSTEM_DISPLAY_COMMON_H__
