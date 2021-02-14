
#pragma once

#ifndef __TS3_GPUAPI_COMMAND_SYSTEM_H__
#define __TS3_GPUAPI_COMMAND_SYSTEM_H__

#include "commonCommandDefs.h"
#include <unordered_map>

namespace ts3::gpuapi
{

	class TS3_GPUAPI_CLASS CommandSystem : public GPUDeviceChildObject
	{
		friend class CommandContextDirect;

	public:
		explicit CommandSystem( GPUDevice & pGPUDevice );
		virtual ~CommandSystem();

		virtual std::unique_ptr<CommandContext> acquireCommandContext( ECommandContextType pContextType ) = 0;

		virtual CommandSync submitContext( CommandContextDirect & pContext, const CommandContextSubmitInfo & pSubmitInfo ) = 0;

		template <typename TpContext>
		std::unique_ptr<TpContext> acquireCommandContext()
		{
			return moveInterfaceUniquePtr<TpContext>( acquireCommandContext( TpContext::sContextType ) );
		}
		
		bool setQueueAlias( gpu_cmd_device_queue_id_t pAliasID, gpu_cmd_device_queue_id_t pMappedID );

		bool removeQueueAlias( gpu_cmd_device_queue_id_t pAliasID );

		bool checkQueueAlias( gpu_cmd_device_queue_id_t pAliasID ) const;

		gpu_cmd_device_queue_id_t resolveQueueID( gpu_cmd_device_queue_id_t pQueueID ) const;

	private:
		using DeviceQueueAliasMap = std::unordered_map<gpu_cmd_device_queue_id_t, gpu_cmd_device_queue_id_t>;
		DeviceQueueAliasMap _deviceQueueAliasMap;
	};

}

#endif // __TS3_GPUAPI_COMMAND_SYSTEM_H__
