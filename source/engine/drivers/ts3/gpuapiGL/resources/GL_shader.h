
#pragma once

#ifndef __TS3DRIVER_GPUAPI_GLCOMMON_SHADER_H__
#define __TS3DRIVER_GPUAPI_GLCOMMON_SHADER_H__

#include "../GL_prerequisites.h"
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

		static GLShaderHandle createInstance( GLGPUDevice & pGPUDevice, const ShaderCreateInfo & pCreateInfo );
		static GLShaderHandle createInstanceFromSource( GLGPUDevice & pGPUDevice, EShaderType pShaderType, const void * pSource, size_t pSourceLength );
	};

} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_GLCOMMON_SHADER_H__
