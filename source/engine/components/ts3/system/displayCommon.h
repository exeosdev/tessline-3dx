
#ifndef __TS3_SYSTEM_DISPLAY_COMMON_H__
#define __TS3_SYSTEM_DISPLAY_COMMON_H__

#include "visual.h"

#include <ts3/math/vectorOps.h>
#include <ts3/stdext/bitmask.h>
#include <ts3/stdext/uuid.h>

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

#define TS3_SYSTEM_DSM_DRIVER_TYPE_SUPPORT_DXGI 1
#define TS3_SYSTEM_DSM_DRIVER_TYPE_SUPPORT_SDL 0

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

	/// @brief Represents invalid display system index (of an adapter or an output, for example).
	constexpr dsm_index_t CX_DSM_INDEX_INVALID = Limits<dsm_index_t>::maxValue;

	/// @brief Represents invalid display system index (of an adapter or an output, for example).
	constexpr dsm_index_t CX_DSM_INDEX_DEFAULT = Limits<dsm_index_t>::maxValue - 1;

	/// @brief Represents invalid display settings hash. Used to identify/report invalid and/or empty configurations.
	constexpr dsm_video_settings_hash_t CX_DSM_VIDEO_SETTINGS_HASH_INVALID = Limits<dsm_video_settings_hash_t>::maxValue;

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
		// Adapter is reported by system as the primary one.
		E_DISPLAY_ADAPTER_FLAG_PRIMARY_BIT = 0x1,
		// Adapter is a multi-node adapter (e.g. classic CF- or SLI-configured adapter set)
		E_DISPLAY_ADAPTER_FLAG_MULTI_NODE_BIT = 0x2,
		// Adapter is a software adapter. May indicate WARP-capable adapter in case of DXGI backend.
		E_DISPLAY_ADAPTER_FLAG_SOFTWARE_BIT = 0x4,
	};

	/// @brief
	enum EDisplayOutputFlags : uint32
	{
		// Output is the primary output of an adapter
		E_DISPLAY_OUTPUT_FLAG_PRIMARY_BIT = 0x1
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
	};

	constexpr DisplayVideoSettings cvDisplayVideoSettingsEmpty { { 0U, 0U }, 0U, 0U };

	/// @brief
	struct DisplayVideoSettingsFilter
	{
		// Reference settings used to filter video modes. By default, each property in a mode
		// must be greater than or equal to the reference property (note for resolutions: both
		// dimensions must be >=, i.e. 1024x768 satisfies 1024x600, but 1280x720 does not satisfy
		// 1152x864). This does not apply to the flags, which must have exact same bits set.
		// Each property can be ignored by using its respective IGNORE flag. Also, "greater than
		// or equal" condition can be changed to "equal" for a given property by setting MATCH_EXACT
		// flag for that property.
		DisplayVideoSettings refSettings;
		// Filter flags used to control the process. Flags set by default: none.
		Bitmask<EDisplayVideoSettingsFilterFlags> flags = 0;
	};

	constexpr DisplayVideoSettingsFilter cvDisplayVideoSettingsFilterNone { cvDisplayVideoSettingsEmpty, 0U };

	struct DisplayAdapterDesc
	{
		EDisplayDriverType driverType = EDisplayDriverType::Unknown;
		dsm_index_t adapterIndex = CX_DSM_INDEX_INVALID;
		EDisplayAdapterVendorID vendorID = EDisplayAdapterVendorID::Unknown;
		Bitmask<EDisplayAdapterFlags> flags = 0u;
		std::string name;
	};

	struct DisplayOutputDesc
	{
		EDisplayDriverType driverType = EDisplayDriverType::Unknown;
		dsm_index_t outputIndex = CX_DSM_INDEX_INVALID;
		dsm_output_id_t outputID = 0u;
		Bitmask<EDisplayOutputFlags> flags = 0u;
		std::string name;
		ScreenRect screenRect;
	};

	struct DisplayVideoModeDesc
	{
		EDisplayDriverType driverType = EDisplayDriverType::Unknown;
		dsm_index_t videoModeIndex = CX_DSM_INDEX_INVALID;
		dsm_video_mode_id_t videoModeID = 0u;
		dsm_video_settings_hash_t settingsHash = 0u;
		ColorFormat colorFormat = ColorFormat::Unknown;
		DisplayVideoSettings settings;
	};

	using DisplayAdapterList = std::vector<DisplayAdapter *>;
	using DisplayOutputList = std::vector<DisplayOutput *>;
	using DisplayVideoModeList = std::vector<DisplayVideoMode *>;

	TS3_PCL_ATTR_NO_DISCARD dsm_video_settings_hash_t dsmComputeVideoSettingsHash( ColorFormat pFormat, const DisplayVideoSettings & pSettings );

	TS3_PCL_ATTR_NO_DISCARD std::string dsmGetVideoSettingsString( ColorFormat pFormat, const DisplayVideoSettings & pSettings );

	TS3_PCL_ATTR_NO_DISCARD ColorFormat dsmResolveSystemColorFormat( ColorFormat pColorFormat );


} // namespace ts3::system

#if( TS3_PCL_COMPILER & TS3_PCL_COMPILER_CLANG )
#  pragma clang diagnostic pop
#elif( TS3_PCL_COMPILER & TS3_PCL_COMPILER_GCC )
#  pragma GCC diagnostic pop
#elif( TS3_PCL_COMPILER & TS3_PCL_COMPILER_MSVC )
#  pragma warning( pop )
#endif

#endif // __TS3_SYSTEM_DISPLAY_COMMON_H__
