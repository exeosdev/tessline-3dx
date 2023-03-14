
#include "GL_textureObject.h"
#include <ts3/gpuapiGL/GL_coreAPIProxy.h>

namespace ts3::gpuapi
{

	GLTextureObject::GLTextureObject( GLuint pHandle, const GLTextureCreateInfo & pGLCreateInfo )
	: GLObject( GLObjectBaseType::Texture, pHandle )
	, mDimensions( pGLCreateInfo.dimensions )
	, mGLTextureBindTarget( pGLCreateInfo.bindTarget )
	, mGLInternalFormat( pGLCreateInfo.internalFormat )
	, mMultisampleLevel( pGLCreateInfo.msaaLevel )
	{}

	GLTextureObject::~GLTextureObject() = default;

	GLTextureObjectHandle GLTextureObject::create( const GLTextureCreateInfo & pGLCreateInfo )
	{
		GLuint textureHandle = 0;

		glGenTextures( 1, &textureHandle );
		ts3OpenGLHandleLastError();

		glBindTexture( pGLCreateInfo.bindTarget, textureHandle );
		ts3OpenGLHandleLastError();

		GLTextureObjectHandle openglTextureObject{ new GLTextureObject( textureHandle, pGLCreateInfo ) };
		if( !openglTextureObject->initialize( pGLCreateInfo ) )
		{
			return nullptr;
		}

		return openglTextureObject;
	}

	bool GLTextureObject::release()
	{
		glDeleteTextures( 1, &mGLHandle );
		ts3OpenGLHandleLastError();

		return true;
	}

	bool GLTextureObject::validateHandle() const
	{
		auto isBuffer = glIsTexture( mGLHandle );
		ts3OpenGLHandleLastError();

		return isBuffer != GL_FALSE;
	}

	void GLTextureObject::updateCopy2D( GLTextureObject & pSrcTexture, const TextureSubDataCopyDesc & pCopyDesc, GLenum pActiveBindTarget )
	{
	}

	void GLTextureObject::updateCopy2DArray( GLTextureObject & pSrcTexture, const TextureSubDataCopyDesc & pCopyDesc, GLenum pActiveBindTarget )
	{
	}

	void GLTextureObject::updateCopy3D( GLTextureObject & pSrcTexture, const TextureSubDataCopyDesc & pCopyDesc, GLenum pActiveBindTarget )
	{
	}

	void GLTextureObject::updateCopyCubeMap( GLTextureObject & pSrcTexture, const TextureSubDataCopyDesc & pCopyDesc, GLenum pActiveBindTarget )
	{
	}

	void GLTextureObject::updateUpload2D( const GLTextureSubDataUploadDesc & pGLUploadDesc, GLenum pActiveBindTarget )
	{
		ts3DebugAssert( pGLUploadDesc.openglDimensionClass == GL_TEXTURE_2D );
		ts3DebugAssert( ( pActiveBindTarget == 0 ) || ( pActiveBindTarget == GL_TEXTURE_2D ) );

		auto textureBindTarget = checkActiveBindTarget( pActiveBindTarget );

		glTexSubImage2D( textureBindTarget,
		                 pGLUploadDesc.textureSubRegion.uSubReg2D.offset.mipLevel,
		                 pGLUploadDesc.textureSubRegion.uSubReg2D.offset.x,
		                 pGLUploadDesc.textureSubRegion.uSubReg2D.offset.y,
		                 pGLUploadDesc.textureSubRegion.uSubReg2D.size.width,
		                 pGLUploadDesc.textureSubRegion.uSubReg2D.size.height,
		                 pGLUploadDesc.openglInputDataDesc.openglPixelDataLayout,
		                 pGLUploadDesc.openglInputDataDesc.openglPixelDataType,
		                 pGLUploadDesc.openglInputDataDesc.pointer );
		ts3OpenGLHandleLastError();
	}

