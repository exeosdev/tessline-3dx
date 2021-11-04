
#include "DX11_pipelineStateController.h"
#include "DX11_pipelineStateObject.h"
#include "DX11_vertexStreamStateObject.h"
#include <ts3/gpuapiDX11/DX11_commandList.h>
#include <ts3/gpuapiDX11/resources/DX11_shader.h>
#include <ts3/gpuapi/resources/shader.h>

namespace ts3
{
namespace gpuapi
{

	DX11GraphicsPipelineStateController::DX11GraphicsPipelineStateController( DX11CommandList & pDX11CommandList,
	                                                                          DX11GraphicsPipelineStateDescriptorCache & pDescriptorCache )
	: SeparableGraphicsPipelineStateController()
	, mDX11CommandList( &pDX11CommandList )
	, _descriptorCache( &pDescriptorCache )
	{}

	DX11GraphicsPipelineStateController::~DX11GraphicsPipelineStateController() = default;

	bool DX11GraphicsPipelineStateController::setGraphicsPipelineStateObject( const GraphicsPipelineStateObject & pGraphicsPipelineSO )
	{
		return SeparableGraphicsPipelineStateController::setGraphicsPipelineStateObject( pGraphicsPipelineSO );
	}

	bool DX11GraphicsPipelineStateController::setVertexStreamStateObject( const VertexStreamStateObject & pVertexStreamSO )
	{
		return SeparableGraphicsPipelineStateController::setVertexStreamStateObject( pVertexStreamSO );
	}

