
#include "DX11_pipelineStateDesc.h"
#include <ts3/gpuapiDX11/DX11_apiTranslationLayer.h>
#include <ts3/gpuapiDX11/DX11_gpuDevice.h>
#include <ts3/gpuapiDX11/resources/DX11_gpuBuffer.h>
#include <ts3/gpuapiDX11/resources/DX11_shader.h>
#include <ts3/stdext/memory.h>

namespace ts3::gpuapi
{

	DX11VertexDataSourceBinding createDX11VertexDataSourceBinding( const VertexDataSourceBinding & pBinding )
	{
		DX11VertexDataSourceBinding dx11VertexDataSourceBinding;
		dx11VertexDataSourceBinding.vertexBufferActiveIndicesNum = 0;
		memZero( dx11VertexDataSourceBinding.indexBufferBinding );
		memZero( dx11VertexDataSourceBinding.vertexBufferBinding );

		if( pBinding.indexBufferBinding.bufferObject )
		{
			auto * dx11IBStorage = pBinding.indexBufferBinding.bufferObject->queryInterface<DX11GPUBuffer>();
			auto dx11IBDataFormat = atl::translateDXBaseDataType( static_cast<EBaseDataType>( pBinding.indexBufferBinding.format ) );
			dx11VertexDataSourceBinding.indexBufferBinding.buffer = dx11IBStorage->mD3D11Buffer.Get();
			dx11VertexDataSourceBinding.indexBufferBinding.offset = numeric_cast<uint32>( pBinding.indexBufferBinding.dataOffset );
			dx11VertexDataSourceBinding.indexBufferBinding.format = dx11IBDataFormat;
		}

		for( vertex_stream_index_t vertexInputStreamIndex = 0; vertexInputStreamIndex < E_GPU_SYSTEM_METRIC_IA_MAX_VERTEX_STREAMS_NUM; ++vertexInputStreamIndex )
		{
			const auto & vbBindingDesc = pBinding.vertexBufferBindingArray[vertexInputStreamIndex];
			if( vbBindingDesc.bufferObject )
			{
				auto * dx11VBStorage = vbBindingDesc.bufferObject->queryInterface<DX11GPUBuffer>();
				dx11VertexDataSourceBinding.vertexBufferBinding.bufferArray[vertexInputStreamIndex] = dx11VBStorage->mD3D11Buffer.Get();
				dx11VertexDataSourceBinding.vertexBufferBinding.offsetArray[vertexInputStreamIndex] = numeric_cast<UINT>( vbBindingDesc.dataOffset );
				dx11VertexDataSourceBinding.vertexBufferBinding.strideArray[vertexInputStreamIndex] = numeric_cast<UINT>( vbBindingDesc.dataStride );
				dx11VertexDataSourceBinding.vertexBufferActiveIndicesNum += 1;
			}
		}

		dx11VertexDataSourceBinding.vertexStreamActiveRangeList = pBinding.vertexStreamActiveRangeList;

		return dx11VertexDataSourceBinding;
	}


	DX11BlendStateDescriptor DX11GraphicsPSDCacheTraits::createBlendDescriptor( const BlendConfigDesc & pConfigDesc,
	                                                                            pipeline_input_desc_hash_t pDescHash,
	                                                                            DX11GPUDevice & pGPUDevice )
	{
	}

