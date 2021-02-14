
#include "GL_framebufferObject.h"
#include "GL_renderbufferObject.h"
#include "GL_textureObject.h"

namespace ts3::gpuapi
{

	GLFramebufferObject::GLFramebufferObject( GLuint pHandle )
	: GLObject( GLObjectBaseType::Framebuffer, pHandle )
	{}

	GLFramebufferObject::~GLFramebufferObject() = default;

	GLFramebufferObjectHandle GLFramebufferObject::create()
	{
		GLuint framebufferHandle = 0;

		glGenFramebuffers( 1, &framebufferHandle );
		ts3GLHandleLastError();

		glBindFramebuffer( GL_FRAMEBUFFER, framebufferHandle );
		ts3GLHandleLastError();

		GLFramebufferObjectHandle openglFramebufferObject{ new GLFramebufferObject( framebufferHandle ) };
		if( !openglFramebufferObject->initialize() )
		{
			return nullptr;
		}

		return openglFramebufferObject;
	}

	bool GLFramebufferObject::release()
	{
		glDeleteFramebuffers( 1, &mGLHandle );
		ts3GLHandleLastError();

		return true;
	}

	bool GLFramebufferObject::validateHandle() const
	{
		auto isBuffer = glIsFramebuffer( mGLHandle );
		ts3GLHandleLastError();

		return isBuffer != GL_FALSE;
	}

	void GLFramebufferObject::bindColorRenderbuffer( uint32 pColorAttachmentIndex,
	                                               GLRenderbufferObject & pGLRenderbuffer,
	                                               GLenum pActiveBindTarget )
	{
		auto framebufferBindTarget = checkActiveBindTarget( pActiveBindTarget );

		glFramebufferRenderbuffer( framebufferBindTarget,
		                           GL_COLOR_ATTACHMENT0 + pColorAttachmentIndex,
		                           GL_RENDERBUFFER,
		                           pGLRenderbuffer.mGLHandle );
		ts3GLHandleLastError();
	}

	void GLFramebufferObject::bindDepthStencilRenderbuffer( GLRenderbufferObject & pGLRenderbuffer,
	                                                        GLenum pActiveBindTarget )
	{
		auto framebufferBindTarget = checkActiveBindTarget( pActiveBindTarget );

		glFramebufferRenderbuffer( framebufferBindTarget,
		                           GL_DEPTH_STENCIL_ATTACHMENT,
		                           GL_RENDERBUFFER,
		                           pGLRenderbuffer.mGLHandle );
		ts3GLHandleLastError();
	}

