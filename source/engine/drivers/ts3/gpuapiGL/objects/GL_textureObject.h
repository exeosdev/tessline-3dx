
#pragma once

#ifndef __TS3DRIVER_GPUAPI_GLCOMMON_TEXTURE_OBJECT_H__
#define __TS3DRIVER_GPUAPI_GLCOMMON_TEXTURE_OBJECT_H__

#include "../GL_prerequisites.h"
#include <ts3/gpuapi/resources/textureCommon.h>

namespace ts3::gpuapi
{

	ts3GLDeclareOpenGLObjectHandle( GLTextureObject );

	struct GLTextureInitDataDesc
	{
		TextureSubTextureInitDataDesc * subTextureInitDataPtr = nullptr;
		GLenum openglPixelDataLayout = 0;
		GLenum openglPixelDataType = 0;

		explicit operator bool() const
		{
			return subTextureInitDataPtr != nullptr;
		}
	};

	struct GLTextureInputDataDesc : public ResourceInputDataDesc
	{
		GLenum openglPixelDataLayout = 0;
		GLenum openglPixelDataType = 0;
	};

	struct GLTextureCreateInfo
	{
		TextureDimensions dimensions;
		GLenum bindTarget = 0;
		GLenum internalFormat = 0;
		GLuint msaaLevel = 0;
		GLTextureInitDataDesc openglInitDataDesc;
	};

	struct GLTextureSubDataUploadDesc
	{
		GLenum openglDimensionClass;
		TextureSubRegion textureSubRegion;
		GLTextureInputDataDesc openglInputDataDesc;
	};

	class GLTextureObject : public GLObject
	{
	public:
		TextureDimensions const mDimensions;
		GLenum const mGLTextureBindTarget = CX_GL_BIND_TARGET_UNKNOWN;
		GLenum const mGLInternalFormat;
		GLuint const mMultisampleLevel;

		GLTextureObject( GLuint pHandle, const GLTextureCreateInfo & pGLCreateInfo );
		virtual ~GLTextureObject();

		virtual bool release();
		virtual bool validateHandle() const;

		void updateCopy2D( GLTextureObject & pSource, const TextureSubDataCopyDesc & pCopyDesc, GLenum pActiveBindTarget = 0 );
		void updateCopy2DArray( GLTextureObject & pSource, const TextureSubDataCopyDesc & pCopyDesc, GLenum pActiveBindTarget = 0 );
		void updateCopy3D( GLTextureObject & pSource, const TextureSubDataCopyDesc & pCopyDesc, GLenum pActiveBindTarget = 0 );
		void updateCopyCubeMap( GLTextureObject & pSource, const TextureSubDataCopyDesc & pCopyDesc, GLenum pActiveBindTarget = 0 );

		void updateUpload2D( const GLTextureSubDataUploadDesc & pGLUploadDesc, GLenum pActiveBindTarget = 0 );
		void updateUpload2DArray( const GLTextureSubDataUploadDesc & pGLUploadDesc, GLenum pActiveBindTarget = 0 );
		void updateUpload3D( const GLTextureSubDataUploadDesc & pGLUploadDesc, GLenum pActiveBindTarget = 0 );
		void updateUploadCubeMap( const GLTextureSubDataUploadDesc & pGLUploadDesc, GLenum pActiveBindTarget = 0 );

		GLenum queryInternalFormat( GLenum pActiveBindTarget = 0 ) const;
		memory_size_t querySize( GLenum pActiveBindTarget = 0 ) const;

		static GLTextureObjectHandle create( const GLTextureCreateInfo & pGLCreateInfo );

	private:
		bool initialize( const GLTextureCreateInfo & pGLCreateInfo );
		bool initialize2D( const GLTextureCreateInfo & pGLCreateInfo );
		bool initialize2DArray( const GLTextureCreateInfo & pGLCreateInfo );
		bool initialize2DMS( const GLTextureCreateInfo & pGLCreateInfo );
		bool initialize3D( const GLTextureCreateInfo & pGLCreateInfo );
		bool initializeCubeMap( const GLTextureCreateInfo & pGLCreateInfo );

		GLenum checkActiveBindTarget( GLenum pBindTarget ) const;

		static GLuint computeInputPixelDataAlignment( GLenum pPixelDataLayout, GLenum pPixelDataType );
	};

} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_GLCOMMON_TEXTURE_OBJECT_H__
