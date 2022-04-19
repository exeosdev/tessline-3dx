
#ifndef __TS3_SYSTEM_INTERNAL_DISPLAY_SYSTEM_PRIVATE_H__
#define __TS3_SYSTEM_INTERNAL_DISPLAY_SYSTEM_PRIVATE_H__

#include <ts3/system/displayConfiguration.h>
#include <ts3/system/displaySystem.h>
#include <deque>
#include <map>

namespace ts3::system
{

	// The default array with all ColorFormats supported by the library.
	// Used by the common driver layer to iterate over support CFs.
	inline constexpr EColorFormat cvColorFormatArray[] =
	{
		EColorFormat::B8G8R8,
		EColorFormat::B8G8R8A8,
		EColorFormat::B8G8R8A8SRGB,
		EColorFormat::B8G8R8X8,
		EColorFormat::R8G8B8A8,
		EColorFormat::R8G8B8A8SRGB,
		EColorFormat::R8G8B8X8,
		EColorFormat::R10G10B10A2,
	};

	static_assert( staticArraySize( cvColorFormatArray ) == CX_ENUM_COLOR_FORMAT_COUNT );

	union DisplayOutputIDGen
	{
		struct
		{
			dsm_index_t uAdapterIndex;
			dsm_index_t uOutputIndex;
		};

		dsm_output_id_t outputID = 0;
	};

	union DisplayVideoModeIDGen
	{
		struct
		{
			dsm_output_id_t uOutputID;
			dsm_index_t uColorFormatIndex;
			dsm_index_t uModeIndex;
		};

		dsm_video_mode_id_t modeID = 0u;
	};

	union DisplayVideoSettingsHashGen
	{
		struct
		{
			uint16 uResWidth;
			uint16 uResHeight;
			uint16 uRefreshRate;
			uint8 uFlags;
			uint8 uColorFormatIndex;
		};

		dsm_video_settings_hash_t hashValue = 0u;
	};

	inline dsm_index_t dsmExtractOutputIDAdapterIndex( dsm_output_id_t pOutputID )
	{
		DisplayOutputIDGen outputIDGen;
		outputIDGen.outputID = pOutputID;
		return outputIDGen.uAdapterIndex;
	}

	inline dsm_index_t dsmExtractOutputIDOutputIndex( dsm_output_id_t pOutputID )
	{
		DisplayOutputIDGen outputIDGen;
		outputIDGen.outputID = pOutputID;
		return outputIDGen.uOutputIndex;
	}

	using DisplayDriverFactoryCallback = std::function<DisplayDriverHandle()>;
	using DisplayDriverFactoryMap = std::map<EDisplayDriverType, DisplayDriverFactoryCallback>;

	struct DisplayManager::DisplayManagerPrivateData
	{
		DisplayDriverFactoryMap driverFactoryMap;
	};


	/// @brief Private, implementation-specific data of the DisplayDriver class.
	struct DisplayDriver::DisplayDriverPrivateData
	{
		// Storage for holding all adapters. We use deque to guarantee pointer persistence.
		std::deque<DisplayAdapterHandle> adapterInstanceList;

		// Additional helper list, with all adapters stored as handles/pointers.
		// This is returned directly via the public accessor and allows us to avoid building it on the fly each time.
		DisplayAdapterList adapterList;

		// Number of active adapters in the system. Equals adapterList.size() (and, thus, adapterInternalStorage.size() too).
		uint32 activeAdaptersNum = 0;

		// Number of all active outputs in the system, i.e. sum of all active outputs for each adapter present in the system.
		uint32 combinedActiveOutputsNum = 0;

		// Points to the default/primary adapter in the system. Usually this will be adapterList[0];
		DisplayAdapter * primaryAdapter = nullptr;
	};

	/// @brief Private, implementation-specific data of the DisplayAdapter class.
	struct DisplayAdapter::DisplayAdapterPrivateData
	{
		//
		DisplayAdapterDesc adapterDesc;

		std::deque<DisplayOutputHandle> outputInstanceList;

		DisplayOutputList outputList;

		uint32 activeOutputsNum = 0;

		DisplayOutput * primaryOutput = nullptr;
	};

	/// @brief Private, implementation-specific data of the DisplayOutput class.
	struct DisplayOutput::DisplayOutputPrivateData
	{
		// Per-color-format data.
		// Video modes supported by the system may be different for each color format the display supports.
		// Hence, we associate the information about supported DMs with a separate struct which is created
		// for each EColorFormat supported. This provides correct way to have the right data in the cache.
		struct ColorFormatData
		{
			// The color format this data refers to.
			EColorFormat colorFormat = EColorFormat::Unknown;

			// List of video modes supported for this EColorFormat. Stored as queue - as usual.
			std::deque<DisplayVideoModeHandle> videoModeInstanceList;

			// Helper list with handles/pointers.
			DisplayVideoModeList videoModeList;
		};

		//
		DisplayOutputDesc outputDesc;

		// The main data. Stores a ColorFormatData struct for each EColorFormat supported/enumerated.
		std::map<EColorFormat, ColorFormatData> colorFormatMap;

		//
		std::vector<EColorFormat> supportedColorFormatList;
	};

	/// @brief Private, implementation-specific data of the DisplayOutput class.
	struct DisplayVideoMode::DisplayVideoModePrivateData
	{
		DisplayVideoModeDesc modeDesc;
	};

} // namespace ts3::system

#endif // __TS3_SYSTEM_INTERNAL_DISPLAY_SYSTEM_PRIVATE_H__
