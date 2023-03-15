
#include "GL_renderBuffer.h"
#include <ts3/gpuapiGL/GL_apiTranslationLayer.h>
#include <ts3/gpuapiGL/GL_gpuDevice.h>

namespace ts3::gpuapi
{

	GLInternalRenderBuffer::GLInternalRenderBuffer(
			GLGPUDevice & pGPUDevice,
			GLRenderbufferObjectHandle pGLRenderbufferObject )
	: GPUDeviceChildObject( pGPUDevice )
	, mGLRenderbufferObject( std::move( pGLRenderbufferObject ) )
	{}

	GLInternalRenderBuffer::GLInternalRenderBuffer(
			GLGPUDevice & pGPUDevice,
			GLTextureObjectHandle pGLTextureObject )
	: GPUDeviceChildObject( pGPUDevice )
	, mGLTextureObject( std::move( pGLTextureObject ) )
	{}

	GpaHandle<GLInternalRenderBuffer> GLInternalRenderBuffer::createInstance(
			GLGPUDevice & pGPUDevice,
			const RenderTargetTextureCreateInfo & pCreateInfo )
	{
		GLRenderbufferCreateInfo openglRenderbufferCreateInfo;
		openglRenderbufferCreateInfo.dimensions.x = pCreateInfo.rttLayout.bufferSize.width;
		openglRenderbufferCreateInfo.dimensions.y = pCreateInfo.rttLayout.bufferSize.height;
		openglRenderbufferCreateInfo.msaaLevel = pCreateInfo.rttLayout.msaaLevel;
		openglRenderbufferCreateInfo.internalFormat =
				atl::translateGLTextureInternalFormat( pCreateInfo.rttLayout.internalDataFormat );

		auto openglRenderbuffer = GLRenderbufferObject::create( openglRenderbufferCreateInfo );
		if( !openglRenderbuffer )
		{
			return nullptr;
		}

		auto renderBuffer = createGPUAPIObject<GLInternalRenderBuffer>( pGPUDevice, std::move( openglRenderbuffer ) );

		return renderBuffer;
	}

} // namespace ts3::gpuapi
