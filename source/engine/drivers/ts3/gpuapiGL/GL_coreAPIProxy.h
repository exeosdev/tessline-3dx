
#pragma once

#ifndef __TS3DRIVER_GPUAPI_GLCOMMON_CORE_LAYER_H__
#define __TS3DRIVER_GPUAPI_GLCOMMON_CORE_LAYER_H__

#include "GL_prerequisites.h"

namespace ts3::gpuapi
{

	using resource_flags_value_t = uint32 ;

	enum GPUDriverConfigFlags : uint32;
	enum ERenderTargetAttachmentFlags : uint32;

	enum class EBlendFactor : enum_default_value_t;
	enum class EBlendOp : enum_default_value_t;
	enum class EBlendRenderTargetWriteMask : enum_default_value_t;
	enum class ECompFunc : enum_default_value_t;
	enum class ECullMode : enum_default_value_t;
	enum class EDepthWriteMask : enum_default_value_t;
	enum class EGPUBufferTarget : enum_default_value_t;
	enum class EPrimitiveFillMode : enum_default_value_t;
	enum class EPrimitiveTopology : enum_default_value_t;
	enum class EShaderType : uint32;
	enum class EStencilOp : enum_default_value_t;
	enum class ETextureAddressMode : enum_default_value_t;
	enum class ETextureCubeMapFace : size_t;
	enum class ETextureDimensionClass : enum_default_value_t;
	enum class ETextureFilter : enum_default_value_t;
	enum class ETextureFormat : gpu_pixel_format_value_t;
	enum class ETextureMipMode : enum_default_value_t;
	enum class ETriangleVerticesOrder : enum_default_value_t;

	namespace GLCoreAPIProxy
	{

		GLenum chooseGLBufferInitFlagsCoreES( GLenum pBindTarget, Bitmask<resource_flags_value_t> pBufferFlags, Bitmask<EGPUMemoryFlags> pMemoryFlags );

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

		GLenum translateGLPrimitiveFillMode( EPrimitiveFillMode pFillMode );

		GLenum translateGLPrimitiveTopology( EPrimitiveTopology pTopology );

		GLenum translateGLEShaderType( EShaderType pShaderType );

		GLenum translateGLStencilOp( EStencilOp pStencilOp );

		GLenum translateGLETextureAddressMode( ETextureAddressMode pAddressMode );

		GLenum translateGLTextureBindTarget( ETextureDimensionClass pTextureDimensionClass );

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

#endif // __TS3DRIVER_GPUAPI_GLCOMMON_CORE_LAYER_H__
