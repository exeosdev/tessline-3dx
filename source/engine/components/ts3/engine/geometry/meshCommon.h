
#pragma once

#ifndef __TS3_ENGINE_MESH_COMMON_H__
#define __TS3_ENGINE_MESH_COMMON_H__

#include "geometryCommonDefs.h"
#include "../res/imageCommon.h"
#include <ts3/math/vectorOps.h>
#include <ts3/stdext/memoryBuffer.h>

namespace ts3
{

	class MeshData;
	class MeshLoader;

	struct MeshVertexFormat
	{
		struct alignas( 4 ) AttributeFormat
		{
			gpuapi::EBaseDataType baseType;
			uint16 componentsNum;
		};

		struct alignas( 4 ) AttributeDefinition
		{
			EVertexAttributeID attributeID = EVertexAttributeID::Undefined;
			AttributeFormat format;
			uint16 streamIndex;
			uint16 relativeOffset;
		};

		using AttributeDefinitionArray = std::array<AttributeDefinition, gpuapi::gpm::IA_MAX_VERTEX_ATTRIBUTES_NUM>;

		Bitmask<EVertexAttributeFlags> activeAttributesMask;

		AttributeDefinitionArray attributes;

		bool isAttributeValid( uint32 pAttributeIndex ) const noexcept
		{
			return attributes[pAttributeIndex].attributeID != EVertexAttributeID::Undefined;
		}
	};

	struct MeshVertexMemoryLayout
	{
		struct AttributeMemoryLayout
		{
			uint16 baseOffsetInBytes = 0;
			uint16 sizeInBytes = 0;
		};

		struct VertexStreamMemoryLayout
		{
			Bitmask<EVertexAttributeFlags> activeAttributesMask;
			uint16 elementSizeInBytes = 0;
		};

		using AttributeMemoryLayoutArray = std::array<AttributeMemoryLayout, gpuapi::gpm::IA_MAX_VERTEX_ATTRIBUTES_NUM>;
		using VertexStreamMemoryLayoutArray = std::array<VertexStreamMemoryLayout, gpa::MAX_GEOMETRY_SEPARATE_VERTEX_COMPONENTS_NUM>;

		AttributeMemoryLayoutArray attributeLayouts;

		VertexStreamMemoryLayoutArray vertexStreamLayouts;
	};

	struct MeshDataLayout
	{
		gpuapi::EIndexDataFormat indexFormat = gpuapi::EIndexDataFormat::Undefined;

		uint32 indexElementSizeInBytes = 0;

		MeshVertexFormat vertexFormat;

		MeshVertexMemoryLayout vertexMemoryLayout;

		bool hasIndexData() const noexcept
		{
			return indexFormat != gpuapi::EIndexDataFormat::Undefined;
		}

		bool isAttributeValid( uint32 pAttributeIndex ) const noexcept
		{
			return vertexFormat.isAttributeValid( pAttributeIndex );
		}

		bool isVertexStreamActive( uint32 pStreamIndex ) const noexcept
		{
			return vertexMemoryLayout.vertexStreamLayouts[pStreamIndex].elementSizeInBytes > 0;
		}
	};

	struct MeshDataMetrics
	{
		uint32 subMeshesNum;
		uint32 indexDataElementsNum;
		uint32 vertexDataElementsNum;
	};

	struct MeshDataStorage
	{
		using VertexDataBufferArray = std::array<DynamicMemoryBuffer, gpa::MAX_GEOMETRY_SEPARATE_VERTEX_COMPONENTS_NUM>;

		DynamicMemoryBuffer indexDataBuffer;

		VertexDataBufferArray vertexDataBuffers;
	};

	struct MeshDataBufferReadRegion
	{
		const byte * basePtr;
		uint32 strideInBytes;
	};

	struct MeshDataBufferWriteRegion
	{
		byte * basePtr;
		uint32 strideInBytes;
	};

