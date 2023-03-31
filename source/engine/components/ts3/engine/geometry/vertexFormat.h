
#pragma once

#ifndef __TS3_ENGINE_VERTEX_FORMAT_H__
#define __TS3_ENGINE_VERTEX_FORMAT_H__

#include "geometryCommonDefs.h"

namespace ts3
{

	enum class EVertexDataRate : enum_default_value_t
	{
		Undefined,
		PerInstance,
		PerVertex
	};

	struct VertexAttributeSemantics
	{
		EVertexAttributeSemanticsID semID;
		std::string semName;

		VertexAttributeSemantics()
		: semID( EVertexAttributeSemanticsID::Undefined )
		{}

		VertexAttributeSemantics( EVertexAttributeSemanticsID pSemanticsID )
		: semID( pSemanticsID )
		{}

		VertexAttributeSemantics( std::string pSemanticsName )
		: semID( EVertexAttributeSemanticsID::Custom )
		, semName( std::move( pSemanticsName ) )
		{}
	};

	struct VertexAttributeDesc
	{
		uint32 attributeBaseIndex;
		VertexAttributeSemantics semantics;
		gpuapi::EVertexAttribFormat baseFormat;
		uint32 componentsNum;
		uint32 streamIndex;
		uint16 streamElementRelativeOffset = gpuapi::cxdefs::IA_VERTEX_ATTRIBUTE_OFFSET_APPEND;
		EVertexDataRate dataRate = EVertexDataRate::PerVertex;
	};

	struct VertexAttributeFormat
	{
		VertexAttributeFormat * nextComponent = nullptr;
		gpuapi::EVertexAttribFormat componentFormat = gpuapi::EVertexAttribFormat::Undefined;
		VertexAttributeSemantics semantics;
		uint16 componentsNum;
		uint16 componentSizeInBytes;
		uint16 attributeTotalSizeInBytes;
		uint16 streamIndex;
		uint16 streamElementRelativeOffset;
		uint16 instanceRate;

		bool active() const noexcept
		{
			return componentFormat != gpuapi::EVertexAttribFormat::Undefined;
		}

		bool isBaseAttribute() const noexcept
		{
			return !semantics.semName.empty();
		}

		EVertexDataRate getAttributeDataRate() const noexcept
		{
			return ( instanceRate != 0 ) ? EVertexDataRate::PerInstance : EVertexDataRate::PerVertex;
		}
	};

	struct VertexStreamFormat
	{
		EVertexDataRate streamDataRate = EVertexDataRate::Undefined;
		Bitmask<gpuapi::EIAVertexAttributeFlags> activeAttributesMask;
		uint16 activeAttributesNum = 0;
		uint16 elementSizeInBytes = 0;

		bool active() const noexcept
		{
			return elementSizeInBytes > 0;
		}
	};

	using VertexAttributeFormatArray = std::array<VertexAttributeFormat, gpa::MAX_GEOMETRY_VERTEX_ATTRIBUTES_NUM>;
	using VertexStreamFormatArray = std::array<VertexStreamFormat, gpa::MAX_GEOMETRY_VERTEX_STREAMS_NUM>;

	class GeometryDataFormat
	{
	public:
		GeometryDataFormat();
		~GeometryDataFormat();

		TS3_ATTR_NO_DISCARD std::string getFormatStringID() const noexcept;

		TS3_ATTR_NO_DISCARD const VertexAttributeFormat & attribute( uint32 pAttributeIndex ) const;

		TS3_ATTR_NO_DISCARD const VertexStreamFormat & vertexStream( uint32 pVertexStreamIndex ) const;

		TS3_ATTR_NO_DISCARD gpuapi::EIndexDataFormat indexDataFormat() const noexcept;

		TS3_ATTR_NO_DISCARD uint32 indexElementSizeInBytes() const noexcept;

		TS3_ATTR_NO_DISCARD bool isFixedAttributeActive( EFixedVertexAttributeID pFixedAttribute ) const noexcept;

		TS3_ATTR_NO_DISCARD bool isAttributeSlotUsed( uint32 pAttributeIndex ) const;

		TS3_ATTR_NO_DISCARD bool isAttributeActive( uint32 pAttributeIndex ) const;

		TS3_ATTR_NO_DISCARD bool isVertexStreamActive( uint32 pVertexStreamIndex ) const;

		TS3_ATTR_NO_DISCARD bool empty() const noexcept;

		TS3_ATTR_NO_DISCARD gpuapi::IAInputLayoutDefinition generateGpaInputLayoutDefinition() const noexcept;

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

		void setIndexDataFormat( gpuapi::EIndexDataFormat pIndexDataFormat );

		void setPrimitiveTopology( gpuapi::EPrimitiveTopology pTopology );

	private:
		template <typename TAttribute>
		TS3_ATTR_NO_DISCARD bool checkAttributeAutoDataFormat(
				gpuapi::EVertexAttribFormat pAttributeComponentFormat,
				uint16 pAttributeComponentsNum ) const noexcept;

		TS3_ATTR_NO_DISCARD bool checkAttributeSlotRangeFree(
				uint32 pAttributeBaseIndex,
				uint32 pAttributeComponentsNum ) const noexcept;

		TS3_ATTR_NO_DISCARD bool checkAttributeVertexStreamCompatibility(
				uint32 pVertexStreamIndex,
				EVertexDataRate pAttributeDataRate ) const noexcept;

		void updateStateWithNewAttribute( uint32 pNewAttributeIndex );

	private:
		VertexAttributeFormatArray _attributes;
		VertexStreamFormatArray _vertexStreams;
		uint32 _activeAttributesNum = 0;
		uint32 _activeAttributeSlotsNum = 0;
		uint32 _activeVertexStreamsNum = 0;
		Bitmask<gpuapi::EIAVertexAttributeFlags> _activeAttributesMask;
		Bitmask<EVertexAttributeSemanticFlags> _activeFixedAttributesMask;
		Bitmask<gpuapi::EIAVertexStreamBindingFlags> _activeVertexStreamsMask;
		gpuapi::EIndexDataFormat _indexDataFormat;
		gpuapi::EPrimitiveTopology _primitiveTopology;
	};

	template <typename TVertex, typename TAttribute>
	inline bool GeometryDataFormat::configureFixedAttribute(
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
	inline bool GeometryDataFormat::configureCustomAttribute(
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
	inline bool GeometryDataFormat::configureCustomAttributeAuto(
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
	inline bool GeometryDataFormat::checkAttributeAutoDataFormat(
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

#endif // __TS3_ENGINE_VERTEX_FORMAT_H__