	bool DX11GraphicsPipelineStateController::updatePipelineState()
	{
		if( !SeparableGraphicsPipelineStateController::updatePipelineState() )
		{
			return false;
		}

		if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_COMMON_SO_GRAPHICS_PIPELINE_BIT ) )
		{
			if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_DESCRIPTOR_BLEND_BIT ) )
			{
				auto blendDescriptorID = _csSeparableStateDescriptorSet.blendDescriptorID;
				const auto & blendDescriptor = _descriptorCache->getBlendDescriptor( blendDescriptorID );

				mDX11CommandList->mD3D11DeviceContext1->OMSetBlendState( blendDescriptor.d3d11BlendState.Get(),
				                                                         blendDescriptor.configDesc.ConstantFactor.rgbaArray,
				                                                         0xFFFFFFFF );

				_stateUpdateMask.unset( E_GRAPHICS_STATE_UPDATE_DESCRIPTOR_BLEND_BIT );
			}

			if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_DESCRIPTOR_DEPTH_STENCIL_BIT ) )
			{
				auto depthStencilDescriptorID = _csSeparableStateDescriptorSet.depthStencilDescriptorID;
				const auto & depthStencilDescriptor = _descriptorCache->getDepthStencilDescriptor( depthStencilDescriptorID );

				mDX11CommandList->mD3D11DeviceContext1->OMSetDepthStencilState( depthStencilDescriptor.d3d11DepthStencilState.Get(),
				                                                                depthStencilDescriptor.configDesc.StencilRefValue );

				_stateUpdateMask.unset( E_GRAPHICS_STATE_UPDATE_DESCRIPTOR_DEPTH_STENCIL_BIT );
			}

			if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_DESCRIPTOR_RASTERIZER_BIT ) )
			{
				auto rasterizerDescriptorID = _csSeparableStateDescriptorSet.rasterizerDescriptorID;
				const auto & rasterizerDescriptor = _descriptorCache->getRasterizerDescriptor( rasterizerDescriptorID );

				mDX11CommandList->mD3D11DeviceContext1->RSSetState( rasterizerDescriptor.d3d11RasterizerState.Get() );

				_stateUpdateMask.unset( E_GRAPHICS_STATE_UPDATE_DESCRIPTOR_RASTERIZER_BIT );
			}

			if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_DESCRIPTOR_VERTEX_INPUT_FORMAT_BIT ) )
			{
				auto vertexInputFormatDescriptorID = _csSeparableStateDescriptorSet.vertexInputFormatDescriptorID;
				const auto & vertexInputFormatDescriptor = _descriptorCache->getVertexInputFormatDescriptor( vertexInputFormatDescriptorID );

				mDX11CommandList->mD3D11DeviceContext1->IASetPrimitiveTopology( vertexInputFormatDescriptor.inputFormatDesc.primitiveTopology );
				mDX11CommandList->mD3D11DeviceContext1->IASetInputLayout( vertexInputFormatDescriptor.d3d11InputLayout.Get() );

				_stateUpdateMask.unset( E_GRAPHICS_STATE_UPDATE_DESCRIPTOR_VERTEX_INPUT_FORMAT_BIT );
			}

			if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_SHADER_ANY_STAGE_BIT ) )
			{
				if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_SHADER_VERTEX_STAGE_BIT ) )
				{
					auto * dx11Shader = _csSeparableShaderBinding.vertexShader->queryInterface<DX11Shader>();
					mDX11CommandList->mD3D11DeviceContext1->VSSetShader( dx11Shader->mD3D11VertexShader, nullptr, 0 );
				}
				if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_SHADER_TESS_CONTROL_STAGE_BIT ) )
				{
					auto * dx11Shader = _csSeparableShaderBinding.tessControlShader->queryInterface<DX11Shader>();
					mDX11CommandList->mD3D11DeviceContext1->HSSetShader( dx11Shader->mD3D11HullShader, nullptr, 0 );
				}
				if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_SHADER_TESS_EVALUATION_STAGE_BIT ) )
				{
					auto * dx11Shader = _csSeparableShaderBinding.tessEvaluationShader->queryInterface<DX11Shader>();
					mDX11CommandList->mD3D11DeviceContext1->DSSetShader( dx11Shader->mD3D11DomainShader, nullptr, 0 );
				}
				if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_SHADER_GEOMETRY_STAGE_BIT ) )
				{
					auto * dx11Shader = _csSeparableShaderBinding.geometryShader->queryInterface<DX11Shader>();
					mDX11CommandList->mD3D11DeviceContext1->GSSetShader( dx11Shader->mD3D11GeometryShader, nullptr, 0 );
				}
				if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_SHADER_PIXEL_STAGE_BIT ) )
				{
					auto * dx11Shader = _csSeparableShaderBinding.pixelShader->queryInterface<DX11Shader>();
					mDX11CommandList->mD3D11DeviceContext1->PSSetShader( dx11Shader->mD3D11PixelShader, nullptr, 0 );
				}
				_stateUpdateMask.unset( E_GRAPHICS_STATE_UPDATE_SHADER_ALL_MASK );
			}

			_stateUpdateMask.unset( E_GRAPHICS_STATE_UPDATE_COMMON_SO_GRAPHICS_PIPELINE_BIT );
		}

		if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_COMMON_SO_VERTEX_STREAM_BIT ) )
		{
			auto * openglVertexStreamSO = _csCommonConfig.soVertexStream->queryInterface<DX11VertexStreamStateObject>();

			const auto & indexBufferBinding = openglVertexStreamSO->mDX11VertexDataSourceBinding.indexBufferBinding;
			const auto & vertexBufferBinding = openglVertexStreamSO->mDX11VertexDataSourceBinding.vertexBufferBinding;

			mDX11CommandList->mD3D11DeviceContext1->IASetIndexBuffer( indexBufferBinding.buffer,
			                                                          indexBufferBinding.format,
			                                                          indexBufferBinding.offset );

			for( const auto & vertexStreamRange : openglVertexStreamSO->mDX11VertexDataSourceBinding.vertexStreamActiveRangeList )
			{
				mDX11CommandList->mD3D11DeviceContext1->IASetVertexBuffers( vertexStreamRange.firstIndex,
				                                                            vertexStreamRange.length,
				                                                            &( vertexBufferBinding.bufferArray[vertexStreamRange.firstIndex] ),
				                                                            &( vertexBufferBinding.strideArray[vertexStreamRange.firstIndex] ),
				                                                            &( vertexBufferBinding.offsetArray[vertexStreamRange.firstIndex] ) );
			}

			_stateUpdateMask.unset( E_GRAPHICS_STATE_UPDATE_COMMON_SO_VERTEX_STREAM_BIT );
		}

		return _stateUpdateMask != 0;
	}

} /* namespace ts3 */
} /* namespace gpuapi */
