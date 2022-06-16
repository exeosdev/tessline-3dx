
#pragma once

#ifndef __TS3_GPUAPI_COMMON_STATE_DEFS_H__
#define __TS3_GPUAPI_COMMON_STATE_DEFS_H__

#include "../memory/commonGPUMemoryDefs.h"
#include <ts3/stdext/hash.h>
#include <list>

namespace ts3::gpuapi
{

	ts3DeclareClassHandle( GPUStateObject );
	ts3DeclareClassHandle( PipelineStateObject );
	ts3DeclareClassHandle( GraphicsPipelineStateObject );
	ts3DeclareClassHandle( VertexStreamStateObject );

	struct GraphicsShaderBinding;
	struct ShaderInputSignature;

	struct GraphicsPipelineStateObjectCreateInfo;
	struct VertexStreamStateObjectCreateInfo;

	using shader_input_ref_id_t = uint64;
	using shader_input_index_t = uint32;

	using vertex_attribute_index_t = uint16;
	using vertex_stream_index_t = uint16;

	using EBlendState = EActiveState;
	using EDepthTestState = EActiveState;
	using EStencilTestState = EActiveState;
	using EScissorTestState = EActiveState;

	constexpr vertex_attribute_index_t CX_INVALID_VERTEX_ATTRIBUTE_INDEX = Limits<vertex_attribute_index_t>::maxValue;
	constexpr vertex_stream_index_t CX_INVALID_VERTEX_STREAM_INDEX = Limits<vertex_stream_index_t>::maxValue;

	enum class EBlendFactor : enum_default_value_t
	{
		Zero,
		One,
		Const,
		ConstInv,
		SrcColor,
		SrcAlpha,
		DstColor,
		DstAlpha,
		SrcColorInv,
		SrcAlphaInv,
		DstColorInv,
		DstAlphaInv,
	};

	enum class EBlendOp : enum_default_value_t
	{
		Add,
		Min,
		Max,
		Subtract,
		SubtractRev
	};

	enum class EBlendRenderTargetWriteMask : enum_default_value_t
	{
		Zero = 0,
		All = ts3::Limits<enum_default_value_t>::maxValue
	};

	enum class ECompFunc : enum_default_value_t
	{
		Never,
		Always,
		Equal,
		NotEqual,
		Greater,
		GreaterEqual,
		Less,
		LessEqual
	};

	enum class ECullMode : enum_default_value_t
	{
		None,
		Back,
		Front
	};

	enum class EDepthWriteMask : enum_default_value_t
	{
		Zero = 0,
		All = ts3::Limits<enum_default_value_t>::maxValue
	};

	enum class EPrimitiveFillMode : enum_default_value_t
	{
		Solid,
		Wireframe
	};

	enum class EPrimitiveTopology : enum_default_value_t
	{
		PointList,
		LineList,
		LineListAdj,
		LineStrip,
		LineStripAdj,
		TriangleList,
		TriangleListAdj,
		TriangleStrip,
		TriangleStripAdj,
		TesselationPatch
	};

	enum class EStencilOp : enum_default_value_t
	{
		Zero,
		Keep,
		Replace,
		IncrClamp,
		IncrWrap,
		DecrClamp,
		DecrWrap,
		Invert
	};

	enum class ETriangleVerticesOrder : enum_default_value_t
	{
		Clockwise,
		CounterClockwise
	};

	enum class EShaderInputParameterType : uint16
	{
		// D3D12: root constants
		// Vulkan: push constants
		// Others: API-specific (explicit direct constants or implicit constant buffer)
		Constant,
		// CBV/SRV/UAV
		Resource,
		//
		Sampler
	};

	enum class EShaderInputDescriptorType : uint16
	{
		// CBV/SRV/UAV
		Resource = 1,
		//
		Sampler,
	};

	enum class EShaderInputResourceClass : uint16
	{
		CBV = 1,
		SRV,
		UAV,
		Unknown = 0,
	};

	inline constexpr uint32 ecDeclareShaderInputResourceType( EShaderInputResourceClass pResourceClass, uint16 pIndex )
	{
		return ( ( static_cast<uint32>( pResourceClass ) << 16u ) | pIndex );
	}

	enum class EShaderInputResourceType : uint32
	{
		CBVConstantBuffer = ecDeclareShaderInputResourceType( EShaderInputResourceClass::CBV, 0 ),
		SRVTextureBuffer  = ecDeclareShaderInputResourceType( EShaderInputResourceClass::SRV, 1 ),
		SRVTextureImage   = ecDeclareShaderInputResourceType( EShaderInputResourceClass::SRV, 2 ),
		UAVStorageBuffer  = ecDeclareShaderInputResourceType( EShaderInputResourceClass::UAV, 3 ),
		UAVStorageImage   = ecDeclareShaderInputResourceType( EShaderInputResourceClass::UAV, 4 ),
		Unknown        = 0
	};

	inline constexpr EShaderInputResourceClass ecGetShaderInputResourceResourceClass( EShaderInputResourceType pResourceType )
	{
		return static_cast<EShaderInputResourceClass>( ( static_cast<uint32>( pResourceType ) >> 16u ) & 0xFFFFu );
	}

	struct RenderTargetClearConfig
	{
		math::RGBAColorR32Norm colorClearValue;
		float depthClearValue;
		uint8 stencilClearValue;
	};

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_COMMON_STATE_DEFS_H__
