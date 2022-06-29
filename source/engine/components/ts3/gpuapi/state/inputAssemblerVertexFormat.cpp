
#include "inputAssemblerVertexFormat.h"

namespace ts3::gpuapi
{

	IAVertexFormatDescriptor::IAVertexFormatDescriptor( Bitmask<EIAVertexAttributeFlags> pActiveAttributesMask, uint32 pActiveAttributesNum )
	: mActiveAttributesMask( pActiveAttributesMask )
	, mActiveAttributesNum( pActiveAttributesNum )
	{}

	IAVertexFormatDescriptor::~IAVertexFormatDescriptor() = default;

	IAVertexInputAttributeArray createIAVertexInputAttributeArray( const ArrayView<IAVertexInputAttributeDesc> & pAttributeDefinitions )
	{
		return {};
	}


	bool IAVertexInputAttributeArray::setAttribute( EIAVertexAttributeIndex pAttribIndex,
													const IAVertexInputAttributeInfo & pAttribInfo )
	{
		if( !ecIsIAVertexAttributeIndexValid( pAttribIndex ) || !pAttribInfo.isValid() )
		{
			return false;
		}

		if( pAttribInfo.isActive() )
		{
			_setAttribute( static_cast<uint32>( pAttribIndex ), pAttribInfo );
		}
		else
		{
			_resetAttribute( static_cast<uint32>( pAttribIndex ) );
		}

		return true;
	}

	void IAVertexInputAttributeArray::resetAttribute( EIAVertexAttributeIndex pAttribIndex )
	{
		if( !ecIsIAVertexAttributeIndexValid( pAttribIndex ) )
		{
			return;
		}

		_resetAttribute( static_cast<input_assembler_index_t>( pAttribIndex ) );
	}

	void IAVertexInputAttributeArray::resetAll()
	{
		for( input_assembler_index_t attribIndex = 0; attribIndex < E_GPU_SYSTEM_METRIC_IA_MAX_VERTEX_ATTRIBUTES_NUM; ++attribIndex )
		{
			_resetAttribute( attribIndex );
		}

		_activeAttributesMask.clear();
		_activeAttributesNum = 0;
	}

	bool IAVertexInputAttributeArray::isAttributeSet( EIAVertexAttributeIndex pAttribIndex ) const
	{
		if( ecIsIAVertexAttributeIndexValid( pAttribIndex ) )
		{
			return false;
		}

		const auto attribIndexValue = static_cast<input_assembler_index_t>( pAttribIndex );
		const auto attribFlag = ecMakeIAVertexAttributeFlag( pAttribIndex );

		return _attributes[attribIndexValue] && _activeAttributesMask.isSet( attribFlag );
	}

	void IAVertexInputAttributeArray::_setAttribute( input_assembler_index_t pAttribIndex,
													 const IAVertexInputAttributeInfo & pAttribInfo )
	{
		auto & internalAttributeInfo = _attributes[pAttribIndex];
		internalAttributeInfo = pAttribInfo;

		const auto attribFlag = ecMakeIAVertexAttributeFlag( pAttribIndex );

		if( !_activeAttributesMask.isSet( attribFlag ) )
		{
			_activeAttributesMask.set( attribFlag );
			_activeAttributesNum += 1;
		}
	}

	void IAVertexInputAttributeArray::_resetAttribute( input_assembler_index_t pAttribIndex )
	{
		auto & internalAttributeInfo = _attributes[pAttribIndex];
		internalAttributeInfo.reset();

		const auto attribFlag = ecMakeIAVertexAttributeFlag( pAttribIndex );

		if( _activeAttributesMask.isSet( attribFlag ) )
		{
			_activeAttributesMask.unset( attribFlag );
			_activeAttributesNum -= 1;
		}
	}

} // namespace ts3::gpuapi
