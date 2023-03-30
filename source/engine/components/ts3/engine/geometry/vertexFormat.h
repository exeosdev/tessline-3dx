
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

	struct VertexFormat2DDefault
	{
		math::Vec2f position;
		math::RGBAColorR32Norm color;
		math::Vec2f texCoords[2];
	};

	struct VertexFormat3DDefaultTex2D0
	{
		math::Vec3f position;
		math::Vec3f normal;
		math::Vec3f tangent;
		math::Vec3f biTangent;
		math::RGBAColorR32Norm color;
		math::Vec2f texCoord0;
	};

	struct VertexFormat3DDefaultTex2DMulti4
	{
		math::Vec3f position;
		math::Vec3f normal;
		math::Vec3f tangent;
		math::Vec3f biTangent;
		math::RGBAColorR32Norm color;
		math::Vec2f texCoord[4];
	};

	struct VertexAttributeDesc
	{
		VertexAttributeDesc * nextComponent = nullptr;
		gpuapi::EVertexAttribFormat componentFormat = gpuapi::EVertexAttribFormat::Undefined;
		std::string semanticName;
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
			return !semanticName.empty();
		}

		EVertexDataRate getAttributeDataRate() const noexcept
		{
			return ( instanceRate != 0 ) ? EVertexDataRate::PerInstance : EVertexDataRate::PerVertex;
		}
	};

	struct VertexStreamDesc
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

	using VertexAttributeDescArray = std::array<VertexAttributeDesc, gpa::MAX_GEOMETRY_VERTEX_ATTRIBUTES_NUM>;
	using VertexStreamDescArray = std::array<VertexStreamDesc, gpa::MAX_GEOMETRY_VERTEX_STREAMS_NUM>;

	struct GeometryVertexStreamLayout
	{
		Bitmask<gpuapi::EIAVertexStreamBindingFlags> activeVertexStreamsMask;
		uint16 activeVertexStreamsNum;
		VertexStreamDescArray vertexStreams;

		bool empty() const noexcept
		{
			return activeVertexStreamsNum == 0;
		}
	};

	class GeometryDataFormat
	{
	public:
		GeometryDataFormat();
		~GeometryDataFormat();

		TS3_ATTR_NO_DISCARD std::string getFormatStringID() const noexcept;

		TS3_ATTR_NO_DISCARD const VertexAttributeDesc & getAttribute( uint32 pAttributeIndex ) const noexcept;

		TS3_ATTR_NO_DISCARD bool isAttributeSlotUsed( uint32 pAttributeIndex ) const noexcept;

		TS3_ATTR_NO_DISCARD bool isAttributeActive( uint32 pAttributeIndex ) const noexcept;

		TS3_ATTR_NO_DISCARD bool isFixedAttributeActive( EFixedVertexAttributeID pFixedAttribute ) const noexcept;

		TS3_ATTR_NO_DISCARD bool empty() const noexcept;

		bool configureFixedAttribute(
				EFixedVertexAttributeID pAttribute,
				uint32 pStreamIndex,
				uint16 pStreamElementRelativeOffset = gpuapi::cxdefs::IA_VERTEX_ATTRIBUTE_OFFSET_APPEND );

		template <typename TVertex, typename TAttribute>
		bool configureFixedAttribute(
				EFixedVertexAttributeID pAttribute,
				uint32 pStreamIndex,
				TAttribute TVertex::* pAttributePtr );

		bool configureCustomAttribute(
				uint32 pAttributeBaseIndex,
				std::string pSemanticName,
				gpuapi::EVertexAttribFormat pAttributeBaseFormat,
				uint32 pAttributeComponentsNum,
				uint32 pStreamIndex,
				uint16 pStreamElementRelativeOffset = gpuapi::cxdefs::IA_VERTEX_ATTRIBUTE_OFFSET_APPEND,
				EVertexDataRate pAttributeDataRate = EVertexDataRate::PerVertex );

		template <typename TVertex, typename TAttribute>
		bool configureCustomAttribute(
				uint32 pAttributeBaseIndex,
				std::string pSemanticName,
				gpuapi::EVertexAttribFormat pAttributeBaseFormat,
				uint32 pAttributeComponentsNum,
				uint32 pStreamIndex,
				TAttribute TVertex::* pAttributePtr,
				EVertexDataRate pAttributeDataRate = EVertexDataRate::PerVertex );

		template <typename TVertex, typename TAttribute>
		bool configureCustomAttributeAuto(
				uint32 pAttributeBaseIndex,
				std::string pSemanticName,
				uint32 pStreamIndex,
				TAttribute TVertex::* pAttributePtr,
				EVertexDataRate pAttributeDataRate = EVertexDataRate::PerVertex );

		void setIndexDataFormat( gpuapi::EIndexDataFormat pIndexDataFormat );
		void setPrimitiveTopology( gpuapi::EPrimitiveTopology pTopology );

	private:
		bool checkAttributeSlotRangeFree( uint32 pAttributeBaseIndex, uint32 pAttributeComponentsNum ) const;
		bool setAttributeActive( uint32 pAttributeIndex );
		void setAttributeFormat( uint32 pAttributeIndex, gpuapi::EVertexAttribFormat pAttributeBaseFormat, uint32 pAttributeComponentsNum );
		void setAttributeStreamLocation( uint32 pAttributeIndex, uint32 pStreamIndex, uint64 pStreamRelativeOffset );

		template <typename TAttribute>
		bool checkAttributeAutoDataFormat( gpuapi::EVertexAttribFormat pAttributeComponentFormat, uint16 pAttributeComponentsNum );

	private:
		uint32 _activeAttributesNum;
		uint32 _activeAttributeSlotsNum;
		VertexAttributeDescArray _attributeDescArray;
		gpuapi::EIndexDataFormat _indexDataFormat;
		Bitmask<gpuapi::EIAVertexAttributeFlags> _gpaActiveAttributesMask;
		Bitmask<EFixedVertexAttributeFlags> _activeFixedAttributesMask;
	};

	template <typename TVertex, typename TAttribute>
	inline bool GeometryDataFormat::configureFixedAttribute(
			EFixedVertexAttributeID pAttribute,
			uint32 pStreamIndex,
			TAttribute TVertex::* pAttributePtr )
	{
		const auto fixedAttributeFormat = cxdefs::getFixedVertexAttributeBaseFormat( pAttribute );
		const auto fixedAttributeComponentsNum = cxdefs::getFixedVertexAttributeComponentsNum( pAttribute );
		if( !checkAttributeAutoDataFormat<TAttribute>( fixedAttributeFormat, fixedAttributeComponentsNum ) )
		{
			return false;
		}

		return configureFixedAttribute(
				pAttribute,
				pStreamIndex,
				memberOffset( pAttributePtr ) );
	}

	template <typename TVertex, typename TAttribute>
	inline bool GeometryDataFormat::configureCustomAttribute(
			uint32 pAttributeBaseIndex,
			std::string pSemanticName,
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
				std::move( pSemanticName ),
				pAttributeBaseFormat,
				pAttributeComponentsNum,
				pStreamIndex,
				memberOffset( pAttributePtr ),
				pAttributeDataRate );
	}

	template <typename TVertex, typename TAttribute>
	inline bool GeometryDataFormat::configureCustomAttributeAuto(
			uint32 pAttributeBaseIndex,
			std::string pSemanticName,
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
				std::move( pSemanticName ),
				AttributeTraits::sVertexAttribFormat,
				AttributeTraits::sVertexAttribComponentsNum,
				pStreamIndex,
				memberOffset( pAttributePtr ),
				pAttributeDataRate );
	}

	template <typename TAttribute>
	inline bool GeometryDataFormat::checkAttributeAutoDataFormat(
			gpuapi::EVertexAttribFormat pAttributeComponentFormat,
			uint16 pAttributeComponentsNum )
	{
		const auto componentByteSize = gpuapi::cxdefs::getVertexAttribFormatByteSize( pAttributeComponentFormat );
		const auto attributeByteSize = componentByteSize * pAttributeComponentsNum;

		return sizeof( TAttribute ) == attributeByteSize;
	}

	namespace gpa
	{

		TS3_ATTR_NO_DISCARD GeometryVertexStreamLayout generateGeometryVertexStreamLayout( const VertexAttributeDescArray & pAttributeArray );

		TS3_ATTR_NO_DISCARD std::string generateVertexFormatStringID( const VertexAttributeDescArray & pAttributeArray );

		TS3_ATTR_NO_DISCARD const char * getFixedAttributeDefaultSemanticName( EFixedVertexAttributeID pFixedAttribute );

		TS3_ATTR_NO_DISCARD const char * getAttributeFormatStringIdentifier( gpuapi::EVertexAttribFormat pAttributeFormat );

	}


} // namespace ts3

#endif // __TS3_ENGINE_VERTEX_FORMAT_H__
