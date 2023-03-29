
#pragma once

#ifndef __TS3_ENGINE_GEOMETRY_COMMON_DEFS_H__
#define __TS3_ENGINE_GEOMETRY_COMMON_DEFS_H__

#include "../gpuapi/commonGpaDefs.h"
#include <ts3/gpuapi/state/inputAssemblerCommon.h>
#include <ts3/gpuapi/resources/gpuBufferReference.h>
#include <list>

namespace ts3
{

	struct GeometryDataFormatInfo;
	struct GeometryReference;
	struct GeometryStorageCreateInfo;

	class GeometryStorage;

	enum EVertexAttributeFlags : uint32
	{
		E_VERTEX_ATTRIBUTE_FLAG_POSITION_BIT = 0x01,
		E_VERTEX_ATTRIBUTE_FLAG_NORMAL_BIT = 0x02,
		E_VERTEX_ATTRIBUTE_FLAG_TANGENT_BIT = 0x04,
		E_VERTEX_ATTRIBUTE_FLAG_BI_TANGENT_BIT = 0x08,
		E_VERTEX_ATTRIBUTE_FLAG_FIXED_COLOR_BIT = 0x10,
		E_VERTEX_ATTRIBUTE_FLAG_TEX_COORD_ANY_BIT = 0x20,
		E_VERTEX_ATTRIBUTE_FLAG_TEX_COORD_0_BIT = 0x0100 | E_VERTEX_ATTRIBUTE_FLAG_TEX_COORD_ANY_BIT,
		E_VERTEX_ATTRIBUTE_FLAG_TEX_COORD_1_BIT = 0x0200 | E_VERTEX_ATTRIBUTE_FLAG_TEX_COORD_ANY_BIT,
		E_VERTEX_ATTRIBUTE_FLAG_TEX_COORD_2_BIT = 0x0400 | E_VERTEX_ATTRIBUTE_FLAG_TEX_COORD_ANY_BIT,
		E_VERTEX_ATTRIBUTE_FLAG_TEX_COORD_3_BIT = 0x0800 | E_VERTEX_ATTRIBUTE_FLAG_TEX_COORD_ANY_BIT,
		E_VERTEX_ATTRIBUTE_FLAG_TEX_COORD_4_BIT = 0x1000 | E_VERTEX_ATTRIBUTE_FLAG_TEX_COORD_ANY_BIT,
		E_VERTEX_ATTRIBUTE_FLAG_TEX_COORD_5_BIT = 0x2000 | E_VERTEX_ATTRIBUTE_FLAG_TEX_COORD_ANY_BIT,
		E_VERTEX_ATTRIBUTE_FLAG_TEX_COORD_6_BIT = 0x4000 | E_VERTEX_ATTRIBUTE_FLAG_TEX_COORD_ANY_BIT,
		E_VERTEX_ATTRIBUTE_FLAG_TEX_COORD_7_BIT = 0x8000 | E_VERTEX_ATTRIBUTE_FLAG_TEX_COORD_ANY_BIT,
	};

	namespace cxdefs
	{

		inline constexpr uint32 makeVertexAttributeID( uint16 pIndex, Bitmask<EVertexAttributeFlags> pAttributeFlags )
		{
			return static_cast<uint32>( ( ( uint32 )pIndex << 24 ) | ( ( uint32 )pAttributeFlags & 0xFFFFFF ) );
		}

	}

	enum class EGeometryVertexType : uint32
	{
		Monolithic = 1,
		MultiComponent = gpa::MAX_GEOMETRY_SEPARATE_VERTEX_COMPONENTS_NUM
	};

	enum class EVertexAttributeID : uint32
	{
		Undefined  = 0,
		Position   = cxdefs::makeVertexAttributeID(  0, E_VERTEX_ATTRIBUTE_FLAG_POSITION_BIT ),
		Normal     = cxdefs::makeVertexAttributeID(  1, E_VERTEX_ATTRIBUTE_FLAG_NORMAL_BIT ),
		Tangent    = cxdefs::makeVertexAttributeID(  2, E_VERTEX_ATTRIBUTE_FLAG_TANGENT_BIT ),
		BiTangent  = cxdefs::makeVertexAttributeID(  3, E_VERTEX_ATTRIBUTE_FLAG_BI_TANGENT_BIT ),
		FixedColor = cxdefs::makeVertexAttributeID(  4, E_VERTEX_ATTRIBUTE_FLAG_FIXED_COLOR_BIT ),
		TexCoord0  = cxdefs::makeVertexAttributeID(  5, E_VERTEX_ATTRIBUTE_FLAG_TEX_COORD_0_BIT ),
		TexCoord1  = cxdefs::makeVertexAttributeID(  6, E_VERTEX_ATTRIBUTE_FLAG_TEX_COORD_1_BIT ),
		TexCoord2  = cxdefs::makeVertexAttributeID(  7, E_VERTEX_ATTRIBUTE_FLAG_TEX_COORD_2_BIT ),
		TexCoord3  = cxdefs::makeVertexAttributeID(  8, E_VERTEX_ATTRIBUTE_FLAG_TEX_COORD_3_BIT ),
		TexCoord4  = cxdefs::makeVertexAttributeID(  9, E_VERTEX_ATTRIBUTE_FLAG_TEX_COORD_4_BIT ),
		TexCoord5  = cxdefs::makeVertexAttributeID( 10, E_VERTEX_ATTRIBUTE_FLAG_TEX_COORD_5_BIT ),
		TexCoord6  = cxdefs::makeVertexAttributeID( 11, E_VERTEX_ATTRIBUTE_FLAG_TEX_COORD_6_BIT ),
		TexCoord7  = cxdefs::makeVertexAttributeID( 12, E_VERTEX_ATTRIBUTE_FLAG_TEX_COORD_7_BIT ),
	};

	namespace cxdefs
	{

		inline uint16 getVertexAttributeIndex( EVertexAttributeID pAttributeID )
		{
			return static_cast<uint16>( ( ( uint32 )pAttributeID >> 24 ) & 0xFF );
		}

		inline Bitmask<EVertexAttributeFlags> getVertexAttributeFlags( EVertexAttributeID pAttributeID )
		{
			return static_cast<uint32>( ( uint32 )pAttributeID & 0xFFFFFF );
		}

	}

	struct GeometryDataFormatInfo
	{
		gpuapi::EIndexDataFormat indexDataFormat = gpuapi::EIndexDataFormat::Undefined;

		uint16 vertexElementByteSize;

		uint16 activeVertexComponentsNum;

		GeometryVertexComponentDataArray<uint16> vertexComponentSizeArray{};

		TS3_ATTR_NO_DISCARD uint16 indexElementByteSize() const noexcept
		{
			return cxdefs::getIndexDataFormatByteSize( indexDataFormat );
		}
	};

} // namespace ts3

#endif // __TS3_ENGINE_GEOMETRY_COMMON_DEFS_H__
