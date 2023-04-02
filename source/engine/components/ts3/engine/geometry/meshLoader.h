
#pragma once

#ifndef __TS3_ENGINE_MESH_LOADER_H__
#define __TS3_ENGINE_MESH_LOADER_H__

#include "meshData.h"

namespace ts3
{

	class MeshImporter
	{
	public:
		virtual std::unique_ptr<MeshData> importMesh(
				const std::string & pFilename,
				const GeometryDataFormat & pGeometryDataFormat ) = 0;
	};

	class MeshImporterAssimp : public MeshImporter
	{
	public:
		virtual std::unique_ptr<MeshData> importMesh(
				const std::string & pFilename,
				const GeometryDataFormat & pGeometryDataFormat ) override final;
	};

	struct MeshInputDesc
	{
		std::string meshName;
		std::string sourceFilename;
	};

	struct MeshImportContext
	{
		MeshImporter * importer = nullptr;
		GeometryManager * geometryManager = nullptr;
		GeometryDataGpuTransfer * geometryDataTransfer = nullptr;
		const GeometryDataFormat * geometryDataFormat = nullptr;

	};

	class MeshLoader
	{
	public:
		std::unique_ptr<MeshGroup> importMeshGroup(
				const MeshImportContext & pImportContext,
				std::string pGroupName,
				const std::vector<MeshInputDesc> & pGroupDesc );

	private:
		GeometryStorage * createGeometryStorageForMeshGroup(
				GeometryManager & pGeometryManager,
				const GeometryDataFormat & pGeometryDataFormat,
				const MeshGroupData & pMeshGroupData );
	};

} // namespace ts3

#endif // __TS3_ENGINE_MESH_LOADER_H__
