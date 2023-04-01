
#pragma once

#ifndef __TS3_ENGINE_GEOMETRY_COMMON_DEFS_H__
#define __TS3_ENGINE_GEOMETRY_COMMON_DEFS_H__

#include "../gpuapi/commonGpaDefs.h"
#include <ts3/gpuapi/state/inputAssemblerCommon.h>
#include <ts3/gpuapi/resources/gpuBufferReference.h>
#include <list>

namespace ts3
{

	struct GeometryReference;
	struct GeometryStorageCreateInfo;

	class GeometryDataFormat;
	class GeometryDataGpuTransfer;
	class GeometryManager;
	class GeometryStorage;

	using GeometryRefHandle = const GeometryReference *;

	enum EVertexAttributeSemanticsFlags : uint32
	{
		E_VERTEX_ATTRIBUTE_SEMANTICS_FLAG_POSITION_BIT    =     0x01,
		E_VERTEX_ATTRIBUTE_SEMANTICS_FLAG_NORMAL_BIT      =     0x02,
		E_VERTEX_ATTRIBUTE_SEMANTICS_FLAG_TANGENT_BIT     =     0x04,
		E_VERTEX_ATTRIBUTE_SEMANTICS_FLAG_BI_TANGENT_BIT  =     0x08,
		E_VERTEX_ATTRIBUTE_SEMANTICS_FLAG_FIXED_COLOR_BIT =     0x10,
		E_VERTEX_ATTRIBUTE_SEMANTICS_FLAG_TEX_COORD_0_BIT =   0x0100,
		E_VERTEX_ATTRIBUTE_SEMANTICS_FLAG_TEX_COORD_1_BIT =   0x0200,
		E_VERTEX_ATTRIBUTE_SEMANTICS_FLAG_TEX_COORD_2_BIT =   0x0400,
		E_VERTEX_ATTRIBUTE_SEMANTICS_FLAG_TEX_COORD_3_BIT =   0x0800,
		E_VERTEX_ATTRIBUTE_SEMANTICS_FLAG_TEX_COORD_4_BIT =   0x1000,
		E_VERTEX_ATTRIBUTE_SEMANTICS_FLAG_TEX_COORD_5_BIT =   0x2000,
		E_VERTEX_ATTRIBUTE_SEMANTICS_FLAG_TEX_COORD_6_BIT =   0x4000,
		E_VERTEX_ATTRIBUTE_SEMANTICS_FLAG_TEX_COORD_7_BIT =   0x8000,
		E_VERTEX_ATTRIBUTE_SEMANTICS_MASK_TEX_COORD_ALL   =   0xFF00,
		E_VERTEX_ATTRIBUTE_SEMANTICS_FLAG_RESERVED_13_BIT = 0x010000,
		E_VERTEX_ATTRIBUTE_SEMANTICS_FLAG_RESERVED_14_BIT = 0x020000,
		E_VERTEX_ATTRIBUTE_SEMANTICS_FLAG_RESERVED_15_BIT = 0x040000,
		E_VERTEX_ATTRIBUTE_SEMANTICS_MASK_RESERVED_ALL    = 0x070000,
		E_VERTEX_ATTRIBUTE_SEMANTICS_FLAG_INSTANCE_0_BIT  = 0x100000,
		E_VERTEX_ATTRIBUTE_SEMANTICS_FLAG_INSTANCE_1_BIT  = 0x200000,
		E_VERTEX_ATTRIBUTE_SEMANTICS_MASK_INSTANCE_ALL    = 0x300000,
		E_VERTEX_ATTRIBUTE_SEMANTICS_MASK_ALL             = 0x37FFFF,

		E_VERTEX_ATTRIBUTE_SEMANTICS_MASK_INSTANCE_0_OVERLAP =
				E_VERTEX_ATTRIBUTE_SEMANTICS_FLAG_TEX_COORD_3_BIT |
				E_VERTEX_ATTRIBUTE_SEMANTICS_FLAG_TEX_COORD_4_BIT |
				E_VERTEX_ATTRIBUTE_SEMANTICS_FLAG_TEX_COORD_5_BIT |
				E_VERTEX_ATTRIBUTE_SEMANTICS_FLAG_TEX_COORD_6_BIT,

		E_VERTEX_ATTRIBUTE_SEMANTICS_MASK_INSTANCE_1_OVERLAP =
				E_VERTEX_ATTRIBUTE_SEMANTICS_FLAG_TEX_COORD_7_BIT |
				E_VERTEX_ATTRIBUTE_SEMANTICS_FLAG_RESERVED_13_BIT |
				E_VERTEX_ATTRIBUTE_SEMANTICS_FLAG_RESERVED_14_BIT |
				E_VERTEX_ATTRIBUTE_SEMANTICS_FLAG_RESERVED_15_BIT,
	};

