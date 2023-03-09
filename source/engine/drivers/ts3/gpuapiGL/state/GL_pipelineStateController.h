
#pragma once

#ifndef __TS3DRIVER_GPUAPI_GLCOMMON_PIPELINE_STATE_CONTROLLER_H__
#define __TS3DRIVER_GPUAPI_GLCOMMON_PIPELINE_STATE_CONTROLLER_H__

#include "GL_inputAssembler.h"
#include "GL_renderTarget.h"
#include <ts3/gpuapi/state/separablePipelineState.h>

namespace ts3::gpuapi
{

	struct GLIAIndexBufferBinding;
	struct GLIAVertexBuffersBindings;

	/// @brief
	class GLGraphicsPipelineStateController : public GraphicsPipelineStateControllerSeparable
	{
		friend class GLCommandList;

	public:
		GLGraphicsPipelineStateController();
		~GLGraphicsPipelineStateController();

		TS3_ATTR_NO_DISCARD const GLDrawTopologyProperties & getGLDrawTopologyProperties() const noexcept;

		TS3_ATTR_NO_DISCARD const GLIAVertexStreamDefinition & getCurrentIAVertexStreamDefinition() const noexcept;

		TS3_ATTR_NO_DISCARD GLRenderTargetBindingInfo getCurrentRenderTargetBindingInfo() const noexcept;

		virtual void applyPipelineStateChanges() override final;

		virtual bool setGraphicsPipelineStateObject( const GraphicsPipelineStateObject & pGraphicsPSO ) override;
		virtual bool resetGraphicsPipelineStateObject() override;

		virtual bool setIAVertexStreamState( const IAVertexStreamDynamicState & pIAVertexStreamState ) override;
		virtual bool setIAVertexStreamState( const IAVertexStreamImmutableState & pIAVertexStreamState ) override;
		virtual bool resetIAVertexStreamState() override;

		virtual bool setRenderTargetBindingState( const RenderTargetBindingDynamicState & pRenderTargetBindingState ) override;
		virtual bool setRenderTargetBindingState( const RenderTargetBindingImmutableState & pRenderTargetBindingState ) override;
		virtual bool resetRenderTargetBindingState() override;

	private:
		void resetDynamicIAVertexStreamState();

		void resetDynamicRenderTargetBindingState();

		// Apply functions: PSO States

		static void applyGLBlendState( const GLBlendImmutableState & pBlendState );
		static void applyGLDepthStencilState( const GLDepthStencilImmutableState & pDepthStencilState );
		static void applyGLRasterizerState( const GLRasterizerImmutableState & pRasterizerState );
		static void applyGLShaderLinkageState( const GLGraphicsShaderLinkageImmutableState & pShaderLinkageState );
		static void applyGLIAInputLayoutState( const GLIAInputLayoutImmutableState & pInputLayoutState, GLDrawTopologyProperties & pDrawTopologyProperties );

		// Apply functions: Vertex Stream (VB/IB bindings)

		static void applyGLIAVertexStreamState( const GLIAVertexStreamDefinition & pVertexStreamDefinition, GLDrawTopologyProperties & pDrawTopologyProperties );
		static void applyGLIAVertexStreamStateIBBinding( const GLIAIndexBufferBinding & pIndexBufferBinding, GLDrawTopologyProperties & pDrawTopologyProperties );
		static void applyGLIAVertexStreamStateVBBindings( const GLIAVertexBuffersBindings & pVertexBufferBindings );

		// Apply functions: Render Target (Attachment bindings)

		static void applyGLRenderTargetBinding( const GLRenderTargetBindingInfo & pGLRenderTargetBinding );

	protected:
		GLDrawTopologyProperties _currentDrawTopologyProperties;
		GLIAVertexStreamDefinition _dynamicIAVertexStreamDefinition;
		GLRenderTargetBindingDefinition _dynamicRenderTargetBindingDefinition;
	};

} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_GLCOMMON_PIPELINE_STATE_CONTROLLER_H__
