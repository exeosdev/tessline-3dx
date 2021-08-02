
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
namespace system
{

	ts3DeclareHandle( DisplayDriver );
	ts3DeclareHandle( DisplayManager );

	struct DsmAdapter;
	struct DsmOutput;
	struct DsmVideoMode;
	struct WindowGeometry;

	using dsm_index_t = uint16;
	using dsm_output_id_t = uint32;
	using dsm_video_mode_id_t = uint64;
	using dsm_video_settings_hash_t = uint64;
	using DisplaySize = math::Size2u;

	/// @brief Represents invalid display system index (of an adapter or an output, for example).
	constexpr dsm_index_t cvDsmIndexInvalid = Limits<dsm_index_t>::maxValue;

	/// @brief Represents invalid display settings hash. Used to identify/report invalid and/or empty configurations.
	constexpr dsm_video_settings_hash_t cvDsmVideoSettingsHashInvalid = Limits<dsm_video_settings_hash_t>::maxValue;

	enum class EDsmDisplayDriverType : enum_default_value_t
	{
		// Unknown driver. Cannot be used as an input.
		// Reported for an uninitialized driver object.
		Unknown,
		// Native, platform-specific driver. Implemented either with dedicated
		// libraries (like SDL) or low-level system API (Win32, XF86, XRR, Cocoa).
		// Available on all platforms.
		Generic,
		// Microsoft DXGI driver. Specific DXGI version depends on the platform.
		// Platform availability: Win32, UWP
		DXGI,
		// Default driver. Actual type is platform-dependent:
		// - Win32/UWP: DXGI
		// - Linux/Android/macOS/iOS: Generic
		Default,
	};

	/// @brief Represents ID of the vendor of the graphics adapter/device.
	enum class EDsmAdapterVendorID : enum_default_value_t
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
	enum EDsmAdapterFlags : uint32
	{
		// Adapter is reported by system as the primary one.
		E_DSM_ADAPTER_FLAG_PRIMARY_BIT = 0x1,
		// Adapter is a multi-node adapter (e.g. classic CF- or SLI-configured adapter set)
		E_DSM_ADAPTER_FLAG_MULTI_NODE_BIT = 0x2,
		// Adapter is a software adapter. May indicate WARP-capable adapter in case of DXGI backend.
		E_DSM_ADAPTER_FLAG_SOFTWARE_BIT = 0x4,
	};

	/// @brief
	enum EDsmOutputFlags : uint32
	{
		// Output is the primary output of an adapter
		E_DSM_OUTPUT_FLAG_PRIMARY_BIT = 0x1
	};

	/// @brief
	enum EDsmVideoSettingsFlags : uint16
	{
		E_DSM_VIDEO_SETTINGS_FLAG_SCAN_INTERLACED_BIT = 0x1,
		E_DSM_VIDEO_SETTINGS_FLAG_SCAN_PROGRESSIVE_BIT = 0x2,
		E_DSM_VIDEO_SETTINGS_FLAG_STEREO_BIT = 0x4
	};

	enum EDsmVideoSettingsFilterFlags : uint32
	{
		// If set, resolution is not used as a filter criterion.
		E_DSM_VIDEO_MODE_FILTER_FLAG_RESOLUTION_IGNORE_BIT = 0x1,
		// If set, resolution must match exactly for the mode to be included.
		E_DSM_VIDEO_MODE_FILTER_FLAG_RESOLUTION_MATCH_EXACT_BIT = 0x2,
		// If set, refresh rate is not used as a filter criterion.
		E_DSM_VIDEO_MODE_FILTER_FLAG_REFRESH_RATE_IGNORE_BIT = 0x4,
		// If set, refresh rate must match exactly for the mode to be included.
		E_DSM_VIDEO_MODE_FILTER_FLAG_REFRESH_RATE_MATCH_EXACT_BIT = 0x8,
		// If set, additional flags are not used as a filter criterion.
		E_DSM_VIDEO_MODE_FILTER_FLAG_FLAGS_IGNORE_BIT = 0x40,
	};

	struct DsmScreenRect
	{
		math::Vec2i32 offset;
		math::Vec2u32 size;
	};

	/// @brief
	struct DsmVideoSettings
	{
		math::Size2u resolution;
		uint16 refreshRate = 0u;
		Bitmask<EDsmVideoSettingsFlags> flags = 0u;
	};

	constexpr DsmVideoSettings cvDsmVideoSettingsEmpty { { 0U, 0U }, 0U, 0U };