	namespace cxdefs
	{

		inline constexpr uint32 declareVertexAttributeSemanticsID( EVertexAttributeSemanticsFlags pSemanticsFlags )
		{
			return static_cast<uint32>( pSemanticsFlags );
		}

		inline constexpr uint64 declareFixedVertexAttributeID(
				uint16 pBaseIndex,
				uint16 pComponentsNum,
				gpuapi::EVertexAttribFormat pBaseFormat,
				Bitmask<EVertexAttributeSemanticsFlags> pSemanticsFlags )
		{
			return static_cast<uint64>(
					( ( ( uint64 )pBaseIndex & 0xF ) << 60 ) |
					( ( ( uint64 )pComponentsNum & 0xF ) << 56 ) |
					( ( ( uint64 )pBaseFormat & Limits<uint32>::maxValue ) << 24 ) |
					( ( uint64 )pSemanticsFlags & E_VERTEX_ATTRIBUTE_SEMANTICS_MASK_ALL ) );
		}

	}

	enum class EVertexAttributeSemanticsID : uint32
	{
		Undefined  = 0,
		Custom     = static_cast<uint32>( E_VERTEX_ATTRIBUTE_SEMANTICS_MASK_ALL ) ^ Limits<uint32>::maxValue,
		Position   = cxdefs::declareVertexAttributeSemanticsID( E_VERTEX_ATTRIBUTE_SEMANTICS_FLAG_POSITION_BIT    ),
		Normal     = cxdefs::declareVertexAttributeSemanticsID( E_VERTEX_ATTRIBUTE_SEMANTICS_FLAG_NORMAL_BIT      ),
		Tangent    = cxdefs::declareVertexAttributeSemanticsID( E_VERTEX_ATTRIBUTE_SEMANTICS_FLAG_TANGENT_BIT     ),
		BiTangent  = cxdefs::declareVertexAttributeSemanticsID( E_VERTEX_ATTRIBUTE_SEMANTICS_FLAG_BI_TANGENT_BIT  ),
		FixedColor = cxdefs::declareVertexAttributeSemanticsID( E_VERTEX_ATTRIBUTE_SEMANTICS_FLAG_FIXED_COLOR_BIT ),
		TexCoord0  = cxdefs::declareVertexAttributeSemanticsID( E_VERTEX_ATTRIBUTE_SEMANTICS_FLAG_TEX_COORD_0_BIT ),
		TexCoord1  = cxdefs::declareVertexAttributeSemanticsID( E_VERTEX_ATTRIBUTE_SEMANTICS_FLAG_TEX_COORD_1_BIT ),
		TexCoord2  = cxdefs::declareVertexAttributeSemanticsID( E_VERTEX_ATTRIBUTE_SEMANTICS_FLAG_TEX_COORD_2_BIT ),
		TexCoord3  = cxdefs::declareVertexAttributeSemanticsID( E_VERTEX_ATTRIBUTE_SEMANTICS_FLAG_TEX_COORD_3_BIT ),
		TexCoord4  = cxdefs::declareVertexAttributeSemanticsID( E_VERTEX_ATTRIBUTE_SEMANTICS_FLAG_TEX_COORD_4_BIT ),
		TexCoord5  = cxdefs::declareVertexAttributeSemanticsID( E_VERTEX_ATTRIBUTE_SEMANTICS_FLAG_TEX_COORD_5_BIT ),
		TexCoord6  = cxdefs::declareVertexAttributeSemanticsID( E_VERTEX_ATTRIBUTE_SEMANTICS_FLAG_TEX_COORD_6_BIT ),
		TexCoord7  = cxdefs::declareVertexAttributeSemanticsID( E_VERTEX_ATTRIBUTE_SEMANTICS_FLAG_TEX_COORD_7_BIT ),
		Instance0  = cxdefs::declareVertexAttributeSemanticsID( E_VERTEX_ATTRIBUTE_SEMANTICS_FLAG_INSTANCE_0_BIT  ),
		Instance1  = cxdefs::declareVertexAttributeSemanticsID( E_VERTEX_ATTRIBUTE_SEMANTICS_FLAG_INSTANCE_1_BIT  ),
	};

