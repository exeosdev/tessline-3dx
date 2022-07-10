
#include "inputAssemblerVertexFormat.h"

namespace ts3::gpuapi
{

	IAVertexFormatDescriptor::IAVertexFormatDescriptor(
			GPUDevice & pGPUDevice,
			pipeline_descriptor_id_t pDescriptorID,
			const IAVertexInputAttributeArray & pAttributeArray,
			EPrimitiveTopology pPrimitiveTopology )
	: GraphicsPipelineDescriptor( pGPUDevice, pDescriptorID, EGraphicsPipelineDescriptorType::IAVertexStream )
	, mActiveAttributesMask( pAttributeArray.activeAttributesMask )
	, mActiveAttributesNum( pAttributeArray.activeAttributesMask )
	, mPrimitiveTopology( pPrimitiveTopology )
	{}

	IAVertexFormatDescriptor::~IAVertexFormatDescriptor() = default;

	bool IAVertexFormatDescriptor::isAttributeSet( input_assembler_index_t pAttributeIndex ) const noexcept
	{
		return mActiveAttributesMask.isSet( ecMakeIAVertexAttributeFlag( pAttributeIndex ) );
	}

	namespace smu
	{

		IAVertexInputAttributeArray createIAVertexInputAttributeArray( const ArrayView<IAVertexInputAttributeDesc> & pAttributeDefinitions ) noexcept
		{
			IAVertexInputAttributeArray vertexAttributeArray{};

			for( const auto & attributeDesc : pAttributeDefinitions )
			{
				TS3_DEBUG_CODE({
					if( !ecIsIAVertexAttributeIndexValid( attributeDesc.attributeIndex ) )
					{
					    ts3DebugOutputFmt( "Invalid IA Attribute Index: %u", static_cast<uint32>( attributeDesc.attributeIndex ) );
					    continue;
					}
				})

				if( ecIsIAVertexAttributeIndexValid( attributeDesc.attributeIndex ) )
				{
					const auto attributeIndex = static_cast<input_assembler_index_t>( attributeDesc.attributeIndex );
					const auto attributeFlag = ecMakeIAVertexAttributeFlag( attributeDesc.attributeIndex );

					vertexAttributeArray.attributes[attributeIndex] = attributeDesc.attributeInfo;

					if( !vertexAttributeArray.activeAttributesMask.isSet( attributeFlag ) )
					{
						vertexAttributeArray.activeAttributesMask.set( attributeFlag );
						vertexAttributeArray.activeAttributesNum += 1;
					}
				}
			}

			return vertexAttributeArray;
		}

		bool validateIAVertexInputAttributeArray( const IAVertexInputAttributeArray & pAttributeArray ) noexcept
		{
			uint32 activeAttributesNum = 0u;

			for( input_assembler_index_t attributeIndex = 0; attributeIndex < E_IA_CONSTANT_MAX_VERTEX_ATTRIBUTES_NUM; ++attributeIndex )
			{
				const auto attributeFlag = ecMakeIAVertexAttributeFlag( attributeIndex );
				if( pAttributeArray.activeAttributesMask.isSet( attributeFlag ) )
				{
					const auto & attributeInfo = pAttributeArray.attributes[attributeIndex];
					if( !attributeInfo.isActive() )
					{
						return false;
					}
				}

				++activeAttributesNum;
			}

			if( activeAttributesNum != pAttributeArray.activeAttributesNum )
			{
				return false;
			}

			return true;
		}

	}

} // namespace ts3::gpuapi
