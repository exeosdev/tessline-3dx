
#pragma once

#ifndef __TS3_GPUAPI_COMMON_STATE_DEFS_H__
#define __TS3_GPUAPI_COMMON_STATE_DEFS_H__

#include "../memory/commonGPUMemoryDefs.h"
#include <ts3/stdext/hash.h>

namespace ts3::gpuapi
{

	ts3DeclareClassHandle( GPUStateObject );
	ts3DeclareClassHandle( PipelineStateObject );
	ts3DeclareClassHandle( GraphicsPipelineStateObject );
	ts3DeclareClassHandle( VertexStreamStateObject );

	struct GraphicsPipelineStateObjectCreateInfo;
	struct VertexStreamStateObjectCreateInfo;

	using shader_input_ref_id_t = uint64;
	using shader_input_index_t = uint32;

	using pipeline_descriptor_id_t = HashObject<HashAlgo::FNV1A>;
	using pipeline_descriptor_input_hash_t = HashObject<HashAlgo::FNV1A>;

	inline constexpr auto CX_PIPELINE_DESCRIPTOR_ID_INVALID = HashObject<HashAlgo::FNV1A>{};

	inline constexpr gpu_memory_size_t CX_VERTEX_ATTRIBUTE_OFFSET_APPEND = Limits<gpu_memory_size_t>::maxValue;

	/// @brief
	enum ERenderTargetAttachmentIndex : uint8
	{
		E_RENDER_TARGET_ATTACHMENT_INDEX_COLOR_0,
		E_RENDER_TARGET_ATTACHMENT_INDEX_COLOR_1,
		E_RENDER_TARGET_ATTACHMENT_INDEX_COLOR_2,
		E_RENDER_TARGET_ATTACHMENT_INDEX_COLOR_3,
		E_RENDER_TARGET_ATTACHMENT_INDEX_COLOR_4,
		E_RENDER_TARGET_ATTACHMENT_INDEX_COLOR_5,
		E_RENDER_TARGET_ATTACHMENT_INDEX_COLOR_6,
		E_RENDER_TARGET_ATTACHMENT_INDEX_COLOR_7,
		E_RENDER_TARGET_ATTACHMENT_INDEX_DEPTH_STENCIL
	};

	/// @brief A set of bit flags representing render target attachments.
	enum ERenderTargetAttachmentFlags : uint32
	{
		E_RENDER_TARGET_ATTACHMENT_FLAG_COLOR_0_BIT    = 1 << E_RENDER_TARGET_ATTACHMENT_INDEX_COLOR_0,
		E_RENDER_TARGET_ATTACHMENT_FLAG_COLOR_1_BIT    = 1 << E_RENDER_TARGET_ATTACHMENT_INDEX_COLOR_1,
		E_RENDER_TARGET_ATTACHMENT_FLAG_COLOR_2_BIT    = 1 << E_RENDER_TARGET_ATTACHMENT_INDEX_COLOR_2,
		E_RENDER_TARGET_ATTACHMENT_FLAG_COLOR_3_BIT    = 1 << E_RENDER_TARGET_ATTACHMENT_INDEX_COLOR_3,
		E_RENDER_TARGET_ATTACHMENT_FLAG_COLOR_4_BIT    = 1 << E_RENDER_TARGET_ATTACHMENT_INDEX_COLOR_4,
		E_RENDER_TARGET_ATTACHMENT_FLAG_COLOR_5_BIT    = 1 << E_RENDER_TARGET_ATTACHMENT_INDEX_COLOR_5,
		E_RENDER_TARGET_ATTACHMENT_FLAG_COLOR_6_BIT    = 1 << E_RENDER_TARGET_ATTACHMENT_INDEX_COLOR_6,
		E_RENDER_TARGET_ATTACHMENT_FLAG_COLOR_7_BIT    = 1 << E_RENDER_TARGET_ATTACHMENT_INDEX_COLOR_7,
		E_RENDER_TARGET_ATTACHMENT_MASK_COLOR_ALL      = 0x7F,
		E_RENDER_TARGET_ATTACHMENT_FLAG_DEPTH_STENCIL  = 1 << E_RENDER_TARGET_ATTACHMENT_INDEX_DEPTH_STENCIL,
		E_RENDER_TARGET_ATTACHMENT_MASK_ALL =
				E_RENDER_TARGET_ATTACHMENT_MASK_COLOR_ALL |
				E_RENDER_TARGET_ATTACHMENT_FLAG_DEPTH_STENCIL,
		E_RENDER_TARGET_ATTACHMENT_FLAGS_DEFAULT_C0DS =
				E_RENDER_TARGET_ATTACHMENT_FLAG_COLOR_0_BIT |
				E_RENDER_TARGET_ATTACHMENT_FLAG_DEPTH_STENCIL,
	};

