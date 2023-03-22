
#pragma once

#ifndef __TS3DRIVER_GPUAPI_GLCOMMON_FRAMEBUFFER_OBJECT_H__
#define __TS3DRIVER_GPUAPI_GLCOMMON_FRAMEBUFFER_OBJECT_H__

#include "../GL_prerequisites.h"
#include <ts3/gpuapi/resources/textureCommon.h>

namespace ts3::gpuapi
{

	class GLRenderbufferObject;
	class GLTextureObject;

	ts3GLDeclareOpenGLObjectHandle( GLFramebufferObject );

	class GLFramebufferObject : public GLObject
	{
	public:
		GLFramebufferObject( GLuint pHandle );
		virtual ~GLFramebufferObject();

		virtual bool release();
		virtual bool validateHandle() const;

		void bindColorRenderbuffer(
				uint32 pColorAttachmentIndex,
				GLRenderbufferObject & pGLRenderbuffer,
				GLenum pActiveBindTarget = 0 );

		void bindDepthStencilRenderbuffer(
				GLRenderbufferObject & pGLRenderbuffer,
				Bitmask<uint32> pBufferMask,
				GLenum pActiveBindTarget = 0 );

		void bindColorTexture(
				uint32 pColorAttachmentIndex,
				GLTextureObject & pGLTexture,
				const TextureSubResource & pSubResource,
				GLenum pActiveBindTarget = 0 );

		void bindDepthStencilTexture(
				GLTextureObject & pGLTexture,
				const TextureSubResource & pSubResource,
				Bitmask<uint32> pBufferMask,
				GLenum pActiveBindTarget = 0 );

		bool checkStatus( GLenum pActiveBindTarget = 0 ) const;

		static GLFramebufferObjectHandle create();

		static GLFramebufferObjectHandle createForDefaultFramebuffer();

	private:
		GLenum checkActiveBindTarget( GLenum pBindTarget ) const;
	};

} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_GLCOMMON_FRAMEBUFFER_OBJECT_H__
