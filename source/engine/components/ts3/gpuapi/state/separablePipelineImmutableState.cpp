
#include "separablePipelineImmutableState.h"
#include <ts3/gpuapi/resources/shader.h>

namespace ts3::gpuapi
{

	SeparableGraphicsShaderImmutableState::SeparableGraphicsShaderImmutableState(
			GPUDevice & pGPUDevice,
			const GraphicsShaderSet & pShaderSet,
			const GraphicsShaderLinkageCommonProperties & pCommonProperties )
	: GraphicsShaderLinkageImmutableState( pGPUDevice, pCommonProperties )
	{}

	SeparableGraphicsShaderImmutableState::~SeparableGraphicsShaderImmutableState() = default;


	SeparablePipelineImmutableStateFactory::SeparablePipelineImmutableStateFactory( GPUDevice & pGPUDevice )
	: mGPUDevice( pGPUDevice )
	{}

	SeparablePipelineImmutableStateFactory::~SeparablePipelineImmutableStateFactory() = default;

	GraphicsShaderLinkageImmutableStateHandle SeparablePipelineImmutableStateFactory::createGraphicsShaderLinkageState(
			const GraphicsShaderSet & pShaderSet )
	{
		if( !pShaderSet[EShaderType::GSVertex] || !pShaderSet[EShaderType::GSPixel] )
		{
			return nullptr;
		}

		if( !pShaderSet.validateShaders() )
		{
			return nullptr;
		}

		const auto stateCommonProperties = smutil::getGraphicsShaderLinkageCommonPropertiesForShaderSet( pShaderSet );

		const auto shaderImmutableState = createGPUAPIObject<SeparableGraphicsShaderImmutableState>(
				mGPUDevice,
				std::move( pShaderSet ),
				stateCommonProperties );

		return shaderImmutableState;
	}

	namespace smutil
	{

		GraphicsShaderLinkageCommonProperties getGraphicsShaderLinkageCommonPropertiesForShaderSet(
				const GraphicsShaderSet & pShaderSet ) noexcept
		{
			GraphicsShaderLinkageCommonProperties commonProperties{};
			commonProperties.activeStagesMask = pShaderSet.getActiveShaderStagesMask();
			commonProperties.activeStagesNum = pShaderSet.getActiveShaderStagesNum();
			return commonProperties;
		}

	}

}
