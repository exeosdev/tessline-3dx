
#pragma once

#ifndef __TS3_GPUAPI_IS_H__
#define __TS3_GPUAPI_IS_H__

#include "commonGPUStateDefs.h"
#include "../resources/shaderCommon.h"
#include <unordered_map>

namespace ts3::gpuapi
{

	enum EShaderConstantAccessClass : uint32
	{
		ACAllActive  = E_SHADER_STAGE_MASK_ALL,
		ACGSVertex   = E_SHADER_STAGE_FLAG_GRAPHICS_VERTEX_BIT,
		ACGSHull     = E_SHADER_STAGE_FLAG_GRAPHICS_HULL_BIT,
		ACGSDomain   = E_SHADER_STAGE_FLAG_GRAPHICS_DOMAIN_BIT,
		ACGSGeometry = E_SHADER_STAGE_FLAG_GRAPHICS_GEOMETRY_BIT,
		ACGSPixel    = E_SHADER_STAGE_FLAG_GRAPHICS_PIXEL_BIT,
		ACCSCompute  = E_SHADER_STAGE_FLAG_COMPUTE_BIT,
	};

	enum class EShaderInputParameterType : uint16
	{
		// D3D12: root constants
		// Vulkan: push constants
		// Others: API-specific (explicit direct constants or implicit constant buffer)
		Constant,
		// CBV/SRV/UAV
		Resource,
		//
		Sampler
	};

	enum class EShaderInputDescriptorType : uint16
	{
		// CBV/SRV/UAV
		Resource = 1,
		//
		Sampler,
	};

	enum class EShaderInputResourceClass : uint16
	{
		CBV = 1,
		SRV,
		UAV,
		Unknown = 0,
	};

	namespace CxDefs
	{

		inline constexpr uint16 declareShaderInputResourceType( EShaderInputResourceClass pResourceClass, uint8 pIndex )
		{
			return ( ( static_cast<uint16>( pResourceClass ) << 8u ) | pIndex );
		}

	}

	enum class EShaderInputResourceType : uint16
	{
		CBVConstantBuffer = CxDefs::declareShaderInputResourceType( EShaderInputResourceClass::CBV, 0 ),
		SRVTextureBuffer  = CxDefs::declareShaderInputResourceType( EShaderInputResourceClass::SRV, 1 ),
		SRVTextureImage   = CxDefs::declareShaderInputResourceType( EShaderInputResourceClass::SRV, 2 ),
		UAVStorageBuffer  = CxDefs::declareShaderInputResourceType( EShaderInputResourceClass::UAV, 3 ),
		UAVStorageImage   = CxDefs::declareShaderInputResourceType( EShaderInputResourceClass::UAV, 4 ),
		Unknown           = 0
	};

	namespace CxDefs
	{

		inline constexpr EShaderInputResourceClass getShaderInputResourceResourceClass( EShaderInputResourceType pResourceType )
		{
			return static_cast<EShaderInputResourceClass>( ( static_cast<uint32>( pResourceType ) >> 8u ) & 0xFFu );
		}

	}

	inline constexpr Bitmask<EShaderStageFlags> ecGetShaderConstantVisibilityStageMask( EShaderConstantAccessClass pAccessClass )
	{
		return static_cast<uint32>( pAccessClass );
	}

	struct ShaderInputSignatureDesc
	{
		struct ConstantDesc
		{
			shader_input_index_t bindingIndex;
			shader_input_ref_id_t refID;
			EShaderInputConstantFormat format;
		};

		struct ConstantGroup
		{
			using ConstantList = std::array<ConstantDesc, CxDefs::GPU_SYSTEM_METRIC_IS_MAX_CONSTANT_GROUP_SIZE>;
			EShaderConstantAccessClass accessClass;
			ConstantList constantList;
			uint32 constantsNum = 0;
		};

		struct DescriptorDesc
		{
			struct ResourceDesc
			{
				EShaderInputResourceType resourceType;
				shader_input_index_t resourceBaseRegisterIndex;
				shader_input_index_t resourceArraySize;
			};

			struct SamplerDesc
			{
				shader_input_index_t samplerBindingIndex;
			};

			shader_input_ref_id_t refID;
			EShaderInputDescriptorType descriptorType;
			Bitmask<EShaderStageFlags> shaderVisibilityMask;

			union
			{
				ResourceDesc uResourceDesc;
				SamplerDesc uSamplerDesc;
			};
		};

		struct DescriptorSet
		{
			using DescriptorList = std::array<DescriptorDesc, CxDefs::GPU_SYSTEM_METRIC_IS_MAX_DESCRIPTOR_SET_SIZE>;
			EShaderInputDescriptorType descriptorType;
			DescriptorList descriptorList;
			uint32 descriptorsNum = 0;
		};

