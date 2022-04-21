
#include "GL_texture.h"
#include <ts3/gpuapiGL/GL_coreAPIProxy.h>
#include <ts3/gpuapiGL/GL_gpuDevice.h>

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
		openglCreateInfo.bindTarget = GLCoreAPIProxy::translateGLTextureBindTarget( createInfo.dimensionClass );
		openglCreateInfo.dimensions = createInfo.dimensions;
		openglCreateInfo.msaaLevel = createInfo.msaaLevel;
		openglCreateInfo.internalFormat = GLCoreAPIProxy::translateGLTextureInternalFormat( createInfo.pixelFormat );
		openglCreateInfo.openglInitDataDesc.subTextureInitDataPtr = std::move( createInfo.initDataDesc.subTextureInitDataBasePtr );
		auto textureInitDataBaseType = ecGetTextureFormatBaseDataType( createInfo.pixelFormat );
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
		textureLayout.dimensionClass = createInfo.dimensionClass;
		textureLayout.dimensions = createInfo.dimensions;
		textureLayout.pixelFormat = createInfo.pixelFormat;
		textureLayout.msaaLevel = createInfo.msaaLevel;
		textureLayout.storageSize = trunc_numeric_cast<uint32>( textureSize );
		textureLayout.bitsPerPixel = GLCoreAPIProxy::queryGLTextureInternalFormatBPP( textureInternalFormat );

		auto openglTexture = createDynamicInterfaceObject<GLTexture>( pGLGPUDevice,
		                                                              textureMemoryInfo,
		                                                              textureProperties,
		                                                              textureLayout,
		                                                              std::move( openglTextureObject ) );

		return openglTexture;
	}

} // namespace ts3::gpuapi
