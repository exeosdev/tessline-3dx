
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
		ts3GLHandleLastError();

		glBindTexture( pGLCreateInfo.bindTarget, textureHandle );
		ts3GLHandleLastError();

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
		ts3GLHandleLastError();

		return true;
	}

	bool GLTextureObject::validateHandle() const
	{
		auto isBuffer = glIsTexture( mGLHandle );
		ts3GLHandleLastError();

		return isBuffer != GL_FALSE;
	}

	void GLTextureObject::updateCopy2D( GLTextureObject & pSource, const TextureSubDataCopyDesc & pCopyDesc, GLenum pActiveBindTarget )
	{
	}

	void GLTextureObject::updateCopy2DArray( GLTextureObject & pSource, const TextureSubDataCopyDesc & pCopyDesc, GLenum pActiveBindTarget )
	{
	}

	void GLTextureObject::updateCopy3D( GLTextureObject & pSource, const TextureSubDataCopyDesc & pCopyDesc, GLenum pActiveBindTarget )
	{
	}

	void GLTextureObject::updateCopyCubeMap( GLTextureObject & pSource, const TextureSubDataCopyDesc & pCopyDesc, GLenum pActiveBindTarget )
	{
	}

	void GLTextureObject::updateUpload2D( const GLTextureSubDataUploadDesc & pGLUploadDesc, GLenum pActiveBindTarget )
	{
		ts3DebugAssert( pGLUploadDesc.openglDimensionClass == GL_TEXTURE_2D );
		ts3DebugAssert( ( pActiveBindTarget == 0 ) || ( pActiveBindTarget == GL_TEXTURE_2D ) );

		auto textureBindTarget = checkActiveBindTarget( pActiveBindTarget );

		glTexSubImage2D( textureBindTarget,
		                 pGLUploadDesc.textureSubRegion.offset.u2D.mipLevel,
		                 pGLUploadDesc.textureSubRegion.offset.u2D.x,
		                 pGLUploadDesc.textureSubRegion.offset.u2D.y,
		                 pGLUploadDesc.textureSubRegion.size.u2D.width,
		                 pGLUploadDesc.textureSubRegion.size.u2D.height,
		                 pGLUploadDesc.openglInputDataDesc.openglPixelDataLayout,
		                 pGLUploadDesc.openglInputDataDesc.openglPixelDataType,
		                 pGLUploadDesc.openglInputDataDesc.pointer );
		ts3GLHandleLastError();
	}

	void GLTextureObject::updateUpload2DArray( const GLTextureSubDataUploadDesc & pGLUploadDesc, GLenum pActiveBindTarget )
	{
		ts3DebugAssert( pGLUploadDesc.openglDimensionClass == GL_TEXTURE_2D_ARRAY );
		ts3DebugAssert( ( pActiveBindTarget == 0 ) || ( pActiveBindTarget == GL_TEXTURE_2D_ARRAY ) );

		auto textureBindTarget = checkActiveBindTarget( pActiveBindTarget );

		glTexSubImage3D( textureBindTarget,
		                 pGLUploadDesc.textureSubRegion.offset.u2DArray.mipLevel,
		                 pGLUploadDesc.textureSubRegion.offset.u2DArray.x,
		                 pGLUploadDesc.textureSubRegion.offset.u2DArray.y,
		                 pGLUploadDesc.textureSubRegion.offset.u2DArray.arrayIndex,
		                 pGLUploadDesc.textureSubRegion.size.u2DArray.width,
		                 pGLUploadDesc.textureSubRegion.size.u2DArray.height,
		                 1,
		                 pGLUploadDesc.openglInputDataDesc.openglPixelDataLayout,
		                 pGLUploadDesc.openglInputDataDesc.openglPixelDataType,
		                 pGLUploadDesc.openglInputDataDesc.pointer );
		ts3GLHandleLastError();
	}

	void GLTextureObject::updateUpload3D( const GLTextureSubDataUploadDesc & pGLUploadDesc, GLenum pActiveBindTarget )
	{
		ts3DebugAssert( pGLUploadDesc.openglDimensionClass == GL_TEXTURE_3D );
		ts3DebugAssert( ( pActiveBindTarget == 0 ) || ( pActiveBindTarget == GL_TEXTURE_3D ) );

		auto textureBindTarget = checkActiveBindTarget( pActiveBindTarget );

		glTexSubImage3D( textureBindTarget,
		                 pGLUploadDesc.textureSubRegion.offset.u3D.mipLevel,
		                 pGLUploadDesc.textureSubRegion.offset.u3D.x,
		                 pGLUploadDesc.textureSubRegion.offset.u3D.y,
		                 pGLUploadDesc.textureSubRegion.offset.u3D.z,
		                 pGLUploadDesc.textureSubRegion.size.u3D.width,
		                 pGLUploadDesc.textureSubRegion.size.u3D.height,
		                 pGLUploadDesc.textureSubRegion.size.u3D.depth,
		                 pGLUploadDesc.openglInputDataDesc.openglPixelDataLayout,
		                 pGLUploadDesc.openglInputDataDesc.openglPixelDataType,
		                 pGLUploadDesc.openglInputDataDesc.pointer );
		ts3GLHandleLastError();
	}

	void GLTextureObject::updateUploadCubeMap( const GLTextureSubDataUploadDesc & pGLUploadDesc, GLenum pActiveBindTarget )
	{
		ts3DebugAssert( pGLUploadDesc.openglDimensionClass == GL_TEXTURE_CUBE_MAP );
		ts3DebugAssert( ( pActiveBindTarget == 0 ) || ( pActiveBindTarget == GL_TEXTURE_CUBE_MAP ) );

		checkActiveBindTarget( pActiveBindTarget );

		glTexSubImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + pGLUploadDesc.textureSubRegion.offset.uCubeMap.uFaceIndex,
		                 pGLUploadDesc.textureSubRegion.offset.uCubeMap.mipLevel,
		                 pGLUploadDesc.textureSubRegion.offset.uCubeMap.x,
		                 pGLUploadDesc.textureSubRegion.offset.uCubeMap.y,
		                 pGLUploadDesc.textureSubRegion.size.uCubeMap.width,
		                 pGLUploadDesc.textureSubRegion.size.uCubeMap.height,
		                 pGLUploadDesc.openglInputDataDesc.openglPixelDataLayout,
		                 pGLUploadDesc.openglInputDataDesc.openglPixelDataType,
		                 pGLUploadDesc.openglInputDataDesc.pointer );
		ts3GLHandleLastError();
	}

	GLenum GLTextureObject::queryInternalFormat( GLenum pActiveBindTarget ) const
	{
		auto textureBindTarget = checkActiveBindTarget( pActiveBindTarget );

		GLint textureInternalFormat = 0;
		glGetTexLevelParameteriv( textureBindTarget, 0, GL_TEXTURE_INTERNAL_FORMAT, &textureInternalFormat );
		ts3GLHandleLastError();

		return textureInternalFormat;
	}

	memory_size_t GLTextureObject::querySize( GLenum pActiveBindTarget ) const
	{
		auto textureBindTarget = checkActiveBindTarget( pActiveBindTarget );

		memory_size_t textureImageSize = 0;

		GLint textureCompressedFlag = 0;
		glGetTexLevelParameteriv( textureBindTarget, 0, GL_TEXTURE_COMPRESSED, &textureCompressedFlag );
		ts3GLHandleLastError();

		if( textureCompressedFlag )
		{
			GLint textureCompressedImageSize = 0;
			glGetTexLevelParameteriv( textureBindTarget, 0, GL_TEXTURE_COMPRESSED_IMAGE_SIZE, &textureCompressedImageSize );
			ts3GLHandleLastError();

			textureImageSize = textureCompressedImageSize;
		}
		else
		{
			GLint textureWidth = 0;
			glGetTexLevelParameteriv( textureBindTarget, 0, GL_TEXTURE_WIDTH, &textureWidth );
			ts3GLHandleLastError();

			GLint textureHeight = 0;
			glGetTexLevelParameteriv( textureBindTarget, 0, GL_TEXTURE_HEIGHT, &textureHeight );
			ts3GLHandleLastError();

			GLint textureDepth = 0;
			glGetTexLevelParameteriv( textureBindTarget, 0, GL_TEXTURE_DEPTH, &textureDepth );
			ts3GLHandleLastError();

			GLint textureInternalFormat = 0;
			glGetTexLevelParameteriv( textureBindTarget, 0, GL_TEXTURE_INTERNAL_FORMAT, &textureInternalFormat );
			ts3GLHandleLastError();

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
		ts3GLHandleLastError();

		if( pGLCreateInfo.openglInitDataDesc )
		{
			const auto & textureInitData = pGLCreateInfo.openglInitDataDesc.subTextureInitDataPtr[0];

			GLTextureSubDataUploadDesc subDataUploadDesc;
			subDataUploadDesc.openglDimensionClass = GL_TEXTURE_2D;
			subDataUploadDesc.textureSubRegion.offset.u2D.x = 0;
			subDataUploadDesc.textureSubRegion.offset.u2D.y = 0;
			subDataUploadDesc.openglInputDataDesc.openglPixelDataLayout = pGLCreateInfo.openglInitDataDesc.openglPixelDataLayout;
			subDataUploadDesc.openglInputDataDesc.openglPixelDataType = pGLCreateInfo.openglInitDataDesc.openglPixelDataType;

			for( uint32 mipLevelIndex = 0; mipLevelIndex < pGLCreateInfo.dimensions.mipLevelsNum; ++mipLevelIndex )
			{
				const auto & mipLevelInitData = textureInitData.mipLevelInitDataArray[mipLevelIndex];

				subDataUploadDesc.textureSubRegion.offset.u2D.mipLevel = mipLevelInitData.mipLevelIndex;
				subDataUploadDesc.textureSubRegion.size.u2D.width = mipLevelInitData.mipWidth;
				subDataUploadDesc.textureSubRegion.size.u2D.height = mipLevelInitData.mipHeight;
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
		ts3GLHandleLastError();

		if( pGLCreateInfo.openglInitDataDesc )
		{
			GLTextureSubDataUploadDesc subDataUploadDesc;
			subDataUploadDesc.openglDimensionClass = GL_TEXTURE_2D_ARRAY;
			subDataUploadDesc.textureSubRegion.offset.u2DArray.x = 0;
			subDataUploadDesc.textureSubRegion.offset.u2DArray.y = 0;
			subDataUploadDesc.openglInputDataDesc.openglPixelDataLayout = pGLCreateInfo.openglInitDataDesc.openglPixelDataLayout;
			subDataUploadDesc.openglInputDataDesc.openglPixelDataType = pGLCreateInfo.openglInitDataDesc.openglPixelDataType;

			for( uint32 arraySubTextureIndex = 0; arraySubTextureIndex < pGLCreateInfo.dimensions.arraySize; ++arraySubTextureIndex )
			{
				const auto & textureInitData = pGLCreateInfo.openglInitDataDesc.subTextureInitDataPtr[arraySubTextureIndex];
				subDataUploadDesc.textureSubRegion.offset.u2DArray.arrayIndex = textureInitData.subTextureIndex;

				for( uint32 mipLevelIndex = 0; mipLevelIndex < pGLCreateInfo.dimensions.mipLevelsNum; ++mipLevelIndex )
				{
					const auto & mipLevelInitData = textureInitData.mipLevelInitDataArray[mipLevelIndex];

					subDataUploadDesc.textureSubRegion.offset.u2DArray.mipLevel = mipLevelInitData.mipLevelIndex;
					subDataUploadDesc.textureSubRegion.size.u2DArray.width = mipLevelInitData.mipWidth;
					subDataUploadDesc.textureSubRegion.size.u2DArray.height = mipLevelInitData.mipHeight;
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
		ts3GLHandleLastError();

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
		ts3GLHandleLastError();

		if( pGLCreateInfo.openglInitDataDesc )
		{
			const auto & textureInitData = pGLCreateInfo.openglInitDataDesc.subTextureInitDataPtr[0];

			GLTextureSubDataUploadDesc subDataUploadDesc;
			subDataUploadDesc.openglDimensionClass = GL_TEXTURE_3D;
			subDataUploadDesc.textureSubRegion.offset.u3D.x = 0;
			subDataUploadDesc.textureSubRegion.offset.u3D.y = 0;
			subDataUploadDesc.textureSubRegion.offset.u3D.z = 0;
			subDataUploadDesc.openglInputDataDesc.openglPixelDataLayout = pGLCreateInfo.openglInitDataDesc.openglPixelDataLayout;
			subDataUploadDesc.openglInputDataDesc.openglPixelDataType = pGLCreateInfo.openglInitDataDesc.openglPixelDataType;

			for( uint32 mipLevelIndex = 0; mipLevelIndex < pGLCreateInfo.dimensions.mipLevelsNum; ++mipLevelIndex )
			{
				const auto & mipLevelInitData = textureInitData.mipLevelInitDataArray[mipLevelIndex];

				subDataUploadDesc.textureSubRegion.offset.u3D.mipLevel = mipLevelInitData.mipLevelIndex;
				subDataUploadDesc.textureSubRegion.size.u3D.width = mipLevelInitData.mipWidth;
				subDataUploadDesc.textureSubRegion.size.u3D.height = mipLevelInitData.mipHeight;
				subDataUploadDesc.textureSubRegion.size.u3D.depth = mipLevelInitData.mipDepth;
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
		ts3GLHandleLastError();

		if( pGLCreateInfo.openglInitDataDesc )
		{
			GLTextureSubDataUploadDesc subDataUploadDesc;
			subDataUploadDesc.openglDimensionClass = GL_TEXTURE_CUBE_MAP;
			subDataUploadDesc.textureSubRegion.offset.uCubeMap.x = 0;
			subDataUploadDesc.textureSubRegion.offset.uCubeMap.y = 0;
			subDataUploadDesc.openglInputDataDesc.openglPixelDataLayout = pGLCreateInfo.openglInitDataDesc.openglPixelDataLayout;
			subDataUploadDesc.openglInputDataDesc.openglPixelDataType = pGLCreateInfo.openglInitDataDesc.openglPixelDataType;

			for( uint32 cubeMapFaceIndex = 0; cubeMapFaceIndex < pGLCreateInfo.dimensions.arraySize; ++cubeMapFaceIndex )
			{
				const auto & cubeMapInitData = pGLCreateInfo.openglInitDataDesc.subTextureInitDataPtr[cubeMapFaceIndex];
				subDataUploadDesc.textureSubRegion.offset.uCubeMap.uFaceIndex = cubeMapInitData.subTextureIndex;

				for( uint32 mipLevelIndex = 0; mipLevelIndex < pGLCreateInfo.dimensions.mipLevelsNum; ++mipLevelIndex )
				{
					const auto & mipLevelInitData = cubeMapInitData.mipLevelInitDataArray[mipLevelIndex];

					subDataUploadDesc.textureSubRegion.offset.uCubeMap.mipLevel = mipLevelInitData.mipLevelIndex;
					subDataUploadDesc.textureSubRegion.size.uCubeMap.width = mipLevelInitData.mipWidth;
					subDataUploadDesc.textureSubRegion.size.uCubeMap.height = mipLevelInitData.mipHeight;
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
			ts3GLHandleLastError();
		}

		return pBindTarget;
	}

	GLuint GLTextureObject::computeInputPixelDataAlignment( GLenum pPixelDataLayout, GLenum pPixelDataType )
	{
		return 1;
	}

}
