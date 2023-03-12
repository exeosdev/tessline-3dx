
#include "renderTargetImmutableStates.h"
#include <ts3/gpuapi/gpuDevice.h>

namespace ts3::gpuapi
{

	class RenderTargetBindingImmutableStateDynamicOverride : public RenderTargetBindingImmutableState
	{
	public:
		RenderTargetBindingImmutableStateDynamicOverride()
		: RenderTargetBindingImmutableState( GPUDevice::nullDevice(), {} )
		{}

		virtual ~RenderTargetBindingImmutableStateDynamicOverride() = default;

		bool isDynamicOverrideState() const noexcept override final
		{
			return false;
		}
	};


	RenderTargetBindingImmutableState::RenderTargetBindingImmutableState(
			GPUDevice & pGPUDevice,
			const RenderTargetLayout & pRenderTargetLayout )
	: GraphicsPipelineImmutableState( pGPUDevice )
	, mRenderTargetLayout( pRenderTargetLayout )
	{}

	RenderTargetBindingImmutableState::~RenderTargetBindingImmutableState() = default;

	const RenderTargetBindingImmutableState & RenderTargetBindingImmutableState::getDynamicOverrideState()
	{
		static const RenderTargetBindingImmutableStateDynamicOverride sDynamicOverrideState{};
		return sDynamicOverrideState;
	}


	RenderPassConfigurationImmutableState::RenderPassConfigurationImmutableState( GPUDevice & pGPUDevice )
	: GraphicsPipelineImmutableState( pGPUDevice )
	{}

	RenderPassConfigurationImmutableState::~RenderPassConfigurationImmutableState() = default;

}
