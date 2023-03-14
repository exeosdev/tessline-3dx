
#include "GL_renderTarget.h"
#include <ts3/gpuapi/resources/texture.h>
#include <ts3/gpuapiGL/GL_gpuDevice.h>
#include <ts3/gpuapiGL/objects/GL_framebufferObject.h>
#include <ts3/gpuapiGL/objects/GL_renderbufferObject.h>
#include <ts3/gpuapiGL/resources/GL_renderBuffer.h>
#include <ts3/gpuapiGL/resources/GL_texture.h>

namespace ts3::gpuapi
{

	GLRenderTargetBindingImmutableState::GLRenderTargetBindingImmutableState(
			GLGPUDevice & pGPUDevice,
			const RenderTargetLayout & pRenderTargetLayout,
			GLRenderTargetBindingFBOData pGLFBOData )
	: RenderTargetBindingImmutableState( pGPUDevice, pRenderTargetLayout )
	, mGLFBOData( std::move( pGLFBOData ) )
	{}

	GLRenderTargetBindingImmutableState::~GLRenderTargetBindingImmutableState() = default;

	GLRenderTargetBindingInfo GLRenderTargetBindingImmutableState::getGLRenderTargetBindingInfo() const
	{
		GLRenderTargetBindingInfo glcRTBindingInfo{};
		glcRTBindingInfo.renderFBO = mGLFBOData.renderFBO.get();
		glcRTBindingInfo.resolveFBO = mGLFBOData.resolveFBO.get();
		glcRTBindingInfo.rtLayout = &mRenderTargetLayout;
		return glcRTBindingInfo;
	}

	GpaHandle<GLRenderTargetBindingImmutableState> GLRenderTargetBindingImmutableState::createInstance(
			GLGPUDevice & pGPUDevice,
			const RenderTargetBindingDefinition & pBindingDefinition )
	{
		if( !smutil::validateRenderTargetBindingDefinition( pBindingDefinition ) )
		{
			return nullptr;
		}

		auto glcRenderTargetBindingDefinition = smutil::translateRenderTargetBindingDefinition( pBindingDefinition );

		auto immutableState = createGPUAPIObject<GLRenderTargetBindingImmutableState>(
				pGPUDevice,
				glcRenderTargetBindingDefinition.rtLayout,
				std::move( glcRenderTargetBindingDefinition.fboData ) );

		return immutableState;
	}


	GLRenderPassConfigurationImmutableState::GLRenderPassConfigurationImmutableState(
			GLGPUDevice & pGPUDevice,
			const RenderPassConfiguration & pRenderPassConfiguration )
	: RenderPassConfigurationImmutableState( pGPUDevice )
	, mRenderPassConfiguration( pRenderPassConfiguration )
	{}

	GLRenderPassConfigurationImmutableState::~GLRenderPassConfigurationImmutableState() = default;

	GpaHandle<GLRenderPassConfigurationImmutableState> GLRenderPassConfigurationImmutableState::createInstance(
			GLGPUDevice & pGPUDevice,
			const RenderPassConfiguration & pConfiguration )
	{
		auto immutableState = createGPUAPIObject<GLRenderPassConfigurationImmutableState>(
				pGPUDevice,
				pConfiguration );

		return immutableState;
	}


	namespace smutil
	{

		GLRenderTargetBindingInfo getGLRenderTargetBindingInfo(
				const GLRenderTargetBindingDefinition & pBindingDefinition )
		{
			GLRenderTargetBindingInfo glcRTBindingInfo{};
			glcRTBindingInfo.renderFBO = pBindingDefinition.fboData.renderFBO.get();
			glcRTBindingInfo.resolveFBO = pBindingDefinition.fboData.resolveFBO.get();
			glcRTBindingInfo.rtLayout = &( pBindingDefinition.rtLayout );
			return glcRTBindingInfo;
		}

