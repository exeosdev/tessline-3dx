
#include "geometryDataFormat.h"

namespace ts3
{

	GeometryDataFormatBase::GeometryDataFormatBase(
			VertexAttributeArrayLayoutBase * pAttribLayoutPtr,
			VertexStreamArrayLayoutBase * pStreamLayoutPtr )
	: _attribLayoutPtr( pAttribLayoutPtr )
	, _streamLayoutPtr( pStreamLayoutPtr )
	{}

	GeometryDataFormatBase::~GeometryDataFormatBase() = default;

	std::string GeometryDataFormatBase::getFormatStringID() const noexcept
	{
		return {}; // gpa::generateVertexFormatStringID( _attributes );
	}

	const VertexAttributeFormat & GeometryDataFormatBase::attribute( uint32 pAttributeIndex ) const
	{
		return _attribLayoutPtr->attributeFormat( pAttributeIndex );
	}

	const VertexStreamFormat & GeometryDataFormatBase::vertexStream( uint32 pStreamIndex ) const
	{
		return _streamLayoutPtr->streamFormat( pStreamIndex );
	}

	ArrayView<const uint16> GeometryDataFormatBase::activeVertexStreams() const noexcept
	{
		return bindArrayView( _streamLayoutPtr->activeVertexStreamArrayPtr, _streamLayoutPtr->activeVertexStreamsNum );
	}

	uint16 GeometryDataFormatBase::firstActiveVertexStream() const noexcept
	{
		const auto * activeStreamArray = _streamLayoutPtr->activeVertexStreamArrayPtr;
		return ( _streamLayoutPtr->activeVertexStreamsNum > 0 ) ? activeStreamArray[0] : gpuapi::cxdefs::IA_VERTEX_BUFFER_BINDING_INDEX_UNDEFINED;
	}

	uint32 GeometryDataFormatBase::vertexStreamElementSizeInBytes( uint32 pStreamIndex ) const
	{
		const auto & vertexStreamFormat = _streamLayoutPtr->streamFormat( pStreamIndex );
		return vertexStreamFormat.elementSizeInBytes;
	}

	gpuapi::EIndexDataFormat GeometryDataFormatBase::indexDataFormat() const noexcept
	{
		return _indexDataFormat;
	}

	uint32 GeometryDataFormatBase::indexElementSizeInBytes() const noexcept
	{
		return gpuapi::cxdefs::getIndexDataFormatByteSize( _indexDataFormat );
	}

	bool GeometryDataFormatBase::isAttributeSemanticsActive( EVertexAttributeSemanticsID pAttributeSemantics ) const noexcept
	{
		const auto attributeSemanticsBit = static_cast<uint32>( pAttributeSemantics );
		return _attribLayoutPtr->activeAttributeSemanticsMask.isSet( attributeSemanticsBit );
	}

	bool GeometryDataFormatBase::isFixedAttributeActive( EFixedVertexAttributeID pFixedAttribute ) const noexcept
	{
		const auto fixedAttributeSemanticsBit = cxdefs::getFixedVertexAttributeSemanticsFlags( pFixedAttribute );
		return _attribLayoutPtr->activeAttributeSemanticsMask.isSet( fixedAttributeSemanticsBit );
	}

	bool GeometryDataFormatBase::isIndexedGeometry() const noexcept
	{
		return _indexDataFormat != gpuapi::EIndexDataFormat::Undefined;
	}

	bool GeometryDataFormatBase::isAttributeSlotUsed( uint32 pAttributeIndex ) const
	{
		if( pAttributeIndex < _attribLayoutPtr->attributeArraySize )
		{
			const auto & attributeFormat = _attribLayoutPtr->attributeFormat( pAttributeIndex );
			return attributeFormat.active();
		}

		return false;
	}

	bool GeometryDataFormatBase::isAttributeActive( uint32 pAttributeIndex ) const
	{
		if( pAttributeIndex < _attribLayoutPtr->attributeArraySize )
		{
			const auto & attributeFormat = _attribLayoutPtr->attributeFormat( pAttributeIndex );
			return attributeFormat.isBaseAttribute();
		}

		return false;
	}

	bool GeometryDataFormatBase::isVertexStreamActive( uint32 pStreamIndex ) const
	{
		if( pStreamIndex < _streamLayoutPtr->vertexStreamArraySize )
		{
			const auto & vertexStreamFormat = _streamLayoutPtr->streamFormat( pStreamIndex );
			return vertexStreamFormat.active();
		}

		return false;
	}

