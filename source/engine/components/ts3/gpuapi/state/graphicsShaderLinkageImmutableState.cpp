
#include "graphicsShaderLinkageImmutableState.h"

namespace ts3::gpuapi
{

	GraphicsShaderLinkageImmutableState::GraphicsShaderLinkageImmutableState(
			GPUDevice & pGPUDevice,
			const GraphicsShaderLinkageCommonProperties & pCommonProperties )
	: GraphicsPipelineImmutableState( pGPUDevice )
	, mCommonProperties( pCommonProperties )
	{}

	GraphicsShaderLinkageImmutableState::~GraphicsShaderLinkageImmutableState() = default;


	GraphicsShaderLinkageImmutableStateSeparable::GraphicsShaderLinkageImmutableStateSeparable(
			GPUDevice & pGPUDevice,
			const GraphicsShaderLinkageCommonProperties & pCommonProperties,
			const GraphicsShaderSet & pShaderSet )
	: GraphicsShaderLinkageImmutableState( pGPUDevice, pCommonProperties )
	, mShaderSet( pShaderSet )
	{}

	GraphicsShaderLinkageImmutableStateSeparable::~GraphicsShaderLinkageImmutableStateSeparable() = default;


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
