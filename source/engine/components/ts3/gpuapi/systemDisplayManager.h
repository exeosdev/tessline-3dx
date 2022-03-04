
#pragma once

#ifndef __TS3_GPUAPI_SYSTEM_DISPLAY_MANAGER_H__
#define __TS3_GPUAPI_SYSTEM_DISPLAY_MANAGER_H__

#include "displayManager.h"

namespace ts3
{
namespace gpuapi
{

	ts3DeclareClassHandle( SystemDisplayManager );

	struct SystemDisplayManagerCreateInfo
	{
		system::temContext * exfSystemContext = nullptr;
		ts3::ESysDsmDisplayDriverType exfDisplayDriverType = ts3::ESysDsmDisplayDriverType::Default;
	};

	/// @brief
	class TS3_GPUAPI_CLASS SystemDisplayManager final : public DisplayManager
	{
		friend class GPUDriver;

	public:
		system::DisplayDriver * const mExfDisplayDriver = nullptr;

		explicit SystemDisplayManager( system::DisplayDriver * pExfDisplayDriver );
		virtual ~SystemDisplayManager();

		/// @brief
		virtual void reset() override final;

		/// @brief
		virtual AdapterList enumAdapterList() const override;

		/// @brief
		virtual AdapterDesc getDefaultAdapter() const override;

		/// @brief
		virtual OutputList enumOutputList( display_system_id_t pAdapterID ) const override;

		/// @brief
		virtual OutputDesc getDefaultOutput( display_system_id_t pAdapterID ) const override;

		/// @brief
		virtual VideoModeList enumVideoModeList( display_system_id_t pOutputID, system::EColorFormat pFormat ) const override;
	};

} /* namespace ts3 */
} /* namespace gpuapi */

#endif // __TS3_GPUAPI_SYSTEM_DISPLAY_MANAGER_H__
