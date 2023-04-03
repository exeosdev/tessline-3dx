
#include "geometryContainer.h"
#include "geometryDataFormat.h"
#include "geometryStorage.h"

namespace ts3
{

	void GeometryDataRegion::append( const GeometryDataRegion & pOther )
	{
		ts3DebugAssert( pOther.offsetInElementsNum == ( offsetInElementsNum + sizeInElementsNum ) );
		sizeInElementsNum += pOther.sizeInElementsNum;
	}

	void GeometrySize::append( const GeometrySize & pOther )
	{
		indexElementsNum += pOther.indexElementsNum;
		vertexElementsNum += pOther.vertexElementsNum;
	}


	GeometryDataReference::GeometryDataReference( const GeometryDataFormat & pDataFormat )
	: dataFormat( &pDataFormat )
	{}

	GeometryDataReference::operator bool() const noexcept
	{
		return dataFormat && !dataFormat->empty();
	}

	bool GeometryDataReference::isIndexedGeometry() const noexcept
	{
		return dataFormat->isIndexedGeometry();
	}

	bool GeometryDataReference::isVertexStreamActive( uint32 pVertexStreamIndex ) const
	{
		return dataFormat->isVertexStreamActive( pVertexStreamIndex );
	}

	uint32 GeometryDataReference::vertexStreamElementSizeInBytes( uint32 pVertexStreamIndex ) const
	{
		return dataFormat->vertexStreamElementSizeInBytes( pVertexStreamIndex );
	}

	uint32 GeometryDataReference::vertexDataOffsetInElementsNum() const noexcept
	{
		const auto firstActiveStream = dataFormat->firstActiveVertexStream();
		ts3DebugAssert( firstActiveStream != gpuapi::cxdefs::IA_VERTEX_BUFFER_BINDING_INDEX_UNDEFINED );
		return vertexStreamDataRegions[firstActiveStream].offsetInElementsNum;
	}

	uint32 GeometryDataReference::vertexElementSizeInBytes() const noexcept
	{
		return dataFormat->vertexElementSizeInBytes();
	}

	uint32 GeometryDataReference::indexElementSizeInBytes() const noexcept
	{
		return dataFormat->indexElementSizeInBytes();
	}

	GeometrySize GeometryDataReference::calculateGeometrySize() const noexcept
	{
		const auto firstActiveStream = dataFormat->firstActiveVertexStream();
		ts3DebugAssert( firstActiveStream != gpuapi::cxdefs::IA_VERTEX_BUFFER_BINDING_INDEX_UNDEFINED );

		return {
			vertexStreamDataRegions[firstActiveStream].sizeInElementsNum,
			indexDataRegion.sizeInElementsNum
		};
	}

	void GeometryDataReference::append( const GeometryDataReference & pOther )
	{
		for( auto iVertexStream : dataFormat->activeVertexStreams() )
		{
			vertexStreamDataRegions[iVertexStream].append( pOther.vertexStreamDataRegions[iVertexStream] );
		}

		indexDataRegion.append( pOther.indexDataRegion );
	}


	GeometryReference::GeometryReference( GeometryStorage & pStorage )
	: storage( &pStorage )
	, geometryIndex( cxdefs::GEOMETRY_INDEX_INVALID )
	, dataReference( pStorage.mDataFormat )
	{}


	namespace gmutil
	{

		GeometryDataReference getGeometryDataReferenceSubRegion(
				const GeometryDataReference & pGeometryDataRef,
				uint32 pVertexDataOffsetInElementsNum,
				uint32 pVertexElementsNum,
				uint32 pIndexDataOffsetInElementsNum,
				uint32 pIndexElementsNum )
		{
			GeometryDataReference subGeometryDataRef{ *pGeometryDataRef.dataFormat };

			for( auto iVertexStream : pGeometryDataRef.dataFormat->activeVertexStreams() )
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
			 GeometryDataReference advGeometryDataRef{ *pGeometryDataRef.dataFormat };

			for( auto iVertexStream : pGeometryDataRef.dataFormat->activeVertexStreams() )
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