	bool GeometryDataFormatBase::empty() const noexcept
	{
		return _attribLayoutPtr->activeAttributesNum == 0;
	}

	void GeometryDataFormatBase::setIndexDataFormat( gpuapi::EIndexDataFormat pIndexDataFormat )
	{
		_indexDataFormat = pIndexDataFormat;
	}

	void GeometryDataFormatBase::setPrimitiveTopology( gpuapi::EPrimitiveTopology pTopology )
	{
		_primitiveTopology = pTopology;
	}

	bool GeometryDataFormatBase::configureFixedAttribute(
			EFixedVertexAttributeID pFixedAttributeID,
			uint32 pStreamIndex,
			uint16 pStreamElementRelativeOffset )
	{
		const auto attributeBaseIndex = cxdefs::getFixedVertexAttributeBaseIndex( pFixedAttributeID );
		const auto attributeBaseFormat = cxdefs::getFixedVertexAttributeBaseFormat( pFixedAttributeID );
		const auto attributeSemanticsID = cxdefs::getFixedVertexAttributeSemanticsID( pFixedAttributeID );
		const auto attributeComponentsNum = cxdefs::getFixedVertexAttributeComponentsNum( pFixedAttributeID );
		const auto fixedAttributeFlags = cxdefs::getFixedVertexAttributeSemanticsFlags( pFixedAttributeID );
		const auto attributeDataRate =
				fixedAttributeFlags.isSetAnyOf( E_VERTEX_ATTRIBUTE_SEMANTICS_MASK_INSTANCE_ALL ) ? EVertexDataRate::PerInstance : EVertexDataRate::PerVertex;

		const auto configureResult = configureCustomAttribute(
				attributeBaseIndex,
				attributeSemanticsID,
				attributeBaseFormat,
				attributeComponentsNum,
				pStreamIndex,
				pStreamElementRelativeOffset,
				attributeDataRate );

		return configureResult;
	}

	bool GeometryDataFormatBase::configureCustomAttribute(
			uint32 pAttributeBaseIndex,
			VertexAttributeSemantics pSemantics,
			gpuapi::EVertexAttribFormat pAttributeBaseFormat,
			uint32 pAttributeComponentsNum,
			uint32 pStreamIndex,
			uint16 pStreamElementRelativeOffset,
			EVertexDataRate pAttributeDataRate )
	{
		if( !checkAttributeDefinitionParams( pAttributeBaseIndex, pAttributeComponentsNum, pStreamIndex, pAttributeDataRate ) )
		{
			return false;
		}

		if( !resolveAttributeSemanticsDefinition( pSemantics ) )
		{
			return false;
		}

		auto & baseAttributeDefinition = _attribLayoutPtr->attributeFormat( pAttributeBaseIndex );
		baseAttributeDefinition.semantics = std::move( pSemantics );
		baseAttributeDefinition.componentFormat = pAttributeBaseFormat;
		baseAttributeDefinition.componentsNum = numeric_cast<uint16>( pAttributeComponentsNum );
		baseAttributeDefinition.componentSizeInBytes = numeric_cast<uint16>( gpuapi::cxdefs::getVertexAttribFormatByteSize( pAttributeBaseFormat ) );
		baseAttributeDefinition.attributeTotalSizeInBytes = baseAttributeDefinition.componentSizeInBytes * pAttributeComponentsNum;
		baseAttributeDefinition.streamIndex = numeric_cast<uint16>( pStreamIndex );
		baseAttributeDefinition.streamElementRelativeOffset = pStreamElementRelativeOffset;
		baseAttributeDefinition.instanceRate = ( pAttributeDataRate == EVertexDataRate::PerInstance ) ? 1 : 0;

		_attribLayoutPtr->activeAttributesNum += 1;
		_attribLayoutPtr->activeAttributeSlotsNum += 1;
		_attribLayoutPtr->activeAttributesMask.set( gpuapi::cxdefs::makeIAVertexAttributeFlag( pAttributeBaseIndex ) );

		if( cxdefs::isVertexAttributeSemanticsValid( pSemantics.semID ) )
		{
			_attribLayoutPtr->activeAttributeSemanticsMask.set( static_cast<uint32>( pSemantics.semID ) );
			_attribLayoutPtr->semanticsMap[pSemantics.semName] = pAttributeBaseIndex;
		}

		if( pAttributeComponentsNum > 1 )
		{
			auto * attributePtr = &baseAttributeDefinition;

			for( uint32 iComponent = 1; iComponent < pAttributeComponentsNum; ++iComponent )
			{
				const auto subAttributeIndex = pAttributeBaseIndex + iComponent;

				auto & subAttributeDefinition = _attribLayoutPtr->attributeFormat( subAttributeIndex );
				subAttributeDefinition.componentFormat = pAttributeBaseFormat;
				attributePtr->nextComponent = &subAttributeDefinition;

				_attribLayoutPtr->activeAttributeSlotsNum += 1;
				_attribLayoutPtr->activeAttributesMask.set( gpuapi::cxdefs::makeIAVertexAttributeFlag( subAttributeIndex ) );

				attributePtr = attributePtr->nextComponent;
			}
		}

		updateStateWithNewAttribute( pAttributeBaseIndex );

		return true;
	}



