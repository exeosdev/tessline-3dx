
#include "gpuUtils.h"
#include "gpuDevice.h"
#include <fstream>

namespace ts3::gpuapi::Utils
{

	ShaderHandle createShaderFromFile( GPUDevice & pGPUDevice, EShaderType pShaderType, const char * pFilename )
	{
		std::ifstream inputFile( pFilename, std::ios::in );
		if( !inputFile )
		{
			return nullptr;
		}

		inputFile.seekg( 0, std::ios::end );

		size_t fileSize = inputFile.tellg();
		if( fileSize == static_cast<std::ifstream::pos_type>( -1 ) )
		{
			return nullptr;
		}

		inputFile.seekg( 0, std::ios::beg );

		std::vector<char> fileContent;
		fileContent.resize( fileSize + 1 );
		inputFile.read( fileContent.data(), fileSize );
		fileContent[fileSize] = 0;

		ShaderCreateInfo shaderCreateInfo;
		shaderCreateInfo.shaderType = pShaderType;
		shaderCreateInfo.sourceCode = fileContent.data();
		shaderCreateInfo.sourceCodeLength = fileSize;
	#if ( TS3_DEBUG )
		shaderCreateInfo.createFlags = E_SHADER_CREATE_FLAG_DEBUG_BIT;
	#else
		shaderCreateInfo.createFlags = E_SHADER_CREATE_FLAG_OPTIMIZATION_L1_BIT;
	#endif

		auto shader = pGPUDevice.createShader( shaderCreateInfo );
		return shader;
	}

}
