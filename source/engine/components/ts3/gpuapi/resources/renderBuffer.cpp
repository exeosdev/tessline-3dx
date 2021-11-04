
#include "renderBuffer.h"
#include "texture.h"
#include <set>

namespace ts3
{
namespace gpuapi
{

	static bool checkColorRenderBufferFormat( ETextureFormat pFormat );
	static bool checkDepthStencilRenderBufferFormat( ETextureFormat pFormat );


	RenderBuffer::RenderBuffer( GPUDevice & pGPUDevice, ERenderBufferType pRenderBufferType, const RenderBufferLayout & pRenderBufferLayout )
	: GPUBaseObject( pGPUDevice )
	, mERenderBufferType( pRenderBufferType )
	, mRenderBufferLayout( pRenderBufferLayout )
	{}

	RenderBuffer::~RenderBuffer() = default;

	bool RenderBuffer::validateCreateInfo( const RenderBufferCreateInfo & pCreateInfo )
	{
		if( pCreateInfo.bufferType == ERenderBufferType::RBColor )
		{
			if( !checkColorRenderBufferFormat( pCreateInfo.layout.internalDataFormat ) )
			{
				return false;
			}
		}
		else if( pCreateInfo.bufferType == ERenderBufferType::RBDepthStencil )
		{
			if( !checkDepthStencilRenderBufferFormat( pCreateInfo.layout.internalDataFormat ) )
			{
				return false;
			}
		}
		else
		{
			ts3DebugInterrupt();
			return false;
		}

		return true;
	}



	bool checkColorRenderBufferFormat( ETextureFormat pFormat )
	{
		// Color render buffer can be created with any format except for compressed ones
		// and depth/stencil-specific. Since it's easier to do that, we define a set of
		// INVALID formats here.

		static const std::set<ETextureFormat> sInvalidColorFormats =
		{
			ETextureFormat::BC1_RGBA_UNORM,
			ETextureFormat::BC1_RGBA_UNORM_SRGB,
			ETextureFormat::BC2_UNORM,
			ETextureFormat::BC2_UNORM_SRGB,
			ETextureFormat::BC3_UNORM,
			ETextureFormat::BC3_UNORM_SRGB,
			ETextureFormat::BC4_SNORM,
			ETextureFormat::BC4_UNORM,
			ETextureFormat::BC5_SNORM,
			ETextureFormat::BC5_UNORM,
			ETextureFormat::BC6H_SFLOAT,
			ETextureFormat::BC6H_UFLOAT,
			ETextureFormat::BC7_UNORM,
			ETextureFormat::BC7_UNORM_SRGB,
			ETextureFormat::D16_UNORM,
			ETextureFormat::D24_UNORM_S8_UINT,
			ETextureFormat::D24_UNORM_X8,
			ETextureFormat::D32_FLOAT,
		};

		return sInvalidColorFormats.count( pFormat ) == 0;
	}

	bool checkDepthStencilRenderBufferFormat( ETextureFormat pFormat )
	{
		// Only depth and combined depth/stencil formats are valid for DS render buffer.
		// Check if the specified format is one of those.

		static const std::set<ETextureFormat> sValidDepthStencilFormats =
		{
			ETextureFormat::D16_UNORM,
			ETextureFormat::D24_UNORM_S8_UINT,
			ETextureFormat::D24_UNORM_X8,
			ETextureFormat::D32_FLOAT,
		};

		return sValidDepthStencilFormats.count( pFormat ) != 0;
	}

} /* namespace ts3 */
} /* namespace gpuapi */