	gpuapi::IAInputLayoutDefinition GeometryDataFormatBase::generateGpaInputLayoutDefinition() const noexcept
	{
		gpuapi::IAInputLayoutDefinition gpaInputLayoutDefinition;
		gpaInputLayoutDefinition.primitiveTopology = _primitiveTopology;
		gpaInputLayoutDefinition.activeAttributesMask = _attribLayoutPtr->activeAttributesMask;

		for( uint32 iAttribute = 0; iAttribute < _attribLayoutPtr->attributeArraySize; /* No increment */ )
		{
			const auto & baseAttributeFormat = _attribLayoutPtr->attributeFormat( iAttribute );
			if( baseAttributeFormat.isBaseAttribute() )
			{
				auto & gpaAttributeInfo = gpaInputLayoutDefinition.attributeArray[iAttribute];
				gpaAttributeInfo.streamIndex = baseAttributeFormat.streamIndex;
				gpaAttributeInfo.semanticName = baseAttributeFormat.semantics.semName;
				gpaAttributeInfo.semanticIndex = 0;
				gpaAttributeInfo.format = baseAttributeFormat.componentFormat;
				gpaAttributeInfo.relativeOffset = baseAttributeFormat.streamElementRelativeOffset;
				gpaAttributeInfo.instanceRate = baseAttributeFormat.instanceRate;

				if( baseAttributeFormat.componentsNum > 1 )
				{
					auto * attributeSubComponent = baseAttributeFormat.nextComponent;
					auto attributeSubComponentIndex = 1;

					while( attributeSubComponent )
					{
						auto & gpaSubAttributeInfo = gpaInputLayoutDefinition.attributeArray[iAttribute + attributeSubComponentIndex];
						gpaAttributeInfo.streamIndex = baseAttributeFormat.streamIndex;
						gpaAttributeInfo.semanticName = baseAttributeFormat.semantics.semName;
						gpaAttributeInfo.semanticIndex = numeric_cast<gpuapi::input_assembler_index_t>( attributeSubComponentIndex );
						gpaAttributeInfo.format = baseAttributeFormat.componentFormat;
						gpaAttributeInfo.instanceRate = baseAttributeFormat.instanceRate;
						gpaAttributeInfo.relativeOffset =
								baseAttributeFormat.streamElementRelativeOffset + attributeSubComponentIndex * baseAttributeFormat.componentSizeInBytes;

						attributeSubComponent = attributeSubComponent->nextComponent;
						attributeSubComponentIndex += 1;
					}
				}

				iAttribute += baseAttributeFormat.componentsNum;
			}
		}

		return gpaInputLayoutDefinition;
	}

	bool GeometryDataFormatBase::checkAttributeDefinitionParams(
			uint32 pAttributeBaseIndex,
			uint32 pAttributeComponentsNum,
			uint32 pStreamIndex,
			EVertexDataRate pAttributeDataRate )
	{
		if( ( pAttributeBaseIndex >= _attribLayoutPtr->attributeArraySize ) || ( pStreamIndex >= _streamLayoutPtr->vertexStreamArraySize ) )
		{
			return false;
		}

		if( pAttributeDataRate == EVertexDataRate::Undefined )
		{
			return false;
		}

		if( !gpuapi::cxdefs::isIAVertexAttributeIndexValid( pAttributeBaseIndex + pAttributeComponentsNum ) )
		{
			return false;
		}

		if( !checkAttributeSlotRangeFree( pAttributeBaseIndex, pAttributeComponentsNum ) )
		{
			return false;
		}

		if( !checkAttributeVertexStreamCompatibility( pStreamIndex, pAttributeDataRate ) )
		{
			return false;
		}

		return true;
	}