	enum class EBlendFactor : uint8
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

	enum class EBlendOp : uint8
	{
		Add,
		Min,
		Max,
		Subtract,
		SubtractRev
	};

	enum class EBlendRenderTargetWriteMask : uint8
	{
		Zero = 0,
		All = Limits<uint8>::maxValue
	};

	enum class ECompFunc : uint8
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

	enum class ECullMode : uint8
	{
		None,
		Back,
		Front
	};

	enum class EDepthWriteMask : uint8
	{
		Zero = 0,
		All = Limits<uint8>::maxValue
	};

	enum class EPrimitiveFillMode : uint8
	{
		Solid,
		Wireframe
	};

	enum class EPrimitiveTopology : uint8
	{
		Undefined,
		PointList,
		LineList,
		LineListAdj,
		LineStrip,
		LineStripAdj,
		TriangleList,
		TriangleListAdj,
		TriangleStrip,
		TriangleStripAdj,
		TesselationPatch,
	};

	/// @brief
	enum class ERenderTargetAttachmentID : uint8
	{
		RTColor0,
		RTColor1,
		RTColor2,
		RTColor3,
		RTColor4,
		RTColor5,
		RTColor6,
		RTColor7,
		RTDepthStencil,
		RTUndefined
	};

	enum class EResourceUsageState : enum_default_value_t
	{
		Undefined,
		CommonCopySource,
		CommonCopyTarget,
		RTDepthOnlyAttachment,
		RTDepthStencilAttachment,
		RTDepth
	};

	enum class EStencilOp : uint8
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

	enum class ETriangleVerticesOrder : uint8
	{
		Clockwise,
		CounterClockwise
	};

	enum class EShaderInputParameterType : uint8
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

	enum class EShaderInputDescriptorType : uint8
	{
		// CBV/SRV/UAV
		Resource = 1,
		//
		Sampler,
	};

	enum class EShaderInputResourceClass : uint8
	{
		CBV = 1,
		SRV,
		UAV,
		Unknown = 0,
	};

	inline constexpr uint16 ecDeclareShaderInputResourceType( EShaderInputResourceClass pResourceClass, uint8 pIndex )
	{
		return ( ( static_cast<uint16>( pResourceClass ) << 8u ) | pIndex );
	}

	enum class EShaderInputResourceType : uint16
	{
		CBVConstantBuffer = ecDeclareShaderInputResourceType( EShaderInputResourceClass::CBV, 0 ),
		SRVTextureBuffer  = ecDeclareShaderInputResourceType( EShaderInputResourceClass::SRV, 1 ),
		SRVTextureImage   = ecDeclareShaderInputResourceType( EShaderInputResourceClass::SRV, 2 ),
		UAVStorageBuffer  = ecDeclareShaderInputResourceType( EShaderInputResourceClass::UAV, 3 ),
		UAVStorageImage   = ecDeclareShaderInputResourceType( EShaderInputResourceClass::UAV, 4 ),
		Unknown           = 0
	};

	inline constexpr EShaderInputResourceClass ecGetShaderInputResourceResourceClass( EShaderInputResourceType pResourceType )
	{
		return static_cast<EShaderInputResourceClass>( ( static_cast<uint32>( pResourceType ) >> 8u ) & 0xFFu );
	}

	enum class EGraphicsPipelineDescriptorType : uint16
	{
		/// BlendStateDescriptor
		BlendState,

		/// DepthStencilStateDescriptor
		DepthStencilState,

		/// IAVertexFormatDescriptor
		IAVertexFormat,

		/// IAVertexStreamDescriptor
		IAVertexStream,

		/// RasterizerStateDescriptor
		RasterizerState,

		/// RTAttachmentDescriptor
		RTAttachment,

		/// RTLayoutDescriptor
		RTLayout,

		/// GraphicsShaderBindingDescriptor
		ShaderBinding,

		///
		Unknown
	};

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_COMMON_STATE_DEFS_H__
