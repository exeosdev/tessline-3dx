
#pragma once

#ifndef __TS3DRIVER_GPUAPI_GLCOMMON_SHADER_INTERFACE_H__
#define __TS3DRIVER_GPUAPI_GLCOMMON_SHADER_INTERFACE_H__

#include "../objects/GL_shaderObject.h"
#include "../objects/GL_shaderPipelineObject.h"
#include "../objects/GL_shaderProgramObject.h"
#include <ts3/gpuapi/resources/shader.h>

namespace ts3::gpuapi
{

	ts3DeclareClassHandle( GLShader );

	class TS3_GPUAPI_CLASS GLShader : public Shader
	{
	public:
		GLShaderObjectHandle const mGLShaderObject;
		GLShaderProgramObjectHandle const mGLShaderProgramObject;

	public:
		GLShader( GLGPUDevice & pGPUDevice, EShaderType pShaderType, GLShaderObjectHandle pGLShaderObject );
		GLShader( GLGPUDevice & pGPUDevice, EShaderType pShaderType, GLShaderProgramObjectHandle pGLShaderProgramObject );
		virtual ~GLShader();

		static GLShaderHandle createShader( GLGPUDevice & pGLGPUDevice, const ShaderCreateInfo & pCreateInfo );
		static GLShaderHandle createSeparableStage( GLGPUDevice & pGLGPUDevice, const ShaderCreateInfo & pCreateInfo );
	};

}

#endif // __TS3DRIVER_GPUAPI_GLCOMMON_SHADER_INTERFACE_H__