	bool GeometryDataFormatBase::checkAttributeSlotRangeFree(
			uint32 pAttributeBaseIndex,
			uint32 pAttributeComponentsNum ) const noexcept
	{
		for( uint32 nAttribute = 0; nAttribute < pAttributeComponentsNum; ++nAttribute )
		{
			const auto & attributeFormat = _attribLayoutPtr->attributeFormat( pAttributeBaseIndex + nAttribute );
			if( attributeFormat.active() )
			{
				return false;
			}
		}

		return true;
	}

	bool GeometryDataFormatBase::checkAttributeVertexStreamCompatibility(
			uint32 pStreamIndex,
			EVertexDataRate pAttributeDataRate ) const noexcept
	{
		auto & vertexStreamFormat = _streamLayoutPtr->streamFormat( pStreamIndex );
		return ( vertexStreamFormat.streamDataRate == EVertexDataRate::Undefined ) || ( vertexStreamFormat.streamDataRate == pAttributeDataRate );
	}

	void GeometryDataFormatBase::updateStateWithNewAttribute( uint32 pNewAttributeIndex )
	{
		auto & attributeFormat = _attribLayoutPtr->attributeFormat( pNewAttributeIndex );
		auto & vertexStreamFormat = _streamLayoutPtr->streamFormat( attributeFormat.streamIndex );

		ts3DebugAssert( attributeFormat.isBaseAttribute() );

		if( vertexStreamFormat.streamDataRate == EVertexDataRate::Undefined )
		{
			vertexStreamFormat.streamDataRate = attributeFormat.getAttributeDataRate();
			_streamLayoutPtr->activeVertexStreamArrayPtr[_streamLayoutPtr->activeVertexStreamsNum] = attributeFormat.streamIndex;
			_streamLayoutPtr->activeVertexStreamsMask.set( gpuapi::cxdefs::makeIAVertexBufferFlag( attributeFormat.streamIndex ) );
			_streamLayoutPtr->activeVertexStreamsNum += 1;
		}

		if( attributeFormat.streamElementRelativeOffset == gpuapi::cxdefs::IA_VERTEX_ATTRIBUTE_OFFSET_APPEND )
		{
			attributeFormat.streamElementRelativeOffset = vertexStreamFormat.elementSizeInBytes;
		}

		vertexStreamFormat.elementSizeInBytes += attributeFormat.attributeTotalSizeInBytes;
		vertexStreamFormat.activeAttributesMask.set( gpuapi::cxdefs::makeIAVertexAttributeFlag( pNewAttributeIndex ) );
		vertexStreamFormat.activeAttributesNum += 1;
	}

	bool GeometryDataFormatBase::resolveAttributeSemanticsDefinition(
			VertexAttributeSemantics & pAttributeSemantics )
	{
		if( cxdefs::isVertexAttributeSemanticsValid( pAttributeSemantics.semID )  )
		{
			pAttributeSemantics.semName = gpa::getStandardVertexAttributeSemanticsName( pAttributeSemantics.semID );
		}
		else if( pAttributeSemantics.semID == EVertexAttributeSemanticsID::Custom )
		{
			if( pAttributeSemantics.semName.empty() )
			{
				return false;
			}
		}
		else
		{
			return false;
		}

		if( isAttributeSemanticsActive( pAttributeSemantics.semID ) )
		{
			return false;
		}

		return true;
	}

	namespace gpa
	{

		const char * getStandardVertexAttributeSemanticsName( EVertexAttributeSemanticsID pSemanticsID )
		{
			switch( pSemanticsID )
			{
				ts3CaseReturn( EVertexAttributeSemanticsID::Position   , "POSITION"  );
				ts3CaseReturn( EVertexAttributeSemanticsID::Normal     , "NORMAL"    );
				ts3CaseReturn( EVertexAttributeSemanticsID::Tangent    , "TANGENT"   );
				ts3CaseReturn( EVertexAttributeSemanticsID::FixedColor , "COLOR"     );
				ts3CaseReturn( EVertexAttributeSemanticsID::TexCoord0  , "TEXCOORD0" );
				ts3CaseReturn( EVertexAttributeSemanticsID::TexCoord1  , "TEXCOORD1" );
				ts3CaseReturn( EVertexAttributeSemanticsID::TexCoord2  , "TEXCOORD2" );
				ts3CaseReturn( EVertexAttributeSemanticsID::TexCoord3  , "TEXCOORD3" );
				ts3CaseReturn( EVertexAttributeSemanticsID::TexCoord4  , "TEXCOORD4" );
				ts3CaseReturn( EVertexAttributeSemanticsID::TexCoord5  , "TEXCOORD5" );
				ts3CaseReturn( EVertexAttributeSemanticsID::TexCoord6  , "TEXCOORD6" );
				ts3CaseReturn( EVertexAttributeSemanticsID::TexCoord7  , "TEXCOORD7" );
				ts3CaseReturn( EVertexAttributeSemanticsID::Instance0  , "INSTANCE0" );
				ts3CaseReturn( EVertexAttributeSemanticsID::Instance1  , "INSTANCE1" );
				ts3CaseDefaultBreak();
			}

			return "#";
		}