	struct MeshDataSubMeshReference
	{
		uint32 subMeshIndex;
		uint32 indexDataOffsetInElementsNum;
		uint32 indexDataElementsNum;
		uint32 vertexDataOffsetInElementsNum;
		uint32 vertexDataElementsNum;
	};

	class MeshData
	{
		friend class MeshLoader;

	public:
		MeshDataLayout const mDataLayout;

	public:
		MeshData( const MeshDataLayout & pDataLayout )
		: mDataLayout( pDataLayout )
		{}

		MeshData( MeshData && pSource )
		: mDataLayout( pSource.mDataLayout )
		, _dataStorage( std::move( pSource._dataStorage ) )
		{}

		MeshData & operator=( MeshData && pRhs )
		{
			_dataStorage = std::move( pRhs._dataStorage );
			return *this;
		}

		void initializeStorage( uint32 pIndicesNum, uint32 pVerticesNum );

		MeshDataSubMeshReference & addSubMesh( uint32 pIndicesNum, uint32 pVerticesNum );

		MeshDataBufferReadRegion getIndexDataWriteRegion( const MeshDataSubMeshReference & pSubMeshReference ) const;

		MeshDataBufferReadRegion getVertexDataWriteRegion( const MeshDataSubMeshReference & pSubMeshReference, uint32 pAttributeIndex ) const;

		MeshDataBufferWriteRegion getIndexDataWriteRegion( const MeshDataSubMeshReference & pSubMeshReference );

		MeshDataBufferWriteRegion getVertexDataWriteRegion( const MeshDataSubMeshReference & pSubMeshReference, uint32 pAttributeIndex );

	protected:
		byte * getWritableVertexDataBuffer( uint32 pStreamIndex );

		size_t getIndexDataElementByteOffset( uint32 pElementNum );

		size_t getVertexDataElementByteOffset( uint32 pStreamIndex, uint32 pElementNum );

	private:
		MeshDataStorage _dataStorage;
		std::vector<MeshDataSubMeshReference> _subMeshes;
	};

	class MeshImporter
	{
	public:
		virtual MeshData importMeshDefault( const std::string & pFilename ) = 0;
	};

	class AssimpMeshImporter : public MeshImporter
	{
	public:
		virtual MeshData importMeshDefault( const std::string & pFilename ) override final;
	};

	namespace gmutil
	{

		TS3_ATTR_NO_DISCARD MeshVertexMemoryLayout generateMeshVertexMemoryLayoutForVertexFormat( const MeshVertexFormat & pMeshVertexFormat );

	}

	///////////

	using FileLoadCallback = std::function<DynamicMemoryBuffer( const std::string & )>;

	struct VertexDefaultP3N3T
	{
		math::Vec3f position;
		math::Vec3f normal;
		math::Vec3f tangent;
		math::Vec3f biTangent;
		math::Vec4f color;
		math::Vec2f texCoord;
	};

	template <typename TVertex, typename TIndex = uint32>
	struct MeshDataFixed
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

		bool vfaNormals = false;
		bool vfaTangents = false;
		bool vfaBiTangents = false;

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

	using MeshDefaultP3N3T = MeshDataFixed<VertexDefaultP3N3T, uint32>;

	struct MaterialData
	{
		ImageData diffuseTextureData;
	};

	template <typename TVertex, typename TIndex = uint32>
	struct MeshPartDefinition
	{
		MeshDataFixed<TVertex, TIndex> meshData;

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


	namespace geom
	{

