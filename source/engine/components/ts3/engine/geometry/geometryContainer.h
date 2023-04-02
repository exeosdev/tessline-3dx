
#pragma once

#ifndef __TS3_ENGINE_GEOMETRY_CONTAINER_H__
#define __TS3_ENGINE_GEOMETRY_CONTAINER_H__

#include "geometryDataFormat.h"

namespace ts3
{

	struct GeometryContainerStorageCapacity
	{
		uint32 indexDataCapacityInElementsNum = 0;
		uint32 vertexDataCapacityInElementsNum = 0;
	};

	struct GeometryContainerStorageMetrics
	{
		GeometryContainerStorageCapacity capacity;
		GeometrySize geometrySize;
		uint32 indexDataAllocationOffsetInElementsNum = 0;
		uint32 vertexDataAllocationOffsetInElementsNum = 0;
	};

	class GeometryContainer
	{
	public:
		const GeometryDataFormat & mDataFormat;

	public:
		GeometryContainer( const GeometryDataFormat & pDataFormat );

		TS3_ATTR_NO_DISCARD bool isStorageInitialized() const noexcept;

		TS3_ATTR_NO_DISCARD bool isIndexedGeometryContainer() const noexcept;

		TS3_ATTR_NO_DISCARD const GeometryDataReference & getAllGeometryDataRef() const noexcept;

		TS3_ATTR_NO_DISCARD const GeometryContainerStorageMetrics & getStorageMetrics() const noexcept;

		TS3_ATTR_NO_DISCARD const GeometrySize & getGeometrySize() const noexcept;

	protected:
		bool checkStorageAvailableSpace( uint32 pVertexElementsNum, uint32 pIndexElementsNum ) const noexcept;

		void setContainerCapacity( uint32 pVertexElementsNum, uint32 pIndexElementsNum );

		void updateStoredGeometrySize( uint32 pVertexElementsNum, uint32 pIndexElementsNum );

	protected:
		GeometryContainerStorageMetrics _storageMetrics;
		GeometryDataReference _allGeometryDataRef;
	};

} // namespace ts3

#endif // __TS3_ENGINE_GEOMETRY_CONTAINER_H__
