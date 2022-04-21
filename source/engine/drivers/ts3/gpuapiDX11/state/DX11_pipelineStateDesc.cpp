
#include "DX11_pipelineStateDesc.h"
#include <ts3/gpuapiDX11/DX11_coreAPIProxy.h>
#include <ts3/gpuapiDX11/DX11_gpuDevice.h>
#include <ts3/gpuapiDX11/resources/DX11_gpuBuffer.h>
#include <ts3/gpuapiDX11/resources/DX11_shader.h>
#include <ts3/stdext/memory.h>

namespace ts3::gpuapi
{

	DX11VertexDataSourceBinding createDX11VertexDataSourceBinding( const VertexDataSourceBinding & pBinding )
	{
		DX11VertexDataSourceBinding dx11VertexDataSourceBinding;
		dx11VertexDataSourceBinding.vertexBufferActiveBindingsNum = 0;
		memZero( dx11VertexDataSourceBinding.indexBufferBinding );
		memZero( dx11VertexDataSourceBinding.vertexBufferBinding );

		if( pBinding.indexBufferBinding.bufferObject )
		{
			auto * dx11IBStorage = pBinding.indexBufferBinding.bufferObject->queryInterface<DX11GPUBuffer>();
			auto dx11IBDataFormat = DX11CoreAPIProxy::translateDXBaseDataType( static_cast<EBaseDataType>( pBinding.indexBufferBinding.format ) );
			dx11VertexDataSourceBinding.indexBufferBinding.buffer = dx11IBStorage->mD3D11Buffer.Get();
			dx11VertexDataSourceBinding.indexBufferBinding.offset = trunc_numeric_cast<uint32>( pBinding.indexBufferBinding.dataOffset );
			dx11VertexDataSourceBinding.indexBufferBinding.format = dx11IBDataFormat;
		}

		for( vertex_stream_index_t vertexInputStreamIndex = 0; vertexInputStreamIndex < GPU_SYSTEM_METRIC_IA_MAX_VERTEX_INPUT_STREAMS_NUM; ++vertexInputStreamIndex )
		{
			const auto & vbBindingDesc = pBinding.vertexBufferBindingArray[vertexInputStreamIndex];
			if( vbBindingDesc.bufferObject )
			{
				auto * dx11VBStorage = vbBindingDesc.bufferObject->queryInterface<DX11GPUBuffer>();
				dx11VertexDataSourceBinding.vertexBufferBinding.bufferArray[vertexInputStreamIndex] = dx11VBStorage->mD3D11Buffer.Get();
				dx11VertexDataSourceBinding.vertexBufferBinding.offsetArray[vertexInputStreamIndex] = trunc_numeric_cast<UINT>( vbBindingDesc.dataOffset );
				dx11VertexDataSourceBinding.vertexBufferBinding.strideArray[vertexInputStreamIndex] = trunc_numeric_cast<UINT>( vbBindingDesc.dataStride );
				dx11VertexDataSourceBinding.vertexBufferActiveBindingsNum += 1;
			}
		}

		dx11VertexDataSourceBinding.vertexStreamActiveRangeList = pBinding.vertexStreamActiveRangeList;

		return dx11VertexDataSourceBinding;
	}


	DX11BlendStateDescriptor DX11GraphicsPSDCacheTraits::createBlendDescriptor( const BlendConfigDesc & pConfigDesc,
	                                                                            pipeline_input_desc_hash_t pDescHash,
	                                                                            DX11GPUDevice & pGPUDevice )
	{
		DX11BlendStateDescriptor dx11BSDescriptor;

		dx11BSDescriptor.configDesc.AlphaToCoverageEnable = FALSE;
		dx11BSDescriptor.configDesc.IndependentBlendEnable = FALSE;

		dx11BSDescriptor.configDesc.RenderTarget[0].BlendEnable = ( pConfigDesc.blendState == EBlendState::Enabled ) ? TRUE : FALSE;
		dx11BSDescriptor.configDesc.RenderTarget[0].SrcBlend = DX11CoreAPIProxy::translateDX11BlendFactor( pConfigDesc.factorSrcColor );
		dx11BSDescriptor.configDesc.RenderTarget[0].DestBlend = DX11CoreAPIProxy::translateDX11BlendFactor( pConfigDesc.factorDstColor );
		dx11BSDescriptor.configDesc.RenderTarget[0].BlendOp = DX11CoreAPIProxy::translateDX11BlendOp( pConfigDesc.opColor );
		dx11BSDescriptor.configDesc.RenderTarget[0].SrcBlendAlpha = DX11CoreAPIProxy::translateDX11BlendFactor( pConfigDesc.factorSrcAlpha );
		dx11BSDescriptor.configDesc.RenderTarget[0].DestBlendAlpha = DX11CoreAPIProxy::translateDX11BlendFactor( pConfigDesc.factorDstAlpha );
		dx11BSDescriptor.configDesc.RenderTarget[0].BlendOpAlpha = DX11CoreAPIProxy::translateDX11BlendOp( pConfigDesc.opAlpha );
		dx11BSDescriptor.configDesc.RenderTarget[0].RenderTargetWriteMask = DX11CoreAPIProxy::translateDX11BlendRenderTargetWriteMask( pConfigDesc.renderTargetWriteMask );
		dx11BSDescriptor.configDesc.ConstantFactor = pConfigDesc.constantFactor;

		auto hResult = pGPUDevice.mD3D11Device1->CreateBlendState( &( dx11BSDescriptor.configDesc ), dx11BSDescriptor.d3d11BlendState.GetAddressOf() );
		if( FAILED( hResult ) )
		{
			ts3DebugInterrupt();
			return nullptr;
		}

		dx11BSDescriptor.inputDescHash = pDescHash;
		dx11BSDescriptor.descriptorID = computePipelineStateDescriptorID( dx11BSDescriptor.configDesc );

		return dx11BSDescriptor;
	}

