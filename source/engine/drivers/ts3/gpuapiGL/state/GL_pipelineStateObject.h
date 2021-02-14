
#pragma once

#ifndef __TS3DRIVER_GPUAPI_GLCOMMON_PIPELINE_STATE_OBJECT_H__
#define __TS3DRIVER_GPUAPI_GLCOMMON_PIPELINE_STATE_OBJECT_H__

#include "GL_pipelineStateDesc.h"
#include <ts3/gpuapi/state/pipelineStateObject.h>
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
		GLVertexArrayObjectHandle const mGLVertexArrayObject;

	public:
		GLGraphicsPipelineStateObject( GLGPUDevice & pGPUDevice,
		                               RenderTargetLayout pRenderTargetLayout,
		                               GraphicsShaderBinding pShaderBinding,
		                               ShaderInputSignature pShaderInputSignature,
		                               const GraphicsPipelineStateDescriptorSet & pSeparableDescriptorSet,
		                               GLShaderPipelineObjectHandle pGLShaderPipelineObject,
		                               GLVertexArrayObjectHandle pGLVertexArrayObject );

		virtual ~GLGraphicsPipelineStateObject();

		static Handle<GLGraphicsPipelineStateObject> create( GLGPUDevice & pGPUDevice,
		                                                     const GraphicsPipelineStateObjectCreateInfo & pCreateInfo );

	private:
		static GLShaderPipelineObjectHandle createGraphicsShaderPipelineObject( const GraphicsShaderBinding & pShaderBinding );
		static GLShaderProgramObjectHandle createGraphicsShaderProgramObject( const GraphicsShaderBinding & pShaderBinding );
		static GLVertexArrayObjectHandle createVertexArrayObjectFormatOnly( const GLVertexInputFormatStateDescriptor & pVertexInputFormatDescriptor );
	};

}

#endif // __TS3DRIVER_GPUAPI_GLCOMMON_PIPELINE_STATE_OBJECT_H__
