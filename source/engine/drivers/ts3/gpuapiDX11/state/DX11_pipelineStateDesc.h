
#pragma once

#ifndef __TS3DRIVER_GPUAPI_DX11_PIPELINE_STATE_DESC_H__
#define __TS3DRIVER_GPUAPI_DX11_PIPELINE_STATE_DESC_H__

#include "../DX11_prerequisites.h"
#include <ts3/gpuapi/state/pipelineStateDesc.h>
#include <ts3/gpuapi/state/vertexDataSourceDesc.h>
#include <ts3/gpuapi/state/graphicsPipelineStateDescriptorCache.h>

namespace ts3::gpuapi
{

	class DX11GraphicsPipelineStateObject;
	class DX11VertexStreamStateObject;

	struct DX11BlendConfigDesc : public D3D11_BLEND_DESC
	{
		math::RGBAColorR32Norm ConstantFactor;
	};

	struct DX11DepthStencilConfigDesc : public D3D11_DEPTH_STENCIL_DESC
	{
		UINT8 StencilRefValue;
	};

	struct DX11RasterizerConfigDesc : public D3D11_RASTERIZER_DESC
	{
	};

	struct DX11VertexInputFormatDesc
	{
		using DX11VertexAttribute = D3D11_INPUT_ELEMENT_DESC;
		using DX11VertexAttributeArray = std::array<DX11VertexAttribute, E_GPU_SYSTEM_METRIC_IA_MAX_VERTEX_ATTRIBUTES_NUM>;
		D3D11_PRIMITIVE_TOPOLOGY primitiveTopology;
		uint32 activeVertexAttributesNum;
		DX11VertexAttributeArray vertexAttributeArray;
	};

	// Common data for all OpenDX11 state descriptors.
	// State descriptor contains a set of related pipeline state
	// configuration values, which can be treated as a single state.
	struct DX11StateDescriptorCommonData
	{
		// Hash of the input Desc structure used to create this descriptor.
		// For instance, DX11BlendStateDescriptor contains a hash of the
		// BlendConfigDesc structure passed as a definition of the blend.
		pipeline_input_desc_hash_t inputDescHash;
		// ID of the descriptor itself. Internally it is a 64-bit hash value.
		pipeline_state_descriptor_id_t descriptorID;
	};

	struct DX11BlendStateDescriptor : public DX11StateDescriptorCommonData
	{
		DX11BlendConfigDesc configDesc;
		ComPtr<ID3D11BlendState> d3d11BlendState;

		DX11BlendStateDescriptor() = default;

		DX11BlendStateDescriptor( std::nullptr_t )
		{}
	};

	struct DX11DepthStencilStateDescriptor : public DX11StateDescriptorCommonData
	{
		DX11DepthStencilConfigDesc configDesc;
		ComPtr<ID3D11DepthStencilState> d3d11DepthStencilState;

		DX11DepthStencilStateDescriptor() = default;

		DX11DepthStencilStateDescriptor( std::nullptr_t )
		{}
	};

	struct DX11RasterizerStateDescriptor : public DX11StateDescriptorCommonData
	{
		DX11RasterizerConfigDesc configDesc;
		ComPtr<ID3D11RasterizerState> d3d11RasterizerState;

		DX11RasterizerStateDescriptor() = default;

		DX11RasterizerStateDescriptor( std::nullptr_t )
		{}
	};

	struct DX11VertexInputFormatStateDescriptor : public DX11StateDescriptorCommonData
	{
		DX11VertexInputFormatDesc inputFormatDesc;
		ComPtr<ID3D11InputLayout> d3d11InputLayout;

		DX11VertexInputFormatStateDescriptor() = default;

		DX11VertexInputFormatStateDescriptor( std::nullptr_t )
		{}
	};

	struct DX11VertexDataSourceBinding
	{
		struct DX11IndexBufferBinding
		{
			ID3D11Buffer * buffer;
			uint32 offset;
			DXGI_FORMAT format;
		};
		struct DX11VertexBufferBinding
		{
			ID3D11Buffer * bufferArray[E_GPU_SYSTEM_METRIC_IA_MAX_VERTEX_INPUT_STREAMS_NUM];
			UINT           offsetArray[E_GPU_SYSTEM_METRIC_IA_MAX_VERTEX_INPUT_STREAMS_NUM];
			UINT           strideArray[E_GPU_SYSTEM_METRIC_IA_MAX_VERTEX_INPUT_STREAMS_NUM];
		};
		using DX11VertexStreamRange = VertexDataSourceBinding::VertexStreamRange;
		using DX11VertexStreamRangeList = VertexDataSourceBinding::VertexStreamRangeList;
		DX11IndexBufferBinding indexBufferBinding;
		uint32 vertexBufferActiveBindingsNum;
		DX11VertexBufferBinding vertexBufferBinding;
		DX11VertexStreamRangeList vertexStreamActiveRangeList;

		DX11VertexDataSourceBinding() = default;

		DX11VertexDataSourceBinding( const InitEmptyTag & )
		: vertexBufferActiveBindingsNum( 0 )
		{ }

		explicit operator bool() const
		{
			return ( vertexBufferActiveBindingsNum > 0 ) && !vertexStreamActiveRangeList.empty();
		}
	};

	DX11VertexDataSourceBinding createDX11VertexDataSourceBinding( const VertexDataSourceBinding & pBinding );

	struct DX11GraphicsPSDCacheTraits
	{
		using BlendDescriptorType = DX11BlendStateDescriptor;
		using DepthStencilDescriptorType = DX11DepthStencilStateDescriptor;
		using RasterizerDescriptorType = DX11RasterizerStateDescriptor;
		using VertexInputFormatDescriptorType = DX11VertexInputFormatStateDescriptor;

		static DX11BlendStateDescriptor createBlendDescriptor( const BlendConfigDesc & pConfigDesc,
		                                                       pipeline_input_desc_hash_t pDescHash,
		                                                       DX11GPUDevice & pGPUDevice );

		static DX11DepthStencilStateDescriptor createDepthStencilDescriptor( const DepthStencilConfigDesc & pConfigDesc,
		                                                                     pipeline_input_desc_hash_t pDescHash,
		                                                                     DX11GPUDevice & pGPUDevice );

		static DX11RasterizerStateDescriptor createRasterizerDescriptor( const RasterizerConfigDesc & pConfigDesc,
		                                                                 pipeline_input_desc_hash_t pDescHash,
		                                                                 DX11GPUDevice & pGPUDevice );

		static DX11VertexInputFormatStateDescriptor createVertexInputFormatDescriptor( const VertexInputFormatDesc & pInputFormatDesc,
		                                                                               pipeline_input_desc_hash_t pDescHash,
		                                                                               DX11GPUDevice & pGPUDevice,
		                                                                               Shader & pVertexShader );
	};

	using DX11GraphicsPipelineStateDescriptorCache = GraphicsPipelineStateDescriptorCache<DX11GraphicsPSDCacheTraits>;

} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_DX11_PIPELINE_STATE_DESC_H__
