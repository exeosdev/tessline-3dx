
#include "inputAssemblerImmutableStates.h"
#include <ts3/gpuapi/gpuDevice.h>

namespace ts3::gpuapi
{

	class IAVertexStreamImmutableStateDynamicOverride : public IAVertexStreamImmutableState
	{
	public:
		IAVertexStreamImmutableStateDynamicOverride()
		: IAVertexStreamImmutableState( GPUDevice::nullDevice(), {} )
		{}

		virtual ~IAVertexStreamImmutableStateDynamicOverride() = default;

		bool isDynamicOverrideState() const noexcept override final
		{
			return true;
		}
	};


	IAInputLayoutImmutableState::IAInputLayoutImmutableState(
			GPUDevice & pGPUDevice,
			const IAInputLayoutStateCommonProperties & pCommonProperties )
	: GraphicsPipelineImmutableState( pGPUDevice )
	, mCommonProperties( pCommonProperties )
	{}

	IAInputLayoutImmutableState::~IAInputLayoutImmutableState() = default;

	EGraphicsPipelineImmutableStateType IAInputLayoutImmutableState::queryStateType() const noexcept
	{
		return EGraphicsPipelineImmutableStateType::IAInputLayout;
	}


	IAVertexStreamImmutableState::IAVertexStreamImmutableState(
			GPUDevice & pGPUDevice,
			const IAVertexStreamStateCommonProperties & pCommonProperties )
	: GraphicsPipelineImmutableState( pGPUDevice )
	, mCommonProperties( pCommonProperties )
	{}

	IAVertexStreamImmutableState::~IAVertexStreamImmutableState() = default;

	EGraphicsPipelineImmutableStateType IAVertexStreamImmutableState::queryStateType() const noexcept
	{
		return EGraphicsPipelineImmutableStateType::IAVertexStream;
	}

	const IAVertexStreamImmutableState & IAVertexStreamImmutableState::getDynamicOverrideState()
	{
		static const IAVertexStreamImmutableStateDynamicOverride sDynamicOverrideState{};
		return sDynamicOverrideState;
	}

	namespace smutil
	{

		IAInputLayoutStateCommonProperties getIAInputLayoutStateCommonProperties(
				const IAInputLayoutDefinition & pInputLayoutDefinition )
		{
			IAInputLayoutStateCommonProperties properties{};
			properties.activeAttributesMask = pInputLayoutDefinition.activeAttributesMask;
			properties.primitiveTopology = pInputLayoutDefinition.primitiveTopology;
			properties.activeAttributesNum = popCount( pInputLayoutDefinition.activeAttributesMask );
			return properties;
		}

		IAVertexStreamStateCommonProperties getIAVertexStreamStateCommonProperties(
				const IAVertexStreamDefinition & pVertexStreamDefinition )
		{
			IAVertexStreamStateCommonProperties properties{};
			return properties;
		}

	}

}
