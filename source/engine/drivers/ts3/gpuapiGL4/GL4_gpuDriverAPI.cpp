
#include "GL4_gpuDriverAPI.h"
#include "GL4_gpuDriver.h"
#include "GL4_gpuDevice.h"
#include <ts3/gpuapiGL/GL_presentationLayer.h>

namespace ts3::gpuapi
{

	GPUDriverHandle GL4GPUDriverInterface::createDriver( const GPUDriverCreateInfo & pCreateInfo )
	{
		GL4GPUDriverCreateInfo gl4CreateInfo;
		gl4CreateInfo.sysContext = pCreateInfo.sysContext;
		gl4CreateInfo.sysContextCreateInfo = pCreateInfo.sysContextCreateInfo;
		gl4CreateInfo.configFlags = pCreateInfo.configFlags;

		return GL4GPUDriver::create( gl4CreateInfo );
	}

	PresentationLayerHandle GL4GPUDriverInterface::createScreenPresentationLayer( GPUDevice & pDevice, const PresentationLayerCreateInfo & pCreateInfo )
	{
		GLPresentationLayerCreateInfo openglCreateInfo;
		openglCreateInfo.screenRect = pCreateInfo.screenRect;
		openglCreateInfo.visualConfig = pCreateInfo.visualConfig;
		openglCreateInfo.displayConfigFlags = pCreateInfo.displayConfigFlags;

		auto * deviceGL = pDevice.queryInterface<GLGPUDevice>();
		return GLScreenPresentationLayer::create( *deviceGL, openglCreateInfo );
	}

}