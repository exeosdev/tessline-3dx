
#pragma once

#ifndef __TS3DRIVER_GPUAPI_GLCOMMON_PIPELINE_STATE_CONTROLLER_H__
#define __TS3DRIVER_GPUAPI_GLCOMMON_PIPELINE_STATE_CONTROLLER_H__

#include "GL_inputAssembler.h"
#include "GL_renderTarget.h"
#include "GL_vertexArrayObjectCache.h"
#include "../objects/GL_vertexArrayObject.h"
#include <ts3/gpuapi/state/separablePipelineState.h>

namespace ts3::gpuapi
{

	struct GLIAIndexBufferBinding;
	struct GLIAVertexBuffersBindings;

	class GLCommandList;
	class GLGraphicsPipelineStateObject;
	class GLGraphicsShaderLinkageImmutableStateCore;
	class GLGraphicsShaderLinkageImmutableStateCompat;

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

		virtual bool applyStateChanges() override;

		virtual bool setGraphicsPipelineStateObject( const GraphicsPipelineStateObject & pGraphicsPSO ) override;
		virtual bool resetGraphicsPipelineStateObject() override;

		virtual bool setIAVertexStreamState( const IAVertexStreamDynamicState & pIAVertexStreamState ) override;
		virtual bool setIAVertexStreamState( const IAVertexStreamImmutableState & pIAVertexStreamState ) override;
		virtual bool resetIAVertexStreamState() override;

		virtual bool setRenderTargetBindingState( const RenderTargetBindingDynamicState & pRenderTargetBindingState ) override;
		virtual bool setRenderTargetBindingState( const RenderTargetBindingImmutableState & pRenderTargetBindingState ) override;
		virtual bool resetRenderTargetBindingState() override;

		virtual bool setBlendConstantColor( const math::RGBAColorR32Norm & pColor ) override;
		virtual bool setViewport( const ViewportDesc & pViewportDesc ) override;
		virtual bool setShaderConstant( shader_input_ref_id_t pParamRefID, const void * pData ) override;
		virtual bool setShaderConstantBuffer( shader_input_ref_id_t pParamRefID, GPUBuffer & pConstantBuffer ) override;
		virtual bool setShaderTextureImage( shader_input_ref_id_t pParamRefID, Texture & pTexture ) override;
		virtual bool setShaderTextureSampler( shader_input_ref_id_t pParamRefID, Sampler & pSampler ) override;

	protected:
		void resetDynamicIAVertexStreamState();
		void resetDynamicRenderTargetBindingState();

		const GLVertexArrayObject & getCachedVertexArrayObject(
			const GLIAInputLayoutImmutableState & pInputLayoutState,
			const GLIAVertexStreamImmutableState & pVertexStreamState );

		// Apply functions: PSO States
		static void applyGLBlendState( const GLBlendImmutableState & pBlendState );
		static void applyGLDepthStencilState( const GLDepthStencilImmutableState & pDepthStencilState );
		static void applyGLRasterizerState( const GLRasterizerImmutableState & pRasterizerState );

		// Apply functions: Vertex Stream (VB/IB bindings)
		static void applyGLIAIndexBufferBinding(
				const GLIAIndexBufferBinding & pIndexBufferBinding,
				GLDrawTopologyProperties & pDrawTopologyProperties );

		// Apply functions: Render Target (Attachment bindings)
		static void applyGLRenderTargetBinding( const GLRenderTargetBindingInfo & pGLRenderTargetBinding );

	private:
		virtual void updateShaderInputInlineConstantData(
				const GLGraphicsShaderLinkageImmutableState & pShaderState,
				const ShaderInputParameterConstant & pConstantInfo,
				const void * pConstantData ) = 0;

	protected:
		GLDrawTopologyProperties _currentDrawTopologyProperties;
		GLIAVertexStreamDefinition _dynamicIAVertexStreamDefinition;
		GLRenderTargetBindingDefinition _dynamicRenderTargetBindingDefinition;
		GLVertexArrayObjectCache _vaoCache;
	};

	class GLGraphicsPipelineStateControllerCore : public GLGraphicsPipelineStateController
	{
	public:
		virtual bool applyStateChanges() override final;

	private:
		virtual void updateShaderInputInlineConstantData(
				const GLGraphicsShaderLinkageImmutableState & pShaderState,
				const ShaderInputParameterConstant & pConstantInfo,
				const void * pConstantData ) override final;

		static void applyGLShaderLinkageState( const GLGraphicsShaderLinkageImmutableStateCore & pShaderLinkageState );

		static void applyGLIAInputLayoutState(
				const GLIAInputLayoutImmutableState & pInputLayoutState,
				GLDrawTopologyProperties & pDrawTopologyProperties );

		static void applyGLIAVertexStreamState(
				const GLIAVertexStreamDefinition & pVertexStreamDefinition,
				GLDrawTopologyProperties & pDrawTopologyProperties );

		static void applyGLIAVertexBufferBindings( const GLIAVertexBuffersBindings & pVertexBufferBindings );
	};

	class GLGraphicsPipelineStateControllerCompat : public GLGraphicsPipelineStateController
	{
	public:
		virtual bool applyStateChanges() override final;

	private:
		const GLVertexArrayObject & getTransientVertexArrayObject(
				const GLIAInputLayoutDefinition & pInputLayoutDefinition,
				const GLIAVertexStreamDefinition & pVertexStreamDefinition );

		virtual void updateShaderInputInlineConstantData(
				const GLGraphicsShaderLinkageImmutableState & pShaderState,
				const ShaderInputParameterConstant & pConstantInfo,
				const void * pConstantData ) override final;

		static void applyGLShaderLinkageState( const GLGraphicsShaderLinkageImmutableStateCompat & pShaderLinkageState );

		static void applyGLCombinedInputAssemblerState(
				const GLVertexArrayObject & pVertexArrayObject,
				const GLIAInputLayoutDefinition & pInputLayoutDefinition,
				const GLIAVertexStreamDefinition & pVertexStreamDefinition,
				GLDrawTopologyProperties & pDrawTopologyProperties );

	private:
		GLVertexArrayObjectHandle _transientVertexArrayObject;
	};

} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_GLCOMMON_PIPELINE_STATE_CONTROLLER_H__
