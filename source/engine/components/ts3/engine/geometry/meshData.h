
#pragma once

#ifndef __TS3_ENGINE_MESH_DATA_H__
#define __TS3_ENGINE_MESH_DATA_H__

#include "geometryContainer.h"
#include <ts3/stdext/memoryBuffer.h>

namespace ts3
{

	class Mesh;
	class MeshGroup;
	class MeshLoader;

	struct MeshSubComponentData
	{
		std::string name;

		uint32 componentIndex;

		GeometryDataReferenceBase geometryDataRef;

		MeshSubComponentData( const GeometryDataFormat & pDataFormat )
		: geometryDataRef( pDataFormat )
		{}
	};

	class MeshData : public GeometryContainer
	{
	public:
		MeshData( const GeometryDataFormat & pGeometryDataFormat )
		: GeometryContainer( pGeometryDataFormat )
		{}

		TS3_ATTR_NO_DISCARD uint32 getMeshSubComponentsNum() const noexcept;

		TS3_ATTR_NO_DISCARD const std::string & getMeshName() const noexcept;

		TS3_ATTR_NO_DISCARD const MeshSubComponentData * getMeshSubComponentData( uint32 pIndex ) const noexcept;

		TS3_ATTR_NO_DISCARD const void * getVertexStreamBaseDataPtr( uint32 pIndex ) const noexcept;

		TS3_ATTR_NO_DISCARD const void * getIndexBaseDataPtr() const noexcept;

		TS3_ATTR_NO_DISCARD DataBufferRegionSubElementRefReadOnly getIndexDataSubRegionReadOnly(
				const CPUGeometryDataReferenceBase & pMeshDataRef ) const noexcept;

		TS3_ATTR_NO_DISCARD DataBufferRegionSubElementRefReadOnly getVertexAttributeDataSubRegionReadOnly(
				const CPUGeometryDataReferenceBase & pMeshDataRef,
				uint32 pAttributeIndex ) const noexcept;

		TS3_ATTR_NO_DISCARD DataBufferRegionSubElementRefReadWrite getIndexDataSubRegionReadWrite(
				const CPUGeometryDataReferenceBase & pMeshDataRef ) noexcept;

		TS3_ATTR_NO_DISCARD DataBufferRegionSubElementRefReadWrite getVertexAttributeDataSubRegionReadWrite(
				const CPUGeometryDataReferenceBase & pMeshDataRef,
				uint32 pAttributeIndex ) noexcept;

		void initializeStorage( uint32 pVertexElementsNum, uint32 pIndexElementsNum );

		void setMeshName( std::string pName );

		MeshSubComponentData * addMeshComponent( uint32 pVertexElementsNum, uint32 pIndexElementsNum );

	private:
		using VertexDataBufferArray = std::array<DynamicMemoryBuffer, gpa::MAX_GEOMETRY_VERTEX_STREAMS_NUM>;
		GeometrySize _geometrySize;
		std::string _meshName;
		DynamicMemoryBuffer _indexDataBuffer;
		VertexDataBufferArray _vertexDataBuffers;
		std::vector<MeshSubComponentData> _meshSubComponents;
	};

	inline uint32 MeshData::getMeshSubComponentsNum() const noexcept
	{
		return static_cast<uint32>( _meshSubComponents.size() );
	}

	inline const std::string & MeshData::getMeshName() const noexcept
	{
		return _meshName;
	}

	inline const MeshSubComponentData * MeshData::getMeshSubComponentData( uint32 pIndex ) const noexcept
	{
		return ( pIndex < _meshSubComponents.size() ) ? &( _meshSubComponents[pIndex] ) : nullptr;
	}

	inline const void * MeshData::getVertexStreamBaseDataPtr( uint32 pIndex ) const noexcept
	{
		return _vertexDataBuffers[pIndex].data();
	}

	inline const void * MeshData::getIndexBaseDataPtr() const noexcept
	{
		return _indexDataBuffer.data();
	}


	class MeshGroupData
	{
	public:
		GeometryDataFormat const mGeometryDataFormat;

	public:
		MeshGroupData( const GeometryDataFormat & pGeometryDataFormat )
		: mGeometryDataFormat( pGeometryDataFormat )
		{}

		TS3_ATTR_NO_DISCARD uint32 getMeshesNum() const noexcept
		{
			return static_cast<uint32>( _meshDataArray.size() );
		}

		TS3_ATTR_NO_DISCARD const GeometrySize & geometrySize() const noexcept
		{
			return _geometrySize;
		}

		TS3_ATTR_NO_DISCARD bool empty() const noexcept
		{
			return _meshDataArray.empty();
		}

		TS3_ATTR_NO_DISCARD const MeshData * getMeshData( uint32 pIndex ) const noexcept
		{
			return ( pIndex < _meshDataArray.size() ) ? _meshDataArray[pIndex].get() : nullptr;
		}

		MeshData & addMeshData( std::unique_ptr<MeshData> pMeshData );

		MeshData & addMeshData( uint32 pVertexElementsNum, uint32 pIndexElementsNum );

