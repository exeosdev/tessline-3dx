
#include "inputAssemblerVertexFormat.h"

namespace ts3::GpuAPI
{

	IAVertexInputLayoutDescriptor::IAVertexInputLayoutDescriptor(
			GPUDevice & pGPUDevice,
			PipelineDescriptorID pDescriptorID,
			const IAVertexInputFormat & pInputFormat,
			EPrimitiveTopology pPrimitiveTopology )
	: GraphicsPipelineDescriptor( pGPUDevice, pDescriptorID, EGraphicsPipelineDescriptorType::IAVertexInputLayout )
	, mActiveAttributesMask( pInputFormat.activeAttributesMask )
	, mActiveAttributesNum( pInputFormat.activeAttributesNum )
	, mPrimitiveTopology( pPrimitiveTopology )
	{}

	IAVertexInputLayoutDescriptor::~IAVertexInputLayoutDescriptor() = default;


	namespace StateMgmt
	{

		Bitmask<EIAVertexAttributeFlags> getIAVertexInputActiveAttributesMask(
				const IAVertexAttributeInfoArray & pVertexAttributes ) noexcept
		{
			Bitmask<EIAVertexAttributeFlags> attributesMask = 0;

			for( uint32 attribIndex = 0; attribIndex < E_GPU_SYSTEM_METRIC_IA_MAX_VERTEX_ATTRIBUTES_NUM; ++attribIndex )
			{
				if( pVertexAttributes[attribIndex].active() )
				{
					attributesMask.set( CxDefs::makeIAVertexAttributeFlag( attribIndex ) );
				}
			}

			return attributesMask;
		}

		uint32 getIAVertexInputActiveAttributesNum( const IAVertexAttributeInfoArray & pVertexAttributes ) noexcept
		{
			uint32 attributesNum = 0;

			for( uint32 attribIndex = 0; attribIndex < E_GPU_SYSTEM_METRIC_IA_MAX_VERTEX_ATTRIBUTES_NUM; ++attribIndex )
			{
				if( pVertexAttributes[attribIndex].active() )
				{
					++attributesNum;
				}
			}

			return attributesNum;
		}

		IAVertexInputFormat createIAVertexInputFormat(
				const ArrayView<IAVertexInputAttributeDesc> & pAttributeDefinitions ) noexcept
		{
			IAVertexInputFormat vertexInputFormat{};

			for( const auto & attributeDesc : pAttributeDefinitions )
			{
				if( !CxDefs::isIAVertexAttributeIndexValid( attributeDesc.attributeIndex ) )
				{
					ts3DebugOutputFmt( "Invalid IA Attribute Index: %u", attributeDesc.attributeIndex );
					continue;
				}

				vertexInputFormat.attributes[attributeDesc.attributeIndex] = attributeDesc.attributeInfo;
			}

			vertexInputFormat.activeAttributesMask = StateMgmt::getIAVertexInputActiveAttributesMask( vertexInputFormat.attributes );
			vertexInputFormat.activeAttributesNum = StateMgmt::getIAVertexInputActiveAttributesNum( vertexInputFormat.attributes );

			return vertexInputFormat;
		}

		bool validateIAVertexInputFormat( const IAVertexInputFormat & pInputFormat ) noexcept
		{
			if( ( pInputFormat.activeAttributesNum == 0 ) || ( pInputFormat.activeAttributesNum > CxDefs::IA_MAX_VERTEX_ATTRIBUTES_NUM ) )
			{
				return false;
			}

			if( !pInputFormat.activeAttributesMask.isSetAnyOf( CxDefs::IA_VERTEX_ATTRIBUTE_MASK_ALL ) )
			{
				return false;
			}

			uint32 activeAttributesNum = 0u;

			// Iterate over supported attribute index range.
			for( input_assembler_index_t attributeIndex = 0; attributeIndex < CxDefs::IA_MAX_VERTEX_ATTRIBUTES_NUM; ++attributeIndex )
			{
				// Compose the bitmask value for the current index and check
				// if that attribute is present in the activeAttributesMask.
				const auto attributeFlag = CxDefs::makeIAVertexAttributeFlag( attributeIndex );

				if( pInputFormat.activeAttributesMask.isSet( attributeFlag ) )
				{
					const auto & attributeInfo = pInputFormat.attributes[attributeIndex];
					if( !attributeInfo.active() )
					{
						// If the attribute is empty/inactive, but is present on the active attribute mask,
						// the specified input format is considered invalid (bitmask does not match the actual state).
						return false;
					}
				}

				++activeAttributesNum;
			}

			if( activeAttributesNum != pInputFormat.activeAttributesNum )
			{
				// If the specified number of active attributes is different from
				// the actual number, the attribute array is also considered invalid.
				return false;
			}

			return true;
		}

	}

} // namespace ts3::GpuAPI