	/// @brief
	struct DsmVideoSettingsFilter
	{
		// Reference settings used to filter video modes. By default, each property in a mode
		// must be greater than or equal to the reference property (note for resolutions: both
		// dimensions must be >=, i.e. 1024x768 satisfies 1024x600, but 1280x720 does not satisfy
		// 1152x864). This does not apply to flags, which must have exact same bits set.
		// Each property can be ignored by using its respective IGNORE flag. Also, "greater than
		// or equal" condition can be changed to "equal" for a given property by setting MATCH_EXACT
		// flag for that property.
		DsmVideoSettings refSettings;
		// Filter flags used to control the process. Flags set by default: none.
		Bitmask<EDsmVideoSettingsFilterFlags> flags = 0;
	};

	constexpr DsmVideoSettingsFilter cvDsmVideoSettingsFilterNone { cvDsmVideoSettingsEmpty, 0U };

	struct DsmAdapterDesc
	{
		EDsmDisplayDriverType driverType = EDsmDisplayDriverType::Unknown;
		dsm_index_t index = cvDsmIndexInvalid;
		std::string name;
		EDsmAdapterVendorID vendorID;
		Bitmask<EDsmAdapterFlags> flags = 0u;
		void * driverReserved = nullptr;
	};

	struct DsmOutputDesc
	{
		EDsmDisplayDriverType driverType = EDsmDisplayDriverType::Unknown;
		dsm_index_t index = cvDsmIndexInvalid;
		dsm_output_id_t id = 0u;
		Bitmask<EDsmOutputFlags> flags = 0u;
		std::string name;
		DsmScreenRect screenRect;
		void * driverReserved = nullptr;
	};

	struct DsmVideoModeDesc
	{
		EDsmDisplayDriverType driverType = EDsmDisplayDriverType::Unknown;
		dsm_index_t index = cvDsmIndexInvalid;
		dsm_video_mode_id_t id = 0u;
		dsm_video_settings_hash_t settingsHash = 0u;
		GfxColorFormat format = GfxColorFormat::Unknown;
		DsmVideoSettings settings;
		void * driverReserved = nullptr;
	};

	union DsmOutputID
	{
		struct
		{
			dsm_index_t uAdapterIndex;
			dsm_index_t uOutputIndex;
		};
		dsm_output_id_t outputID = 0u;
	};

	union DsmVideoModeID
	{
		struct
		{
			dsm_output_id_t uOutputID;
			dsm_index_t uColorFormatIndex;
			dsm_index_t uModeIndex;
		};
		dsm_video_mode_id_t modeID = 0u;
	};

	union DsmVideoModeHash
	{
		struct
		{
			uint16 uResWidth;
			uint16 uResHeight;
			uint16 uRefreshRate;
			uint8 uFlags;
			uint8 uColorFormatIndex;
		};
		dsm_video_settings_hash_t modeHash = 0u;
	};

	inline bool operator==( const DsmVideoSettings & pLhs, const DsmVideoSettings & pRhs )
	{
		return ( pLhs.resolution == pRhs.resolution ) && ( pLhs.refreshRate == pRhs.refreshRate ) && ( pLhs.flags == pRhs.flags );
	}

	inline bool operator!=( const DsmVideoSettings & pLhs, const DsmVideoSettings & pRhs )
	{
		return ( pLhs.resolution != pRhs.resolution ) || ( pLhs.refreshRate != pRhs.refreshRate ) || ( pLhs.flags != pRhs.flags );
	}

	inline bool operator==( const DsmVideoSettingsFilter & pLhs, const DsmVideoSettingsFilter & pRhs )
	{
		return ( pLhs.refSettings == pRhs.refSettings ) && ( pLhs.flags == pRhs.flags );
	}

	inline bool operator!=( const DsmVideoSettingsFilter & pLhs, const DsmVideoSettingsFilter & pRhs )
	{
		return ( pLhs.refSettings != pRhs.refSettings ) || ( pLhs.flags != pRhs.flags );
	}

	dsm_video_settings_hash_t sysDsmComputeVideoSettingsHash( GfxColorFormat pFormat, const DsmVideoSettings & pSettings );

	std::string sysDsmGetVideoSettingsString( GfxColorFormat pFormat, const DsmVideoSettings & pSettings );

} // namespace system
} // namespace ts3

#if( TS3_PCL_COMPILER & TS3_PCL_COMPILER_CLANG )
#  pragma clang diagnostic pop
#elif( TS3_PCL_COMPILER & TS3_PCL_COMPILER_GCC )
#  pragma GCC diagnostic pop
#elif( TS3_PCL_COMPILER & TS3_PCL_COMPILER_MSVC )
#  pragma warning( pop )
#endif

#endif // __TS3_SYSTEM_DISPLAY_COMMON_H__