	private:
		using MeshDataPtr = std::unique_ptr<MeshData>;
		using MeshDataArray = std::vector<MeshDataPtr>;
		GeometrySize _geometrySize;
		MeshDataArray _meshDataArray;
	};

	struct VertexDefaultP3N3T
	{
		math::Vec3f position;
		math::Vec3f normal;
		math::Vec4f color;
		math::Vec2f texCoord;
		math::Vec3f tangent;
		math::Vec3f biTangent;
	};

//	namespace geom
//	{
//
//		template <typename TVertex, typename TIndex>
//		inline void generateTangentsAndBiNormals( MeshDataFixed<TVertex, TIndex> & pMeshData )
//		{
//			auto * vertexArray = reinterpret_cast<TVertex *>( pMeshData.verticesData.data() );
//
//			for( native_uint iVertex = 0; iVertex < pMeshData.verticesNum; ++iVertex )
//			{
//				math::Vec3f & vTangent = vertexArray[iVertex].tangent;
//				math::Vec3f & vBiNormal = vertexArray[iVertex].biTangent;
//				vTangent = { 0.0f, 0.0f, 0.0f };
//				vBiNormal = { 0.0f, 0.0f, 0.0f };
//				continue;
//			}
//
//			const auto * indexArray = reinterpret_cast<const TIndex *>( pMeshData.indicesData.data() );
//
//			for( native_uint iIndex = 0; iIndex < pMeshData.indicesNum; iIndex += 3 )
//			{
//				const auto faceIndex0 = indexArray[iIndex + 0];
//				const auto faceIndex1 = indexArray[iIndex + 1];
//				const auto faceIndex2 = indexArray[iIndex + 2];
//
//				const math::Vec3f & vPosition0 = vertexArray[faceIndex0].position;
//				const math::Vec3f & vPosition1 = vertexArray[faceIndex1].position;
//				const math::Vec3f & vPosition2 = vertexArray[faceIndex2].position;
//
//				const math::Vec2f & vTexCoord0 = vertexArray[faceIndex0].texCoord;
//				const math::Vec2f & vTexCoord1 = vertexArray[faceIndex1].texCoord;
//				const math::Vec2f & vTexCoord2 = vertexArray[faceIndex2].texCoord;
//
//				const math::Vec3f & vNormal0 = vertexArray[faceIndex0].normal;
//				const math::Vec3f & vNormal1 = vertexArray[faceIndex1].normal;
//				const math::Vec3f & vNormal2 = vertexArray[faceIndex2].normal;
//
//				const math::Vec3f dPosition1 = vPosition1 - vPosition0;
//				const math::Vec3f dPosition2 = vPosition2 - vPosition0;
//
//				const math::Vec2f dTexCoord1 = vTexCoord1 - vTexCoord0;
//				const math::Vec2f dTexCoord2 = vTexCoord2 - vTexCoord0;
//
//				// Calculate tangent and bitangent
//				const auto r = 1.0f / ( dTexCoord1.x * dTexCoord2.y - dTexCoord1.y * dTexCoord2.x );
//				const auto tangent = ( dPosition1 * dTexCoord2.y - dPosition2 * dTexCoord1.y ) * r;
//				const auto biTangent = ( dPosition2 * dTexCoord1.x - dPosition1 * dTexCoord2.x ) * r;
//
//
//				math::Vec3f & vTangent0 = vertexArray[faceIndex0].tangent;
//				math::Vec3f & vTangent1 = vertexArray[faceIndex1].tangent;
//				math::Vec3f & vTangent2 = vertexArray[faceIndex2].tangent;
//
//				math::Vec3f & vBiNormal0 = vertexArray[faceIndex0].biTangent;
//				math::Vec3f & vBiNormal1 = vertexArray[faceIndex1].biTangent;
//				math::Vec3f & vBiNormal2 = vertexArray[faceIndex2].biTangent;
//
//				vTangent0 += math::normalize(tangent - vNormal0 * math::dot( vNormal0, tangent ) );
//				vTangent1 += math::normalize(tangent - vNormal1 * math::dot( vNormal1, tangent ) );
//				vTangent2 += math::normalize(tangent - vNormal2 * math::dot( vNormal2, tangent ) );
//
//				vBiNormal0 += math::normalize( biTangent - vNormal0 * math::dot( vNormal0, biTangent ) );
//				vBiNormal1 += math::normalize( biTangent - vNormal1 * math::dot( vNormal1, biTangent ) );
//				vBiNormal2 += math::normalize( biTangent - vNormal2 * math::dot( vNormal2, biTangent ) );
//			}
//
//			for( native_uint iVertex = 0; iVertex < pMeshData.verticesNum; ++iVertex )
//			{
//				math::Vec3f & vTangent = vertexArray[iVertex].tangent;
//				math::Vec3f & vBiNormal = vertexArray[iVertex].biTangent;
//				vTangent = math::normalize( vTangent );
//				vBiNormal = math::normalize( vBiNormal );
//				continue;
//			}
//		}
//
//	}

} // namespace ts3

#endif // __TS3_ENGINE_MESH_DATA_H__
