
#pragma once

#ifndef __TS3_ENGINE_MESH_GROUP_H__
#define __TS3_ENGINE_MESH_GROUP_H__

#include "meshCommon.h"
#include "../geometry/geometryReference.h"
#include "../geometry/vertexFormat.h"

namespace ts3
{

	struct MeshGroupInfo
	{
		uint32 meshesNum;
	};

	// Meshes with same GeometryDataFormat, which can be rendered with a single rendering state (PSO/IL).
	class MeshGroup
	{
	public:
		GeometryDataFormat const mGeometryDataFormat;

	public:

	private:
		uint32 _indexDataSizeInBytes = 0;
		uint32 _vertexDataSizeInBytes = 0;
		std::unique_ptr<GeometryStorage> _geometryStorage;
	};

} // namespace ts3

#endif // __TS3_ENGINE_MESH_GROUP_H__
