
#include "commandSystem.h"
#include "gpuDevice.h"
#include <ts3/stdext/mapUtils.h>

namespace ts3::gpuapi
{

	CommandSystem::CommandSystem( GPUDevice & pGPUDevice )
	: GPUDeviceChildObject( pGPUDevice )
	{}

	CommandSystem::~CommandSystem() = default;

	bool CommandSystem::setQueueAlias( gpu_cmd_device_queue_id_t pAliasID, gpu_cmd_device_queue_id_t pMappedID )
	{
		if( ( pAliasID == cxCmdDeviceQueueIDInvalid ) || ( pMappedID == cxCmdDeviceQueueIDInvalid ) )
		{
			return false;
		}
		auto aliasIter = _deviceQueueAliasMap.find( pAliasID );
		if( aliasIter == _deviceQueueAliasMap.end() )
		{
			_deviceQueueAliasMap[pAliasID] = pMappedID;
			return true;
		}
		return false;
	}

	bool CommandSystem::removeQueueAlias( gpu_cmd_device_queue_id_t pAliasID )
	{
		auto aliasIter = _deviceQueueAliasMap.find( pAliasID );
		if( aliasIter != _deviceQueueAliasMap.end() )
		{
			_deviceQueueAliasMap.erase( aliasIter );
			return true;
		}
		return false;
	}

	bool CommandSystem::checkQueueAlias( gpu_cmd_device_queue_id_t pAliasID ) const
	{
		return resolveQueueID( pAliasID ) != pAliasID;
	}

	gpu_cmd_device_queue_id_t CommandSystem::resolveQueueID( gpu_cmd_device_queue_id_t pQueueID ) const
	{
		// Check if the specified ID is an alias. If so, the actual queue ID is the resolved name.
		auto resolvedID = ts3::getMapValueOrDefault( _deviceQueueAliasMap, pQueueID, cxCmdDeviceQueueIDInvalid );
		if( resolvedID == cxCmdDeviceQueueIDInvalid )
		{
			// If no such alias could be found, we assume this is a direct ID of a queue.
			resolvedID = pQueueID;
		}
		return resolvedID;
	}

}
