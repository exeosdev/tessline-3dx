
#pragma once

#ifndef __TS3DRIVER_GPUAPI_GLCOMMON_PIPELINE_STATE_CONTROLLER_H__
#define __TS3DRIVER_GPUAPI_GLCOMMON_PIPELINE_STATE_CONTROLLER_H__

#include "../GL_prerequisites.h"
#include "../state/GL_pipelineStateDesc.h"
#include "../state/GL_pipelineStateObject.h"
#include <ts3/gpuapi/state/pipelineStateController.h>

namespace ts3::gpuapi
{

	/// @brief
	class GLGraphicsPipelineStateController : public SeparableGraphicsPipelineStateController
	{
		friend class GLCommandList;

	public:
		struct GLPipelineConfig
		{
			GLintptr indexBufferBaseOffset = 0;
			GLenum indexBufferDataType = 0;
			GLuint indexBufferElementByteSize = 0;
			GLenum primitiveTopology = 0;
			GLuint shaderPipelineObjectHandle = cxGLObjectHandleInvalid;
			GLuint vertexArrayObjectHandle = cxGLObjectHandleInvalid;
		};

	public:
		GLGraphicsPipelineStateController( GLGraphicsPipelineStateDescriptorCache & pDescriptorCache );
		~GLGraphicsPipelineStateController();

		virtual bool setGraphicsPipelineStateObject( const GraphicsPipelineStateObject & pGraphicsPipelineSO ) override;
		virtual bool setVertexStreamStateObject( const VertexStreamStateObject & pVertexStreamSO ) override;

		const GLPipelineConfig & getGLPipelineConfig() const
		{
			return _csGLPipelineConfig;
		}

	protected:
		virtual bool updatePipelineState() override;

	private:
		void _setBlendState( const GLBlendStateDescriptor & pGLBlendDescriptor );
		void _setDepthStencilState( const GLDepthStencilStateDescriptor & pGLDepthStencilDescriptor );
		void _setRasterizerState( const GLRasterizerStateDescriptor & pGLStateDescriptor );

	protected:
		GLPipelineConfig _csGLPipelineConfig;
		GLGraphicsPipelineStateDescriptorCache * _descriptorCache;
	};

} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_GLCOMMON_PIPELINE_STATE_CONTROLLER_H__