	void GLTextureObject::updateUpload2DArray( const GLTextureSubDataUploadDesc & pGLUploadDesc, GLenum pActiveBindTarget )
	{
		ts3DebugAssert( pGLUploadDesc.openglDimensionClass == GL_TEXTURE_2D_ARRAY );
		ts3DebugAssert( ( pActiveBindTarget == 0 ) || ( pActiveBindTarget == GL_TEXTURE_2D_ARRAY ) );

		auto textureBindTarget = checkActiveBindTarget( pActiveBindTarget );

		glTexSubImage3D( textureBindTarget,
		                 pGLUploadDesc.textureSubRegion.uSubReg2DArray.offset.mipLevel,
		                 pGLUploadDesc.textureSubRegion.uSubReg2DArray.offset.x,
		                 pGLUploadDesc.textureSubRegion.uSubReg2DArray.offset.y,
		                 pGLUploadDesc.textureSubRegion.uSubReg2DArray.offset.arrayIndex,
		                 pGLUploadDesc.textureSubRegion.uSubReg2DArray.size.width,
		                 pGLUploadDesc.textureSubRegion.uSubReg2DArray.size.height,
		                 1,
		                 pGLUploadDesc.openglInputDataDesc.openglPixelDataLayout,
		                 pGLUploadDesc.openglInputDataDesc.openglPixelDataType,
		                 pGLUploadDesc.openglInputDataDesc.pointer );
		ts3OpenGLHandleLastError();
	}

	void GLTextureObject::updateUpload3D( const GLTextureSubDataUploadDesc & pGLUploadDesc, GLenum pActiveBindTarget )
	{
		ts3DebugAssert( pGLUploadDesc.openglDimensionClass == GL_TEXTURE_3D );
		ts3DebugAssert( ( pActiveBindTarget == 0 ) || ( pActiveBindTarget == GL_TEXTURE_3D ) );

		auto textureBindTarget = checkActiveBindTarget( pActiveBindTarget );

		glTexSubImage3D( textureBindTarget,
		                 pGLUploadDesc.textureSubRegion.uSubReg3D.offset.mipLevel,
		                 pGLUploadDesc.textureSubRegion.uSubReg3D.offset.x,
		                 pGLUploadDesc.textureSubRegion.uSubReg3D.offset.y,
		                 pGLUploadDesc.textureSubRegion.uSubReg3D.offset.z,
		                 pGLUploadDesc.textureSubRegion.uSubReg3D.size.width,
		                 pGLUploadDesc.textureSubRegion.uSubReg3D.size.height,
		                 pGLUploadDesc.textureSubRegion.uSubReg3D.size.depth,
		                 pGLUploadDesc.openglInputDataDesc.openglPixelDataLayout,
		                 pGLUploadDesc.openglInputDataDesc.openglPixelDataType,
		                 pGLUploadDesc.openglInputDataDesc.pointer );
		ts3OpenGLHandleLastError();
	}

