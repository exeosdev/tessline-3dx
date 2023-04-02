
#pragma once

#ifndef __TS3_ENGINE_INDEXED_MESH_DATA_BUFFER_BUILDER_H__
#define __TS3_ENGINE_INDEXED_MESH_DATA_BUFFER_BUILDER_H__

#include "meshData.h"
#include <ts3/stdext/memoryBuffer.h>

namespace ts3
{

	template <typename TVertex, typename TIndex = uint32>
	class IndexedMeshDataBufferBuilder
	{
	public:
		struct MeshReference
		{
			uint32 meshIndex;
			memory_size_t indexDataOffsetBytes;
			memory_size_t indexDataOffsetNumElements;
			memory_size_t vertexDataOffsetBytes;
			memory_size_t vertexDataOffsetNumElements;
		};

	public:
		IndexedMeshDataBufferBuilder()
		{}

		const TVertex * vertexData() const noexcept
		{
			return reinterpret_cast<const TVertex *>( _vertexDataBuffer.data() );
		}

		const TIndex * indexData() const noexcept
		{
			return reinterpret_cast<const TIndex *>( _indexDataBuffer.data() );
		}

		uint32 vertexDataSize() const noexcept
		{
			return numeric_cast<uint32>( _vertexDataBuffer.size() );
		}

		uint32 indexDataSize() const noexcept
		{
			return numeric_cast<uint32>( _indexDataBuffer.data() );
		}

		void allocateStorage( memory_size_t pVerticesNum, memory_size_t pIndicesNum )
		{
			if( !_vertexDataBuffer.empty() || !_indexDataBuffer.empty() || ( pVerticesNum == 0 )  || ( pIndicesNum == 0 ) )
			{
				return;
			}

			const auto vertexDataSize = pVerticesNum * sizeof( TVertex );
			const auto indexDataSize = pIndicesNum * sizeof( TIndex );

			_vertexDataBuffer.resize( vertexDataSize );
			_indexDataBuffer.resize( indexDataSize );
		}

		template <typename TOtherIndex, typename std::enable_if<sizeof( TOtherIndex ) <= sizeof( TIndex ), int>::type = 0>
		void allocateStorage( const MeshDefinition<TVertex, TOtherIndex> & pMeshDefinition )
		{
			if( !_vertexDataBuffer.empty() || !_indexDataBuffer.empty() || pMeshDefinition.empty() )
			{
				return;
			}

			memory_size_t totalVerticesNum = 0;
			memory_size_t totalIndicesNum = 0;

			for( const auto & subMesh : pMeshDefinition.subMeshes )
			{
				totalVerticesNum += subMesh.meshData.verticesNum;
				totalIndicesNum += subMesh.meshData.indicesNum;
			}

			allocateStorage( totalVerticesNum, totalIndicesNum );
		}

		template <typename TOtherIndex, typename std::enable_if<sizeof( TOtherIndex ) <= sizeof( TIndex ), int>::type = 0>
		void addMeshData( const MeshDataFixed<TVertex, TOtherIndex> & pMeshData )
		{
			reserveSpaceForData( pMeshData.verticesNum, pMeshData.indicesNum );

			auto currentVertexDataBufferOffsetBytes = _currentVertexDataBufferOffset;

			memCopyUnchecked(
					_vertexDataBuffer.data() + currentVertexDataBufferOffsetBytes,
					pMeshData.verticesSizeInBytes,
					pMeshData.verticesData.data(),
					pMeshData.verticesSizeInBytes );

			if( _currentVertexDataBufferOffset == 0 )
			{
				memCopyUnchecked(
						_indexDataBuffer.data(),
						pMeshData.indicesSizeInBytes,
						pMeshData.indicesData.data(),
						pMeshData.indicesNum );
			}
			else
			{
				auto currentVertexDataBufferOffsetNumElements = _currentVertexDataBufferOffset / sizeof( TVertex );
				auto currentIndexDataBufferOffsetNumElements = _currentIndexDataBufferOffset / sizeof( TIndex );

				const auto * sourceIndexDataPtr = reinterpret_cast<const TOtherIndex *>( pMeshData.indicesData.data() );
				auto * targetIndexDataPtr = reinterpret_cast<TIndex *>( _indexDataBuffer.data() + _currentIndexDataBufferOffset );

				for( memory_size_t iIndex = 0; iIndex < pMeshData.indicesNum; ++iIndex )
				{
					targetIndexDataPtr[iIndex] = sourceIndexDataPtr[iIndex] + currentVertexDataBufferOffsetNumElements;
				}
			}

			_currentVertexDataBufferOffset += pMeshData.verticesSizeInBytes;
			_currentIndexDataBufferOffset += pMeshData.indicesSizeInBytes;
		}

		template <typename TOtherIndex, typename std::enable_if<sizeof( TOtherIndex ) <= sizeof( TIndex ), int>::type = 0>
		void addMeshDefinition( const MeshDefinition<TVertex, TOtherIndex> & pMeshDefinition )
		{
			for( const auto & subMesh : pMeshDefinition.subMeshes )
			{
				addMeshData( subMesh.meshData );
			}
		}

	private:
		void reserveSpaceForData( memory_size_t pVerticesNum, memory_size_t pIndicesNum )
		{
			const auto newVertexDataSize = pVerticesNum * sizeof( TVertex );
			const auto newIndexDataSize = pIndicesNum * sizeof( TIndex );
			const auto vertexDataFreeSpace = _vertexDataBuffer.size() - _currentVertexDataBufferOffset;
			const auto indexDataFreeSpace = _indexDataBuffer.size() - _currentIndexDataBufferOffset;

			if( vertexDataFreeSpace < newVertexDataSize )
			{
				_vertexDataBuffer.resize( _vertexDataBuffer.size() + newVertexDataSize );
			}

			if( indexDataFreeSpace < newIndexDataSize )
			{
				_indexDataBuffer.resize( _indexDataBuffer.size() + newIndexDataSize );
			}
		}

	private:
		DynamicMemoryBuffer _vertexDataBuffer;
		DynamicMemoryBuffer _indexDataBuffer;
		memory_size_t _currentVertexDataBufferOffset = 0;
		memory_size_t _currentIndexDataBufferOffset = 0;
	};

	using IndexedMeshDataBufferBuilderP3N3T = IndexedMeshDataBufferBuilder<VertexDefaultP3N3T>;

} // namespace ts3

#endif // __TS3_ENGINE_INDEXED_MESH_DATA_BUFFER_BUILDER_H__
