
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


	GraphicsShaderImmutableStateSeparable::GraphicsShaderImmutableStateSeparable(
			GPUDevice & pGPUDevice,
			const GraphicsShaderSet & pShaderSet,
			const GraphicsShaderLinkageCommonProperties & pCommonProperties )
			: GraphicsShaderLinkageImmutableState( pGPUDevice, pCommonProperties )
	{}

	GraphicsShaderImmutableStateSeparable::~GraphicsShaderImmutableStateSeparable() = default;


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