	enum class EFixedVertexAttributeID : uint64
	{
		Undefined  = 0,
		Position   = cxdefs::declareFixedVertexAttributeID(  0, 1, gpuapi::EVertexAttribFormat::Vec3F32, E_VERTEX_ATTRIBUTE_SEMANTICS_FLAG_POSITION_BIT ),
		Normal     = cxdefs::declareFixedVertexAttributeID(  1, 1, gpuapi::EVertexAttribFormat::Vec3F32, E_VERTEX_ATTRIBUTE_SEMANTICS_FLAG_NORMAL_BIT ),
		Tangent    = cxdefs::declareFixedVertexAttributeID(  2, 1, gpuapi::EVertexAttribFormat::Vec3F32, E_VERTEX_ATTRIBUTE_SEMANTICS_FLAG_TANGENT_BIT ),
		BiTangent  = cxdefs::declareFixedVertexAttributeID(  3, 1, gpuapi::EVertexAttribFormat::Vec3F32, E_VERTEX_ATTRIBUTE_SEMANTICS_FLAG_BI_TANGENT_BIT ),
		FixedColor = cxdefs::declareFixedVertexAttributeID(  4, 1, gpuapi::EVertexAttribFormat::Vec4F32, E_VERTEX_ATTRIBUTE_SEMANTICS_FLAG_FIXED_COLOR_BIT ),
		TexCoord0  = cxdefs::declareFixedVertexAttributeID(  5, 1, gpuapi::EVertexAttribFormat::Vec2F32, E_VERTEX_ATTRIBUTE_SEMANTICS_FLAG_TEX_COORD_0_BIT ),
		TexCoord1  = cxdefs::declareFixedVertexAttributeID(  6, 1, gpuapi::EVertexAttribFormat::Vec2F32, E_VERTEX_ATTRIBUTE_SEMANTICS_FLAG_TEX_COORD_1_BIT ),
		TexCoord2  = cxdefs::declareFixedVertexAttributeID(  7, 1, gpuapi::EVertexAttribFormat::Vec2F32, E_VERTEX_ATTRIBUTE_SEMANTICS_FLAG_TEX_COORD_2_BIT ),
		TexCoord3  = cxdefs::declareFixedVertexAttributeID(  8, 1, gpuapi::EVertexAttribFormat::Vec2F32, E_VERTEX_ATTRIBUTE_SEMANTICS_FLAG_TEX_COORD_3_BIT ),
		TexCoord4  = cxdefs::declareFixedVertexAttributeID(  9, 1, gpuapi::EVertexAttribFormat::Vec2F32, E_VERTEX_ATTRIBUTE_SEMANTICS_FLAG_TEX_COORD_4_BIT ),
		TexCoord5  = cxdefs::declareFixedVertexAttributeID( 10, 1, gpuapi::EVertexAttribFormat::Vec2F32, E_VERTEX_ATTRIBUTE_SEMANTICS_FLAG_TEX_COORD_5_BIT ),
		TexCoord6  = cxdefs::declareFixedVertexAttributeID( 11, 1, gpuapi::EVertexAttribFormat::Vec2F32, E_VERTEX_ATTRIBUTE_SEMANTICS_FLAG_TEX_COORD_6_BIT ),
		TexCoord7  = cxdefs::declareFixedVertexAttributeID( 12, 1, gpuapi::EVertexAttribFormat::Vec2F32, E_VERTEX_ATTRIBUTE_SEMANTICS_FLAG_TEX_COORD_7_BIT ),
		Reserved13 = cxdefs::declareFixedVertexAttributeID( 13, 1, gpuapi::EVertexAttribFormat::Vec4F32, E_VERTEX_ATTRIBUTE_SEMANTICS_FLAG_RESERVED_13_BIT ),
		Reserved14 = cxdefs::declareFixedVertexAttributeID( 14, 1, gpuapi::EVertexAttribFormat::Vec4F32, E_VERTEX_ATTRIBUTE_SEMANTICS_FLAG_RESERVED_14_BIT ),
		Reserved15 = cxdefs::declareFixedVertexAttributeID( 15, 1, gpuapi::EVertexAttribFormat::Vec4F32, E_VERTEX_ATTRIBUTE_SEMANTICS_FLAG_RESERVED_15_BIT ),
		Instance0  = cxdefs::declareFixedVertexAttributeID(  8, 4, gpuapi::EVertexAttribFormat::Vec4F32, E_VERTEX_ATTRIBUTE_SEMANTICS_FLAG_INSTANCE_0_BIT ),
		Instance1  = cxdefs::declareFixedVertexAttributeID( 12, 4, gpuapi::EVertexAttribFormat::Vec4F32, E_VERTEX_ATTRIBUTE_SEMANTICS_FLAG_INSTANCE_1_BIT ),
	};

	namespace cxdefs
	{

		inline uint16 getFixedVertexAttributeBaseIndex( EFixedVertexAttributeID pFixedAttributeID )
		{
			return static_cast<uint16>( ( ( uint64 )pFixedAttributeID >> 60 ) & 0xF );
		}