		const char * getAttributeFormatStringIdentifier( gpuapi::EVertexAttribFormat pAttributeFormat );

		std::string getAttributeFormatString( uint32 pAttributeBaseIndex, const VertexAttributeFormat & pAttributeFormat );

		std::string generateVertexFormatStringID( const VertexAttributeFormatArray & pAttributeArray )
		{
			std::string vertexStreamStrings[gpa::MAX_GEOMETRY_VERTEX_STREAMS_NUM];

			for( uint32 iAttribute = 0; iAttribute < gpuapi::gpm::IA_MAX_VERTEX_ATTRIBUTES_NUM; ++iAttribute )
			{
				const auto & attributeFormat = pAttributeArray[iAttribute];
				if( attributeFormat.isBaseAttribute() )
				{
					const auto attributeFormatStr = getAttributeFormatString( iAttribute, attributeFormat );
					if( !vertexStreamStrings[attributeFormat.streamIndex].empty() )
					{
						vertexStreamStrings[attributeFormat.streamIndex].append( 1, '|' );
					}
					vertexStreamStrings[attributeFormat.streamIndex].append( attributeFormatStr );
				}
			}

			std::string resultStringID;

			for( uint32 iStream = 0; iStream < gpa::MAX_GEOMETRY_VERTEX_STREAMS_NUM; ++iStream )
			{
				if( !vertexStreamStrings[iStream].empty() )
				{
					if( !resultStringID.empty() )
					{
						resultStringID.append( 1, '_' );
					}
					resultStringID.append( 1, 's' );
					resultStringID.append( std::to_string( iStream ) );
					resultStringID.append( 1, '<' );
					resultStringID.append( vertexStreamStrings[iStream] );
					resultStringID.append( 1, '>' );
				}
			}

			return resultStringID;
		}

		const char * getAttributeSemanticsIDString( const std::string & pSemanticName )
		{
			if( pSemanticName == "POSITION"  ) return "P";
			if( pSemanticName == "NORMAL"    ) return "N";
			if( pSemanticName == "TANGENT"   ) return "T";
			if( pSemanticName == "BITANGENT" ) return "B";
			if( pSemanticName == "COLOR"     ) return "C";
			if( pSemanticName == "TEXCOORD0" ) return "UV0";
			if( pSemanticName == "TEXCOORD1" ) return "UV1";
			if( pSemanticName == "TEXCOORD2" ) return "UV2";
			if( pSemanticName == "TEXCOORD3" ) return "UV3";
			if( pSemanticName == "TEXCOORD4" ) return "UV4";
			if( pSemanticName == "TEXCOORD5" ) return "UV5";
			if( pSemanticName == "TEXCOORD6" ) return "UV6";
			if( pSemanticName == "TEXCOORD7" ) return "UV7";
			if( pSemanticName == "INSTANCE0" ) return "I0";
			if( pSemanticName == "INSTANCE1" ) return "I1";

			return nullptr;
		}

