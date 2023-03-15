
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

	GLShaderHandle GLShader::createShaderObjectWithBinary( GLGPUDevice & pGPUDevice, EShaderType pShaderType, const ShaderBinary & pShaderBinary )
	{
		return nullptr;
	}

	GLShaderHandle GLShader::createShaderObjectWithSource( GLGPUDevice & pGPUDevice, EShaderType pShaderType, const void * pSource, size_t pSourceLength )
	{
		const auto openglShaderType = atl::translateShaderType( pShaderType );

		auto openglShader = GLShaderObject::createWithSource( openglShaderType, pSource, pSourceLength );
		if( !openglShader )
		{
			return nullptr;
		}

		auto shaderInterface = std::make_unique<GLShader>( pGPUDevice, pShaderType, std::move( openglShader ) );

		return shaderInterface;
	}

	GLShaderHandle GLShader::createShaderSeparableStageWithBinary( GLGPUDevice & pGPUDevice, EShaderType pShaderType, const ShaderBinary & pShaderBinary )
	{
		return nullptr;
	}

	GLShaderHandle GLShader::createShaderSeparableStageWithSource( GLGPUDevice & pGPUDevice, EShaderType pShaderType, const void * pSource, size_t pSourceLength )
	{
		const auto openglShaderType = atl::translateShaderType( pShaderType );

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

		auto shaderInterface = std::make_unique<GLShader>( pGPUDevice, pShaderType, std::move( openglProgram ) );

		return shaderInterface;
	}

	namespace rcutil
	{

		GLShaderHandle createShaderObject( GLGPUDevice & pGPUDevice, const ShaderCreateInfo & pCreateInfo )
		{
			if( pCreateInfo.shaderBinary && !pCreateInfo.shaderBinary->empty() )
			{
				return GLShader::createShaderObjectWithBinary(
						pGPUDevice,
						pCreateInfo.shaderType,
						*( pCreateInfo.shaderBinary ) );
			}
			else if( !pCreateInfo.shaderSource.empty() )
			{
				return GLShader::createShaderObjectWithSource(
						pGPUDevice,
						pCreateInfo.shaderType,
						pCreateInfo.shaderSource.data(),
						pCreateInfo.shaderSource.size() );
			}
			else if( pCreateInfo.shaderSourceView )
			{
				return GLShader::createShaderObjectWithSource(
						pGPUDevice,
						pCreateInfo.shaderType,
						pCreateInfo.shaderSourceView.data(),
						pCreateInfo.shaderSourceView.size() );
			}

			return nullptr;
		}

		GLShaderHandle createShaderSeparableStage( GLGPUDevice & pGPUDevice, const ShaderCreateInfo & pCreateInfo )
		{
			if( pCreateInfo.shaderBinary && !pCreateInfo.shaderBinary->empty() )
			{
				return GLShader::createShaderSeparableStageWithBinary(
						pGPUDevice,
						pCreateInfo.shaderType,
						*( pCreateInfo.shaderBinary ) );
			}
			else if( !pCreateInfo.shaderSource.empty() )
			{
				return GLShader::createShaderSeparableStageWithSource(
						pGPUDevice,
						pCreateInfo.shaderType,
						pCreateInfo.shaderSource.data(),
						pCreateInfo.shaderSource.size() );
			}
			else if( pCreateInfo.shaderSourceView )
			{
				return GLShader::createShaderSeparableStageWithSource(
						pGPUDevice,
						pCreateInfo.shaderType,
						pCreateInfo.shaderSourceView.data(),
						pCreateInfo.shaderSourceView.size() );
			}

			return nullptr;
		}

	}

} // namespace ts3::gpuapi
