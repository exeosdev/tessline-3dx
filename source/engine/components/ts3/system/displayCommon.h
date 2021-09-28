
#ifndef __TS3_SYSTEM_DISPLAY_COMMON_H__
#define __TS3_SYSTEM_DISPLAY_COMMON_H__

#include "visual.h"

#include <ts3/math/vectorOps.h>
#include <ts3/stdext/arrayView.h>
#include <ts3/stdext/bitmask.h>
#include <ts3/stdext/uuid.h>

#if( TS3_PCL_COMPILER & TS3_PCL_COMPILER_CLANG )
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wgnu-anonymous-struct"
#  pragma clang diagnostic ignored "-Wnested-anon-types"
#elif( TS3_PCL_COMPILER & TS3_PCL_COMPILER_MSVC )
#  pragma warning( push )
#  pragma warning( disable: 4201 )  // 'Nonstandard extension used: nameless struct/union'
#elif( TS3_PCL_COMPILER & ( TS3_PCL_COMPILER_GCC | TS3_PCL_COMPILER_MINGW64 ) )
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wpedantic"
#endif

#if !defined( TS3_SYSTEM_DSM_DRIVER_TYPE_SUPPORT_DXGI )
#  if( TS3_PCL_TARGET_OS & TS3_PCL_TARGET_FLAG_OS_WINFAMILY )
#    define TS3_SYSTEM_DSM_DRIVER_TYPE_SUPPORT_DXGI 1
#  else
#    define TS3_SYSTEM_DSM_DRIVER_TYPE_SUPPORT_DXGI 0
#  endif
#endif

#if !defined( TS3_SYSTEM_DSM_DRIVER_TYPE_SUPPORT_SDL )
#  define TS3_SYSTEM_DSM_DRIVER_TYPE_SUPPORT_SDL 0
#endif

namespace ts3::system
{

    ts3SysDeclareHandle( DisplayManager );
    ts3SysDeclareHandle( DisplayDriver );

    class DisplayAdapter;
    class DisplayOutput;
    class DisplayVideoMode;

	using dsm_index_t = uint16;
	using dsm_output_id_t = uint32;
	using dsm_video_mode_id_t = uint64;
	using dsm_video_settings_hash_t = uint64;
	using DisplayOffset = math::Pos2i;
	using DisplaySize = math::Size2u;

	using DisplayAdapterList = std::vector<DisplayAdapter *>;
	using DisplayOutputList = std::vector<DisplayOutput *>;
	using DisplayVideoModeList = std::vector<DisplayVideoMode *>;

	/// @brief Represents invalid display system index (of an adapter or an output, for example).
	inline constexpr dsm_index_t CX_DSM_INDEX_INVALID = Limits<dsm_index_t>::maxValue - 1;

	/// @brief
	inline constexpr dsm_index_t CX_DSM_INDEX_DEFAULT = Limits<dsm_index_t>::maxValue;

	/// @brief
	inline constexpr dsm_output_id_t CX_DSM_OUTPUT_ID_DEFAULT = Limits<dsm_output_id_t>::maxValue;

	/// @brief Represents invalid display settings hash. Used to identify/report invalid and/or empty configurations.
	inline constexpr dsm_video_settings_hash_t CX_DSM_VIDEO_SETTINGS_HASH_INVALID = Limits<dsm_video_settings_hash_t>::maxValue;

	/// @brief Specifies supported types of drivers available through a DisplayManager.
	/// Driver support is platform-specific and some of them may not be available on some systems.
	/// The default driver is always supported - if a certain platform does not support the required
	///  display-related functionality, DisplayManager creation will fail with NotSupported error.
	enum class EDisplayDriverType : enum_default_value_t
	{
		// Unknown driver. Cannot be used as an input.
		// Reported for an uninitialized driver object.
		Unknown,

		// Native, platform-specific driver. Implemented either with dedicated
		// libraries (like SDL) or low-level system API (Win32, XF86, XRR, Cocoa).
		// Platform availability: All
		Generic,

		// Microsoft DXGI driver. Specific DXGI version depends on the platform.
		// Platform availability: Win32, UWP
		DXGI,

		// SDL-based driver implemented using only the library's functionality.
		// Provided for compliance tests and for compatibility reasons.
		// Platform availability: Linux, macOS
		SDL,

		// Default driver. Actual type is platform-specific, but it must resolve
		// to a driver type which is always supported on the current platform.
		// Most likely, the actual driver type will be as follows:
		// - Win32/UWP: DXGI
		// - Linux/Android/macOS/iOS: Generic
		Default = 0xFFFF,
	};

