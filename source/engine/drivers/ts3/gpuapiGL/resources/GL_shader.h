
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

		static GLShaderHandle createShaderObjectWithBinary( GLGPUDevice & pGLGPUDevice, EShaderType pShaderType, const ShaderBinary & pShaderBinary );
		static GLShaderHandle createShaderObjectWithSource( GLGPUDevice & pGLGPUDevice, EShaderType pShaderType, const void * pSource, size_t pSourceLength );
		static GLShaderHandle createShaderSeparableStageWithBinary( GLGPUDevice & pGLGPUDevice, EShaderType pShaderType, const ShaderBinary & pShaderBinary );
		static GLShaderHandle createShaderSeparableStageWithSource( GLGPUDevice & pGLGPUDevice, EShaderType pShaderType, const void * pSource, size_t pSourceLength );
	};

	namespace rcutil
	{

		GLShaderHandle createShaderObject( GLGPUDevice & pGLGPUDevice, const ShaderCreateInfo & pCreateInfo );

		GLShaderHandle createShaderSeparableStage( GLGPUDevice & pGLGPUDevice, const ShaderCreateInfo & pCreateInfo );

	}

} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_GLCOMMON_SHADER_INTERFACE_H__
