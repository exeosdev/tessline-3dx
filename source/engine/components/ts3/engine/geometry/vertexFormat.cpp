
#include "vertexFormat.h"

namespace ts3
{

	GeometryDataFormat::GeometryDataFormat()
	{}

	GeometryDataFormat::~GeometryDataFormat() = default;

	std::string GeometryDataFormat::getFormatStringID() const noexcept
	{
		return gpa::generateVertexFormatStringID( _attributeDescArray );
	}

	const VertexAttributeDesc & GeometryDataFormat::getAttribute( uint32 pAttributeIndex ) const noexcept
	{
		return _attributeDescArray.at( pAttributeIndex );
	}

	bool GeometryDataFormat::isAttributeSlotUsed( uint32 pAttributeIndex ) const noexcept
	{
		const auto & attributeDesc = _attributeDescArray.at( pAttributeIndex );
		return attributeDesc.componentFormat != gpuapi::EVertexAttribFormat::Undefined;
	}

	bool GeometryDataFormat::isAttributeActive( uint32 pAttributeIndex ) const noexcept
	{
		const auto & attributeDesc = _attributeDescArray.at( pAttributeIndex );
		return !attributeDesc.semanticName.empty();
	}

	bool GeometryDataFormat::isFixedAttributeActive( EFixedVertexAttributeID pFixedAttribute ) const noexcept
	{
		const auto fixedAttributeBit = cxdefs::getFixedVertexAttributeFlags( pFixedAttribute );
		return _activeFixedAttributesMask.isSet( fixedAttributeBit );
	}

	bool GeometryDataFormat::empty() const noexcept
	{
		return _activeAttributesNum == 0;
	}

	bool GeometryDataFormat::configureFixedAttribute(
			EFixedVertexAttributeID pAttribute,
			uint32 pStreamIndex,
			uint16 pStreamElementRelativeOffset )
	{
		const auto attributeBaseIndex = cxdefs::getFixedVertexAttributeBaseIndex( pAttribute );
		const auto attributeBaseFormat = cxdefs::getFixedVertexAttributeBaseFormat( pAttribute );
		const auto attributeComponentsNum = cxdefs::getFixedVertexAttributeComponentsNum( pAttribute );
		const auto fixedAttributeFlags = cxdefs::getFixedVertexAttributeFlags( pAttribute );
		const auto attributeDataRate =
				fixedAttributeFlags.isSetAnyOf( E_FIXED_VERTEX_ATTRIBUTE_MASK_INSTANCE_ALL ) ? EVertexDataRate::PerInstance : EVertexDataRate::PerVertex;

		std::string attributeSemanticName = gpa::getFixedAttributeDefaultSemanticName( pAttribute );

		const auto configureResult = configureCustomAttribute(
				attributeBaseIndex,
				std::move( attributeSemanticName ),
				attributeBaseFormat,
				attributeComponentsNum,
				pStreamIndex,
				pStreamElementRelativeOffset,
				attributeDataRate );

		if( configureResult )
		{
			_activeFixedAttributesMask.set( fixedAttributeFlags );
		}

		return configureResult;
	}

	bool GeometryDataFormat::configureCustomAttribute(
			uint32 pAttributeBaseIndex,
			std::string pSemanticName,
			gpuapi::EVertexAttribFormat pAttributeBaseFormat,
			uint32 pAttributeComponentsNum,
			uint32 pStreamIndex,
			uint16 pStreamElementRelativeOffset,
			EVertexDataRate pAttributeDataRate )
	{
		if( pSemanticName.empty() || ( pAttributeDataRate == EVertexDataRate::Undefined ) )
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

		auto & baseAttributeDefinition = _attributeDescArray[pAttributeBaseIndex];
		baseAttributeDefinition.semanticName = std::move( pSemanticName );
		baseAttributeDefinition.componentFormat = pAttributeBaseFormat;
		baseAttributeDefinition.componentsNum = pAttributeComponentsNum;
		baseAttributeDefinition.componentSizeInBytes = gpuapi::cxdefs::getVertexAttribFormatByteSize( pAttributeBaseFormat );
		baseAttributeDefinition.attributeTotalSizeInBytes = baseAttributeDefinition.componentSizeInBytes * pAttributeComponentsNum;
		baseAttributeDefinition.streamIndex = pStreamIndex;
		baseAttributeDefinition.streamElementRelativeOffset = pStreamElementRelativeOffset;
		baseAttributeDefinition.instanceRate = ( pAttributeDataRate == EVertexDataRate::PerInstance ) ? 1 : 0;

		_activeAttributesNum += 1;
		_activeAttributeSlotsNum += 1;
		_gpaActiveAttributesMask.set( gpuapi::cxdefs::makeIAVertexAttributeFlag( pAttributeBaseIndex ) );

		if( pAttributeComponentsNum > 1 )
		{
			auto * attributePtr = &baseAttributeDefinition;

			for( uint32 iComponent = 1; iComponent < pAttributeComponentsNum; ++iComponent )
			{
				auto & subAttribute = _attributeDescArray[iComponent];
				subAttribute.componentFormat = pAttributeBaseFormat;
				attributePtr->nextComponent = &subAttribute;

				_activeAttributeSlotsNum += 1;
				_gpaActiveAttributesMask.set( gpuapi::cxdefs::makeIAVertexAttributeFlag( pAttributeBaseIndex + iComponent ) );

				attributePtr = attributePtr->nextComponent;
			}
		}

		return true;
	}


	namespace gpa
	{

