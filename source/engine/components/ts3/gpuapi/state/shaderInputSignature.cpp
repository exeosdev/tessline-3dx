
#include "shaderInputSignature.h"
#include "../resources/shader.h"

namespace ts3
{
namespace gpuapi
{

	struct InputConstantLayoutInfo
	{
		uint32 constantsNum = 0;
		uint32 totalDwordSize = 0;
	};

	struct InputDescriptorLayoutInfo
	{
		uint32 descriptorsNum = 0;
		uint32 descriptorSetsNum = 0;
	};

	static bool createInputSignatureConstantLayout( const ShaderInputSignatureDesc & pInputSignatureDesc,
	                                                ShaderInputSignature & pOutSignature );
	static bool createInputSignatureDescriptorLayout( const ShaderInputSignatureDesc & pInputSignatureDesc,
	                                                  ShaderInputSignature & pOutSignature );
	static bool validateInputSignatureConstantLayoutDesc( const ShaderInputSignatureDesc & pInputSignatureDesc,
	                                                      InputConstantLayoutInfo & pOutConstantLayoutInfo );
	static bool validateInputSignatureDescriptorLayoutDesc( const ShaderInputSignatureDesc & pInputSignatureDesc,
	                                                        InputDescriptorLayoutInfo & pOutDescriptorLayoutInfo );
	static uint32 computeConstantDwordSize( size_t pByteSize );

	ShaderInputSignature createShaderInputSignature( const ShaderInputSignatureDesc & pInputSignatureDesc )
	{
		ShaderInputSignature inputSignature;
		inputSignature.dwordSize = 0;
		inputSignature.constantsNum = 0;
		inputSignature.descriptorsNum = 0;
		inputSignature.descriptorSetsNum = 0;
		inputSignature.parametersNum = 0;

		if( !createInputSignatureConstantLayout( pInputSignatureDesc, inputSignature ) )
		{
			return cvInitEmpty;
		}

		if( !createInputSignatureDescriptorLayout( pInputSignatureDesc, inputSignature ) )
		{
			return cvInitEmpty;
		}

		inputSignature.constantsNum = inputSignature.constantLayout.constantsNum;
		inputSignature.descriptorsNum = inputSignature.descriptorLayout.totalDescriptorsNum;
		inputSignature.descriptorSetsNum = inputSignature.descriptorLayout.descriptorSetsNum;
		inputSignature.parametersNum = inputSignature.constantsNum + inputSignature.descriptorsNum;
		inputSignature.dwordSize = inputSignature.constantLayout.dwordSize + inputSignature.descriptorSetsNum;

		uint32 globalParamIndex = 0;

		for( auto & constant : inputSignature.constantLayout.commonConstantArray )
		{
			constant.cParamIndex = globalParamIndex++;
			inputSignature.commonParameterList.push_back( &constant );
			inputSignature.constantMap[constant.cRefID] = &constant;
		}
		for( auto & resourceDescriptor : inputSignature.descriptorLayout.commonDescriptorArray )
		{
			resourceDescriptor.cParamIndex = globalParamIndex++;
			inputSignature.commonParameterList.push_back( &resourceDescriptor );
			inputSignature.descriptorMap[resourceDescriptor.cRefID] = &resourceDescriptor;
		}

		return inputSignature;
	}

	bool createInputSignatureConstantLayout( const ShaderInputSignatureDesc & pInputSignatureDesc, ShaderInputSignature & pOutSignature )
	{
		InputConstantLayoutInfo constantLayoutInfo;
		if( !validateInputSignatureConstantLayoutDesc( pInputSignatureDesc, constantLayoutInfo ) )
		{
			return false;
		}

		ShaderInputSignature::ConstantLayout constantLayout;
		constantLayout.constantsNum = 0;
		constantLayout.dwordSize = 0;
		constantLayout.commonConstantArray.reserve( constantLayoutInfo.constantsNum );

		for( uint32 inputConstantGroupIndex = 0; inputConstantGroupIndex < pInputSignatureDesc.constantGroupsNum; ++inputConstantGroupIndex )
		{
			const auto & constantGroupDesc = pInputSignatureDesc.constantGroupArray[inputConstantGroupIndex];

			for( uint32 inputConstantIndex = 0; inputConstantIndex < constantGroupDesc.constantsNum; ++inputConstantIndex )
			{
				const auto & constantDesc = constantGroupDesc.constantList[inputConstantIndex];

				auto constantParameter = constantLayout.commonConstantArray.emplace_back();
				constantParameter.cRefID = constantDesc.refID;
				constantParameter.cParamType = EShaderInputParameterType::Constant;
				constantParameter.iFormat = constantDesc.format;
				constantParameter.iStageIndex = constantDesc.bindingIndex;
				constantParameter.iByteSize = ecGetVertexAttribFormatByteSize( constantDesc.format );
				constantParameter.iDwordSize = computeConstantDwordSize( constantParameter.iByteSize );
				constantParameter.iAccessClass = constantGroupDesc.accessClass;

				constantLayout.constantsNum += 1;
				constantLayout.dwordSize += constantParameter.iDwordSize;
			}
		}

		std::sort( constantLayout.commonConstantArray.begin(),
		           constantLayout.commonConstantArray.end(),
		           []( const ShaderInputParameterConstant & pLeft, const ShaderInputParameterConstant & pRight ) -> bool {
			           return ( pLeft.iAccessClass < pRight.iAccessClass ) ||
			                  ( ( pLeft.iAccessClass < pRight.iAccessClass ) && ( pLeft.iStageIndex < pRight.iStageIndex ) );
		           } );

		for( uint32 globalConstantIndex = 0; globalConstantIndex < constantLayout.constantsNum; ++globalConstantIndex )
		{
			auto & constant = constantLayout.commonConstantArray.at( globalConstantIndex );
			constant.iGlobalIndex = globalConstantIndex;

			if( constant.iAccessClass == EShaderConstantAccessClass::AllActiveStages )
			{
				constant.iVisibilityMask = pInputSignatureDesc.activeShaderStagesMask;
			}
			else
			{
				constant.iVisibilityMask = ecGetShaderConstantVisibilityStageBit( constant.iAccessClass );
			}
		}

		pOutSignature.constantLayout = std::move( constantLayout );

		return true;
	}

