
#include "GL_renderTargetStateObject.h"
#include <ts3/gpuapi/resources/renderBuffer.h>
#include <ts3/gpuapi/resources/texture.h>
#include <ts3/gpuapiGL/GL_gpuDevice.h>
#include <ts3/gpuapiGL/objects/GL_framebufferObject.h>
#include <ts3/gpuapiGL/objects/GL_renderbufferObject.h>
#include <ts3/gpuapiGL/resources/GL_renderBuffer.h>
#include <ts3/gpuapiGL/resources/GL_texture.h>

namespace ts3
{
namespace gpuapi
{

	GLRenderTargetStateObject::GLRenderTargetStateObject( GLGPUDevice & pGPUDevice,
	                                                      const RenderTargetLayout & pRTLayout,
	                                                      const RenderTargetResourceBinding & pRTResourceBinding,
	                                                      GLFramebufferObjectHandle pGLFramebufferObject )
	: RenderTargetStateObject( pGPUDevice, pRTLayout, pRTResourceBinding )
	, mGLFramebufferObject( std::move( pGLFramebufferObject ) )
	{}

	GLRenderTargetStateObject::~GLRenderTargetStateObject() = default;

	GpaHandle<GLRenderTargetStateObject> GLRenderTargetStateObject::create( GLGPUDevice & pGPUDevice,
	                                                                        const RenderTargetStateObjectCreateInfo & pCreateInfo )
	{
		RenderTargetLayout rtLayout;
		RenderTargetResourceBinding rtResourceBinding;

		if( !createRenderTargetLayoutAndResourceBinding( pCreateInfo.rtResourceBindingDesc, rtLayout, rtResourceBinding ) )
		{
			return nullptr;
		}

		auto framebufferObject = GLFramebufferObject::create();
		if( !framebufferObject )
		{
			return nullptr;
		}

		for( uint32 colorAttachmentIndex = 0; colorAttachmentIndex < GPU_SYSTEM_METRIC_RT_MAX_COLOR_ATTACHMENTS_NUM; ++colorAttachmentIndex )
		{
			auto & colorAttachmentBinding = rtResourceBinding.colorAttachmentArray[colorAttachmentIndex];
			if( colorAttachmentBinding )
			{
				if( colorAttachmentBinding.attachmentResourceType == ERenderTargetResourceType::RenderBuffer )
				{
					auto * openglRenderBuffer = colorAttachmentBinding.uRenderBufferRef.renderBuffer->queryInterface<GLRenderBuffer>();
					framebufferObject->bindColorRenderbuffer( colorAttachmentIndex,
					                                          *( openglRenderBuffer->mGLRenderbufferObject ) );
				}
				else
				{
					auto * openglTexture = colorAttachmentBinding.uTextureRef.texture->queryInterface<GLTexture>();
					framebufferObject->bindColorTexture( colorAttachmentIndex,
					                                     *( openglTexture->mGLTextureObject ),
					                                     colorAttachmentBinding.uTextureRef.textureSubResource );
				}
			}
		}

		auto & depthStencilAttachmentBinding = rtResourceBinding.depthStencilAttachment;
		if( depthStencilAttachmentBinding )
		{
			if( depthStencilAttachmentBinding.attachmentResourceType == ERenderTargetResourceType::RenderBuffer )
			{
				auto * openglRenderBuffer = depthStencilAttachmentBinding.uRenderBufferRef.renderBuffer->queryInterface<GLRenderBuffer>();
				framebufferObject->bindDepthStencilRenderbuffer( *( openglRenderBuffer->mGLRenderbufferObject ) );
			}
			else
			{
				auto * openglTexture = depthStencilAttachmentBinding.uTextureRef.texture->queryInterface<GLTexture>();
				framebufferObject->bindDepthStencilTexture( *( openglTexture->mGLTextureObject ),
				                                            depthStencilAttachmentBinding.uTextureRef.textureSubResource );
			}
		}

		if( !framebufferObject->checkStatus() )
		{
			ts3DebugInterrupt();
			return nullptr;
		}

		auto openglRenderTargetSO = std::make_shared<GLRenderTargetStateObject>( pGPUDevice,
		                                                                         rtLayout,
		                                                                         rtResourceBinding,
		                                                                         std::move( framebufferObject ) );

		return openglRenderTargetSO;
	}

} /* namespace ts3 */
} /* namespace gpuapi */
