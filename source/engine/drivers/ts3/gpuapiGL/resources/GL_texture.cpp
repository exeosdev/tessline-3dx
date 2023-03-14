
#include "GL_texture.h"
#include "GL_renderBuffer.h"
#include <ts3/gpuapiGL/GL_coreAPIProxy.h>
#include <ts3/gpuapiGL/GL_gpuDevice.h>

#include <ts3/gpuapi/resources/renderTargetTexture.h>

namespace ts3::gpuapi
{

	GLTexture::GLTexture( GLGPUDevice & pGLGPUDevice,
	                      const ResourceMemoryInfo & pResourceMemory,
	                      const TextureProperties & pTextureProperties,
	                      const TextureLayout & pTextureLayout,
	                      GLTextureObjectHandle pGLTextureObject )
	: Texture( pGLGPUDevice, pResourceMemory, pTextureProperties, pTextureLayout )
	, mGLTextureObject( std::move( pGLTextureObject ) )
	{ }

	GLTexture::~GLTexture() = default;

	GLTextureHandle GLTexture::create( GLGPUDevice & pGLGPUDevice, const TextureCreateInfo & pCreateInfo )
	{
		auto createInfo = pCreateInfo;
		if( !validateTextureCreateInfo( createInfo ) )
		{
			return nullptr;
		}

		GLTextureCreateInfo openglCreateInfo;
		openglCreateInfo.bindTarget = GLCoreAPIProxy::translateGLTextureBindTarget( createInfo.texClass );
		openglCreateInfo.dimensions = createInfo.dimensions;
		openglCreateInfo.msaaLevel = createInfo.msaaLevel;
		openglCreateInfo.internalFormat = GLCoreAPIProxy::translateGLTextureInternalFormat( createInfo.pixelFormat );
		openglCreateInfo.openglInitDataDesc.subTextureInitDataPtr = std::move( createInfo.initDataDesc.subTextureInitDataBasePtr );
		auto textureInitDataBaseType = cxdefs::getTextureFormatBaseDataType( createInfo.pixelFormat );
		openglCreateInfo.openglInitDataDesc.openglPixelDataLayout = GLCoreAPIProxy::translateGLTexturePixelDataLayout( createInfo.pixelFormat );
		openglCreateInfo.openglInitDataDesc.openglPixelDataType = GLCoreAPIProxy::translateGLBaseDataType( textureInitDataBaseType );

		auto openglTextureObject = GLTextureObject::create( openglCreateInfo );
		ts3DebugAssert( openglTextureObject );

		const auto textureInternalFormat = openglTextureObject->queryInternalFormat();
		const auto textureSize = openglTextureObject->querySize();

		ResourceMemoryInfo textureMemoryInfo;
		textureMemoryInfo.sourceHeapRegion.offset = 0;
		textureMemoryInfo.sourceHeapRegion.size = textureSize;
		textureMemoryInfo.baseAlignment = createInfo.memoryBaseAlignment;
		textureMemoryInfo.memoryFlags = createInfo.memoryFlags;

		TextureProperties textureProperties;
		textureProperties.resourceFlags = createInfo.resourceFlags;

		TextureLayout textureLayout;
		textureLayout.texClass = createInfo.texClass;
		textureLayout.dimensions = createInfo.dimensions;
		textureLayout.pixelFormat = createInfo.pixelFormat;
		textureLayout.msaaLevel = createInfo.msaaLevel;
		textureLayout.storageSize = numeric_cast<uint32>( textureSize );
		textureLayout.bitsPerPixel = GLCoreAPIProxy::queryGLTextureInternalFormatBPP( textureInternalFormat );

		auto openglTexture = createDynamicInterfaceObject<GLTexture>( pGLGPUDevice,
		                                                              textureMemoryInfo,
		                                                              textureProperties,
		                                                              textureLayout,
		                                                              std::move( openglTextureObject ) );

		return openglTexture;
	}

	RenderTargetTextureHandle GLTexture::createRTT( GLGPUDevice & pGLGPUDevice, const RenderTargetTextureCreateInfo & pCreateInfo )
	{
		const auto renderBufferIncompatibleBindFlags =
				E_GPU_RESOURCE_USAGE_FLAG_SHADER_INPUT_BIT |
				E_GPU_RESOURCE_USAGE_FLAG_RENDER_TARGET_COLOR_BIT |
				E_GPU_RESOURCE_USAGE_FLAG_TRANSFER_TARGET_BIT;

		const auto rttType = rcutil::queryRenderTargetTextureType( pCreateInfo.targetTexture->mTextureLayout.pixelFormat );

		if( pCreateInfo.bindFlags.isSetAnyOf( renderBufferIncompatibleBindFlags ) )
		{
			TextureCreateInfo textureCreateInfo;
			textureCreateInfo.texClass = ETextureClass::T2D;
			textureCreateInfo.dimensions.width = pCreateInfo.rttLayout.bufferSize.width;
			textureCreateInfo.dimensions.height = pCreateInfo.rttLayout.bufferSize.height;
			textureCreateInfo.memoryFlags = E_GPU_MEMORY_ACCESS_FLAG_GPU_READ_BIT;
			textureCreateInfo.resourceFlags = ( pCreateInfo.bindFlags & E_GPU_RESOURCE_USAGE_MASK_ALL );
			textureCreateInfo.pixelFormat = pCreateInfo.rttLayout.internalDataFormat;
			textureCreateInfo.initialTarget = rcutil::getTextureTargetFromResourceFlags( pCreateInfo.bindFlags );

			auto glcTexture = GLTexture::create( pGLGPUDevice, textureCreateInfo );
			if( !glcTexture )
			{
				return nullptr;
			}

			auto textureRTT = createGPUAPIObject<RenderTargetTexture>(
					pGLGPUDevice,
					rttType,
					pCreateInfo.rttLayout,
					TextureReference{ glcTexture } );

			return textureRTT;
		}
		else
		{
			auto glcRenderBuffer = GLInternalRenderBuffer::createInstance( pGLGPUDevice, pCreateInfo );
			if( !glcRenderBuffer )
			{
				return nullptr;
			}

			auto renderBufferRTT = createGPUAPIObject<RenderTargetTexture>(
					pGLGPUDevice,
					rttType,
					pCreateInfo.rttLayout,
					glcRenderBuffer,
					pCreateInfo.bindFlags );

			return renderBufferRTT;
		}
	}

} // namespace ts3::gpuapi