	bool createInputSignatureDescriptorLayout( const ShaderInputSignatureDesc & pInputSignatureDesc, ShaderInputSignature & pOutSignature )
	{
		InputDescriptorLayoutInfo descriptorLayoutInfo;
		if( !validateInputSignatureDescriptorLayoutDesc( pInputSignatureDesc, descriptorLayoutInfo ) )
		{
			return false;
		}

		ShaderInputSignature::DescriptorLayout descriptorLayout;
		descriptorLayout.descriptorSetsNum = descriptorLayoutInfo.descriptorSetsNum;
		descriptorLayout.totalDescriptorsNum = descriptorLayoutInfo.descriptorsNum;
		descriptorLayout.commonDescriptorArray.reserve( descriptorLayout.totalDescriptorsNum );
		descriptorLayout.descriptorSets.reserve( descriptorLayout.descriptorSetsNum );

		for( uint32 inputDescriptorSetIndex = 0; inputDescriptorSetIndex < pInputSignatureDesc.descriptorSetsNum; ++inputDescriptorSetIndex )
		{
			const auto & descriptorSetDesc = pInputSignatureDesc.descriptorSetArray[inputDescriptorSetIndex];

			auto descriptorSetArrayOffset = descriptorLayout.commonDescriptorArray.size();
			auto descriptorSetIndex = descriptorLayout.descriptorSets.size();

			uint32 setDescriptorsNum = 0;
			Bitmask<EShaderStageFlags> descriptorSetVisibilityMask = 0;

			for( uint32 inputDescriptorIndex = 0; inputDescriptorIndex < descriptorSetDesc.descriptorsNum; ++inputDescriptorIndex )
			{
				const auto & descriptorDesc = descriptorSetDesc.descriptorList[inputDescriptorIndex];

				auto & descriptor = descriptorLayout.commonDescriptorArray.emplace_back();
				descriptor.cRefID = descriptorDesc.refID;
				descriptor.dDescriptorType = descriptorSetDesc.descriptorType;
				descriptor.dSetIndex = trunc_numeric_cast<shader_input_index_t>( descriptorSetIndex );
				descriptor.dShaderVisibilityMask = descriptorDesc.shaderVisibilityMask & pInputSignatureDesc.activeShaderStagesMask;

				if( descriptorSetDesc.descriptorType == EShaderInputDescriptorType::Resource )
				{
					descriptor.cParamType = EShaderInputParameterType::Resource;
					descriptor.uResourceInfo.resourceArraySize = descriptorDesc.uResourceDesc.resourceArraySize;
					descriptor.uResourceInfo.resourceType = descriptorDesc.uResourceDesc.resourceType;
					descriptor.uResourceInfo.resourceClass = ecGetShaderInputResourceResourceClass( descriptorDesc.uResourceDesc.resourceType );
					descriptor.uResourceInfo.resourceBaseRegisterIndex = descriptorDesc.uResourceDesc.resourceBaseRegisterIndex;
				}
				else if( descriptorSetDesc.descriptorType == EShaderInputDescriptorType::Sampler )
				{
					descriptor.cParamType = EShaderInputParameterType::Sampler;
					descriptor.uSamplerInfo.samplerBindingIndex = descriptorDesc.uSamplerDesc.samplerBindingIndex;
				}

				setDescriptorsNum += 1;
				descriptorSetVisibilityMask.set( descriptor.dShaderVisibilityMask );
			}

			auto & descriptorSet = descriptorLayout.descriptorSets.emplace_back();
			descriptorSet.descriptorType = descriptorSetDesc.descriptorType;
			descriptorSet.arrayOffset = trunc_numeric_cast<uint32>( descriptorSetArrayOffset );
			descriptorSet.basePtr = descriptorLayout.commonDescriptorArray.data() + descriptorSetArrayOffset;
			descriptorSet.setDescriptorsNum = setDescriptorsNum;
			descriptorSet.setIndex = trunc_numeric_cast<uint32>( descriptorSetIndex );
			descriptorSet.shaderVisibilityMask = descriptorSetVisibilityMask;
		}

		pOutSignature.descriptorLayout = std::move( descriptorLayout );

		return true;
	}

