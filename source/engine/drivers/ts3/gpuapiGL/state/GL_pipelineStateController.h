
#pragma once

#ifndef __TS3DRIVER_GPUAPI_GLCOMMON_PIPELINE_STATE_CONTROLLER_H__
#define __TS3DRIVER_GPUAPI_GLCOMMON_PIPELINE_STATE_CONTROLLER_H__

#include "../GL_prerequisites.h"
#include "../state/GL_graphicsPipelineState.h"
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

		virtual bool setGraphicsPipelineStateObject( const GraphicsPipelineStateObject & pGraphicsPipelineSO ) override;

		virtual bool resetGraphicsPipelineStateObject() override;

		virtual bool setVertexStreamStateObject( const VertexStreamStateObject & pVertexStreamSO ) override;

		virtual bool resetVertexStreamStateObject() override;

		TS3_ATTR_NO_DISCARD const GLPipelineConfig & getGLPipelineConfig() const
		{
			return _currentGLPipelineConfig;
		}

		static void updateGLBlendState( const GLBlendStateDescriptor & pDescriptor );

		static void updateGLDepthStencilState( const GLDepthStencilStateDescriptor & pDescriptor );

		static void updateGLRasterizerState( const GLRasterizerStateDescriptor & pDescriptor );

		static void updateGLVertexInputFormatState( const GLVertexInputFormatStateDescriptor & pDescriptor );

		static void updateGLShaderBinding( const GLShaderPipelineObject & pShaderPipelineObject );

		static void updateGLIndexBufferBinding( const GLIndexBufferBinding & pBinding );

		static void updateGLVertexBuffersBindings( const GLVertexBuffersBindings & pBinding );

	protected:
		GLPipelineConfig _currentGLPipelineConfig;
		GLGraphicsPipelineStateDescriptorCache * _descriptorCache;
	};

} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_GLCOMMON_PIPELINE_STATE_CONTROLLER_H__
