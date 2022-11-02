
#pragma once

#ifndef __TS3_GPUAPI_DISPLAY_MANAGER_H__
#define __TS3_GPUAPI_DISPLAY_MANAGER_H__

#include "displayCommon.h"

namespace ts3::GpuAPI
{

	ts3DeclareClassHandle( DisplayManager );

	using AdapterList = std::vector<AdapterDesc>;
	using OutputList = std::vector<OutputDesc>;
	using VideoModeList = std::vector<VideoModeDesc>;

	/// @brief
	class TS3_GPUAPI_API DisplayManager : public DynamicInterface
	{
	public:
		DisplayManager();
		virtual ~DisplayManager();

		/// @brief
		virtual void reset() = 0;

		/// @brief
		virtual AdapterList enumAdapterList() const = 0;

		/// @brief
		virtual AdapterDesc getDefaultAdapter() const = 0;

		/// @brief
		virtual OutputList enumOutputList( display_system_id_t pAdapterID ) const = 0;

		/// @brief
		virtual OutputDesc getDefaultOutput( display_system_id_t pAdapterID ) const = 0;

		/// @brief
		virtual VideoModeList enumVideoModeList( display_system_id_t pOutputID, system::EColorFormat pFormat ) const = 0;
	};

} // namespace ts3::GpuAPI

#endif // __TS3_GPUAPI_DISPLAY_MANAGER_H__
