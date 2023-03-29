
#pragma once

#ifndef __TS3_ENGINE_GEOMETRY_REFERENCE_H__
#define __TS3_ENGINE_GEOMETRY_REFERENCE_H__

#include "../gpuapi/commonGpaDefs.h"
#include <ts3/gpuapi/resources/gpuBufferReference.h>
#include <list>

namespace ts3
{

	struct GeometryDataFormatInfo;
	struct GeometryReference;

	class GeometryStorage;

	using GeometryReferenceList = std::list<GeometryReference>;
	using GeometryReferenceSelfIter = GeometryReferenceList::iterator;

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

		GeometryVertexComponentDataArray<DataRegion> vertexComponentDataRegionArray{};
	};

} // namespace ts3

#endif // __TS3_ENGINE_GEOMETRY_REFERENCE_H__
