
#include "geometryCommonDefs.h"
#include "geometryDataFormat.h"

namespace ts3
{

	namespace gmutil
	{

		GeometryDataReference getGeometryDataReferenceSubRegion(
				const GeometryDataReference & pGeometryDataRef,
				uint32 pVertexDataOffsetInElementsNum,
				uint32 pVertexElementsNum,
				uint32 pIndexDataOffsetInElementsNum,
				uint32 pIndexElementsNum )
		{
			GeometryDataReference subGeometryDataRef;

			for( uint32 iVertexStream = 0; iVertexStream < gpa::MAX_GEOMETRY_VERTEX_STREAMS_NUM; ++iVertexStream )
			{
				const auto & baseDataRegion = pGeometryDataRef.vertexStreamDataRegions[iVertexStream];
				if( baseDataRegion.sizeInElementsNum > 0 )
				{
					const auto relativeOffsetInElementsNum = getMinOf( pVertexDataOffsetInElementsNum, baseDataRegion.sizeInElementsNum );
					const auto sizeInElementsNum = getMinOf( pVertexElementsNum, baseDataRegion.sizeInElementsNum - relativeOffsetInElementsNum );

					auto & subDataRegion = subGeometryDataRef.vertexStreamDataRegions[iVertexStream];
					subDataRegion.elementSize = baseDataRegion.elementSize;
					subDataRegion.offsetInElementsNum = baseDataRegion.offsetInElementsNum + relativeOffsetInElementsNum;
					subDataRegion.sizeInElementsNum = sizeInElementsNum;
					subDataRegion.dataPtr = baseDataRegion.dataPtr + ( pVertexDataOffsetInElementsNum * baseDataRegion.elementSize );
				}
			}

			const auto & baseDataRegion = pGeometryDataRef.indexDataRegion;
			if( baseDataRegion.sizeInElementsNum > 0 )
			{
				const auto relativeOffsetInElementsNum = getMinOf( pIndexDataOffsetInElementsNum, baseDataRegion.sizeInElementsNum );
				const auto sizeInElementsNum = getMinOf( pIndexElementsNum, baseDataRegion.sizeInElementsNum - relativeOffsetInElementsNum );

				auto & subDataRegion = subGeometryDataRef.indexDataRegion;
				subDataRegion.elementSize = baseDataRegion.elementSize;
				subDataRegion.offsetInElementsNum = baseDataRegion.offsetInElementsNum + relativeOffsetInElementsNum;
				subDataRegion.sizeInElementsNum = sizeInElementsNum;
				subDataRegion.dataPtr = baseDataRegion.dataPtr + ( pIndexDataOffsetInElementsNum * baseDataRegion.elementSize );
			}

			return subGeometryDataRef;
		}

		GeometryDataReference advanceGeometryDataReference(
				const GeometryDataReference & pGeometryDataRef,
				uint32 pVertexElementsNum,
				uint32 pIndexElementsNum )
		{
			GeometryDataReference advGeometryDataRef;

			for( uint32 iVertexStream = 0; iVertexStream < gpa::MAX_GEOMETRY_VERTEX_STREAMS_NUM; ++iVertexStream )
			{
				const auto & baseDataRegion = pGeometryDataRef.vertexStreamDataRegions[iVertexStream];
				if( baseDataRegion.sizeInElementsNum > 0 )
				{
					auto & advDataRegion = advGeometryDataRef.vertexStreamDataRegions[iVertexStream];
					advDataRegion.elementSize = baseDataRegion.elementSize;
					advDataRegion.offsetInElementsNum = baseDataRegion.offsetInElementsNum + baseDataRegion.sizeInElementsNum;
					advDataRegion.sizeInElementsNum = pVertexElementsNum;
					advDataRegion.dataPtr = baseDataRegion.dataPtr + ( baseDataRegion.sizeInElementsNum * baseDataRegion.elementSize );
				}
			}

			const auto & baseDataRegion = pGeometryDataRef.indexDataRegion;
			if( baseDataRegion.sizeInElementsNum > 0 )
			{
				auto & advDataRegion = advGeometryDataRef.indexDataRegion;
				advDataRegion.elementSize = baseDataRegion.elementSize;
				advDataRegion.offsetInElementsNum = baseDataRegion.offsetInElementsNum + baseDataRegion.sizeInElementsNum;
				advDataRegion.sizeInElementsNum = pIndexElementsNum;
				advDataRegion.dataPtr = baseDataRegion.dataPtr + ( baseDataRegion.sizeInElementsNum * baseDataRegion.elementSize );
			}

			return advGeometryDataRef;
		}
	}

}