	bool validateInputSignatureConstantLayoutDesc( const ShaderInputSignatureDesc & pInputSignatureDesc,
	                                               InputConstantLayoutInfo & pOutConstantLayoutInfo )
	{
		size_t constantsNum = 0;
		size_t totalDwordSize = 0;

		if( pInputSignatureDesc.constantGroupsNum > GPU_SYSTEM_METRIC_SHADER_COMBINED_STAGES_NUM + 1 )
		{
			ts3DebugInterrupt();
			return false;
		}

		for( uint32 inputConstantGroupIndex = 0; inputConstantGroupIndex < pInputSignatureDesc.constantGroupsNum; ++inputConstantGroupIndex )
		{
			const auto & constantGroupDesc = pInputSignatureDesc.constantGroupArray[inputConstantGroupIndex];
			auto groupShaderStageBit = ecGetShaderConstantVisibilityStageBit( constantGroupDesc.accessClass );

			constantsNum += constantGroupDesc.constantsNum;

			if( !pInputSignatureDesc.activeShaderStagesMask.isSet( groupShaderStageBit ) )
			{
				ts3DebugInterrupt();
				return false;
			}

			for( uint32 inputConstantIndex = 0; inputConstantIndex < constantGroupDesc.constantsNum; ++inputConstantIndex )
			{
				const auto & constantDesc = constantGroupDesc.constantList[inputConstantIndex];

				auto constantByteSize = ecGetVertexAttribFormatByteSize( constantDesc.format );
				auto constantDwordSize = computeConstantDwordSize( constantByteSize );
				totalDwordSize += constantDwordSize;

				if( totalDwordSize > GPU_SYSTEM_METRIC_IS_MAX_DWORD_SIZE )
				{
					ts3DebugInterrupt();
					return false;
				}
			}
		}

		pOutConstantLayoutInfo.constantsNum = trunc_numeric_cast<uint32>( constantsNum );
		pOutConstantLayoutInfo.totalDwordSize = trunc_numeric_cast<uint32>( totalDwordSize );

		return true;
	}

	bool validateInputSignatureDescriptorLayoutDesc( const ShaderInputSignatureDesc & pInputSignatureDesc,
	                                                 InputDescriptorLayoutInfo & pOutDescriptorLayoutInfo )
	{
		size_t descriptorsNum = 0;
		size_t descriptorSetsNum = pInputSignatureDesc.descriptorSetsNum;

		if( descriptorSetsNum > GPU_SYSTEM_METRIC_IS_MAX_DESCRIPTOR_SETS_NUM )
		{
			ts3DebugInterrupt();
			return false;
		}

		for( uint32 inputDescriptorSetIndex = 0; inputDescriptorSetIndex < pInputSignatureDesc.descriptorSetsNum; ++inputDescriptorSetIndex )
		{
			const auto & descriptorSetDesc = pInputSignatureDesc.descriptorSetArray[inputDescriptorSetIndex];
			descriptorsNum += descriptorSetDesc.descriptorsNum;

			for( uint32 inputDescriptorIndex = 0; inputDescriptorIndex < descriptorSetDesc.descriptorsNum; ++inputDescriptorIndex )
			{
				const auto & descriptorDesc = descriptorSetDesc.descriptorList[inputDescriptorIndex];
				if( descriptorDesc.descriptorType != descriptorSetDesc.descriptorType )
				{
					ts3DebugInterrupt();
					return false;
				}
			}
		}

		pOutDescriptorLayoutInfo.descriptorsNum = trunc_numeric_cast<uint32>( descriptorsNum );
		pOutDescriptorLayoutInfo.descriptorSetsNum = trunc_numeric_cast<uint32>( descriptorSetsNum );

		return true;
	}

	uint32 computeConstantDwordSize( size_t pByteSize )
	{
		auto dwordSize = pByteSize / sizeof( uint32 );
		auto dwordSizeMod = pByteSize % sizeof( uint32 );
		return ( dwordSizeMod == 0 ) ? trunc_numeric_cast<uint32>( dwordSize ) : trunc_numeric_cast<uint32>( dwordSize + 1 );
	}

} /* namespace ts3 */
} /* namespace gpuapi */
