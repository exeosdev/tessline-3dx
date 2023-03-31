
#pragma once

#ifndef __TS3_ENGINE_GEOMETRY_REFERENCE_H__
#define __TS3_ENGINE_GEOMETRY_REFERENCE_H__

#include "geometryCommonDefs.h"
#include <ts3/gpuapi/resources/gpuBufferReference.h>
#include <list>

namespace ts3
{

	struct GeometryDataFormatInfo;
	struct GeometryReference;

	class GeometryStorage;

	using GeometryReferenceList = std::list<GeometryReference>;
	using GeometryReferenceSelfIter = GeometryReferenceList::iterator;

	struct GeometryReference
	{
		struct DataRegion
		{
			uint32 offsetInElementsNum;
			uint32 sizeInElementsNum;
		};

		GeometryStorage * geometryStorage;

		const GeometryDataFormatInfo * formatInfo;

		uint32 geometryIndex;

		DataRegion indexDataRegion;

		GeometryVertexStreamGenericArray<DataRegion> vertexStreamDataRegionArray{};
	};

} // namespace ts3

#endif // __TS3_ENGINE_GEOMETRY_REFERENCE_H__
