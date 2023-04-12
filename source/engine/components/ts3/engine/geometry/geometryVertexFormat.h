
#pragma once

#ifndef __TS3_ENGINE_GEOMETRY_VERTEX_FORMAT_H__
#define __TS3_ENGINE_GEOMETRY_VERTEX_FORMAT_H__

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
		uint16 streamElementRelativeOffset;
		EVertexDataRate dataRate;
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
		uint32 strideInBytes = 0;

		bool active() const noexcept
		{
			return elementSizeInBytes > 0;
		}
	};

	struct GeometryVertexStreamLayoutBase
	{
		uint32 activeVertexStreamsNum;

		VertexStreamFormat * vertexStreamFormatsPtr;

		GeometryVertexStreamLayoutBase
	};

	using VertexAttributeFormatArray = std::array<VertexAttributeFormat, gpa::MAX_GEOMETRY_VERTEX_ATTRIBUTES_NUM>;
	using VertexStreamFormatArray = std::array<VertexStreamFormat, gpa::MAX_GEOMETRY_VERTEX_STREAMS_NUM>;

	struct P3SVertex
	{
		math::Vec3f position;
		math::Vec3f normal;
		math::Vec4f color;
		math::Vec2f texcoord0;
	};


} // namespace ts3

#endif // __TS3_ENGINE_GEOMETRY_VERTEX_FORMAT_H__