		template <typename TVertex, typename TIndex>
		inline void generateTangentsAndBiTangents( MeshDataFixed<TVertex, TIndex> & pMeshData )
		{
			auto * vertexArray = reinterpret_cast<TVertex *>( pMeshData.verticesData.data() );

			for( native_uint iVertex = 0; iVertex < pMeshData.verticesNum; ++iVertex )
			{
				math::Vec3f & vTangent = vertexArray[iVertex].tangent;
				math::Vec3f & vBiTangent = vertexArray[iVertex].biTangent;
				vTangent = { 0.0f, 0.0f, 0.0f };
				vBiTangent = { 0.0f, 0.0f, 0.0f };
				continue;
			}

			const auto * indexArray = reinterpret_cast<const TIndex *>( pMeshData.indicesData.data() );

			for( native_uint iIndex = 0; iIndex < pMeshData.indicesNum; iIndex += 3 )
			{
				const auto faceIndex0 = indexArray[iIndex + 0];
				const auto faceIndex1 = indexArray[iIndex + 1];
				const auto faceIndex2 = indexArray[iIndex + 2];

				const math::Vec3f & vPosition0 = vertexArray[faceIndex0].position;
				const math::Vec3f & vPosition1 = vertexArray[faceIndex1].position;
				const math::Vec3f & vPosition2 = vertexArray[faceIndex2].position;

				const math::Vec2f & vTexCoord0 = vertexArray[faceIndex0].texCoord;
				const math::Vec2f & vTexCoord1 = vertexArray[faceIndex1].texCoord;
				const math::Vec2f & vTexCoord2 = vertexArray[faceIndex2].texCoord;

				const math::Vec3f & vNormal0 = vertexArray[faceIndex0].normal;
				const math::Vec3f & vNormal1 = vertexArray[faceIndex1].normal;
				const math::Vec3f & vNormal2 = vertexArray[faceIndex2].normal;

				const math::Vec3f dPosition1 = vPosition1 - vPosition0;
				const math::Vec3f dPosition2 = vPosition2 - vPosition0;

				const math::Vec2f dTexCoord1 = vTexCoord1 - vTexCoord0;
				const math::Vec2f dTexCoord2 = vTexCoord2 - vTexCoord0;

				// Calculate tangent and bitangent
				const auto r = 1.0f / ( dTexCoord1.x * dTexCoord2.y - dTexCoord1.y * dTexCoord2.x );
				const auto tangent = ( dPosition1 * dTexCoord2.y - dPosition2 * dTexCoord1.y ) * r;
				const auto biTangent = ( dPosition2 * dTexCoord1.x - dPosition1 * dTexCoord2.x ) * r;


				math::Vec3f & vTangent0 = vertexArray[faceIndex0].tangent;
				math::Vec3f & vTangent1 = vertexArray[faceIndex1].tangent;
				math::Vec3f & vTangent2 = vertexArray[faceIndex2].tangent;

				math::Vec3f & vBiTangent0 = vertexArray[faceIndex0].biTangent;
				math::Vec3f & vBiTangent1 = vertexArray[faceIndex1].biTangent;
				math::Vec3f & vBiTangent2 = vertexArray[faceIndex2].biTangent;

				vTangent0 += math::normalize(tangent - vNormal0 * math::dot( vNormal0, tangent ) );
				vTangent1 += math::normalize(tangent - vNormal1 * math::dot( vNormal1, tangent ) );
				vTangent2 += math::normalize(tangent - vNormal2 * math::dot( vNormal2, tangent ) );

				vBiTangent0 += math::normalize( biTangent - vNormal0 * math::dot( vNormal0, biTangent ) );
				vBiTangent1 += math::normalize( biTangent - vNormal1 * math::dot( vNormal1, biTangent ) );
				vBiTangent2 += math::normalize( biTangent - vNormal2 * math::dot( vNormal2, biTangent ) );
			}

			for( native_uint iVertex = 0; iVertex < pMeshData.verticesNum; ++iVertex )
			{
				math::Vec3f & vTangent = vertexArray[iVertex].tangent;
				math::Vec3f & vBiTangent = vertexArray[iVertex].biTangent;
				vTangent = math::normalize( vTangent );
				vBiTangent = math::normalize( vBiTangent );
				continue;
			}
		}

	}

} // namespace ts3

#endif // __TS3_ENGINE_MESH_COMMON_H__
