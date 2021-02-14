
#include "DX11_gpuDriverAPI.h"
#include "DX11_gpuDriver.h"
#include "DX11_gpuDevice.h"
#include "DX11_presentationLayer.h"

namespace ts3::gpuapi
{

	GPUDriverHandle DX11GPUDriverInterface::createDriver( const GPUDriverCreateInfo & pCreateInfo )
	{
		DX11GPUDriverCreateInfo dx11CreateInfo;
		dx11CreateInfo.sysContext = pCreateInfo.sysContext;
		dx11CreateInfo.sysContextCreateInfo = pCreateInfo.sysContextCreateInfo;
		dx11CreateInfo.configFlags = pCreateInfo.configFlags;

		return DX11GPUDriver::create( dx11CreateInfo );
	}

	PresentationLayerHandle DX11GPUDriverInterface::createScreenPresentationLayer( GPUDevice & pDevice, const PresentationLayerCreateInfo & pCreateInfo )
	{
		DX11PresentationLayerCreateInfo dx11CreateInfo;
		dx11CreateInfo.screenRect = pCreateInfo.screenRect;
		dx11CreateInfo.visualConfig = pCreateInfo.visualConfig;
		dx11CreateInfo.displayConfigFlags = pCreateInfo.displayConfigFlags;

		auto * deviceDX11 = pDevice.queryInterface<DX11GPUDevice>();
		return DX11ScreenPresentationLayer::create( *deviceDX11, dx11CreateInfo );
	}

}