		using ConstantGroupArray = std::array<ConstantGroup, CxDefs::GPU_SYSTEM_METRIC_SHADER_COMBINED_STAGES_NUM>;
		using DescriptorSetArray = std::array<DescriptorSet, CxDefs::GPU_SYSTEM_METRIC_IS_MAX_DESCRIPTOR_SETS_NUM>;

		Bitmask<EShaderStageFlags> activeShaderStagesMask;
		ConstantGroupArray constantGroupArray;
		uint32 constantGroupsNum = 0;
		DescriptorSetArray descriptorSetArray;
		uint32 descriptorSetsNum = 0;
	};

	struct ShaderInputParameterBase
	{
		shader_input_ref_id_t cRefID;
		shader_input_index_t cParamIndex;
		EShaderInputParameterType cParamType;
	};

	struct ShaderInputParameterConstant : public ShaderInputParameterBase
	{
		uint32 iByteSize;
		uint32 iDwordSize;
		EShaderInputConstantFormat iFormat;
		shader_input_index_t iGlobalIndex;
		shader_input_index_t iStageIndex;
		EShaderConstantAccessClass iAccessClass;
		Bitmask<EShaderStageFlags> iVisibilityMask;
	};

	struct ShaderInputParameterDescriptor : public ShaderInputParameterBase
	{
		struct ResourceInfo
		{
			EShaderInputResourceType resourceType;
			EShaderInputResourceClass resourceClass;
			shader_input_index_t resourceArraySize;
			shader_input_index_t resourceBaseRegisterIndex;
		};

		struct SamplerInfo
		{
			shader_input_index_t samplerBindingIndex;
		};

		EShaderInputDescriptorType dDescriptorType;
		shader_input_index_t dSetIndex;
		Bitmask<EShaderStageFlags> dShaderVisibilityMask;

		union
		{
			ResourceInfo uResourceInfo;
			SamplerInfo uSamplerInfo;
		};
	};

	struct ShaderInputSignature
	{
		struct ConstantLayout
		{
			using CommonConstantArray = std::vector<ShaderInputParameterConstant>;

			Bitmask<EShaderStageFlags> activeShaderStageMask;
			CommonConstantArray commonConstantArray;
			uint32 constantsNum;
			uint32 dwordSize;

			explicit operator bool() const
			{
				return !commonConstantArray.empty();
			}
		};

		struct DescriptorLayout
		{
			struct DescriptorSet
			{
				EShaderInputDescriptorType descriptorType;
				ShaderInputParameterDescriptor * basePtr;
				uint32 arrayOffset;
				uint32 setDescriptorsNum;
				uint32 setIndex;
				Bitmask<EShaderStageFlags> shaderVisibilityMask;
			};

			using CommonDescriptorArray = std::vector<ShaderInputParameterDescriptor>;
			using DescriptorSetList = std::vector<DescriptorSet>;

			CommonDescriptorArray commonDescriptorArray;
			DescriptorSetList descriptorSets;
			uint32 descriptorSetsNum;
			uint32 totalDescriptorsNum;

			explicit operator bool() const
			{
				return !commonDescriptorArray.empty();
			}
		};

		using CommonParameterList = std::vector<ShaderInputParameterBase *>;
		using ConstantMap = std::unordered_map<shader_input_ref_id_t, ShaderInputParameterConstant *>;
		using DescriptorMap = std::unordered_map<shader_input_ref_id_t, ShaderInputParameterDescriptor *>;

		Bitmask<EShaderStageFlags> activeShaderStagesMask;
		//
		CommonParameterList commonParameterList;
		//
		ConstantLayout constantLayout;
		//
		ConstantMap constantMap;
		//
		DescriptorLayout descriptorLayout;
		//
		DescriptorMap descriptorMap;

		uint32 dwordSize;
		uint32 constantsNum;
		uint32 descriptorsNum;
		uint32 descriptorSetsNum;
		uint32 parametersNum;

		ShaderInputSignature() = default;

		ShaderInputSignature( const InitEmptyTag & )
        : dwordSize( 0 )
		{}

		explicit operator bool() const
		{
			return ( dwordSize > 0 ) && !commonParameterList.empty();
		}

		const ShaderInputParameterConstant & getConstantInfo( shader_input_ref_id_t pParamRefID ) const
		{
			const auto * constantInfoPtr = constantMap.at( pParamRefID );
			return *constantInfoPtr;
		}

		const ShaderInputParameterDescriptor & getDescriptorInfo( shader_input_ref_id_t pParamRefID ) const
		{
			const auto * descriptorInfoPtr = descriptorMap.at( pParamRefID );
			return *descriptorInfoPtr;
		}
	};

	TS3_GPUAPI_API ShaderInputSignature createShaderInputSignature( const ShaderInputSignatureDesc & pSignatureDesc );

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_IS_H__