	DX11DepthStencilStateDescriptor DX11GraphicsPSDCacheTraits::createDepthStencilDescriptor( const DepthStencilConfigDesc & pConfigDesc,
	                                                                                          pipeline_input_desc_hash_t pDescHash,
	                                                                                          DX11GPUDevice & pGPUDevice )
	{
		DX11DepthStencilStateDescriptor dx11DSSDescriptor;

		dx11DSSDescriptor.configDesc.DepthEnable = ( pConfigDesc.depthTestState == EDepthTestState::Enabled ) ? TRUE : FALSE;
		auto & depthSettings = pConfigDesc.depthSettings;
		dx11DSSDescriptor.configDesc.DepthFunc = atl::translateDX11CompFunc( depthSettings.depthCompFunc );
		dx11DSSDescriptor.configDesc.DepthWriteMask = atl::translateDX11DepthWriteMask( depthSettings.depthWriteMask );

		dx11DSSDescriptor.configDesc.StencilEnable = ( pConfigDesc.stencilTestState == EStencilTestState::Enabled ) ? TRUE : FALSE;
		dx11DSSDescriptor.configDesc.StencilReadMask = pConfigDesc.stencilSettings.readMask;
		dx11DSSDescriptor.configDesc.StencilWriteMask = pConfigDesc.stencilSettings.writeMask;
		dx11DSSDescriptor.configDesc.StencilRefValue = pConfigDesc.stencilSettings.refValue;
		auto & stencilBackFaceDesc = pConfigDesc.stencilSettings.backFace;
		dx11DSSDescriptor.configDesc.BackFace.StencilFunc = atl::translateDX11CompFunc( stencilBackFaceDesc.compFunc );
		dx11DSSDescriptor.configDesc.BackFace.StencilFailOp = atl::translateDX11StencilOp( stencilBackFaceDesc.opFail );
		dx11DSSDescriptor.configDesc.BackFace.StencilDepthFailOp = atl::translateDX11StencilOp( stencilBackFaceDesc.opPassDepthFail );
		dx11DSSDescriptor.configDesc.BackFace.StencilPassOp = atl::translateDX11StencilOp( stencilBackFaceDesc.opPassDepthPass );
		auto & stencilFrontFaceDesc = pConfigDesc.stencilSettings.frontFace;
		dx11DSSDescriptor.configDesc.FrontFace.StencilFunc = atl::translateDX11CompFunc( stencilFrontFaceDesc.compFunc );
		dx11DSSDescriptor.configDesc.FrontFace.StencilFailOp = atl::translateDX11StencilOp( stencilFrontFaceDesc.opFail );
		dx11DSSDescriptor.configDesc.FrontFace.StencilDepthFailOp = atl::translateDX11StencilOp( stencilFrontFaceDesc.opPassDepthFail );
		dx11DSSDescriptor.configDesc.FrontFace.StencilPassOp = atl::translateDX11StencilOp( stencilFrontFaceDesc.opPassDepthPass );

		auto hResult = pGPUDevice.mD3D11Device1->CreateDepthStencilState( &( dx11DSSDescriptor.configDesc ), dx11DSSDescriptor.d3d11DepthStencilState.GetAddressOf() );
		if( FAILED( hResult ) )
		{
			ts3DebugInterrupt();
			return nullptr;
		}

		dx11DSSDescriptor.inputDescHash = pDescHash;
		dx11DSSDescriptor.descriptorID = computePipelineStateDescriptorID( dx11DSSDescriptor.configDesc );

		return dx11DSSDescriptor;
	}

	DX11RasterizerStateDescriptor DX11GraphicsPSDCacheTraits::createRasterizerDescriptor( const RasterizerConfigDesc & pConfigDesc,
	                                                                                      pipeline_input_desc_hash_t pDescHash,
	                                                                                      DX11GPUDevice & pGPUDevice )
	{
		DX11RasterizerStateDescriptor dx11RSDescriptor;

		dx11RSDescriptor.configDesc.CullMode = atl::translateDX11CullMode( pConfigDesc.cullMode );
		dx11RSDescriptor.configDesc.FillMode = atl::translateDX11PrimitiveFillMode( pConfigDesc.primitiveFillMode );
		dx11RSDescriptor.configDesc.DepthClipEnable = TRUE;
		dx11RSDescriptor.configDesc.MultisampleEnable = FALSE;
		dx11RSDescriptor.configDesc.FrontCounterClockwise = FALSE;
		dx11RSDescriptor.configDesc.ScissorEnable = FALSE;
		dx11RSDescriptor.configDesc.DepthBias = 0;
		dx11RSDescriptor.configDesc.DepthBiasClamp = 0.0f;
		dx11RSDescriptor.configDesc.SlopeScaledDepthBias = 0.0f;

		if( pConfigDesc.triangleFrontFaceOrder == ETriangleVerticesOrder::CounterClockwise )
		{
			dx11RSDescriptor.configDesc.FrontCounterClockwise = TRUE;
		}
		if( pConfigDesc.scissorTestState == EStencilTestState::Enabled )
		{
			dx11RSDescriptor.configDesc.ScissorEnable = TRUE;
		}

		auto hResult = pGPUDevice.mD3D11Device1->CreateRasterizerState( &( dx11RSDescriptor.configDesc ), dx11RSDescriptor.d3d11RasterizerState.GetAddressOf() );
		if( FAILED( hResult ) )
		{
			ts3DebugInterrupt();
			return nullptr;
		}

		dx11RSDescriptor.inputDescHash = pDescHash;
		dx11RSDescriptor.descriptorID = computePipelineStateDescriptorID( dx11RSDescriptor.configDesc );

		return dx11RSDescriptor;
	}

