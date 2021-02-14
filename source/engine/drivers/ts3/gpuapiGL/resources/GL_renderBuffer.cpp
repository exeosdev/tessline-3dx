
#include "GL_renderBuffer.h"
#include <ts3/gpuapiGL/GL_coreAPIProxy.h>
#include <ts3/gpuapiGL/GL_gpuDevice.h>

namespace ts3::gpuapi
{

	GLRenderBuffer::GLRenderBuffer( GLGPUDevice & pGLGPUDevice,
	                                ERenderBufferType pRenderBufferType,
	                                const RenderBufferLayout & pRenderBufferLayout,
	                                GLRenderbufferObjectHandle pGLRenderbufferObject )
	: RenderBuffer( pGLGPUDevice, pRenderBufferType, pRenderBufferLayout )
	, mGLRenderbufferObject( std::move( pGLRenderbufferObject ) )
	{}

	GLRenderBuffer::GLRenderBuffer( GLGPUDevice & pGLGPUDevice,
	                                ERenderBufferType pRenderBufferType,
	                                const RenderBufferLayout & pRenderBufferLayout,
	                                GLTextureObjectHandle pGLTextureObject )
	: RenderBuffer( pGLGPUDevice, pRenderBufferType, pRenderBufferLayout )
	, mGLTextureObject( std::move( pGLTextureObject ) )
	{}

	GLRenderBuffer::~GLRenderBuffer() = default;

	bool GLRenderBuffer::isNull() const
	{
		return !mGLRenderbufferObject && !mGLTextureObject;
	}

	Handle<GLRenderBuffer> GLRenderBuffer::create( GLGPUDevice & pGLGPUDevice, const RenderBufferCreateInfo & pCreateInfo )
	{
		if( !validateCreateInfo( pCreateInfo ) )
		{
			return nullptr;
		}

		GLRenderbufferCreateInfo openglRBCreateInfo;
		openglRBCreateInfo.dimensions.x = pCreateInfo.layout.bufferSize.width;
		openglRBCreateInfo.dimensions.y = pCreateInfo.layout.bufferSize.height;
		openglRBCreateInfo.internalFormat = GLCoreAPIProxy::translateGLTextureInternalFormat( pCreateInfo.layout.internalDataFormat );
		openglRBCreateInfo.msaaLevel = pCreateInfo.layout.msaaLevel;

		auto openglRenderbuffer = GLRenderbufferObject::create( openglRBCreateInfo );
		if( !openglRenderbuffer )
		{
			return nullptr;
		}

		auto renderBuffer = std::make_shared<GLRenderBuffer>( pGLGPUDevice,
		                                                      pCreateInfo.bufferType,
		                                                      pCreateInfo.layout,
		                                                      std::move( openglRenderbuffer ) );

		return renderBuffer;
	}

}