	DX11DepthStencilStateDescriptor DX11GraphicsPSDCacheTraits::createDepthStencilDescriptor( const DepthStencilConfigDesc & pConfigDesc,
	                                                                                          pipeline_input_desc_hash_t pDescHash,
	                                                                                          DX11GPUDevice & pGPUDevice )
	{
		DX11DepthStencilStateDescriptor dx11DSSDescriptor;

		dx11DSSDescriptor.configDesc.DepthEnable = ( pConfigDesc.depthTestState == EDepthTestState::Enabled ) ? TRUE : FALSE;
		auto & depthSettings = pConfigDesc.depthSettings;
		dx11DSSDescriptor.configDesc.DepthFunc = DX11CoreAPIProxy::translateDX11CompFunc( depthSettings.depthCompFunc );
		dx11DSSDescriptor.configDesc.DepthWriteMask = DX11CoreAPIProxy::translateDX11DepthWriteMask( depthSettings.depthWriteMask );

		dx11DSSDescriptor.configDesc.StencilEnable = ( pConfigDesc.stencilTestState == EStencilTestState::Enabled ) ? TRUE : FALSE;
		dx11DSSDescriptor.configDesc.StencilReadMask = pConfigDesc.stencilSettings.readMask;
		dx11DSSDescriptor.configDesc.StencilWriteMask = pConfigDesc.stencilSettings.writeMask;
		dx11DSSDescriptor.configDesc.StencilRefValue = pConfigDesc.stencilSettings.refValue;
		auto & stencilBackFaceDesc = pConfigDesc.stencilSettings.backFace;
		dx11DSSDescriptor.configDesc.BackFace.StencilFunc = DX11CoreAPIProxy::translateDX11CompFunc( stencilBackFaceDesc.compFunc );
		dx11DSSDescriptor.configDesc.BackFace.StencilFailOp = DX11CoreAPIProxy::translateDX11StencilOp( stencilBackFaceDesc.opFail );
		dx11DSSDescriptor.configDesc.BackFace.StencilDepthFailOp = DX11CoreAPIProxy::translateDX11StencilOp( stencilBackFaceDesc.opPassDepthFail );
		dx11DSSDescriptor.configDesc.BackFace.StencilPassOp = DX11CoreAPIProxy::translateDX11StencilOp( stencilBackFaceDesc.opPassDepthPass );
		auto & stencilFrontFaceDesc = pConfigDesc.stencilSettings.frontFace;
		dx11DSSDescriptor.configDesc.FrontFace.StencilFunc = DX11CoreAPIProxy::translateDX11CompFunc( stencilFrontFaceDesc.compFunc );
		dx11DSSDescriptor.configDesc.FrontFace.StencilFailOp = DX11CoreAPIProxy::translateDX11StencilOp( stencilFrontFaceDesc.opFail );
		dx11DSSDescriptor.configDesc.FrontFace.StencilDepthFailOp = DX11CoreAPIProxy::translateDX11StencilOp( stencilFrontFaceDesc.opPassDepthFail );
		dx11DSSDescriptor.configDesc.FrontFace.StencilPassOp = DX11CoreAPIProxy::translateDX11StencilOp( stencilFrontFaceDesc.opPassDepthPass );

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

		dx11RSDescriptor.configDesc.CullMode = DX11CoreAPIProxy::translateDX11CullMode( pConfigDesc.cullMode );
		dx11RSDescriptor.configDesc.FillMode = DX11CoreAPIProxy::translateDX11PrimitiveFillMode( pConfigDesc.primitiveFillMode );
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
		for( uint32 attributeIndex = 0; attributeIndex < GPU_SYSTEM_METRIC_IA_MAX_VERTEX_ATTRIBUTES_NUM; ++attributeIndex )
		{
			if( const auto & vertexAttributeDesc = pInputFormatDesc.vertexAttributeArray[attributeIndex] )
			{
				auto attributeRelativeOffset = vertexAttributeDesc.relativeOffset;
				if( attributeRelativeOffset == cxVertexAttributeOffsetPackedAppend )
				{
					attributeRelativeOffset = D3D11_APPEND_ALIGNED_ELEMENT;
				}

				auto & dx11VertexAttribute = dx11VIFDescriptor.inputFormatDesc.vertexAttributeArray[vertexAttributeDesc.attributeIndex];
				dx11VertexAttribute.AlignedByteOffset = trunc_numeric_cast<UINT>( attributeRelativeOffset );
				dx11VertexAttribute.SemanticName = vertexAttributeDesc.semanticName;
				dx11VertexAttribute.SemanticIndex = vertexAttributeDesc.semanticIndex;
				dx11VertexAttribute.Format = DX11CoreAPIProxy::translateDXVertexAttribFormat( vertexAttributeDesc.format );
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

		dx11VIFDescriptor.inputFormatDesc.primitiveTopology = DX11CoreAPIProxy::translateDX11PrimitiveTopology( pInputFormatDesc.primitiveTopology );
		dx11VIFDescriptor.inputFormatDesc.activeVertexAttributesNum = activeVertexAttributesNum;
		dx11VIFDescriptor.inputDescHash = pDescHash;
		dx11VIFDescriptor.descriptorID = computePipelineStateDescriptorID( dx11VIFDescriptor.inputFormatDesc );

		return dx11VIFDescriptor;
	}

} // namespace ts3::gpuapi
