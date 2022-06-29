
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

	struct GraphicsPipelineStateObjectCreateInfo;
	struct VertexStreamStateObjectCreateInfo;

	using shader_input_ref_id_t = uint64;
	using shader_input_index_t = uint32;

	using pipeline_input_desc_hash_t = HashObject<HashAlgo::FNV1A>;
	using pipeline_descriptor_id_t = HashObject<HashAlgo::FNV1A>;

	/// @brief
	enum ERenderTargetAttachmentIndex : uint32
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
	enum class ERenderTargetAttachmentID : enum_default_value_t
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

	enum class GraphicsPipelineDescriptorType : enum_default_value_t
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
	};

	struct PipelineDescriptorID
	{
		pipeline_input_desc_hash_t inputDescHash;
	};

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_COMMON_STATE_DEFS_H__
