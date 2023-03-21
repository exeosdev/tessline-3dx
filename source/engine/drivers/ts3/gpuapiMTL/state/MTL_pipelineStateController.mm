
#include "MTL_pipelineStateController.h"

namespace ts3::gpuapi
{

	MTLGraphicsPipelineStateController::MTLGraphicsPipelineStateController( MetalCommandList & pMetalCommandList )
	: mMetalCommandList( &pMetalCommandList )
	{}

	MTLGraphicsPipelineStateController::~MTLGraphicsPipelineStateController() = default;

	bool MTLGraphicsPipelineStateController::applyStateChanges()
	{}

	bool MTLGraphicsPipelineStateController::setGraphicsPipelineStateObject( const GraphicsPipelineStateObject & pGraphicsPSO )
	{}

	bool MTLGraphicsPipelineStateController::resetGraphicsPipelineStateObject()
	{}

	bool MTLGraphicsPipelineStateController::setIAVertexStreamState( const IAVertexStreamDynamicState & pIAVertexStreamState )
	{}

	bool MTLGraphicsPipelineStateController::setIAVertexStreamState( const IAVertexStreamImmutableState & pIAVertexStreamState )
	{}

	bool MTLGraphicsPipelineStateController::resetIAVertexStreamState()
	{}

	bool MTLGraphicsPipelineStateController::setRenderTargetBindingState( const RenderTargetBindingDynamicState & pRenderTargetBindingState )
	{}

	bool MTLGraphicsPipelineStateController::setRenderTargetBindingState( const RenderTargetBindingImmutableState & pRenderTargetBindingState )
	{}

	bool MTLGraphicsPipelineStateController::resetRenderTargetBindingState()
	{}

	bool MTLGraphicsPipelineStateController::setViewport( const ViewportDesc & pViewportDesc )
	{}

	bool MTLGraphicsPipelineStateController::setShaderConstant( shader_input_ref_id_t pParamRefID, const void * pData )
	{}

	bool MTLGraphicsPipelineStateController::setShaderConstantBuffer( shader_input_ref_id_t pParamRefID, GPUBuffer & pConstantBuffer )
	{}

	bool MTLGraphicsPipelineStateController::setShaderTextureImage( shader_input_ref_id_t pParamRefID, Texture & pTexture )
	{}

	bool MTLGraphicsPipelineStateController::setShaderTextureSampler( shader_input_ref_id_t pParamRefID, Sampler & pSampler )
	{}
	
}
