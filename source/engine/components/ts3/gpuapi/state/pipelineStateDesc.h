
#pragma once

#ifndef __TS3_GPUAPI_PIPELINE_STATE_DESC_COMMON_H__
#define __TS3_GPUAPI_PIPELINE_STATE_DESC_COMMON_H__

#include "commonGPUStateDefs.h"
#include <ts3/stdext/hash.h>

namespace ts3::gpuapi
{

	struct ComputePipelineStateObjectCreateInfo;
	struct GraphicsPipelineStateObjectCreateInfo;
	struct VertexStreamStateObjectCreateInfo;

	ts3DeclareClassHandle( ComputePipelineStateObject );
	ts3DeclareClassHandle( GraphicsPipelineStateObject );
	ts3DeclareClassHandle( SeparableGraphicsPipelineStateObject );
	ts3DeclareClassHandle( VertexStreamStateObject );

	ts3DeclareClassHandle( Sampler );
	ts3DeclareClassHandle( Shader );

	using pipeline_input_desc_hash_t = ts3::HashObject<ts3::HashAlgo::FNV1A>;
	using pipeline_state_descriptor_id_t = ts3::HashObject<ts3::HashAlgo::FNV1A>;

	inline constexpr pipeline_state_descriptor_id_t cxInvalidPipelineStateDescriptorID = ts3::cvEmptyHash<ts3::HashAlgo::FNV1A>;

	inline constexpr gpu_memory_offset_t cxVertexAttributeOffsetPackedAppend = ts3::Limits<gpu_memory_offset_t>::maxValue;

	struct GraphicsPipelineStateDescriptorSet
	{
		pipeline_state_descriptor_id_t blendDescriptorID;
		pipeline_state_descriptor_id_t depthStencilDescriptorID;
		pipeline_state_descriptor_id_t rasterizerDescriptorID;
		pipeline_state_descriptor_id_t vertexInputFormatDescriptorID;
	};

	struct BlendConfigDesc
	{
		EBlendState blendState;
		math::RGBAColorU8 constantFactor;
		EBlendFactor factorSrcColor;
		EBlendFactor factorSrcAlpha;
		EBlendFactor factorDstColor;
		EBlendFactor factorDstAlpha;
		EBlendOp opColor;
		EBlendOp opAlpha;
		EBlendRenderTargetWriteMask renderTargetWriteMask;
	};

	struct DepthStencilConfigDesc
	{
		struct DepthSettings
		{
			ECompFunc depthCompFunc;
			EDepthWriteMask depthWriteMask;
		};
		struct StencilFaceOp
		{
			ECompFunc compFunc;
			EStencilOp   opFail;
			EStencilOp   opPassDepthFail;
			EStencilOp   opPassDepthPass;
		};
		struct StencilSettings
		{
			StencilFaceOp frontFace;
			StencilFaceOp backFace;
			uint8 refValue;
			uint8 readMask;
			uint8 writeMask;
		};
		EDepthTestState depthTestState;
		DepthSettings depthSettings;
		EStencilTestState stencilTestState;
		StencilSettings stencilSettings;
	};

	struct RasterizerConfigDesc
	{
		ECullMode cullMode;
		EPrimitiveFillMode primitiveFillMode;
		ETriangleVerticesOrder triangleFrontFaceOrder;
		EScissorTestState scissorTestState;
	};

	struct VertexInputFormatDesc
	{
		struct VertexAttribute
		{
			vertex_attribute_index_t attributeIndex = cxInvalidVertexAttributeIndex;
			vertex_stream_index_t streamIndex;
			const char * semanticName;
			uint32 semanticIndex;
			EVertexAttribFormat format;
			gpu_memory_offset_t relativeOffset;
			uint32 instanceRate = 0;
			constexpr explicit operator bool() const
			{
				return attributeIndex != cxInvalidVertexAttributeIndex;
			}
		};
		using VertexAttributeArray = std::array<VertexAttribute, GPU_SYSTEM_METRIC_IA_MAX_VERTEX_ATTRIBUTES_NUM>;
		VertexAttributeArray vertexAttributeArray;
		EPrimitiveTopology primitiveTopology;
	};

#define ts3gaVertexAttributeDesc( pAttributeIndex, pStreamIndex, pSemanticName, pSemanticIndex, pAttribFormat, pRelativeOffset, pInstanceRate ) \
	ts3::gpuapi::VertexInputFormatDesc::VertexAttribute{ pAttributeIndex, pStreamIndex, pSemanticName, pSemanticIndex, pAttribFormat, pRelativeOffset, pInstanceRate }


	TS3_GPUAPI_OBJ const BlendConfigDesc cvPipelineDescBlendConfigDisabled;
	TS3_GPUAPI_OBJ const BlendConfigDesc cvPipelineDescBlendConfigEnabledAdd;
	TS3_GPUAPI_OBJ const DepthStencilConfigDesc cvPipelineDescDepthStencilConfigDisabled;
	TS3_GPUAPI_OBJ const DepthStencilConfigDesc cvPipelineDescDepthStencilConfigEnabledDepthOnly;
	TS3_GPUAPI_OBJ const RasterizerConfigDesc cvPipelineDescRasterizerConfigDefault;
	TS3_GPUAPI_OBJ const VertexInputFormatDesc cvPipelineDescVertexInputFormatEmpty;
	TS3_GPUAPI_OBJ const VertexInputFormatDesc cvPipelineDescVertexInputFormatVp3;
	TS3_GPUAPI_OBJ const VertexInputFormatDesc cvPipelineDescVertexInputFormatVp3Vc4;

	template <typename TpInputDescData>
	inline pipeline_input_desc_hash_t computePipelineInputDescHash( const TpInputDescData & pInputDescData )
	{
		return ts3::HashGen::compute<ts3::HashAlgo::FNV1A>( pInputDescData );
	}

	template <typename TpStateDescriptorData>
	inline pipeline_state_descriptor_id_t computePipelineStateDescriptorID( const TpStateDescriptorData & pStateDescriptorData )
	{
		return ts3::HashGen::compute<ts3::HashAlgo::FNV1A>( pStateDescriptorData );
	}

}

#endif // __TS3_GPUAPI_PIPELINE_STATE_DESC_COMMON_H__
