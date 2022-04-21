
#pragma once

#ifndef __TS3DRIVER_GPUAPI_VKCOMMON_COMMAND_SYSTEM_H__
#define __TS3DRIVER_GPUAPI_VKCOMMON_COMMAND_SYSTEM_H__

#include "VK_prerequisites.h"
#include <ts3/gpuapi/commandSystem.h>

namespace ts3::gpuapi
{

	/// @brief
	class TS3GX_VKCOMMON_CLASS VKCommandSystem : public CommandSystem
	{
	public:
		explicit VKCommandSystem( VKGPUDevice & pVKGPUDevice );
		virtual ~VKCommandSystem();

		virtual std::unique_ptr<CommandContext> acquireCommandContext( ECommandContextType pContextType ) override;
		virtual CommandSync submitContext( CommandContextDirect & pContext, const CommandContextSubmitInfo & pSubmitInfo ) override;
	};

} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_VKCOMMON_COMMAND_SYSTEM_H__
