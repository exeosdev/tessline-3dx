
#include "DX12_gpuCmdManager.h"
#include "DX12_gpuCmdContext.h"
#include "DX12_gpuDevice.h"
#include "DX12_coreAPIProxy.h"

namespace ts3::gpuapi
{

	DX12GPUCmdManager::DX12GPUCmdManager( DX12GPUDevice & pGPUDevice )
	: GPUCmdManager( pGPUDevice )
	, mD3D12Device( pGPUDevice.mD3D12Device )
	{}

	DX12GPUCmdManager::~DX12GPUCmdManager() = default;

	bool DX12GPUCmdManager::initialize()
	{
		// TODO: Vendor-/GPU Family-specific checks:
		//  if(INTEL) DEFAULT_TRANSFER = DEFAULT_GRAPHICS; (UMA architecture)
		//  if(!AMD) DEFAULT_COMPUTE = DEFAULT_GRAPHICS; (Independent, Async Compute Engine support)

		DeviceCommandQueueProperties queueProperties;

		queueProperties.commandClassFlags = GPU_CMD_COMMAND_CLASS_GRAPHICS_BIT;
		if( !initializeDeviceQueue( E_DEVICE_COMMAND_QUEUE_ID_DEFAULT_GRAPHICS, queueProperties ) )
		{
			return false;
		}

		queueProperties.commandClassFlags = GPU_CMD_COMMAND_CLASS_COMPUTE_BIT;
		if( !initializeDeviceQueue( E_DEVICE_COMMAND_QUEUE_ID_DEFAULT_COMPUTE, queueProperties ) )
		{
			return false;
		}

		queueProperties.commandClassFlags = GPU_CMD_COMMAND_CLASS_TRANSFER_BIT;
		if( !initializeDeviceQueue( E_DEVICE_COMMAND_QUEUE_ID_DEFAULT_TRANSFER, queueProperties ) )
		{
			return false;
		}

		return true;
	}

	CommandContext * DX12GPUCmdManager::acquireContext( const CommandContextProperties & pContextProperties )
	{
		DX12CommandContext * dx12CmdContext = nullptr;

		auto commandListType = DX12CoreAPIProxy::translateCommandListType( pContextProperties.contextType, pContextProperties.commandClassFlags );

		auto & availableContextList = _availableContextMap[commandListType];
		if( !availableContextList.empty() )
		{
			dx12CmdContext = availableContextList.back();
			availableContextList.pop_back();
		}

		if( !dx12CmdContext )
		{
			dx12CmdContext = createCmdContext( commandListType, true );
		}

		if( dx12CmdContext )
		{
			dx12CmdContext->setActiveState();
		}

		return dx12CmdContext;
	}

	bool DX12GPUCmdManager::executeContext( gpu_cmd_device_queue_id_t pQueueID, CommandContext & pContext )
	{
		auto * deviceQueueData = ts3::getMapValueOrNullptr( _deviceQueueMap, pQueueID );
		if( !deviceQueueData )
		{
			return false;
		}

		auto * dx12CmdContext = pContext.queryInterface<DX12CommandContext>();
		ID3D12CommandList * commandListArray[] = { dx12CmdContext->mD3D12GraphicsCommandList.Get() };
		deviceQueueData->d3d12CommandQueue->ExecuteCommandLists( 1, commandListArray );

		return true;
	}

	bool DX12GPUCmdManager::initializeDeviceQueue( gpu_cmd_device_queue_id_t pQueueID, const DeviceCommandQueueProperties & pQueueProperties )
	{
		if( checkQueueAlias( pQueueID ) )
		{
			return true;
		}

		auto * existingQueue = ts3::getMapValueOrNullptr( _deviceQueueMap, pQueueID );
		if( existingQueue )
		{
			return true;
		}

		auto commandListType = DX12CoreAPIProxy::translateCommandListType( ECommandContextType::Direct, pQueueProperties.commandClassFlags );
		auto d3d12CommandQueue = DX12CoreAPIProxy::createD3D12CommandQueue( mD3D12Device, commandListType );
		if( !d3d12CommandQueue )
		{
			return false;
		}

		auto & deviceQueueData = _deviceQueueMap[pQueueID];
		deviceQueueData.d3d12CommandQueue = std::move( d3d12CommandQueue );

		return true;
	}

	bool DX12GPUCmdManager::isQueueAvailable( gpu_cmd_device_queue_id_t pQueueID ) const
	{
		pQueueID = resolveQueueAlias( pQueueID );
		auto * deviceQueueData = ts3::getMapValueOrNullptr( _deviceQueueMap, pQueueID );
		return deviceQueueData != nullptr;
	}

	ID3D12CommandQueue * DX12GPUCmdManager::getD3D12DeviceQueue( gpu_cmd_device_queue_id_t pQueueID ) const
	{
		pQueueID = resolveQueueAlias( pQueueID );
		auto * deviceQueueData = ts3::getMapValueOrNullptr( _deviceQueueMap, pQueueID );
		return deviceQueueData ? deviceQueueData->d3d12CommandQueue.Get() : nullptr;
	}

	DX12CommandContext * DX12GPUCmdManager::createCmdContext( D3D12_COMMAND_LIST_TYPE pD3D12CommandListType, bool pAutoAcquire )
	{
		auto d3d12CommandListData = DX12CoreAPIProxy::createD3D12CommandList( mD3D12Device, pD3D12CommandListType );
		if( !d3d12CommandListData )
		{
			return nullptr;
		}

		auto cmdContext = createGPUAPIObject<DX12CommandContext>( *this,
		                                                         std::move( d3d12CommandListData.commandAllocator ),
		                                                         std::move( d3d12CommandListData.graphicsCommandList ) );

		auto * cmdContextPtr = cmdContext.get();
		_cmdContextStorage[cmdContextPtr] = std::move( cmdContext );

		if( pAutoAcquire )
		{
			cmdContextPtr->setActiveState();
		}
		else
		{
			// Every context supports a specific command category, represented with D3D12_COMMAND_LIST_TYPE.
			// Internal map has lists of all available (unused) contexts which support that specific class.
			auto & availableContextList = _availableContextMap[pD3D12CommandListType];
			availableContextList.push_back( cmdContextPtr );
		}

		return cmdContextPtr;
	}

}