	void GLFramebufferObject::bindColorTexture( uint32 pColorAttachmentIndex,
	                                            GLTextureObject & pGLTexture,
	                                            const TextureSubResource & pSubResource,
	                                            GLenum pActiveBindTarget )
	{
		auto framebufferBindTarget = checkActiveBindTarget( pActiveBindTarget );

		switch( pGLTexture.mGLTextureBindTarget )
		{
			case GL_TEXTURE_2D:
			{
				glFramebufferTexture2D( framebufferBindTarget,
				                        GL_COLOR_ATTACHMENT0 + pColorAttachmentIndex,
				                        GL_TEXTURE_2D,
				                        pGLTexture.mGLHandle,
				                        pSubResource.u2D.mipLevel );
				ts3GLHandleLastError();
				break;
			}
			case GL_TEXTURE_2D_ARRAY:
			{
				glFramebufferTextureLayer( framebufferBindTarget,
				                           GL_COLOR_ATTACHMENT0 + pColorAttachmentIndex,
				                           pGLTexture.mGLHandle,
				                           pSubResource.u2DArray.mipLevel,
				                           pSubResource.u2DArray.arrayIndex );
				ts3GLHandleLastError();
				break;
			}
			case GL_TEXTURE_2D_MULTISAMPLE:
			{
				glFramebufferTexture2D( framebufferBindTarget,
				                        GL_COLOR_ATTACHMENT0 + pColorAttachmentIndex,
				                        GL_TEXTURE_2D_MULTISAMPLE,
				                        pGLTexture.mGLHandle,
				                        pSubResource.u2D.mipLevel );
				ts3GLHandleLastError();
				break;
			}
			case GL_TEXTURE_3D:
			{
				glFramebufferTextureLayer( framebufferBindTarget,
				                           GL_COLOR_ATTACHMENT0 + pColorAttachmentIndex,
				                           pGLTexture.mGLHandle,
				                           pSubResource.u3D.mipLevel,
				                           pSubResource.u3D.depthLayerIndex );
				ts3GLHandleLastError();
				break;
			}
			case GL_TEXTURE_CUBE_MAP:
			{
				glFramebufferTexture2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + pSubResource.uCubeMap.uFaceIndex,
				                        GL_COLOR_ATTACHMENT0 + pColorAttachmentIndex,
				                        GL_TEXTURE_2D,
				                        pGLTexture.mGLHandle,
				                        pSubResource.uCubeMap.mipLevel );
				ts3GLHandleLastError();
				break;
			}
		}
	}

	void GLFramebufferObject::bindDepthStencilTexture( GLTextureObject & pGLTexture,
	                                                   const TextureSubResource & pSubResource,
	                                                   GLenum pActiveBindTarget )
	{
		auto framebufferBindTarget = checkActiveBindTarget( pActiveBindTarget );

		switch( pGLTexture.mGLTextureBindTarget )
		{
			case GL_TEXTURE_2D:
			{
				glFramebufferTexture2D( framebufferBindTarget,
				                        GL_DEPTH_STENCIL_ATTACHMENT,
				                        GL_TEXTURE_2D,
				                        pGLTexture.mGLHandle,
				                        pSubResource.u2D.mipLevel );
				ts3GLHandleLastError();
				break;
			}
			case GL_TEXTURE_2D_ARRAY:
			{
				glFramebufferTextureLayer( framebufferBindTarget,
				                           GL_DEPTH_STENCIL_ATTACHMENT,
				                           pGLTexture.mGLHandle,
				                           pSubResource.u2DArray.mipLevel,
				                           pSubResource.u2DArray.arrayIndex );
				ts3GLHandleLastError();
				break;
			}
			case GL_TEXTURE_2D_MULTISAMPLE:
			{
				glFramebufferTexture2D( framebufferBindTarget,
				                        GL_DEPTH_STENCIL_ATTACHMENT,
				                        GL_TEXTURE_2D_MULTISAMPLE,
				                        pGLTexture.mGLHandle,
				                        pSubResource.u2D.mipLevel );
				ts3GLHandleLastError();
				break;
			}
			case GL_TEXTURE_3D:
			{
				glFramebufferTextureLayer( framebufferBindTarget,
				                           GL_DEPTH_STENCIL_ATTACHMENT,
				                           pGLTexture.mGLHandle,
				                           pSubResource.u3D.mipLevel,
				                           pSubResource.u3D.depthLayerIndex );
				ts3GLHandleLastError();
				break;
			}
			case GL_TEXTURE_CUBE_MAP:
			{
				glFramebufferTexture2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + pSubResource.uCubeMap.uFaceIndex,
				                        GL_DEPTH_STENCIL_ATTACHMENT,
				                        GL_TEXTURE_2D,
				                        pGLTexture.mGLHandle,
				                        pSubResource.uCubeMap.mipLevel );
				ts3GLHandleLastError();
				break;
			}
		}
	}

	bool GLFramebufferObject::checkStatus( GLenum pActiveBindTarget ) const
	{
		auto framebufferBindTarget = checkActiveBindTarget( pActiveBindTarget );

		GLenum framebufferStatus = glCheckFramebufferStatus( framebufferBindTarget );
		ts3GLHandleLastError();

		if( framebufferStatus != GL_FRAMEBUFFER_COMPLETE )
		{
			switch( framebufferStatus )
			{
				case GL_FRAMEBUFFER_UNDEFINED:
					ts3DebugInterrupt();
					break;

				case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
					ts3DebugInterrupt();
					break;

				case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
					ts3DebugInterrupt();
					break;

				case GL_FRAMEBUFFER_UNSUPPORTED:
					ts3DebugInterrupt();
					break;

				case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
					ts3DebugInterrupt();
					break;
			}

			return false;
		}

		return true;
	}

	bool GLFramebufferObject::initialize()
	{
		return true;
	}

	GLenum GLFramebufferObject::checkActiveBindTarget( GLenum pBindTarget ) const
	{
		if( pBindTarget == 0 )
		{
			pBindTarget = GL_FRAMEBUFFER;

			glBindFramebuffer( GL_FRAMEBUFFER, mGLHandle );
			ts3GLHandleLastError();
		}

		return pBindTarget;
	}

}