	/// @brief Represents ID of the vendor of the graphics adapter/device.
	enum class EDisplayAdapterVendorID : enum_default_value_t
	{
	    Unknown,
		AMD,
		ARM,
		Google,
		Intel,
		Microsoft,
		Nvidia,
		Qualcomm,
	};

	/// @brief
	enum EDisplayAdapterFlags : uint32
	{
	    // Adapter is active. The meaning of this flag is driver-specific - do not assume an active adapter
	    // to have, for example, any active outputs. Use DisplayAdapter::hasActiveOutputs() to check that.
	    E_DISPLAY_ADAPTER_FLAG_ACTIVE_BIT = 0x1,
	    // Adapter is a primary/default adapter in the system.
	    E_DISPLAY_ADAPTER_FLAG_PRIMARY_BIT = 0x2,
		// Adapter is a software adapter.
		E_DISPLAY_ADAPTER_FLAG_HARDWARE_BIT = 0x4,
		// Adapter is a software adapter. May indicate WARP-capable adapter in case of a DXGI driver.
		E_DISPLAY_ADAPTER_FLAG_SOFTWARE_BIT = 0x8,
		// Adapter is a multi-node adapter (e.g. classic CF- or SLI-configured adapter set).
		E_DISPLAY_ADAPTER_FLAG_MULTI_NODE_BIT = 0x10,
	};

	/// @brief
	enum EDisplayOutputFlags : uint32
	{
	    // Output is active. In most cases it means the output is connected to the desktop.
	    E_DISPLAY_OUTPUT_FLAG_ACTIVE_BIT = 0x1,
		// Output is the primary output of an adapter.
		E_DISPLAY_OUTPUT_FLAG_PRIMARY_BIT = 0x2
	};

	/// @brief
	enum EDisplayVideoSettingsFlags : uint16
	{
		E_DISPLAY_VIDEO_SETTINGS_FLAG_SCAN_INTERLACED_BIT = 0x1,
		E_DISPLAY_VIDEO_SETTINGS_FLAG_SCAN_PROGRESSIVE_BIT = 0x2,
		E_DISPLAY_VIDEO_SETTINGS_FLAG_STEREO_BIT = 0x4
	};

	enum EDisplayVideoSettingsFilterFlags : uint32
	{
		// If set, resolution is not used as a filter criterion.
		E_DISPLAY_VIDEO_SETTINGS_FILTER_FLAG_RESOLUTION_IGNORE_BIT = 0x1,
		// If set, resolution must match exactly for the mode to be included.
		E_DISPLAY_VIDEO_SETTINGS_FILTER_FLAG_RESOLUTION_MATCH_EXACT_BIT = 0x2,
		// If set, refresh rate is not used as a filter criterion.
		E_DISPLAY_VIDEO_SETTINGS_FILTER_FLAG_REFRESH_RATE_IGNORE_BIT = 0x4,
		// If set, refresh rate must match exactly for the mode to be included.
		E_DISPLAY_VIDEO_SETTINGS_FILTER_FLAG_REFRESH_RATE_MATCH_EXACT_BIT = 0x8,
		// If set, additional flags are not used as a filter criterion.
		E_DISPLAY_VIDEO_SETTINGS_FILTER_FLAG_FLAGS_IGNORE_BIT = 0x40,
	};

	struct ScreenRect
	{
	    DisplayOffset offset;
		DisplaySize size;
	};

	/// @brief
	struct DisplayVideoSettings
	{
	    DisplaySize resolution;
		uint16 refreshRate = 0u;
		Bitmask<EDisplayVideoSettingsFlags> flags = 0u;

		bool equals( const DisplayVideoSettings & pOther ) const
		{
		    return ( resolution == pOther.resolution ) && ( refreshRate == pOther.refreshRate ) && ( flags == pOther.flags );
		}

		bool matches( const DisplayVideoSettings & pOther ) const
		{
		    return ( resolution == pOther.resolution ) && ( refreshRate == pOther.refreshRate ) && flags.isSet( pOther.flags );
		}
	};

	inline bool operator==( const DisplayVideoSettings & pLhs, const DisplayVideoSettings & pRhs )
	{
	    return pLhs.equals( pRhs );
	}

	inline bool operator!=( const DisplayVideoSettings & pLhs, const DisplayVideoSettings & pRhs )
	{
	    return !pLhs.equals( pRhs );
	}

