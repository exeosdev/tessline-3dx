
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

	GLShaderHandle GLShader::createShaderObjectWithBinary( GLGPUDevice & pGLGPUDevice, EShaderType pShaderType, const ShaderBinary & pShaderBinary )
	{
		return nullptr;
	}

	GLShaderHandle GLShader::createShaderObjectWithSource( GLGPUDevice & pGLGPUDevice, EShaderType pShaderType, const void * pSource, size_t pSourceLength )
	{
		const auto openglShaderType = GLCoreAPIProxy::translateShaderType( pShaderType );

		auto openglShader = GLShaderObject::createWithSource( openglShaderType, pSource, pSourceLength );
		if( !openglShader )
		{
			return nullptr;
		}

		auto shaderInterface = std::make_unique<GLShader>( pGLGPUDevice, pShaderType, std::move( openglShader ) );

		return shaderInterface;
	}

	GLShaderHandle GLShader::createShaderSeparableStageWithBinary( GLGPUDevice & pGLGPUDevice, EShaderType pShaderType, const ShaderBinary & pShaderBinary )
	{
		return nullptr;
	}

	GLShaderHandle GLShader::createShaderSeparableStageWithSource( GLGPUDevice & pGLGPUDevice, EShaderType pShaderType, const void * pSource, size_t pSourceLength )
	{
		const auto openglShaderType = GLCoreAPIProxy::translateShaderType( pShaderType );

		auto openglShader = GLShaderObject::createWithSource( openglShaderType, pSource, pSourceLength );
		if( !openglShader )
		{
			return nullptr;
		}

		auto openglProgram = GLShaderProgramObject::createSeparableModule( *openglShader );
		if( !openglProgram )
		{
			return nullptr;
		}

		auto shaderInterface = std::make_unique<GLShader>( pGLGPUDevice, pShaderType, std::move( openglProgram ) );

		return shaderInterface;
	}

	namespace rcutil
	{

		GLShaderHandle createShaderObject( GLGPUDevice & pGLGPUDevice, const ShaderCreateInfo & pCreateInfo )
		{
			if( pCreateInfo.shaderBinary && !pCreateInfo.shaderBinary->empty() )
			{
				return GLShader::createShaderObjectWithBinary(
						pGLGPUDevice,
						pCreateInfo.shaderType,
						*( pCreateInfo.shaderBinary ) );
			}
			else if( !pCreateInfo.shaderSource.empty() )
			{
				return GLShader::createShaderObjectWithSource(
						pGLGPUDevice,
						pCreateInfo.shaderType,
						pCreateInfo.shaderSource.data(),
						pCreateInfo.shaderSource.size() );
			}
			else if( pCreateInfo.shaderSourceView )
			{
				return GLShader::createShaderObjectWithSource(
						pGLGPUDevice,
						pCreateInfo.shaderType,
						pCreateInfo.shaderSourceView.data(),
						pCreateInfo.shaderSourceView.size() );
			}

			return nullptr;
		}

		GLShaderHandle createShaderSeparableStage( GLGPUDevice & pGLGPUDevice, const ShaderCreateInfo & pCreateInfo )
		{
			if( pCreateInfo.shaderBinary && !pCreateInfo.shaderBinary->empty() )
			{
				return GLShader::createShaderSeparableStageWithBinary(
						pGLGPUDevice,
						pCreateInfo.shaderType,
						*( pCreateInfo.shaderBinary ) );
			}
			else if( !pCreateInfo.shaderSource.empty() )
			{
				return GLShader::createShaderSeparableStageWithSource(
						pGLGPUDevice,
						pCreateInfo.shaderType,
						pCreateInfo.shaderSource.data(),
						pCreateInfo.shaderSource.size() );
			}
			else if( pCreateInfo.shaderSourceView )
			{
				return GLShader::createShaderSeparableStageWithSource(
						pGLGPUDevice,
						pCreateInfo.shaderType,
						pCreateInfo.shaderSourceView.data(),
						pCreateInfo.shaderSourceView.size() );
			}

			return nullptr;
		}

	}

} // namespace ts3::gpuapi
