
#pragma once

#ifndef __TS3_ENGINE_GEOMETRY_DATA_FORMAT_H__
#define __TS3_ENGINE_GEOMETRY_DATA_FORMAT_H__

#include "geometryCommonDefs.h"
#include "geometryVertexFormat.h"
#include <unordered_map>

namespace ts3
{
	
	struct GeometryDataFormatBaseProperties
	{
		uint32 activeAttributesNum = 0;
		uint32 activeAttributeSlotsNum = 0;
		uint32 activeVertexStreamsNum = 0;
		uint32 vertexElementSizeInBytes = 0;
		Bitmask<gpuapi::EIAVertexAttributeFlags> activeAttributesMask;
		Bitmask<EVertexAttributeSemanticsFlags> activeAttributeSemanticsMask;
		Bitmask<gpuapi::EIAVertexStreamBindingFlags> activeVertexStreamsMask;
	};

	class GeometryDataFormatBase
	{
	public:
		GeometryDataFormatBase(
				VertexAttributeArrayLayoutBase * pAttribLayoutPtr,
				VertexStreamArrayLayoutBase * pStreamLayoutPtr );

		virtual ~GeometryDataFormatBase();

		TS3_ATTR_NO_DISCARD std::string getFormatStringID() const noexcept;

		TS3_ATTR_NO_DISCARD const VertexAttributeFormat & attribute( uint32 pAttributeIndex ) const;

		TS3_ATTR_NO_DISCARD const VertexAttributeFormat & attribute( EVertexAttributeSemanticsID pAttributeSemantics ) const;

		TS3_ATTR_NO_DISCARD const VertexStreamFormat & vertexStream( uint32 pStreamIndex ) const;

		TS3_ATTR_NO_DISCARD ArrayView<const uint16> activeVertexStreams() const noexcept;

		TS3_ATTR_NO_DISCARD uint32 activeVertexStreamsNum() const noexcept;

		TS3_ATTR_NO_DISCARD uint16 firstActiveVertexStream() const noexcept;

		TS3_ATTR_NO_DISCARD uint32 vertexElementSizeInBytes() const noexcept;

		TS3_ATTR_NO_DISCARD uint32 vertexStreamElementSizeInBytes( uint32 pStreamIndex ) const;

		TS3_ATTR_NO_DISCARD gpuapi::EIndexDataFormat indexDataFormat() const noexcept;

		TS3_ATTR_NO_DISCARD uint32 indexElementSizeInBytes() const noexcept;

		TS3_ATTR_NO_DISCARD bool isAttributeSemanticsActive( EVertexAttributeSemanticsID pAttributeSemantics ) const noexcept;

		TS3_ATTR_NO_DISCARD bool isFixedAttributeActive( EFixedVertexAttributeID pFixedAttribute ) const noexcept;

		TS3_ATTR_NO_DISCARD bool isIndexedGeometry() const noexcept;

		TS3_ATTR_NO_DISCARD bool isAttributeSlotUsed( uint32 pAttributeIndex ) const;

		TS3_ATTR_NO_DISCARD bool isAttributeActive( uint32 pAttributeIndex ) const;

		TS3_ATTR_NO_DISCARD bool isVertexStreamActive( uint32 pStreamIndex ) const;

		TS3_ATTR_NO_DISCARD bool empty() const noexcept;

		void setIndexDataFormat( gpuapi::EIndexDataFormat pIndexDataFormat );

		void setPrimitiveTopology( gpuapi::EPrimitiveTopology pTopology );

		bool configureFixedAttribute(
				EFixedVertexAttributeID pFixedAttributeID,
				uint32 pStreamIndex,
				uint16 pStreamElementRelativeOffset = gpuapi::cxdefs::IA_VERTEX_ATTRIBUTE_OFFSET_APPEND );

		template <typename TVertex, typename TAttribute>
		bool configureFixedAttribute(
				EFixedVertexAttributeID pFixedAttributeID,
				uint32 pStreamIndex,
				TAttribute TVertex::* pAttributePtr );

		bool configureCustomAttribute(
				uint32 pAttributeBaseIndex,
				VertexAttributeSemantics pSemantics,
				gpuapi::EVertexAttribFormat pAttributeBaseFormat,
				uint32 pAttributeComponentsNum,
				uint32 pStreamIndex,
				uint16 pStreamElementRelativeOffset = gpuapi::cxdefs::IA_VERTEX_ATTRIBUTE_OFFSET_APPEND,
				EVertexDataRate pAttributeDataRate = EVertexDataRate::PerVertex );

		template <typename TVertex, typename TAttribute>
		bool configureCustomAttribute(
				uint32 pAttributeBaseIndex,
				VertexAttributeSemantics pSemantics,
				gpuapi::EVertexAttribFormat pAttributeBaseFormat,
				uint32 pAttributeComponentsNum,
				uint32 pStreamIndex,
				TAttribute TVertex::* pAttributePtr,
				EVertexDataRate pAttributeDataRate = EVertexDataRate::PerVertex );

		template <typename TVertex, typename TAttribute>
		bool configureCustomAttributeAuto(
				uint32 pAttributeBaseIndex,
				VertexAttributeSemantics pSemantics,
				uint32 pStreamIndex,
				TAttribute TVertex::* pAttributePtr,
				EVertexDataRate pAttributeDataRate = EVertexDataRate::PerVertex );

		TS3_ATTR_NO_DISCARD gpuapi::IAInputLayoutDefinition generateGpaInputLayoutDefinition() const noexcept;

	private:
		template <typename TAttribute>
		TS3_ATTR_NO_DISCARD bool checkAttributeAutoDataFormat(
				gpuapi::EVertexAttribFormat pAttributeComponentFormat,
				uint16 pAttributeComponentsNum ) const noexcept;

