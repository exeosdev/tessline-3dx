
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

	GLShaderHandle GLShader::createInstance( GLGPUDevice & pGPUDevice, const ShaderCreateInfo & pCreateInfo )
	{
		return createInstanceFromSource(
			pGPUDevice,
			pCreateInfo.shaderType,
			pCreateInfo.shaderSourceView.data(),
			pCreateInfo.shaderSourceView.size() );
	}

	GLShaderHandle GLShader::createInstanceFromSource( GLGPUDevice & pGPUDevice, EShaderType pShaderType, const void * pSource, size_t pSourceLength )
	{
		const auto openglShaderType = atl::translateShaderType( pShaderType );
		const auto runtimeVersion = pGPUDevice.mSysGLSupportInfo.apiVersion;

		GLShaderHandle shaderObject = nullptr;

		if( pGPUDevice.isCompatibilityDevice() )
		{
			GLShaderDataLayoutMap shaderLayoutMap{};
			std::string shaderSource{ reinterpret_cast<const char *>( pSource ), pSourceLength };

			rcutil::processGLShaderSourceExplicitLayoutQualifiers( runtimeVersion, shaderSource, shaderLayoutMap );

			auto openglShaderObject = GLShaderObject::createWithSource( openglShaderType, shaderSource.data(), shaderSource.length() );
			if( !openglShaderObject )
			{
				return nullptr;
			}

			openglShaderObject->setDataLayoutMap( std::move( shaderLayoutMap ) );

			shaderObject = std::make_unique<GLShader>( pGPUDevice, pShaderType, std::move( openglShaderObject ) );
		}
		else
		{
			auto openglShaderObject = GLShaderObject::createWithSource( openglShaderType, pSource, pSourceLength );
			if( !openglShaderObject )
			{
				return nullptr;
			}

			auto openglProgramObject = GLShaderProgramObject::createSeparableModule( *openglShaderObject );
			if( !openglProgramObject )
			{
				return nullptr;
			}

			shaderObject = std::make_unique<GLShader>( pGPUDevice, pShaderType, std::move( openglProgramObject ) );
		}

		return shaderObject;
	}

} // namespace ts3::gpuapi
