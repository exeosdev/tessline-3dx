
#include "DX12_gpuDriverAPI.h"
#include "DX12_gpuDriver.h"
#include "DX12_gpuDevice.h"
#include <ts3/gpuapiDX12/system/DX12_presentationLayer.h>

namespace ts3::gpuapi
{

	GPUDriverHandle DX12GPUDriverInterface::createDriver( const GPUDriverCreateInfo & pCreateInfo )
	{
		DX12GPUDriverCreateInfo dx12CreateInfo;
		dx12CreateInfo.exfSysContext = pCreateInfo.exfSysContext;
		dx12CreateInfo.configFlags = pCreateInfo.configFlags;

		return DX12GPUDriver::create( dx12CreateInfo );
	}

	PresentationLayerHandle DX12GPUDriverInterface::createScreenPresentationLayer( GPUDevice & pDevice, const PresentationLayerCreateInfo & pCreateInfo )
	{
		DX12PresentationLayerCreateInfo dx12CreateInfo;
		dx12CreateInfo.coreEngineState = pCreateInfo.coreEngineState;
		dx12CreateInfo.screenRect = pCreateInfo.screenRect;
		dx12CreateInfo.visualConfig = pCreateInfo.visualConfig;
		dx12CreateInfo.displayConfigFlags = pCreateInfo.displayConfigFlags;

		if( pDevice.isDebugDevice() )
		{
			dx12CreateInfo.dxgiFlags.set( DXGI_CREATE_FACTORY_DEBUG );
		}

		auto * deviceDX12 = pDevice.queryInterface<DX12GPUDevice>();
		return DX12ScreenPresentationLayer::create( *deviceDX12, dx12CreateInfo );
	}

}