		TS3_ATTR_NO_DISCARD bool checkAttributeDefinitionParams(
				uint32 pAttributeBaseIndex,
				uint32 pAttributeComponentsNum,
				uint32 pStreamIndex,
				EVertexDataRate pAttributeDataRate );

		TS3_ATTR_NO_DISCARD bool checkAttributeSlotRangeFree(
				uint32 pAttributeBaseIndex,
				uint32 pAttributeComponentsNum ) const noexcept;

		TS3_ATTR_NO_DISCARD bool checkAttributeVertexStreamCompatibility(
				uint32 pStreamIndex,
				EVertexDataRate pAttributeDataRate ) const noexcept;

		void updateStateWithNewAttribute( uint32 pNewAttributeIndex );

		TS3_ATTR_NO_DISCARD bool resolveAttributeSemanticsDefinition(
				VertexAttributeSemantics & pAttributeSemantics );

	private:
		VertexAttributeArrayLayoutBase * _attribLayoutPtr;
		VertexStreamArrayLayoutBase * _streamLayoutPtr;
		gpuapi::EIndexDataFormat _indexDataFormat;
		gpuapi::EPrimitiveTopology _primitiveTopology;
	};

	template <size_t tVertexAttributeArraySize = 0, size_t tVertexStreamArraySize = 0>
	class GeometryDataFormat : public GeometryDataFormatBase
	{
	public:
		using VertexAttributeArrayLayoutType = VertexAttributeArrayLayout<tVertexAttributeArraySize>;
		using VertexStreamArrayLayoutType = VertexStreamArrayLayout<tVertexStreamArraySize>;

		GeometryDataFormat()
		: GeometryDataFormatBase( &_attribLayout, &_streamLayout )
		{}

	private:
		VertexAttributeArrayLayoutType _attribLayout;
		VertexStreamArrayLayoutType _streamLayout;
	};

	template <typename TVertex, typename TAttribute>
	inline bool GeometryDataFormatBase::configureFixedAttribute(
			EFixedVertexAttributeID pFixedAttributeID,
			uint32 pStreamIndex,
			TAttribute TVertex::* pAttributePtr )
	{
		const auto fixedAttributeFormat = cxdefs::getFixedVertexAttributeBaseFormat( pFixedAttributeID );
		const auto fixedAttributeComponentsNum = cxdefs::getFixedVertexAttributeComponentsNum( pFixedAttributeID );
		if( !checkAttributeAutoDataFormat<TAttribute>( fixedAttributeFormat, fixedAttributeComponentsNum ) )
		{
			return false;
		}

		return configureFixedAttribute(
				pFixedAttributeID,
				pStreamIndex,
				memberOffset( pAttributePtr ) );
	}

	template <typename TVertex, typename TAttribute>
	inline bool GeometryDataFormatBase::configureCustomAttribute(
			uint32 pAttributeBaseIndex,
			VertexAttributeSemantics pSemantics,
			gpuapi::EVertexAttribFormat pAttributeBaseFormat,
			uint32 pAttributeComponentsNum,
			uint32 pStreamIndex,
			TAttribute TVertex::* pAttributePtr,
			EVertexDataRate pAttributeDataRate )
	{
		if( !checkAttributeAutoDataFormat<TAttribute>( pAttributeBaseFormat, pAttributeComponentsNum ) )
		{
			return false;
		}

		return configureCustomAttribute(
				pAttributeBaseIndex,
				std::move( pSemantics ),
				pAttributeBaseFormat,
				pAttributeComponentsNum,
				pStreamIndex,
				memberOffset( pAttributePtr ),
				pAttributeDataRate );
	}

	template <typename TVertex, typename TAttribute>
	inline bool GeometryDataFormatBase::configureCustomAttributeAuto(
			uint32 pAttributeBaseIndex,
			VertexAttributeSemantics pSemantics,
			uint32 pStreamIndex,
			TAttribute TVertex::* pAttributePtr,
			EVertexDataRate pAttributeDataRate )
	{
		using AttributeTraits = gpuapi::VertexAttribFormatDataTypeTraits<TAttribute>;
		if( !checkAttributeAutoDataFormat<TAttribute>( AttributeTraits::sVertexAttribFormat, AttributeTraits::sVertexAttribComponentsNum ) )
		{
			return false;
		}

		return configureCustomAttribute(
				pAttributeBaseIndex,
				std::move( pSemantics ),
				AttributeTraits::sVertexAttribFormat,
				AttributeTraits::sVertexAttribComponentsNum,
				pStreamIndex,
				memberOffset( pAttributePtr ),
				pAttributeDataRate );
	}

	template <typename TAttribute>
	inline bool GeometryDataFormatBase::checkAttributeAutoDataFormat(
			gpuapi::EVertexAttribFormat pAttributeComponentFormat,
			uint16 pAttributeComponentsNum ) const noexcept
	{
		const auto componentByteSize = gpuapi::cxdefs::getVertexAttribFormatByteSize( pAttributeComponentFormat );
		const auto attributeByteSize = componentByteSize * pAttributeComponentsNum;

		return sizeof( TAttribute ) == attributeByteSize;
	}

	namespace gpa
	{

		TS3_ATTR_NO_DISCARD const char * getStandardVertexAttributeSemanticsName( EVertexAttributeSemanticsID pSemanticsID );

		TS3_ATTR_NO_DISCARD std::string generateVertexFormatStringID( const VertexAttributeFormatArray & pAttributeArray );

	}


} // namespace ts3

#endif // __TS3_ENGINE_GEOMETRY_DATA_FORMAT_H__