
#include "shader.h"

namespace ts3
{
namespace gpuapi
{

	Shader::Shader( GPUDevice & pGPUDevice, EShaderType pShaderType )
	: GPUBaseObject( pGPUDevice )
	, mEShaderType( pShaderType )
	, mShaderBinary()
	{}

	Shader::Shader( GPUDevice & pGPUDevice, EShaderType pShaderType, ShaderBinary pShaderBinary )
	: GPUBaseObject( pGPUDevice )
	, mEShaderType( pShaderType )
	, mShaderBinary( std::move( pShaderBinary ) )
	{}

	Shader::~Shader() = default;

} /* namespace ts3 */
} /* namespace gpuapi */