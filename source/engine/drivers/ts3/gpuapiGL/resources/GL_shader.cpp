
#include "GL_shader.h"
#include "../GL_gpuDevice.h"
#include "../objects/GL_shaderObject.h"
#include "../objects/GL_shaderProgramObject.h"

namespace ts3::gpuapi
{

	GLShader::GLShader( GLGPUDevice & pGPUDevice, EShaderType pShaderType, GLShaderObjectHandle pGLShaderObject )
	: Shader( pGPUDevice, pShaderType )
	, mGLShaderObject( std::move( pGLShaderObject ) )
	{}

	GLShader::GLShader( GLGPUDevice & pGPUDevice, EShaderType pShaderType, GLShaderProgramObjectHandle pGLShaderProgramObject )
	: Shader( pGPUDevice, pShaderType )
	, mGLShaderProgramObject( std::move( pGLShaderProgramObject ) )
	{}

	GLShader::~GLShader() = default;

	GLShaderHandle GLShader::createShader( GLGPUDevice & pGLGPUDevice, const ShaderCreateInfo & pCreateInfo )
	{
		auto openglEShaderType = GLCoreAPIProxy::translateGLEShaderType( pCreateInfo.shaderType );
		auto openglShader = GLShaderObject::createWithSource( openglEShaderType, pCreateInfo.sourceCode, pCreateInfo.sourceCodeLength );
		if( !openglShader )
		{
			return nullptr;
		}

		auto shaderInterface = std::make_unique<GLShader>( pGLGPUDevice, pCreateInfo.shaderType, std::move( openglShader ) );

		return shaderInterface;
	}

	GLShaderHandle GLShader::createSeparableStage( GLGPUDevice & pGLGPUDevice, const ShaderCreateInfo & pCreateInfo )
	{
		auto openglEShaderType = GLCoreAPIProxy::translateGLEShaderType( pCreateInfo.shaderType );
		auto openglShader = GLShaderObject::createWithSource( openglEShaderType, pCreateInfo.sourceCode, pCreateInfo.sourceCodeLength );
		if( !openglShader )
		{
			return nullptr;
		}

		auto openglProgram = GLShaderProgramObject::createSeparableModule( *openglShader );
		if( !openglProgram )
		{
			return nullptr;
		}

		auto shaderInterface = std::make_unique<GLShader>( pGLGPUDevice, pCreateInfo.shaderType, std::move( openglProgram ) );

		return shaderInterface;
	}

}
