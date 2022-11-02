
#include "graphicsShaderLinkage.h"
#include "../resources/shader.h"
#include <ts3/stdext/memory.h>

namespace ts3::GpuAPI
{

	GraphicsShaderLinkageDescriptor::GraphicsShaderLinkageDescriptor(
			GPUDevice & pGPUDevice,
			PipelineDescriptorID pDescriptorID,
			Bitmask<EShaderStageFlags> pActiveStagesMask )
	: GraphicsPipelineDescriptor( pGPUDevice, pDescriptorID, EGraphicsPipelineDescriptorType::ShaderLinkage )
	, mActiveStagesMask( pActiveStagesMask )
	, mActiveStagesNum( pActiveStagesMask.countBits() )
	{
		ts3DebugAssert( ( mActiveStagesMask & E_SHADER_STAGE_MASK_GRAPHICS_ALL ) == mActiveStagesMask );
		ts3DebugAssert( mActiveStagesNum <= E_GPU_SYSTEM_METRIC_SHADER_GRAPHICS_STAGES_NUM );
	}

	GraphicsShaderLinkageDescriptor::~GraphicsShaderLinkageDescriptor() = default;

	namespace StateMgmt
	{

		Bitmask<EShaderStageFlags> getActiveShaderStagesMask( const ShaderObjectArray & pShaders ) noexcept
		{
			Bitmask<EShaderStageFlags> stagesMask = 0;

			for( uint32 stageIndex = 0; stageIndex < E_GPU_SYSTEM_METRIC_SHADER_GRAPHICS_STAGES_NUM; ++stageIndex )
			{
				if( pShaders[stageIndex] )
				{
					stagesMask.set( CxDefs::makeShaderStageBit( stageIndex ) );
				}
			}

			return stagesMask;
		}

		uint32 getActiveShaderStagesNum( const ShaderObjectArray & pShaders ) noexcept
		{
			uint32 stagesNum = 0;

			for( uint32 stageIndex = 0; stageIndex < E_GPU_SYSTEM_METRIC_SHADER_GRAPHICS_STAGES_NUM; ++stageIndex )
			{
				if( pShaders[stageIndex] )
				{
					++stagesNum;
				}
			}

			return stagesNum;
		}

		PipelineDescriptorHash computePipelineDescriptorHash(
				const GraphicsShaderLinkageDescriptorCreateInfo & pCreateInfo ) noexcept
		{
			PipelineDescriptorHash inputHash{};
			const auto shaders = bindArrayView( pCreateInfo.shaderArray.data(), pCreateInfo.shaderArray.size() );
			HashGen::updateInplace( inputHash, shaders );
			return inputHash;
		}

	} // namespace StateMgmt

} // namespace ts3::GpuAPI