		GLRenderTargetBindingDefinition translateRenderTargetBindingDefinition(
				const RenderTargetBindingDefinition & pBindingDefinition )
		{
			GLRenderTargetBindingDefinition glcRenderTargetBindingDefinition{};

			const auto renderTargetLayout = smutil::getRenderTargetLayoutForBindingDefinition( pBindingDefinition );

			glcRenderTargetBindingDefinition.fboData.renderFBO =
					smutil::createFramebufferObject( pBindingDefinition, pBindingDefinition.activeAttachmentsMask );

			glcRenderTargetBindingDefinition.fboData.resolveFBO =
					smutil::createFramebufferObject( pBindingDefinition, pBindingDefinition.attachmentsActionResolveMask );

			glcRenderTargetBindingDefinition.rtLayout = renderTargetLayout;

			return glcRenderTargetBindingDefinition;
		}

		GLFramebufferObjectHandle createFramebufferObject(
				const RenderTargetBindingDefinition & pBindingDefinition,
				Bitmask<ERTAttachmentFlags> pAttachmentMask )
		{
			if( pAttachmentMask.empty() )
			{
				return nullptr;
			}

			auto framebufferObject = GLFramebufferObject::create();
			if( !framebufferObject )
			{
				return nullptr;
			}

			for( uint32 caIndex = 0; cxdefs::isRTColorAttachmentIndexValid( caIndex ); ++caIndex )
			{
				const auto attachmentBit = cxdefs::makeRTAttachmentFlag( caIndex );
				if( pAttachmentMask.isSet( attachmentBit ) )
				{
					auto & colorAttachmentBinding = pBindingDefinition.colorAttachments[caIndex];
					auto & textureReference = colorAttachmentBinding.attachmentTexture->getTargetTextureRef();
					auto * openglTexture = textureReference.getRefTexture()->queryInterface<GLTexture>();

					framebufferObject->bindColorTexture(
							caIndex,
							*( openglTexture->mGLTextureObject ),
							textureReference.getRefSubResource() );
				}
			}

			if( pAttachmentMask.isSet( E_RT_ATTACHMENT_FLAG_DEPTH_STENCIL_BIT ) )
			{
				auto & depthStencilAttachmentBinding = pBindingDefinition.depthStencilAttachment;

				if( depthStencilAttachmentBinding.attachmentTexture->isDepthStencilRenderBuffer() )
				{
					auto * internalRenderBuffer = depthStencilAttachmentBinding.attachmentTexture->getInternalRenderBuffer();
					auto * openglRenderBuffer = internalRenderBuffer->queryInterface<GLInternalRenderBuffer>();

					framebufferObject->bindDepthStencilRenderbuffer( *( openglRenderBuffer->mGLRenderbufferObject ) );
				}
				else
				{
					auto & textureReference = depthStencilAttachmentBinding.attachmentTexture->getTargetTextureRef();
					auto * openglTexture = textureReference.getRefTexture()->queryInterface<GLTexture>();

					framebufferObject->bindDepthStencilTexture(
							*( openglTexture->mGLTextureObject ),
							textureReference.getRefSubResource() );
				}
			}

			if( !framebufferObject->checkStatus() )
			{
				ts3DebugInterrupt();
				return nullptr;
			}

			return framebufferObject;
		}