		inline uint16 getFixedVertexAttributeComponentsNum( EFixedVertexAttributeID pFixedAttributeID )
		{
			return static_cast<uint16>( ( ( uint64 )pFixedAttributeID >> 56 ) & 0xF );
		}

		inline gpuapi::EVertexAttribFormat getFixedVertexAttributeBaseFormat( EFixedVertexAttributeID pFixedAttributeID )
		{
			return static_cast<gpuapi::EVertexAttribFormat>( ( ( uint64 )pFixedAttributeID >> 24 ) & Limits<uint32>::maxValue );
		}

		inline uint32 getFixedVertexAttributeComponentByteSize( EFixedVertexAttributeID pFixedAttributeID )
		{
			return gpuapi::cxdefs::getVertexAttribFormatByteSize( getFixedVertexAttributeBaseFormat( pFixedAttributeID ) );
		}

		inline uint32 getFixedVertexAttributeByteSize( EFixedVertexAttributeID pFixedAttributeID )
		{
			return getFixedVertexAttributeComponentsNum( pFixedAttributeID ) * getFixedVertexAttributeComponentByteSize( pFixedAttributeID );
		}

		inline Bitmask<EVertexAttributeSemanticsFlags> getFixedVertexAttributeSemanticsFlags( EFixedVertexAttributeID pFixedAttributeID )
		{
			return static_cast<uint32>( ( uint64 )pFixedAttributeID & 0xFFFFFF );
		}

		inline EVertexAttributeSemanticsID getFixedVertexAttributeSemanticsID( EFixedVertexAttributeID pFixedAttributeID )
		{
			return static_cast<EVertexAttributeSemanticsID>( declareVertexAttributeSemanticsID( getFixedVertexAttributeSemanticsFlags( pFixedAttributeID ) ) );
		}

		inline bool isVertexAttributeSemanticsValid( EVertexAttributeSemanticsID pSemanticsID )
		{
			return Bitmask<uint32>( pSemanticsID ).isSetAnyOf( E_VERTEX_ATTRIBUTE_SEMANTICS_MASK_ALL );
		}

		inline bool isVertexAttributeSemanticsDefined( EVertexAttributeSemanticsID pSemanticsID )
		{
			return isVertexAttributeSemanticsValid( pSemanticsID ) || ( pSemanticsID == EVertexAttributeSemanticsID::Custom );
		}

	}

	template <typename TRegion>
	struct VertexIndexDataReference
	{
		TRegion indexDataRegion;

		GeometryVertexStreamGenericArray<TRegion> vertexStreamDataRegions;
	};

	struct GeometryDataRegion
	{
		const byte * dataPtr = nullptr;
		uint32 elementSize = 0;
		uint32 offsetInElementsNum = 0;
		uint32 sizeInElementsNum = 0;
	};

	using GeometryDataReference = VertexIndexDataReference<GeometryDataRegion>;

	namespace gmutil
	{

		TS3_ATTR_NO_DISCARD GeometryDataReference getGeometryDataReferenceSubRegion(
				const GeometryDataReference & pGeometryDataRef,
				uint32 pVertexDataOffsetInElementsNum,
				uint32 pVertexElementsNum,
				uint32 pIndexDataOffsetInElementsNum,
				uint32 pIndexElementsNum );

		TS3_ATTR_NO_DISCARD GeometryDataReference advanceGeometryDataReference(
				const GeometryDataReference & pGeometryDataRef,
				uint32 pVertexElementsNum,
				uint32 pIndexElementsNum );

	}

//	struct GeometryDataOffset
//	{
//		uint32 indexDataOffsetInBytes = 0;
//		uint32 vertexDataOffsetInBytes = 0;
//	};
//
	struct GeometryDataSize
	{
		uint32 indexDataSizeInBytes = 0;
		uint32 indexElementsNum = 0;
		uint32 vertexDataSizeInBytes = 0;
		uint32 vertexElementsNum = 0;
	};
//
//	struct GeometryDataRegion
//	{
//		GeometryDataOffset offset;
//		GeometryDataSize size;
//	};

	struct GeometryDataFormatInfo
	{
		gpuapi::EIndexDataFormat indexDataFormat = gpuapi::EIndexDataFormat::Undefined;

		GeometryVertexStreamGenericArray<uint16> vertexStreamElementSizeArray{};

		TS3_ATTR_NO_DISCARD uint16 indexElementByteSize() const noexcept
		{
			return cxdefs::getIndexDataFormatByteSize( indexDataFormat );
		}
	};

} // namespace ts3

#endif // __TS3_ENGINE_GEOMETRY_COMMON_DEFS_H__
