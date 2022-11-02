
#include "shader.h"

namespace ts3::GpuAPI
{

	Shader::Shader( GPUDevice & pGPUDevice, EShaderType pShaderType )
	: GPUBaseObject( pGPUDevice )
	, mShaderType( pShaderType )
	, mShaderBinary()
	{}

	Shader::Shader(
			GPUDevice & pGPUDevice,
			EShaderType pShaderType,
			std::unique_ptr<ShaderBinary> pShaderBinary,
			resource_id_t pShaderID )
	: GPUBaseObject( pGPUDevice )
	, mShaderType( pShaderType )
	, mShaderBinary( std::move( pShaderBinary ) )
	{}

	Shader::~Shader() = default;


	std::unique_ptr<ShaderBinary> ShaderBinary::create( size_t pBinarySize )
	{
		const auto requiredBinaryStorageSize = pBinarySize - sDataBufferFixedSize;
		auto * shaderBinary = new ( cvAllocNewSizeExplicit, requiredBinaryStorageSize ) ShaderBinary();
		return std::unique_ptr<ShaderBinary>{ shaderBinary };
	}

} // namespace ts3::GpuAPI