	void GLTextureObject::updateUploadCubeMap( const GLTextureSubDataUploadDesc & pGLUploadDesc, GLenum pActiveBindTarget )
	{
		ts3DebugAssert( pGLUploadDesc.openglDimensionClass == GL_TEXTURE_CUBE_MAP );
		ts3DebugAssert( ( pActiveBindTarget == 0 ) || ( pActiveBindTarget == GL_TEXTURE_CUBE_MAP ) );

		checkActiveBindTarget( pActiveBindTarget );

		glTexSubImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + pGLUploadDesc.textureSubRegion.uSubRegCubeMap.offset.faceIndex,
		                 pGLUploadDesc.textureSubRegion.uSubRegCubeMap.offset.mipLevel,
		                 pGLUploadDesc.textureSubRegion.uSubRegCubeMap.offset.x,
		                 pGLUploadDesc.textureSubRegion.uSubRegCubeMap.offset.y,
		                 pGLUploadDesc.textureSubRegion.uSubRegCubeMap.size.width,
		                 pGLUploadDesc.textureSubRegion.uSubRegCubeMap.size.height,
		                 pGLUploadDesc.openglInputDataDesc.openglPixelDataLayout,
		                 pGLUploadDesc.openglInputDataDesc.openglPixelDataType,
		                 pGLUploadDesc.openglInputDataDesc.pointer );
		ts3OpenGLHandleLastError();
	}

	GLenum GLTextureObject::queryInternalFormat( GLenum pActiveBindTarget ) const
	{
		auto textureBindTarget = checkActiveBindTarget( pActiveBindTarget );

		GLint textureInternalFormat = 0;
		glGetTexLevelParameteriv( textureBindTarget, 0, GL_TEXTURE_INTERNAL_FORMAT, &textureInternalFormat );
		ts3OpenGLHandleLastError();

		return textureInternalFormat;
	}

	memory_size_t GLTextureObject::querySize( GLenum pActiveBindTarget ) const
	{
		auto textureBindTarget = checkActiveBindTarget( pActiveBindTarget );

		memory_size_t textureImageSize = 0;
		GLint textureCompressedFlag = 0;

	#if( TS3GX_GL_FEATURE_SUPPORT_TEXTURE_FORMAT_COMPRESSED_BCX )
		glGetTexLevelParameteriv( textureBindTarget, 0, GL_TEXTURE_COMPRESSED, &textureCompressedFlag );
		ts3OpenGLHandleLastError();
	#endif

		if( textureCompressedFlag )
		{
		#if( TS3GX_GL_FEATURE_SUPPORT_TEXTURE_FORMAT_COMPRESSED_BCX )
			GLint textureCompressedImageSize = 0;
			glGetTexLevelParameteriv( textureBindTarget, 0, GL_TEXTURE_COMPRESSED_IMAGE_SIZE, &textureCompressedImageSize );
			ts3OpenGLHandleLastError();

			textureImageSize = textureCompressedImageSize;
		#endif
		}
		else
		{
			GLint textureWidth = 0;
			glGetTexLevelParameteriv( textureBindTarget, 0, GL_TEXTURE_WIDTH, &textureWidth );
			ts3OpenGLHandleLastError();

			GLint textureHeight = 0;
			glGetTexLevelParameteriv( textureBindTarget, 0, GL_TEXTURE_HEIGHT, &textureHeight );
			ts3OpenGLHandleLastError();

			GLint textureDepth = 0;
			glGetTexLevelParameteriv( textureBindTarget, 0, GL_TEXTURE_DEPTH, &textureDepth );
			ts3OpenGLHandleLastError();

			GLint textureInternalFormat = 0;
			glGetTexLevelParameteriv( textureBindTarget, 0, GL_TEXTURE_INTERNAL_FORMAT, &textureInternalFormat );
			ts3OpenGLHandleLastError();

			auto internalFormatBPP = GLCoreAPIProxy::queryGLTextureInternalFormatBPP( textureInternalFormat );
			auto texturePixelCount = textureWidth * textureHeight * textureDepth;

			textureImageSize = texturePixelCount * internalFormatBPP / 8;
		}

		return textureImageSize;
	}

	bool GLTextureObject::initialize( const GLTextureCreateInfo & pGLCreateInfo )
	{
		switch( pGLCreateInfo.bindTarget )
		{
			case GL_TEXTURE_2D:
				return initialize2D( pGLCreateInfo );
			case GL_TEXTURE_2D_ARRAY:
				return initialize2DArray( pGLCreateInfo );
			case GL_TEXTURE_2D_MULTISAMPLE:
				return initialize2DMS( pGLCreateInfo );
			case GL_TEXTURE_3D:
				return initialize3D( pGLCreateInfo );
			case GL_TEXTURE_CUBE_MAP:
				return initializeCubeMap( pGLCreateInfo );
		}

		ts3DebugInterrupt();

		return false;
	}

	bool GLTextureObject::initialize2D( const GLTextureCreateInfo & pGLCreateInfo )
	{
		glTexStorage2D( pGLCreateInfo.bindTarget,
		                pGLCreateInfo.dimensions.mipLevelsNum,
		                pGLCreateInfo.internalFormat,
		                pGLCreateInfo.dimensions.width,
		                pGLCreateInfo.dimensions.height );
		ts3OpenGLHandleLastError();

		if( pGLCreateInfo.openglInitDataDesc )
		{
			const auto & textureInitData = pGLCreateInfo.openglInitDataDesc.subTextureInitDataPtr[0];

			GLTextureSubDataUploadDesc subDataUploadDesc;
			subDataUploadDesc.openglDimensionClass = GL_TEXTURE_2D;
			subDataUploadDesc.textureSubRegion.uSubReg2D.offset.x = 0;
			subDataUploadDesc.textureSubRegion.uSubReg2D.offset.y = 0;
			subDataUploadDesc.openglInputDataDesc.openglPixelDataLayout = pGLCreateInfo.openglInitDataDesc.openglPixelDataLayout;
			subDataUploadDesc.openglInputDataDesc.openglPixelDataType = pGLCreateInfo.openglInitDataDesc.openglPixelDataType;

			for( uint32 mipLevelIndex = 0; mipLevelIndex < pGLCreateInfo.dimensions.mipLevelsNum; ++mipLevelIndex )
			{
				const auto & mipLevelInitData = textureInitData.mipLevelInitDataArray[mipLevelIndex];

				subDataUploadDesc.textureSubRegion.uSubReg2D.offset.mipLevel = mipLevelInitData.mipLevelIndex;
				subDataUploadDesc.textureSubRegion.uSubReg2D.size.width = mipLevelInitData.mipWidth;
				subDataUploadDesc.textureSubRegion.uSubReg2D.size.height = mipLevelInitData.mipHeight;
				subDataUploadDesc.openglInputDataDesc.pointer = mipLevelInitData.pointer;
				subDataUploadDesc.openglInputDataDesc.size = mipLevelInitData.size;

				updateUpload2D( subDataUploadDesc, pGLCreateInfo.bindTarget );
			}
		}

		return true;
	}

	bool GLTextureObject::initialize2DArray( const GLTextureCreateInfo & pGLCreateInfo )
	{
		glTexStorage3D( pGLCreateInfo.bindTarget,
		                pGLCreateInfo.dimensions.mipLevelsNum,
		                pGLCreateInfo.internalFormat,
		                pGLCreateInfo.dimensions.width,
		                pGLCreateInfo.dimensions.height,
		                pGLCreateInfo.dimensions.arraySize );
		ts3OpenGLHandleLastError();

		if( pGLCreateInfo.openglInitDataDesc )
		{
			GLTextureSubDataUploadDesc subDataUploadDesc;
			subDataUploadDesc.openglDimensionClass = GL_TEXTURE_2D_ARRAY;
			subDataUploadDesc.textureSubRegion.uSubReg2DArray.offset.x = 0;
			subDataUploadDesc.textureSubRegion.uSubReg2DArray.offset.y = 0;
			subDataUploadDesc.openglInputDataDesc.openglPixelDataLayout = pGLCreateInfo.openglInitDataDesc.openglPixelDataLayout;
			subDataUploadDesc.openglInputDataDesc.openglPixelDataType = pGLCreateInfo.openglInitDataDesc.openglPixelDataType;

			for( uint32 arraySubTextureIndex = 0; arraySubTextureIndex < pGLCreateInfo.dimensions.arraySize; ++arraySubTextureIndex )
			{
				const auto & textureInitData = pGLCreateInfo.openglInitDataDesc.subTextureInitDataPtr[arraySubTextureIndex];
				subDataUploadDesc.textureSubRegion.uSubReg2DArray.offset.arrayIndex = textureInitData.subTextureIndex;

				for( uint32 mipLevelIndex = 0; mipLevelIndex < pGLCreateInfo.dimensions.mipLevelsNum; ++mipLevelIndex )
				{
					const auto & mipLevelInitData = textureInitData.mipLevelInitDataArray[mipLevelIndex];

					subDataUploadDesc.textureSubRegion.uSubReg2DArray.offset.mipLevel = mipLevelInitData.mipLevelIndex;
					subDataUploadDesc.textureSubRegion.uSubReg2DArray.size.width = mipLevelInitData.mipWidth;
					subDataUploadDesc.textureSubRegion.uSubReg2DArray.size.height = mipLevelInitData.mipHeight;
					subDataUploadDesc.openglInputDataDesc.pointer = mipLevelInitData.pointer;
					subDataUploadDesc.openglInputDataDesc.size = mipLevelInitData.size;

					updateUpload2DArray( subDataUploadDesc, pGLCreateInfo.bindTarget );
				}
			}
		}

		return true;
	}

	bool GLTextureObject::initialize2DMS( const GLTextureCreateInfo & pGLCreateInfo )
	{
		glTexStorage2DMultisample( pGLCreateInfo.bindTarget,
		                           pGLCreateInfo.msaaLevel,
		                           pGLCreateInfo.internalFormat,
		                           pGLCreateInfo.dimensions.width,
		                           pGLCreateInfo.dimensions.height,
		                           GL_FALSE );
		ts3OpenGLHandleLastError();

		return true;
	}

	bool GLTextureObject::initialize3D( const GLTextureCreateInfo & pGLCreateInfo )
	{
		glTexStorage3D( pGLCreateInfo.bindTarget,
		                pGLCreateInfo.dimensions.mipLevelsNum,
		                pGLCreateInfo.internalFormat,
		                pGLCreateInfo.dimensions.width,
		                pGLCreateInfo.dimensions.height,
		                pGLCreateInfo.dimensions.depth );
		ts3OpenGLHandleLastError();

		if( pGLCreateInfo.openglInitDataDesc )
		{
			const auto & textureInitData = pGLCreateInfo.openglInitDataDesc.subTextureInitDataPtr[0];

			GLTextureSubDataUploadDesc subDataUploadDesc;
			subDataUploadDesc.openglDimensionClass = GL_TEXTURE_3D;
			subDataUploadDesc.textureSubRegion.uSubReg3D.offset.x = 0;
			subDataUploadDesc.textureSubRegion.uSubReg3D.offset.y = 0;
			subDataUploadDesc.textureSubRegion.uSubReg3D.offset.z = 0;
			subDataUploadDesc.openglInputDataDesc.openglPixelDataLayout = pGLCreateInfo.openglInitDataDesc.openglPixelDataLayout;
			subDataUploadDesc.openglInputDataDesc.openglPixelDataType = pGLCreateInfo.openglInitDataDesc.openglPixelDataType;

			for( uint32 mipLevelIndex = 0; mipLevelIndex < pGLCreateInfo.dimensions.mipLevelsNum; ++mipLevelIndex )
			{
				const auto & mipLevelInitData = textureInitData.mipLevelInitDataArray[mipLevelIndex];

				subDataUploadDesc.textureSubRegion.uSubReg3D.offset.mipLevel = mipLevelInitData.mipLevelIndex;
				subDataUploadDesc.textureSubRegion.uSubReg3D.size.width = mipLevelInitData.mipWidth;
				subDataUploadDesc.textureSubRegion.uSubReg3D.size.height = mipLevelInitData.mipHeight;
				subDataUploadDesc.textureSubRegion.uSubReg3D.size.depth = mipLevelInitData.mipDepth;
				subDataUploadDesc.openglInputDataDesc.pointer = mipLevelInitData.pointer;
				subDataUploadDesc.openglInputDataDesc.size = mipLevelInitData.size;

				updateUpload3D( subDataUploadDesc, pGLCreateInfo.bindTarget );
			}
		}

		return true;
	}

	bool GLTextureObject::initializeCubeMap( const GLTextureCreateInfo & pGLCreateInfo )
	{
		ts3DebugAssert( pGLCreateInfo.dimensions.arraySize == 6 );

		glTexStorage2D( GL_TEXTURE_CUBE_MAP,
		                pGLCreateInfo.dimensions.mipLevelsNum,
		                pGLCreateInfo.internalFormat,
		                pGLCreateInfo.dimensions.width,
		                pGLCreateInfo.dimensions.height );
		ts3OpenGLHandleLastError();

		if( pGLCreateInfo.openglInitDataDesc )
		{
			GLTextureSubDataUploadDesc subDataUploadDesc;
			subDataUploadDesc.openglDimensionClass = GL_TEXTURE_CUBE_MAP;
			subDataUploadDesc.textureSubRegion.uSubRegCubeMap.offset.x = 0;
			subDataUploadDesc.textureSubRegion.uSubRegCubeMap.offset.y = 0;
			subDataUploadDesc.openglInputDataDesc.openglPixelDataLayout = pGLCreateInfo.openglInitDataDesc.openglPixelDataLayout;
			subDataUploadDesc.openglInputDataDesc.openglPixelDataType = pGLCreateInfo.openglInitDataDesc.openglPixelDataType;

			for( uint32 cubeMapFaceIndex = 0; cubeMapFaceIndex < pGLCreateInfo.dimensions.arraySize; ++cubeMapFaceIndex )
			{
				const auto & cubeMapInitData = pGLCreateInfo.openglInitDataDesc.subTextureInitDataPtr[cubeMapFaceIndex];
				subDataUploadDesc.textureSubRegion.uSubRegCubeMap.offset.faceIndex = cubeMapInitData.subTextureIndex;

				for( uint32 mipLevelIndex = 0; mipLevelIndex < pGLCreateInfo.dimensions.mipLevelsNum; ++mipLevelIndex )
				{
					const auto & mipLevelInitData = cubeMapInitData.mipLevelInitDataArray[mipLevelIndex];

					subDataUploadDesc.textureSubRegion.uSubRegCubeMap.offset.mipLevel = mipLevelInitData.mipLevelIndex;
					subDataUploadDesc.textureSubRegion.uSubRegCubeMap.size.width = mipLevelInitData.mipWidth;
					subDataUploadDesc.textureSubRegion.uSubRegCubeMap.size.height = mipLevelInitData.mipHeight;
					subDataUploadDesc.openglInputDataDesc.pointer = mipLevelInitData.pointer;
					subDataUploadDesc.openglInputDataDesc.size = mipLevelInitData.size;

					updateUploadCubeMap( subDataUploadDesc, pGLCreateInfo.bindTarget );
				}
			}
		}

		return true;
	}

	GLenum GLTextureObject::checkActiveBindTarget( GLenum pBindTarget ) const
	{
		if( pBindTarget == 0 )
		{
			pBindTarget = mGLTextureBindTarget;

			glBindTexture( mGLTextureBindTarget, mGLHandle );
			ts3OpenGLHandleLastError();
		}

		return pBindTarget;
	}

	GLuint GLTextureObject::computeInputPixelDataAlignment( GLenum pPixelDataLayout, GLenum pPixelDataType )
	{
		return 1;
	}

} // namespace ts3::gpuapi
