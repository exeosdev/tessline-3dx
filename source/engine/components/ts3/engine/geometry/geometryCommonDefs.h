
#pragma once

#ifndef __TS3_ENGINE_GEOMETRY_COMMON_DEFS_H__
#define __TS3_ENGINE_GEOMETRY_COMMON_DEFS_H__

#include "../gpuapi/commonGpaDefs.h"
#include <ts3/gpuapi/resources/gpuBufferReference.h>
#include <list>

namespace ts3
{

	struct GeometryDataFormatInfo;
	struct GeometryReference;
	struct GeometryStorageCreateInfo;

	class GeometryStorage;

	enum class EGeometryVertexType : uint32
	{
		Monolithic = 1,
		MultiComponent = gpa::MAX_GEOMETRY_SEPARATE_VERTEX_COMPONENTS_NUM
	};

} // namespace ts3

#endif // __TS3_ENGINE_GEOMETRY_COMMON_DEFS_H__
