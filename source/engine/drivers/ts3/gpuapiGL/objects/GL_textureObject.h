
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
		Bitmask<ETextureInitFlags> textureInitFlags = 0;
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

		uint32 getInitMipLevelsNum() const
		{
			return openglInitDataDesc.textureInitFlags.isSet( E_TEXTURE_INIT_FLAG_GENERATE_MIPMAPS_BIT ) ? 1u : dimensions.mipLevelsNum;
		}
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

		bool isAutoMipGenerationEnabled() const;
		GLenum queryInternalFormat( GLenum pActiveBindTarget = 0 ) const;
		memory_size_t querySize( GLenum pActiveBindTarget = 0 ) const;

		virtual bool release();
		virtual bool validateHandle() const;

		void updateCopy2D( GLTextureObject & pSrcTexture, const TextureSubDataCopyDesc & pCopyDesc, GLenum pActiveBindTarget = 0 );
		void updateCopy2DArray( GLTextureObject & pSrcTexture, const TextureSubDataCopyDesc & pCopyDesc, GLenum pActiveBindTarget = 0 );
		void updateCopy3D( GLTextureObject & pSrcTexture, const TextureSubDataCopyDesc & pCopyDesc, GLenum pActiveBindTarget = 0 );
		void updateCopyCubeMap( GLTextureObject & pSrcTexture, const TextureSubDataCopyDesc & pCopyDesc, GLenum pActiveBindTarget = 0 );

		void updateUpload2D( const GLTextureSubDataUploadDesc & pGLUploadDesc, GLenum pActiveBindTarget = 0 );
		void updateUpload2DArray( const GLTextureSubDataUploadDesc & pGLUploadDesc, GLenum pActiveBindTarget = 0 );
		void updateUpload3D( const GLTextureSubDataUploadDesc & pGLUploadDesc, GLenum pActiveBindTarget = 0 );
		void updateUploadCubeMap( const GLTextureSubDataUploadDesc & pGLUploadDesc, GLenum pActiveBindTarget = 0 );

		static GLTextureObjectHandle createCore( const GLTextureCreateInfo & pGLCreateInfo );
		static GLTextureObjectHandle createCompat( const GLTextureCreateInfo & pGLCreateInfo );

	private:
		void setAutoMipGeneration( bool pEnable );

		bool initializeCore( const GLTextureCreateInfo & pGLCreateInfo );
		bool initializeCore2D( const GLTextureCreateInfo & pGLCreateInfo );
		bool initializeCore2DArray( const GLTextureCreateInfo & pGLCreateInfo );
		bool initializeCore2DMS( const GLTextureCreateInfo & pGLCreateInfo );
		bool initializeCore3D( const GLTextureCreateInfo & pGLCreateInfo );
		bool initializeCoreCubeMap( const GLTextureCreateInfo & pGLCreateInfo );

		bool initializeCompat( const GLTextureCreateInfo & pGLCreateInfo );
		bool initializeCompat2D( const GLTextureCreateInfo & pGLCreateInfo );
		bool initializeCompat2DArray( const GLTextureCreateInfo & pGLCreateInfo );
		bool initializeCompat2DMS( const GLTextureCreateInfo & pGLCreateInfo );
		bool initializeCompat3D( const GLTextureCreateInfo & pGLCreateInfo );
		bool initializeCompatCubeMap( const GLTextureCreateInfo & pGLCreateInfo );

		void initializeData2D( const GLTextureCreateInfo & pGLCreateInfo, bool pAllocateMipMapStorage = false );
		void initializeData2DArray( const GLTextureCreateInfo & pGLCreateInfo, bool pAllocateMipMapStorage = false );
		void initializeData3D( const GLTextureCreateInfo & pGLCreateInfo, bool pAllocateMipMapStorage = false );
		void initializeDataCubeMap( const GLTextureCreateInfo & pGLCreateInfo, bool pAllocateMipMapStorage = false );

		GLenum checkActiveBindTarget( GLenum pBindTarget ) const;

		static GLuint computeInputPixelDataAlignment( GLenum pPixelDataLayout, GLenum pPixelDataType );

	private:
		bool _autoMipGenerationStatus = false;
	};

} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_GLCOMMON_TEXTURE_OBJECT_H__