		GeometryVertexStreamLayout generateGeometryVertexStreamLayout( const VertexAttributeDescArray & pAttributeArray )
		{
			GeometryVertexStreamLayout vertexStreamLayout;

			for( uint32 iAttribute = 0; iAttribute < gpa::MAX_GEOMETRY_VERTEX_ATTRIBUTES_NUM; ++iAttribute )
			{
				const auto & attributeDesc = pAttributeArray[iAttribute];
				if( attributeDesc.isBaseAttribute() )
				{
					auto & vertexStreamDesc = vertexStreamLayout.vertexStreams[attributeDesc.streamIndex];

					if( vertexStreamDesc.streamDataRate == EVertexDataRate::Undefined )
					{
						vertexStreamDesc.streamDataRate = attributeDesc.getAttributeDataRate();
					}

					if( vertexStreamDesc.streamDataRate != attributeDesc.getAttributeDataRate() )
					{
						return {};
					}

					vertexStreamDesc.elementSizeInBytes += attributeDesc.attributeTotalSizeInBytes;
					vertexStreamDesc.activeAttributesMask.set( gpuapi::cxdefs::makeIAVertexAttributeFlag( iAttribute ) );
					vertexStreamDesc.activeAttributesNum += 1;
				}
			}

			for( uint32 iStream = 0; iStream < gpa::MAX_GEOMETRY_VERTEX_STREAMS_NUM; ++iStream )
			{
				const auto & vertexStreamDesc = vertexStreamLayout.vertexStreams[iStream];
				if( vertexStreamDesc.active() )
				{
					vertexStreamLayout.activeVertexStreamsMask.set( gpuapi::cxdefs::makeIAVertexBufferFlag( iStream ) );
					vertexStreamLayout.activeVertexStreamsNum += 1;
				}
			}

			return vertexStreamLayout;
		}

		const char * getFixedAttributeDefaultSemanticName( EFixedVertexAttributeID pFixedAttribute );

		const char * getAttributeFormatStringIdentifier( gpuapi::EVertexAttribFormat pAttributeFormat );

		std::string getAttributeFormatString( uint32 pAttributeBaseIndex, const VertexAttributeDesc & pAttributeDesc );

		std::string generateVertexFormatStringID( const VertexAttributeDescArray & pAttributeArray )
		{
			std::string vertexStreamStrings[gpa::MAX_GEOMETRY_VERTEX_STREAMS_NUM];

			for( uint32 iAttribute = 0; iAttribute < gpuapi::gpm::IA_MAX_VERTEX_ATTRIBUTES_NUM; ++iAttribute )
			{
				const auto & attributeDesc = pAttributeArray[iAttribute];
				if( attributeDesc.isBaseAttribute() )
				{
					const auto attributeFormatStr = getAttributeFormatString( iAttribute, attributeDesc );
					if( !vertexStreamStrings[attributeDesc.streamIndex].empty() )
					{
						vertexStreamStrings[attributeDesc.streamIndex].append( 1, '|' );
					}
					vertexStreamStrings[attributeDesc.streamIndex].append( attributeFormatStr );
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

		const char * getFixedAttributeDefaultSemanticName( EFixedVertexAttributeID pFixedAttribute )
		{
			switch( pFixedAttribute )
			{
				ts3CaseReturn( EFixedVertexAttributeID::Position   , "POSITION" );
				ts3CaseReturn( EFixedVertexAttributeID::Normal     , "NORMAL" );
				ts3CaseReturn( EFixedVertexAttributeID::Tangent    , "TANGENT" );
				ts3CaseReturn( EFixedVertexAttributeID::BiTangent  , "BITANGENT" );
				ts3CaseReturn( EFixedVertexAttributeID::FixedColor , "COLOR" );
				ts3CaseReturn( EFixedVertexAttributeID::TexCoord0  , "TEXCOORD0" );
				ts3CaseReturn( EFixedVertexAttributeID::TexCoord1  , "TEXCOORD1" );
				ts3CaseReturn( EFixedVertexAttributeID::TexCoord2  , "TEXCOORD2" );
				ts3CaseReturn( EFixedVertexAttributeID::TexCoord3  , "TEXCOORD3" );
				ts3CaseReturn( EFixedVertexAttributeID::TexCoord4  , "TEXCOORD4" );
				ts3CaseReturn( EFixedVertexAttributeID::TexCoord5  , "TEXCOORD5" );
				ts3CaseReturn( EFixedVertexAttributeID::TexCoord6  , "TEXCOORD6" );
				ts3CaseReturn( EFixedVertexAttributeID::TexCoord7  , "TEXCOORD7" );
				ts3CaseDefaultBreak();
			}

			return "UNDEFINED";
		}

		const char * getAttributeSemanticIDString( const std::string & pSemanticName )
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

		std::string getAttributeFormatString( uint32 pAttributeBaseIndex, const VertexAttributeDesc & pAttributeDesc )
		{
			std::string formatStr;
			formatStr.reserve( 16 );
			formatStr.append( 1, 'a' );
			formatStr.append( std::to_string( pAttributeBaseIndex ) );

			if( const auto * semanticIDCStr = getAttributeSemanticIDString( pAttributeDesc.semanticName ) )
			{
				formatStr.append( semanticIDCStr );
			}
			else
			{
				formatStr.append( pAttributeDesc.semanticName );
			}

			const auto baseFormatStr = getAttributeFormatStringIdentifier( pAttributeDesc.componentFormat );
			formatStr.append( 1, ':' );
			formatStr.append( baseFormatStr );

			if( pAttributeDesc.componentsNum > 1 )
			{
				formatStr.append( 1, '[' );
				formatStr.append( std::to_string( pAttributeDesc.componentsNum ) );
				formatStr.append( 1, ']' );
			}

			return formatStr;
		}

	}

}
