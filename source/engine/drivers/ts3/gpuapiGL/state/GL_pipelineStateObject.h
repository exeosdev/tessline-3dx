
#pragma once

#ifndef __TS3DRIVER_GPUAPI_GLCOMMON_PIPELINE_STATE_OBJECT_H__
#define __TS3DRIVER_GPUAPI_GLCOMMON_PIPELINE_STATE_OBJECT_H__

#include "GL_pipelineStateDesc.h"
#include <ts3/gpuapi/state/separablePipelineState.h>
#include <ts3/gpuapiGL/objects/GL_shaderPipelineObject.h>

namespace ts3::gpuapi
{

	ts3GLDeclareOpenGLObjectHandle( GLShaderPipelineObject );
	ts3GLDeclareOpenGLObjectHandle( GLShaderProgramObject );
	ts3GLDeclareOpenGLObjectHandle( GLVertexArrayObject );

	class GLGraphicsPipelineStateObject : public SeparableGraphicsPipelineStateObject
	{
	public:
		GLShaderPipelineObjectHandle const mGLShaderPipelineObject;

	public:
		GLGraphicsPipelineStateObject( GLGPUDevice & pGPUDevice,
		                               RenderTargetLayout pRenderTargetLayout,
		                               GraphicsShaderBinding pShaderBinding,
		                               ShaderInputSignature pShaderInputSignature,
		                               const SeparableGraphicsStateDescriptorSet & pStateDescriptors,
		                               GLShaderPipelineObjectHandle pGLShaderPipelineObject );

		virtual ~GLGraphicsPipelineStateObject();

		static GpaHandle<GLGraphicsPipelineStateObject> create( GLGPUDevice & pGPUDevice,
		                                                        const GraphicsPipelineStateObjectCreateInfo & pCreateInfo );

	private:
		static GLShaderPipelineObjectHandle createGraphicsShaderPipelineObject( const GraphicsShaderBinding & pShaderBinding );
		static GLShaderProgramObjectHandle createGraphicsShaderProgramObject( const GraphicsShaderBinding & pShaderBinding );
	};

} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_GLCOMMON_PIPELINE_STATE_OBJECT_H__
