
#pragma once

#ifndef __TS3DRIVER_GPUAPI_MTLCOMMON_GPU_COMMAND_SYSTEM_H__
#define __TS3DRIVER_GPUAPI_MTLCOMMON_GPU_COMMAND_SYSTEM_H__

#include "MTL_prerequisites.h"
#include <ts3/gpuapi/commandSystem.h>

namespace ts3::gpuapi
{

	/// @brief
	class MetalCommandSystem : public CommandSystem
	{
	public:
		explicit MetalCommandSystem( MetalGPUDevice & pMetalGPUDevice );
		virtual ~MetalCommandSystem();

		virtual std::unique_ptr<CommandContext> acquireCommandContext( ECommandContextType pContextType ) override;

		virtual CommandSync submitContext( CommandContextDirect & pContext, const CommandContextSubmitInfo & pSubmitInfo ) override;

	private:
		MetalCommandList * acquireCommandList( ECommandExecutionMode pCommandExecutionMode );
	};

} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_MTLCOMMON_GPU_COMMAND_SYSTEM_H__
