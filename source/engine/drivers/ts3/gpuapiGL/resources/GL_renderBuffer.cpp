
#include "GL_renderBuffer.h"
#include <ts3/gpuapiGL/GL_coreAPIProxy.h>
#include <ts3/gpuapiGL/GL_gpuDevice.h>

namespace ts3::gpuapi
{

	GLInternalRenderBuffer::GLInternalRenderBuffer(
			GLGPUDevice & pGLGPUDevice,
			GLRenderbufferObjectHandle pGLRenderbufferObject )
	: GPUDeviceChildObject( pGLGPUDevice )
	, mGLRenderbufferObject( std::move( pGLRenderbufferObject ) )
	{}

	GLInternalRenderBuffer::GLInternalRenderBuffer(
			GLGPUDevice & pGLGPUDevice,
			GLTextureObjectHandle pGLTextureObject )
	: GPUDeviceChildObject( pGLGPUDevice )
	, mGLTextureObject( std::move( pGLTextureObject ) )
	{}

	GpaHandle<GLInternalRenderBuffer> GLInternalRenderBuffer::createInstance(
			GLGPUDevice & pGLGPUDevice,
			const RenderTargetTextureCreateInfo & pCreateInfo )
	{
		GLRenderbufferCreateInfo openglRenderbufferCreateInfo;
		openglRenderbufferCreateInfo.dimensions.x = pCreateInfo.rttLayout.bufferSize.width;
		openglRenderbufferCreateInfo.dimensions.y = pCreateInfo.rttLayout.bufferSize.height;
		openglRenderbufferCreateInfo.msaaLevel = pCreateInfo.rttLayout.msaaLevel;
		openglRenderbufferCreateInfo.internalFormat =
				GLCoreAPIProxy::translateGLTextureInternalFormat( pCreateInfo.rttLayout.internalDataFormat );

		auto openglRenderbuffer = GLRenderbufferObject::create( openglRenderbufferCreateInfo );
		if( !openglRenderbuffer )
		{
			return nullptr;
		}

		auto renderBuffer = createGPUAPIObject<GLInternalRenderBuffer>( pGLGPUDevice, std::move( openglRenderbuffer ) );

		return renderBuffer;
	}

} // namespace ts3::gpuapi