	DX11VertexInputFormatStateDescriptor DX11GraphicsPSDCacheTraits::createVertexInputFormatDescriptor( const VertexInputFormatDesc & pInputFormatDesc,
	                                                                                                    pipeline_input_desc_hash_t pDescHash,
	                                                                                                    DX11GPUDevice & pGPUDevice,
	                                                                                                    Shader & pVertexShader )
	{
		DX11VertexInputFormatStateDescriptor dx11VIFDescriptor;

		uint32 activeVertexAttributesNum = 0;
		for( uint32 attributeIndex = 0; attributeIndex < E_GPU_SYSTEM_METRIC_IA_MAX_VERTEX_ATTRIBUTES_NUM; ++attributeIndex )
		{
			if( const auto & vertexAttributeDesc = pInputFormatDesc.vertexAttributeArray[attributeIndex] )
			{
				auto attributeRelativeOffset = vertexAttributeDesc.relativeOffset;
				if( attributeRelativeOffset == CX_VERTEX_ATTRIBUTE_OFFSET_APPEND )
				{
					attributeRelativeOffset = D3D11_APPEND_ALIGNED_ELEMENT;
				}

				auto & dx11VertexAttribute = dx11VIFDescriptor.inputFormatDesc.vertexAttributeArray[vertexAttributeDesc.attributeIndex];
				dx11VertexAttribute.AlignedByteOffset = numeric_cast<UINT>( attributeRelativeOffset );
				dx11VertexAttribute.SemanticName = vertexAttributeDesc.semanticName;
				dx11VertexAttribute.SemanticIndex = vertexAttributeDesc.semanticIndex;
				dx11VertexAttribute.Format = atl::translateDXVertexAttribFormat( vertexAttributeDesc.format );
				dx11VertexAttribute.InputSlot = vertexAttributeDesc.streamIndex;
				dx11VertexAttribute.InputSlotClass = ( vertexAttributeDesc.instanceRate == 0 ) ? D3D11_INPUT_PER_VERTEX_DATA : D3D11_INPUT_PER_INSTANCE_DATA;
				dx11VertexAttribute.InstanceDataStepRate = vertexAttributeDesc.instanceRate;

				++activeVertexAttributesNum;
			}
		}

		const auto & vertexShaderBinary = pVertexShader.mShaderBinary;

		auto hResult = pGPUDevice.mD3D11Device1->CreateInputLayout( dx11VIFDescriptor.inputFormatDesc.vertexAttributeArray.data(),
		                                                            activeVertexAttributesNum,
		                                                            vertexShaderBinary.rawBuffer.data(),
		                                                            vertexShaderBinary.rawBufferSize,
		                                                            dx11VIFDescriptor.d3d11InputLayout.GetAddressOf() );
		if( FAILED( hResult ) )
		{
			ts3DebugInterrupt();
			return nullptr;
		}

		dx11VIFDescriptor.inputFormatDesc.primitiveTopology = atl::translateDX11PrimitiveTopology( pInputFormatDesc.primitiveTopology );
		dx11VIFDescriptor.inputFormatDesc.activeVertexAttributesNum = activeVertexAttributesNum;
		dx11VIFDescriptor.inputDescHash = pDescHash;
		dx11VIFDescriptor.descriptorID = computePipelineStateDescriptorID( dx11VIFDescriptor.inputFormatDesc );

		return dx11VIFDescriptor;
	}

} // namespace ts3::gpuapi
