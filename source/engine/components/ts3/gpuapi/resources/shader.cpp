
#include "shader.h"

namespace ts3::gpuapi
{

	Shader::Shader( GPUDevice & pGPUDevice, EShaderType pShaderType )
	: GPUBaseObject( pGPUDevice )
	, mShaderType( pShaderType )
	, mShaderBinary()
	{}

	Shader::Shader( GPUDevice & pGPUDevice, EShaderType pShaderType, ShaderBinary pShaderBinary )
	: GPUBaseObject( pGPUDevice )
	, mShaderType( pShaderType )
	, mShaderBinary( std::move( pShaderBinary ) )
	{}

	Shader::~Shader() = default;

} // namespace ts3::gpuapi
