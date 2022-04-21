
#include "GLES3_gpuDriverAPI.h"
#include "GLES3_gpuDriver.h"
#include "GLES3_gpuDevice.h"
#include <ts3/gpuapiGL/GL_presentationLayer.h>

namespace ts3::gpuapi
{

	GPUDriverHandle GLES3GPUDriverInterface::createDriver( const GPUDriverCreateInfo & pCreateInfo )
	{
		GLES3GPUDriverCreateInfo GLES3CreateInfo;
		GLES3CreateInfo.sysContext = pCreateInfo.sysContext;
		GLES3CreateInfo.sysContextCreateInfo = pCreateInfo.sysContextCreateInfo;
		GLES3CreateInfo.configFlags = pCreateInfo.configFlags;

		return GLES3GPUDriver::create( GLES3CreateInfo );
	}

	PresentationLayerHandle GLES3GPUDriverInterface::createScreenPresentationLayer( GPUDevice & pDevice, const PresentationLayerCreateInfo & pCreateInfo )
	{
		GLPresentationLayerCreateInfo openglCreateInfo;
		openglCreateInfo.screenRect = pCreateInfo.screenRect;
		openglCreateInfo.visualConfig = pCreateInfo.visualConfig;
		openglCreateInfo.displayConfigFlags = pCreateInfo.displayConfigFlags;

		auto * deviceGL = pDevice.queryInterface<GLGPUDevice>();
		return GLScreenPresentationLayer::create( *deviceGL, openglCreateInfo );
	}

} // namespace ts3::gpuapi
