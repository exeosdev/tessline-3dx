
#pragma once

#ifndef __TS3_GPUAPI_SYSTEM_DISPLAY_MANAGER_H__
#define __TS3_GPUAPI_SYSTEM_DISPLAY_MANAGER_H__

#include "displayManager.h"

namespace ts3::gpuapi
{

	ts3DeclareClassHandle( SystemDisplayManager );

	struct SystemDisplayManagerCreateInfo
	{
		ts3::SysContext * exfSysContext = nullptr;
		ts3::ESysDisplayDriverType exfDisplayDriverType = ts3::ESysDisplayDriverType::Default;
	};

	/// @brief
	class TS3_GPUAPI_CLASS SystemDisplayManager final : public DisplayManager
	{
		friend class GPUDriver;

	public:
		ts3::SysDisplayDriver * const mExfDisplayDriver = nullptr;

		explicit SystemDisplayManager( ts3::SysDisplayDriver * pExfDisplayDriver );
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
		virtual VideoModeList enumVideoModeList( display_system_id_t pOutputID, ts3::SysColorFormat pFormat ) const override;
	};

}

#endif // __TS3_GPUAPI_SYSTEM_DISPLAY_MANAGER_H__
