
#pragma once

#ifndef __TS3DRIVER_GPUAPI_MTLCOMMON_API_TRANSLATION_LAYER_H__
#define __TS3DRIVER_GPUAPI_MTLCOMMON_API_TRANSLATION_LAYER_H__

#include "MTL_prerequisites.h"

namespace ts3::gpuapi
{

	using resource_flags_value_t = uint32 ;

	enum EGPUDriverConfigFlags : uint32;
	enum ERenderTargetAttachmentFlags : uint32;
	enum ETextureCubeMapFace : uint32;

	enum class EBlendFactor : uint16;
	enum class EBlendOp : uint16;
	enum class ECompFunc : uint16;
	enum class ECullMode : uint16;
	enum class EStencilOp : uint16;
	enum class EPrimitiveFillMode : uint16;
	enum class EPrimitiveTopology : uint16;
	enum class ETriangleVerticesOrder : uint16;

	enum class EBlendRenderTargetWriteMask : uint32;
	enum class EDepthWriteMask : uint16;

	enum class EGPUBufferTarget : enum_default_value_t;
	enum class EIndexDataFormat : base_data_type_value_t;
	enum class EShaderType : uint32;
	enum class ETextureAddressMode : enum_default_value_t;
	enum class ETextureBorderPredefinedColor : enum_default_value_t;
	enum class ETextureClass : enum_default_value_t;
	enum class ETextureFilter : enum_default_value_t;
	enum class ETextureFormat : gpu_pixel_format_value_t;
	enum class ETextureMipMode : enum_default_value_t;

	namespace atl
	{

		// GLenum translateGLBaseDataType( EBaseDataType pBaseDataType );
		// GLenum translateGLBlendFactor( EBlendFactor pBlendFactor );
		// GLenum translateGLBlendOp( EBlendOp pBlendOp );
		// GLenum translateGLBufferBindTarget( EGPUBufferTarget pBufferTarget );
		// GLenum translateGLBufferMapFlags( EGPUMemoryMapMode pMapMode, Bitmask<EGPUMemoryFlags> pMemoryFlags );
		MTLCompareFunction translateMTLCompFunc( ECompFunc pCompFunc );
		// GLenum translateGLCullMode( ECullMode pCullMode );
		// GLenum translateGLIndexDataFormat( EIndexDataFormat pIndexDataFormat );
		// GLenum translateGLPrimitiveFillMode( EPrimitiveFillMode pFillMode );
		// GLenum translateGLPrimitiveTopology( EPrimitiveTopology pTopology );
		// GLenum translateShaderType( EShaderType pShaderType );
		MTLStencilOperation translateMTLStencilOp( EStencilOp pStencilOp );
		MTLSamplerAddressMode translateMTLTextureAddressMode( ETextureAddressMode pAddressMode );
		MTLSamplerBorderColor translateMTLTextureBorderPredefinedColor( ETextureBorderPredefinedColor pColor );
		MTLSamplerMinMagFilter translateMTLTextureFilter( ETextureFilter pFilter );
		MTLSamplerMipFilter translateMTLTextureMipMode( ETextureMipMode pMipMode );
		// GLenum translateGLTextureBindTarget( ETextureClass pTextureDimensionClass );
		// GLenum translateGLTextureInternalFormat( ETextureFormat pTextureFormat );
		// GLenum translateGLTexturePixelDataLayout( ETextureFormat pTextureFormat );
		// GLenum translateGLTriangleVerticesOrder( ETriangleVerticesOrder pVerticesOrder );

		MTLSamplerBorderColor selectMTLSamplerBorderColor(
				ETextureBorderPredefinedColor pPredefinedColor,
				const math::RGBAColorR32Norm & pCustomColor );

	}

} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_MTLCOMMON_API_TRANSLATION_LAYER_H__
