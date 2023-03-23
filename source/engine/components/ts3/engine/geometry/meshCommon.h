
#pragma once

#ifndef __TS3_ENGINE_MESH_COMMON_H__
#define __TS3_ENGINE_MESH_COMMON_H__

#include "../res/imageCommon.h"
#include <ts3/math/vector.h>
#include <ts3/stdext/memoryBuffer.h>

namespace ts3
{

	using FileLoadCallback = std::function<DynamicMemoryBuffer( const std::string & )>;

	struct VertexDefaultP3N3T
	{
		math::Vec3f position;
		math::Vec4f color;
		math::Vec3f normal;
		math::Vec2f texCoord;
	};

	template <typename TVertex, typename TIndex = uint32>
	struct MeshData
	{
		using VertexType = TVertex;
		using IndexType = TIndex;

		static constexpr native_uint sVertexSize = sizeof( TVertex );
		static constexpr native_uint sIndexSize = sizeof( TIndex );

		DynamicMemoryBuffer verticesData;

		native_uint verticesNum = 0;

		native_uint verticesSizeInBytes = 0;

		DynamicMemoryBuffer indicesData;

		native_uint indicesNum = 0;

		native_uint indicesSizeInBytes = 0;

		void initialize( size_t pVerticesNum, size_t pFacesNum )
		{
			verticesNum = pVerticesNum;
			indicesNum = pFacesNum * 3;
			verticesSizeInBytes = verticesNum * sVertexSize;
			indicesSizeInBytes = indicesNum * sIndexSize;

			verticesData.resize( verticesSizeInBytes );
			indicesData.resize( indicesSizeInBytes );
		}

		TVertex * vertexPtr( size_t pOffset = 0 )
		{
			const auto byteOffset = pOffset * sVertexSize;
			auto * dataPtr = verticesData.data( byteOffset );
			return reinterpret_cast<TVertex *>( dataPtr );
		}

		TIndex * indexPtr( size_t pOffset = 0 )
		{
			const auto byteOffset = pOffset * sIndexSize;
			auto * dataPtr = indicesData.data( byteOffset );
			return reinterpret_cast<TIndex *>( dataPtr );
		}
	};

	using MeshDefaultP3N3T = MeshData<VertexDefaultP3N3T, uint32>;

	struct MaterialData
	{
		ImageData diffuseTextureData;
	};

	template <typename TVertex, typename TIndex = uint32>
	struct MeshPartDefinition
	{
		MeshData<TVertex, TIndex> meshData;

		uint32 materialIndex;
	};

	template <typename TVertex, typename TIndex = uint32>
	struct MeshDefinition
	{
		using SubMeshDefinition = MeshPartDefinition<TVertex, TIndex>;

		using SubMeshArray = std::vector<SubMeshDefinition>;

		using MaterialArray = std::vector<MaterialData>;

		SubMeshArray subMeshes;

		MaterialArray materials;

		native_uint subMeshesTotalVerticesNum = 0;

		native_uint subMeshesTotalVerticesSizeInBytes = 0;

		native_uint subMeshesTotalIndicesNum = 0;

		native_uint subMeshesTotalIndicesSizeInBytes = 0;

		TS3_ATTR_NO_DISCARD explicit operator bool() const noexcept
		{
			return !empty();
		}

		TS3_ATTR_NO_DISCARD bool empty() const noexcept
		{
			return subMeshes.empty();
		}

		void reset()
		{
			for( auto & subMesh : subMeshes )
			{
				subMesh.meshData.verticesData.release();
				subMesh.meshData.indicesData.release();
			}
			materials.clear();
		}
	};

	using MeshDefinitionDefaultP3N3T = MeshDefinition<VertexDefaultP3N3T, uint32>;

	class MeshLoader
	{
	public:
		virtual MeshDefinitionDefaultP3N3T loadMeshDefault( const std::string & pFilename ) = 0;
	};

	class AssimpMeshLoader : public MeshLoader
	{
	public:
		AssimpMeshLoader( FileLoadCallback pFileLoadCallback )
		: _fileLoadCallback( std::move( pFileLoadCallback ) )
		{}

		virtual MeshDefinitionDefaultP3N3T loadMeshDefault( const std::string & pFilename ) override final;

	private:
		FileLoadCallback _fileLoadCallback;
	};

} // namespace ts3

#endif // __TS3_ENGINE_MESH_COMMON_H__
