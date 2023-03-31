
#pragma once

#ifndef __TS3_ENGINE_MESH_GROUP_H__
#define __TS3_ENGINE_MESH_GROUP_H__


#include "meshCommon.h"
#include "../geometry/geometryReference.h"
#include "../geometry/geometryVertexFormat.h"

namespace ts3
{

	struct MeshGroupInfo
	{
		uint32 meshesNum;
		uint32 indexElementsNum = 0;
		uint32 vertexElementsNum = 0;
	};

	// Meshes with same GeometryDataFormat, which can be rendered with a single rendering state (PSO/IL).
	class MeshGroup
	{
	public:
		std::string const mGroupName;
		GeometryDataFormat const mGeometryDataFormat;

	public:
		MeshGroup( std::string pGroupName, const GeometryDataFormat & pGeometryDataFormat )
		: mGroupName( std::move( pGroupName ) )
		, mGeometryDataFormat( pGeometryDataFormat )
		{}

		~MeshGroup();

	private:
		uint32 _indexElementsNum = 0;
		uint32 _vertexElementsNum = 0;
		std::unique_ptr<GeometryStorage> _geometryStorage;
	};

} // namespace ts3

#endif // __TS3_ENGINE_MESH_GROUP_H__