		const char * getAttributeFormatStringIdentifier( gpuapi::EVertexAttribFormat pAttributeFormat )
		{
			switch( pAttributeFormat )
			{
				ts3CaseReturn( gpuapi::EVertexAttribFormat::F16      , "1F16"  );
				ts3CaseReturn( gpuapi::EVertexAttribFormat::F32      , "1F32"  );
				ts3CaseReturn( gpuapi::EVertexAttribFormat::I8       , "1I8"   );
				ts3CaseReturn( gpuapi::EVertexAttribFormat::I16      , "1I16"  );
				ts3CaseReturn( gpuapi::EVertexAttribFormat::I32      , "1I32"  );
				ts3CaseReturn( gpuapi::EVertexAttribFormat::U8       , "1U8"   );
				ts3CaseReturn( gpuapi::EVertexAttribFormat::U16      , "1U16"  );
				ts3CaseReturn( gpuapi::EVertexAttribFormat::U32      , "1U32"  );
				ts3CaseReturn( gpuapi::EVertexAttribFormat::I8N      , "1I8N"  );
				ts3CaseReturn( gpuapi::EVertexAttribFormat::I16N     , "1I16N" );
				ts3CaseReturn( gpuapi::EVertexAttribFormat::U8N      , "1U8N"  );
				ts3CaseReturn( gpuapi::EVertexAttribFormat::U16N     , "1U16N" );
				ts3CaseReturn( gpuapi::EVertexAttribFormat::Vec2F16  , "2F16"  );
				ts3CaseReturn( gpuapi::EVertexAttribFormat::Vec2F32  , "2F32"  );
				ts3CaseReturn( gpuapi::EVertexAttribFormat::Vec2I8   , "2I8"   );
				ts3CaseReturn( gpuapi::EVertexAttribFormat::Vec2I16  , "2I16"  );
				ts3CaseReturn( gpuapi::EVertexAttribFormat::Vec2I32  , "2I32"  );
				ts3CaseReturn( gpuapi::EVertexAttribFormat::Vec2U8   , "2U8"   );
				ts3CaseReturn( gpuapi::EVertexAttribFormat::Vec2U16  , "2U16"  );
				ts3CaseReturn( gpuapi::EVertexAttribFormat::Vec2U32  , "2U32"  );
				ts3CaseReturn( gpuapi::EVertexAttribFormat::Vec2I8N  , "2I8N"  );
				ts3CaseReturn( gpuapi::EVertexAttribFormat::Vec2I16N , "2I16N" );
				ts3CaseReturn( gpuapi::EVertexAttribFormat::Vec2U8N  , "2U8N"  );
				ts3CaseReturn( gpuapi::EVertexAttribFormat::Vec2U16N , "2U16N" );
				ts3CaseReturn( gpuapi::EVertexAttribFormat::Vec3F32  , "3F32"  );
				ts3CaseReturn( gpuapi::EVertexAttribFormat::Vec3I32  , "3I32"  );
				ts3CaseReturn( gpuapi::EVertexAttribFormat::Vec3U32  , "3U32"  );
				ts3CaseReturn( gpuapi::EVertexAttribFormat::Vec4F16  , "4F16"  );
				ts3CaseReturn( gpuapi::EVertexAttribFormat::Vec4F32  , "4F32"  );
				ts3CaseReturn( gpuapi::EVertexAttribFormat::Vec4I8   , "4I8"   );
				ts3CaseReturn( gpuapi::EVertexAttribFormat::Vec4I16  , "4I16"  );
				ts3CaseReturn( gpuapi::EVertexAttribFormat::Vec4I32  , "4I32"  );
				ts3CaseReturn( gpuapi::EVertexAttribFormat::Vec4U8   , "4U8"   );
				ts3CaseReturn( gpuapi::EVertexAttribFormat::Vec4U16  , "4U16"  );
				ts3CaseReturn( gpuapi::EVertexAttribFormat::Vec4U32  , "4U32"  );
				ts3CaseReturn( gpuapi::EVertexAttribFormat::Vec4I8N  , "4I8N"  );
				ts3CaseReturn( gpuapi::EVertexAttribFormat::Vec4I16N , "4I16N" );
				ts3CaseReturn( gpuapi::EVertexAttribFormat::Vec4U8N  , "4U8N"  );
				ts3CaseReturn( gpuapi::EVertexAttribFormat::Vec4U16N , "4U16N" );
				ts3CaseDefaultBreak();
			}

			return "#";
		}

		std::string getAttributeFormatString( uint32 pAttributeBaseIndex, const VertexAttributeFormat & pAttributeFormat )
		{
			std::string formatStr;
			formatStr.reserve( 16 );
			formatStr.append( 1, 'a' );
			formatStr.append( std::to_string( pAttributeBaseIndex ) );

			if( const auto * semanticsIDCStr = getAttributeSemanticsIDString( pAttributeFormat.semantics.semName ) )
			{
				formatStr.append( semanticsIDCStr );
			}
			else
			{
				formatStr.append( pAttributeFormat.semantics.semName );
			}

			const auto baseFormatStr = getAttributeFormatStringIdentifier( pAttributeFormat.componentFormat );
			formatStr.append( 1, ':' );
			formatStr.append( baseFormatStr );

			if( pAttributeFormat.componentsNum > 1 )
			{
				formatStr.append( 1, '[' );
				formatStr.append( std::to_string( pAttributeFormat.componentsNum ) );
				formatStr.append( 1, ']' );
			}

			return formatStr;
		}

	}

}
