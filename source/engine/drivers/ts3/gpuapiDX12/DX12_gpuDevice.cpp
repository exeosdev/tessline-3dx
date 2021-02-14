
#include "DX12_gpuDevice.h"
#include "DX12_gpuDriver.h"
#include "DX12_gpuCmdContext.h"
#include "DX12_gpuCmdManager.h"
#include "DX12_coreAPIProxy.h"

namespace ts3::gpuapi
{

	DX12GPUDevice::DX12GPUDevice( DX12GPUDriver & pDriver,
	                              ComPtr<IDXGIFactory3> pDXGIFactory3,
	                              ComPtr<ID3D12Device> pD3D12Device,
	                              ComPtr<ID3D12Debug> pD3D12DebugInterface )
	: DXGPUDevice( pDriver, pDXGIFactory3 )
	, mDXGIFactory3( std::move( pDXGIFactory3 ) )
	, mD3D12Device( std::move( pD3D12Device ) )
	, mD3D12DebugInterface( std::move( pD3D12DebugInterface ) )
	{ }

	DX12GPUDevice::~DX12GPUDevice() = default;

	DX12GPUDeviceHandle DX12GPUDevice::create( DX12GPUDriver & pDX12Driver, const DX12GPUDeviceCreateInfo & pCreateInfo )
	{
		auto d3d12DebugInterface = DX12CoreAPIProxy::initializeD3D12DebugInterface( pDX12Driver.getConfigFlags() );

		auto d3d12Device = DX12CoreAPIProxy::createD3D12Device();
		ts3DebugAssert( d3d12Device );

		auto dxgiFactory3 = DX12CoreAPIProxy::createDXGIFactoryForD3D12Device( d3d12Device, d3d12DebugInterface );
		ts3DebugAssert( dxgiFactory3 );

		auto dx12GPUDevice = createGPUAPIObject<DX12GPUDevice>( pDX12Driver, dxgiFactory3, d3d12Device, d3d12DebugInterface );
		ts3DebugAssert( dx12GPUDevice );

		dx12GPUDevice->initializeCommandSystem();

		if( pCreateInfo.flags.isSet( E_GPU_DEVICE_CREATE_FLAG_INIT_DEFAULT_PRESENT_QUEUE_BIT ) )
		{
			dx12GPUDevice->initializeDefaultPresentDeviceQueue();
		}

		return dx12GPUDevice;
	}

	ID3D12CommandQueue * DX12GPUDevice::getD3D12DeviceQueue( gpu_cmd_device_queue_id_t pQueueID ) const
	{
		return _cmdManager->queryInterface<DX12GPUCmdManager>()->getD3D12DeviceQueue( pQueueID );
	}

	void DX12GPUDevice::initializeCommandSystem()
	{
		ts3DebugAssert( !_cmdManager );
		auto cmdManager = createGPUAPIObject<DX12GPUCmdManager>( *this );
		auto initResult = cmdManager->initialize();
		if( initResult )
		{
			_cmdManager = std::move( cmdManager );
		}
	}

	void DX12GPUDevice::initializeDefaultPresentDeviceQueue()
	{
		auto * dx12CmdManager = _cmdManager->queryInterface<DX12GPUCmdManager>();
		if( !_cmdManager->isQueueAvailable( E_DEVICE_COMMAND_QUEUE_ID_PRESENT ) )
		{
			ts3DebugAssert( _cmdManager->isQueueAvailable( E_DEVICE_COMMAND_QUEUE_ID_DEFAULT_GRAPHICS ) );
			_cmdManager->setQueueAlias( E_DEVICE_COMMAND_QUEUE_ID_PRESENT, E_DEVICE_COMMAND_QUEUE_ID_DEFAULT_GRAPHICS );
		}
	}

}
