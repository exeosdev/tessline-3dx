
#pragma once

#ifndef __TS3DRIVER_GPUAPI_GLCOMMON_API_TRANSLATION_LAYER_H__
#define __TS3DRIVER_GPUAPI_GLCOMMON_API_TRANSLATION_LAYER_H__

#include "GL_prerequisites.h"

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
	enum class ETextureClass : enum_default_value_t;
	enum class ETextureFilter : enum_default_value_t;
	enum class ETextureFormat : gpu_pixel_format_value_t;
	enum class ETextureMipMode : enum_default_value_t;

	namespace atl
	{

		GLenum chooseGLBufferStorageFlags( GLenum pBindTarget, Bitmask<resource_flags_value_t> pBufferFlags, Bitmask<EGPUMemoryFlags> pMemoryFlags );

		GLenum chooseGLBufferUsagePolicy( GLenum pBindTarget, Bitmask<resource_flags_value_t> pBufferFlags );

		GLenum chooseGLTextureMagFilter( ETextureFilter pMagFilter, ETextureMipMode pMipMode );

		GLenum chooseGLTextureMinFilter( ETextureFilter pMinFilter, ETextureMipMode pMipMode );

		GLsizei queryGLTextureInternalFormatBPP( GLenum pGLInternalFormat );

		GLenum translateGLBaseDataType( EBaseDataType pBaseDataType );

		GLenum translateGLBlendFactor( EBlendFactor pBlendFactor );

		GLenum translateGLBlendOp( EBlendOp pBlendOp );

		GLenum translateGLBufferBindTarget( EGPUBufferTarget pBufferTarget );

		GLenum translateGLBufferMapFlags( EGPUMemoryMapMode pMapMode, Bitmask<EGPUMemoryFlags> pMemoryFlags );

		GLenum translateGLCompFunc( ECompFunc pCompFunc );

		GLenum translateGLCullMode( ECullMode pCullMode );

		GLenum translateGLIndexDataFormat( EIndexDataFormat pIndexDataFormat );

		GLenum translateGLPrimitiveFillMode( EPrimitiveFillMode pFillMode );

		GLenum translateGLPrimitiveTopology( EPrimitiveTopology pTopology );

		GLenum translateShaderType( EShaderType pShaderType );

		GLenum translateGLStencilOp( EStencilOp pStencilOp );

		GLenum translateGLTextureAddressMode( ETextureAddressMode pAddressMode );

		GLenum translateGLTextureBindTarget( ETextureClass pTextureDimensionClass );

		GLenum translateGLTextureInternalFormat( ETextureFormat pTextureFormat );

		GLenum translateGLTexturePixelDataLayout( ETextureFormat pTextureFormat );

		GLenum translateGLTriangleVerticesOrder( ETriangleVerticesOrder pVerticesOrder );

		const char * translateGLDebugOutputExtensionName( GLDebugOutputVersion pDebugOutputVersion );

		const char * translateGLDebugEventCategoryStrAMD( GLenum pEventCategory );

		const char * translateGLDebugEventSeverityStr( GLenum pEventSeverity );

		const char * translateGLDebugEventSourceStr( GLenum pEventSource );

		const char * translateGLDebugEventTypeStr( GLenum pEventType );

	}

} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_GLCOMMON_API_TRANSLATION_LAYER_H__