	inline constexpr DisplayVideoSettings cvDisplayVideoSettingsEmpty { { 0U, 0U }, 0U, 0U };

	/// @brief
	struct DisplayVideoSettingsFilter
	{
		// Reference settings used to filter video modes. By default, each property in a mode
		// must be greater than or equal to the reference property (note for resolutions: both
		// dimensions must be >=, i.e. 1024x768 satisfies 1024x600, but 1280x720 does not satisfy
		// 1152x864). This does not apply to the flags, which must have exactly the same bits set.
		// Each property can be ignored by using its respective IGNORE flag. Also, "greater than
		// or equal" condition can be changed to "equal" for a given property by setting MATCH_EXACT
		// flag for that property.
		DisplayVideoSettings refSettings;
		// Filter flags used to control the process. Flags set by default: none.
		Bitmask<EDisplayVideoSettingsFilterFlags> flags = 0;
	};

	inline bool operator==( const DisplayVideoSettingsFilter & pLhs, const DisplayVideoSettingsFilter & pRhs )
	{
	    return ( pLhs.refSettings == pRhs.refSettings ) && ( pLhs.flags == pRhs.flags );
	}

	inline bool operator!=( const DisplayVideoSettingsFilter & pLhs, const DisplayVideoSettingsFilter & pRhs )
	{
	    return ( pLhs.refSettings != pRhs.refSettings ) || ( pLhs.flags != pRhs.flags );
	}

	inline constexpr DisplayVideoSettingsFilter cvDisplayVideoSettingsFilterNone { cvDisplayVideoSettingsEmpty, 0U };

	struct DisplayAdapterDesc
	{
		EDisplayDriverType driverType = EDisplayDriverType::Unknown;
		dsm_index_t adapterIndex = CX_DSM_INDEX_INVALID;
		EDisplayAdapterVendorID vendorID = EDisplayAdapterVendorID::Unknown;
		Bitmask<EDisplayAdapterFlags> flags = 0u;
		std::string name;

		std::string toString() const;
	};

	struct DisplayOutputDesc
	{
		EDisplayDriverType driverType = EDisplayDriverType::Unknown;
		dsm_index_t outputIndex = CX_DSM_INDEX_INVALID;
		dsm_output_id_t outputID = 0u;
		Bitmask<EDisplayOutputFlags> flags = 0u;
		std::string name;
		ScreenRect screenRect;

		std::string toString() const;
	};

	struct DisplayVideoModeDesc
	{
		EDisplayDriverType driverType = EDisplayDriverType::Unknown;
		dsm_index_t videoModeIndex = CX_DSM_INDEX_INVALID;
		dsm_video_mode_id_t videoModeID = 0u;
		dsm_video_settings_hash_t settingsHash = 0u;
		EColorFormat colorFormat = EColorFormat::Unknown;
		DisplayVideoSettings settings;

		std::string toString() const;
	};

	TS3_PCL_ATTR_NO_DISCARD dsm_output_id_t dsmCreateDisplayOutputID( dsm_index_t pAdapterIndex, dsm_index_t pOutputIndex );

	TS3_PCL_ATTR_NO_DISCARD dsm_video_settings_hash_t dsmComputeVideoSettingsHash( EColorFormat pFormat, const DisplayVideoSettings & pSettings );

	TS3_PCL_ATTR_NO_DISCARD std::string dsmGetVideoSettingsString( EColorFormat pFormat, const DisplayVideoSettings & pSettings );

	TS3_PCL_ATTR_NO_DISCARD EDisplayAdapterVendorID dsmResolveAdapterVendorID( const std::string & pAdapterName );

	TS3_PCL_ATTR_NO_DISCARD bool dsmCheckSettingsFilterMatch( const DisplayVideoSettingsFilter & pFilter, const DisplayVideoSettings & pSettings );


} // namespace ts3::system

#if( TS3_PCL_COMPILER & TS3_PCL_COMPILER_CLANG )
#  pragma clang diagnostic pop
#elif( TS3_PCL_COMPILER & TS3_PCL_COMPILER_MSVC )
#  pragma warning( pop )
#elif( TS3_PCL_COMPILER & ( TS3_PCL_COMPILER_GCC | TS3_PCL_COMPILER_MINGW64 ) )
#  pragma GCC diagnostic pop
#endif

#endif // __TS3_SYSTEM_DISPLAY_COMMON_H__
