
#include "GL_pipelineStateObject.h"
#include "GL_commonGraphicsConfig.h"
#include "GL_inputAssembler.h"
#include "GL_graphicsShaderState.h"
#include <ts3/gpuapiGL/GL_gpuDevice.h>
#include <ts3/gpuapiGL/objects/GL_vertexArrayObject.h>
#include <ts3/gpuapiGL/resources/GL_shader.h>
#include <ts3/gpuapi/resources/shader.h>

namespace ts3::gpuapi
{

	GLGraphicsPipelineStateObject::GLGraphicsPipelineStateObject(
			GLGPUDevice & pGPUDevice,
			RenderTargetLayout pRenderTargetLayout,
			ShaderInputSignature pShaderInputSignature,
			const SeparablePSOStateSet & pPSOImmutableStates )
	: GraphicsPipelineStateObjectSeparable(
		pGPUDevice,
		std::move( pRenderTargetLayout ),
		std::move( pShaderInputSignature ),
		pPSOImmutableStates )
	{}

	GLGraphicsPipelineStateObject::~GLGraphicsPipelineStateObject() = default;

	const GLBlendImmutableState & GLGraphicsPipelineStateObject::getBlendState() const noexcept
	{
		ts3DebugAssert( mSeparableStates.blendState );
		return *( mSeparableStates.blendState->queryInterface<GLBlendImmutableState>() );
	}

	const GLDepthStencilImmutableState & GLGraphicsPipelineStateObject::getDepthStencilState() const noexcept
	{
		ts3DebugAssert( mSeparableStates.depthStencilState );
		return *( mSeparableStates.depthStencilState->queryInterface<GLDepthStencilImmutableState>() );
	}

	const GLRasterizerImmutableState & GLGraphicsPipelineStateObject::getRasterizerState() const noexcept
	{
		ts3DebugAssert( mSeparableStates.rasterizerState );
		return *( mSeparableStates.rasterizerState->queryInterface<GLRasterizerImmutableState>() );
	}

	const GLGraphicsShaderLinkageImmutableState & GLGraphicsPipelineStateObject::getGraphicsShaderLinkageState() const noexcept
	{
		ts3DebugAssert( mSeparableStates.shaderLinkageState );
		return *( mSeparableStates.shaderLinkageState->queryInterface<GLGraphicsShaderLinkageImmutableState>() );
	}

	const GLIAInputLayoutImmutableState & GLGraphicsPipelineStateObject::getIAInputLayoutState() const noexcept
	{
		ts3DebugAssert( mSeparableStates.iaInputLayoutState );
		return *( mSeparableStates.iaInputLayoutState->queryInterface<GLIAInputLayoutImmutableState>() );
	}

	GpaHandle<GLGraphicsPipelineStateObject> GLGraphicsPipelineStateObject::create(
			GLGPUDevice & pGPUDevice,
			const GraphicsPipelineStateObjectCreateInfo & pCreateInfo )
	{
		SeparablePSOStateSet separableStates{};
		separableStates.blendState = pCreateInfo.blendState.get();
		separableStates.depthStencilState = pCreateInfo.depthStencilState.get();
		separableStates.rasterizerState = pCreateInfo.rasterizerState.get();
		separableStates.shaderLinkageState = pCreateInfo.shaderLinkageState.get();
		separableStates.iaInputLayoutState = pCreateInfo.inputLayoutState.get();

		const auto & renderTargetLayout = *( pCreateInfo.renderTargetLayout );
		auto & shaderInputSignature = pCreateInfo.shaderInputSignature;

		auto graphicsPSO = createGPUAPIObject<GLGraphicsPipelineStateObject>(
				pGPUDevice,
				renderTargetLayout,
				std::move( shaderInputSignature ),
				separableStates );

		return graphicsPSO;
	}

} // namespace ts3::gpuapi
