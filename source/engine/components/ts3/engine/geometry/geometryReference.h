
#pragma once

#ifndef __TS3_ENGINE_GEOMETRY_REFERENCE_H__
#define __TS3_ENGINE_GEOMETRY_REFERENCE_H__

#include "geometryCommonDefs.h"
#include <ts3/gpuapi/resources/gpuBufferReference.h>
#include <list>

namespace ts3
{

	struct GeometryReference
	{
		GeometryStorage * storage;

		uint32 geometryIndex;

		const GeometryDataFormat * dataFormat;

		GeometryDataReference dataReference;
	};

} // namespace ts3

#endif // __TS3_ENGINE_GEOMETRY_REFERENCE_H__