		void clearRenderPassFramebuffer( const RenderPassConfiguration & pRenderPassConfiguration )
		{
			if( pRenderPassConfiguration.attachmentsActionClearMask != 0 )
			{
				auto colorAttachmentsClearMask = pRenderPassConfiguration.attachmentsActionClearMask & E_RT_ATTACHMENT_MASK_COLOR_ALL;
				for( uint32 caIndex = 0; cxdefs::isRTColorAttachmentIndexValid( caIndex ); ++caIndex )
				{
					const auto attachmentBit = cxdefs::makeRTAttachmentFlag( caIndex );
					if( colorAttachmentsClearMask.isSet( attachmentBit ) )
					{
						const auto & attachmentConfig = pRenderPassConfiguration.colorAttachments[caIndex];
						const auto & clearColor = attachmentConfig.clearConfig.colorValue;

						if( attachmentConfig.clearMask.isSet( E_RENDER_TARGET_BUFFER_FLAG_COLOR_BIT ) )
						{
							GLfloat clearArray[4] = { clearColor.fpRed, clearColor.fpGreen, clearColor.fpBlue, clearColor.fpAlpha };
							glClearBufferfv( GL_COLOR, caIndex, clearArray );
							ts3OpenGLHandleLastError();
						}

						colorAttachmentsClearMask.unset( attachmentBit );
					}

					if( colorAttachmentsClearMask.empty() )
					{
						break;
					}
				}

				if( pRenderPassConfiguration.attachmentsActionClearMask.isSet( E_RT_ATTACHMENT_FLAG_DEPTH_STENCIL_BIT ) )
				{
					const auto & attachmentConfig = pRenderPassConfiguration.depthStencilAttachment;
					const auto & clearData = attachmentConfig.clearConfig;

					if( attachmentConfig.clearMask.isSet( E_RENDER_TARGET_BUFFER_FLAG_DEPTH_BIT ) )
					{
						const GLfloat depthValue = clearData.depthValue;
						glClearBufferfv( GL_DEPTH, 0, &depthValue );
						ts3OpenGLHandleLastError();
					}

					if( attachmentConfig.clearMask.isSet( E_RENDER_TARGET_BUFFER_FLAG_STENCIL_BIT ) )
					{
						const GLint stencilValue = clearData.stencilValue;
						glClearBufferiv( GL_STENCIL, 0, &stencilValue );
						ts3OpenGLHandleLastError();
					}
				}
			}
		}

		void resolveRenderPassFramebuffer(
				const GLRenderTargetBindingInfo & pRTBindingInfo,
				const RenderPassConfiguration & pRenderPassConfiguration )
		{
			if( pRenderPassConfiguration.attachmentsActionResolveMask != 0 )
			{
				ts3DebugAssert( pRTBindingInfo.resolveFBO );

				GLint drawFramebufferHandle = -1;
				glGetIntegerv( GL_DRAW_FRAMEBUFFER_BINDING, &drawFramebufferHandle );

				GLint readFramebufferHandle = -1;
				glGetIntegerv( GL_READ_FRAMEBUFFER_BINDING, &readFramebufferHandle );

				const auto & fboImageSize = pRTBindingInfo.rtLayout->sharedImageSize;

				glBindFramebuffer( GL_READ_FRAMEBUFFER, drawFramebufferHandle );
				ts3OpenGLHandleLastError();

				glBindFramebuffer( GL_DRAW_FRAMEBUFFER, pRTBindingInfo.renderFBO->mGLHandle );
				ts3OpenGLHandleLastError();

				auto colorAttachmentsResolveMask = pRenderPassConfiguration.attachmentsActionResolveMask;
				for( uint32 caIndex = 0; cxdefs::isRTColorAttachmentIndexValid( caIndex ); ++caIndex )
				{
					const auto attachmentBit = cxdefs::makeRTAttachmentFlag( caIndex );
					if( colorAttachmentsResolveMask.isSet( attachmentBit ) )
					{
						glReadBuffer( GL_COLOR_ATTACHMENT0 + caIndex );
						ts3OpenGLHandleLastError();

						glDrawBuffer( GL_COLOR_ATTACHMENT0 + caIndex );
						ts3OpenGLHandleLastError();

						glBlitFramebuffer(
								0,
								0,
								fboImageSize.width,
								fboImageSize.height,
								0,
								0,
								fboImageSize.width,
								fboImageSize.height,
								GL_COLOR_BUFFER_BIT,
								GL_LINEAR );
						ts3OpenGLHandleLastError();

						colorAttachmentsResolveMask.unset( attachmentBit );

						if( colorAttachmentsResolveMask.empty() )
						{
							break;
						}
					}

					glBindFramebuffer( GL_READ_FRAMEBUFFER, readFramebufferHandle );
					ts3OpenGLHandleLastError();

					glBindFramebuffer( GL_DRAW_FRAMEBUFFER, drawFramebufferHandle );
					ts3OpenGLHandleLastError();
				}
			}
		}

	}

} // namespace ts3::gpuapi
