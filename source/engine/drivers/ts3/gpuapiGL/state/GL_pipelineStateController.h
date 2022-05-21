
#pragma once

#ifndef __TS3DRIVER_GPUAPI_GLCOMMON_PIPELINE_STATE_CONTROLLER_H__
#define __TS3DRIVER_GPUAPI_GLCOMMON_PIPELINE_STATE_CONTROLLER_H__

#include "../GL_prerequisites.h"
#include "../state/GL_pipelineStateDesc.h"
#include "../state/GL_pipelineStateObject.h"
#include <ts3/gpuapi/state/separablePipelineState.h>

namespace ts3::gpuapi
{

	/// @brief
	class GLGraphicsPipelineStateController : public SeparableGraphicsPipelineStateController
	{
		friend class GLCommandList;

	public:
		struct GLPipelineConfig
		{
			GLuint indexBufferBaseOffset = 0;
			GLenum indexBufferDataType = 0;
			GLuint indexBufferElementByteSize = 0;
			GLenum primitiveTopology = 0;
			GLuint shaderPipelineObjectHandle = CX_GL_OBJECT_HANDLE_INVALID;

			void reset()
			{
				indexBufferBaseOffset = 0;
				indexBufferDataType = 0;
				indexBufferElementByteSize = 0;
				primitiveTopology = 0;
				shaderPipelineObjectHandle = CX_GL_OBJECT_HANDLE_INVALID;
			}
		};

	public:
		GLGraphicsPipelineStateController( GLGraphicsPipelineStateDescriptorCache & pDescriptorCache );
		~GLGraphicsPipelineStateController();

		virtual bool setGraphicsPipelineStateObject( const GraphicsPipelineStateObject * pGraphicsPipelineSO ) override;

		virtual bool setVertexStreamStateObject( const VertexStreamStateObject * pVertexStreamSO ) override;

		TS3_FUNC_NO_DISCARD const GLPipelineConfig & getCurrentPipelineConfig() const
		{
			return _currentGLPipelineConfig;
		}

	protected:
		virtual void applyPendingPipelineStateChange() override;

	private:
		static void _setBlendState( const GLBlendStateDescriptor & pGLBlendSD );
		static void _setDepthStencilState( const GLDepthStencilStateDescriptor & pGLDepthStencilSD );
		static void _setRasterizerState( const GLRasterizerStateDescriptor & pGLRasterizerSD );
		static void _setVertexInputFormatState( const GLVertexInputFormatStateDescriptor & pGLVertexInputFormatSD );

	protected:
		GLPipelineConfig _currentGLPipelineConfig;
		GLGraphicsPipelineStateDescriptorCache * _descriptorCache;
	};

} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_GLCOMMON_PIPELINE_STATE_CONTROLLER_H__
