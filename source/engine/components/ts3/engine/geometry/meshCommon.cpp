
#include "meshCommon.h"
#include "dataTypesConv.h"
#include "geometryVertexFormat.h"
#include <ts3/engine/res/image/pngCommon.h>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/StringUtils.h>

namespace ts3
{

	uint32 MeshData::indexDataSizeInBytes() const noexcept
	{
		return _indexDataSizeInBytes;
	}

	uint32 MeshData::vertexDataSizeInBytes() const noexcept
	{
		return _vertexDataSizeInBytes;
	}

	void MeshData::initializeStorage( uint32 pVertexElementsNum, uint32 pIndexElementsNum )
	{
		for( uint32 iVertexStream = 0; iVertexStream < gpa::MAX_GEOMETRY_VERTEX_STREAMS_NUM; ++iVertexStream )
		{
			if( mGeometryDataFormat.isVertexStreamActive( iVertexStream ) )
			{
				const auto vertexStreamElementSize = mGeometryDataFormat.vertexStream( iVertexStream ).elementSizeInBytes;
				const auto vertexStreamDataBufferSize = vertexStreamElementSize * pVertexElementsNum;
				_vertexDataBuffers[iVertexStream].resize( vertexStreamDataBufferSize );
			}
		}

		if( pIndexElementsNum > 0 )
		{
			const auto indexDataBufferSize = mGeometryDataFormat.indexElementSizeInBytes() * pIndexElementsNum;
			_indexDataBuffer.resize( indexDataBufferSize );
		}
	}

	MeshPartReference & MeshData::addMeshPart( uint32 pIndicesNum, uint32 pVerticesNum )
	{
		MeshPartReference subMeshReference;
		subMeshReference.subMeshIndex = 0;
		subMeshReference.indexDataOffsetInElementsNum = 0;
		subMeshReference.vertexDataOffsetInElementsNum = 0;
		subMeshReference.indexElementsNum = pIndicesNum;
		subMeshReference.vertexElementsNum = pVerticesNum;

		if( !_subMeshes.empty() )
		{
			const auto & lastSubMesh = _subMeshes.back();
			subMeshReference.subMeshIndex = numeric_cast<uint32>( _subMeshes.size() );
			subMeshReference.indexDataOffsetInElementsNum = lastSubMesh.indexDataOffsetInElementsNum + lastSubMesh.indexElementsNum;
			subMeshReference.vertexDataOffsetInElementsNum = lastSubMesh.vertexDataOffsetInElementsNum + lastSubMesh.vertexElementsNum;
		}

		_subMeshes.push_back( subMeshReference );

		return _subMeshes.back();
	}

	MeshDataBufferWriteRegion MeshData::getIndexDataWriteRegion( const MeshPartReference & pMeshPartRef )
	{
		MeshDataBufferWriteRegion writeRegion;
		writeRegion.basePtr = _indexDataBuffer.data() + ( pMeshPartRef.indexDataOffsetInElementsNum * mGeometryDataFormat.indexElementSizeInBytes() );
		writeRegion.strideInBytes = mGeometryDataFormat.indexElementSizeInBytes();
		return writeRegion;
	}

	MeshDataBufferWriteRegion MeshData::getVertexDataWriteRegion( const MeshPartReference & pMeshPartRef, uint32 pAttributeIndex )
	{
		const auto & attributeFormat = mGeometryDataFormat.attribute( pAttributeIndex );
		const auto & vertexStreamFormat = mGeometryDataFormat.vertexStream( attributeFormat.streamIndex );

		auto * bufferBasePtr = _vertexDataBuffers[attributeFormat.streamIndex].data();
		auto * streamBasePtr = bufferBasePtr + ( pMeshPartRef.vertexDataOffsetInElementsNum * vertexStreamFormat.elementSizeInBytes );

		MeshDataBufferWriteRegion writeRegion;
		writeRegion.basePtr = streamBasePtr + attributeFormat.streamElementRelativeOffset;
		writeRegion.strideInBytes = vertexStreamFormat.elementSizeInBytes;
		return writeRegion;
	}


	MeshSizeMetrics getAssimpMeshSizeMetrics( const aiScene * pAiScene )
	{
		MeshSizeMetrics metrics;
		metrics.meshPartsNum = pAiScene->mNumMeshes;
		metrics.indexElementsNum = 0;
		metrics.vertexElementsNum = 0;

		for( native_uint iMesh = 0; iMesh < pAiScene->mNumMeshes; ++iMesh )
		{
			const auto * aiMesh = pAiScene->mMeshes[iMesh];
			metrics.indexElementsNum += aiMesh->mNumFaces * 3;
			metrics.vertexElementsNum += aiMesh->mNumVertices;
		}

		return metrics;
	}

	template <typename TInput>
	void assimpReadMeshVertexAttributeData(
			const TInput * pInputData,
			const size_t pElementsNum,
			const MeshDataBufferWriteRegion & pWriteRegion,
			const VertexAttributeFormat & pAttributeFormat,
			const DataTypeConversionFunction & pConversionFunction )
	{
		const auto vertexAttributeComponentsNum = gpuapi::cxdefs::getVertexAttribFormatComponentsNum( pAttributeFormat.componentFormat );

		auto * currentWritePtr = pWriteRegion.basePtr;

		for( native_uint iElement = 0; iElement < pElementsNum; ++iElement )
		{
			const auto & aiAttributeData = pInputData[iElement];

			pConversionFunction( &aiAttributeData, currentWritePtr, vertexAttributeComponentsNum );

			currentWritePtr += pWriteRegion.strideInBytes;
		}
	}

	void assimpImportMesh( const aiMesh * pAiMesh, MeshData & pMeshData )
	{
		auto & meshReference = pMeshData.addMeshPart( pAiMesh->mNumFaces * 3, pAiMesh->mNumVertices );

		if( pAiMesh->mNumFaces > 0 )
		{
			const auto indexDataWriteRegion = pMeshData.getIndexDataWriteRegion( meshReference );
			const auto conversionFunction = gmutil::getGeometryConversionFunction<math::Vec3u32>( gpuapi::EBaseDataType::Uint32 );

			auto * currentWritePtr = indexDataWriteRegion.basePtr;

			for( native_uint iFace = 0; iFace < pAiMesh->mNumFaces; ++iFace )
			{
				const auto & vFace = pAiMesh->mFaces[iFace];
				conversionFunction( vFace.mIndices, currentWritePtr, 3 );
				currentWritePtr += indexDataWriteRegion.strideInBytes;
			}
		}

		for( uint32 iAttribute = 0; iAttribute < gpa::MAX_GEOMETRY_VERTEX_ATTRIBUTES_NUM; ++iAttribute )
		{
			if( pMeshData.mGeometryDataFormat.isAttributeActive( iAttribute ) )
			{
				const auto & attributeFormat = pMeshData.mGeometryDataFormat.attribute( iAttribute );
				const auto attributeDataBaseType = gpuapi::cxdefs::getVertexAttribFormatBaseDataType( attributeFormat.componentFormat );
				const auto attributeDataWriteRegion = pMeshData.getVertexDataWriteRegion( meshReference, iAttribute );

				switch( attributeFormat.semantics.semID )
				{
					case EVertexAttributeSemanticsID::Position:
					{
						const auto conversionFunction = gmutil::getGeometryConversionFunction<math::Vec3d>( attributeDataBaseType );

						assimpReadMeshVertexAttributeData(
								pAiMesh->mVertices,
								pAiMesh->mNumVertices,
								attributeDataWriteRegion,
								attributeFormat,
								conversionFunction );

						break;
					}
					case EVertexAttributeSemanticsID::Normal:
					{
						const auto conversionFunction = gmutil::getGeometryConversionFunction<math::Vec3d>( attributeDataBaseType );

						assimpReadMeshVertexAttributeData(
								pAiMesh->mNormals,
								pAiMesh->mNumVertices,
								attributeDataWriteRegion,
								attributeFormat,
								conversionFunction );

						break;
					}
					case EVertexAttributeSemanticsID::TexCoord0:
					{
						const auto conversionFunction = gmutil::getGeometryConversionFunction<math::Vec3d>( attributeDataBaseType );

						assimpReadMeshVertexAttributeData(
								pAiMesh->mTextureCoords[0],
								pAiMesh->mNumVertices,
								attributeDataWriteRegion,
								attributeFormat,
								conversionFunction );

						break;
					}
					default:
					{
						break;
					}
				}
			}
		}
	}

	std::unique_ptr<MeshData> assimpImportScene( const aiScene * pAiScene, const GeometryDataFormat & pGeometryDataFormat )
	{
		const auto meshSizeMetrics = getAssimpMeshSizeMetrics( pAiScene );

		auto meshData = std::make_unique<MeshData>( pGeometryDataFormat );
		meshData->initializeStorage( meshSizeMetrics.indexElementsNum, meshSizeMetrics.vertexElementsNum );

		for( native_uint iMesh = 0; iMesh < pAiScene->mNumMeshes; ++iMesh )
		{
			const auto * aiMesh = pAiScene->mMeshes[iMesh];
			assimpImportMesh( aiMesh, *meshData );
		}

		return meshData;
	}


	std::unique_ptr<MeshData> AssimpMeshImporter::importMeshDefault(
			const std::string & pFilename,
			const GeometryDataFormat & pGeometryDataFormat )
	{
		Assimp::Importer aImporter;

		auto * aiSceneObject = aImporter.ReadFile(
				"assets/meshes/tree/Tree.obj",
				aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph | aiProcess_FlipWindingOrder | aiProcess_GenSmoothNormals | aiProcess_OptimizeMeshes );

		if( !aiSceneObject )
		{
			ts3DebugInterrupt();
			throw 0;
		}

		return assimpImportScene( aiSceneObject, pGeometryDataFormat );
	}


//	namespace gmutil
//	{
//
//		MeshVertexMemoryLayout generateMeshVertexMemoryLayoutForVertexFormat( const MeshVertexFormat & pMeshVertexFormat )
//		{
//			MeshVertexMemoryLayout vertexMemoryLayout;
//
//			for( const auto & vertexAttribute : pMeshVertexFormat.attributes )
//			{
//				const auto attributeFlag = cxdefs::getVertexAttributeFlags( vertexAttribute.attributeID );
//				if( pMeshVertexFormat.activeAttributesMask.isSet( attributeFlag ) )
//				{
//					const auto attributeIndex = cxdefs::getVertexAttributeIndex( vertexAttribute.attributeID );
//
//					auto & attributeLayout = vertexMemoryLayout.attributeLayouts[attributeIndex];
//					attributeLayout.baseOffsetInBytes = vertexAttribute.relativeOffset;
//					attributeLayout.sizeInBytes = gpuapi::cxdefs::getVertexAttribFormatByteSize( vertexAttribute.format );
//
//					auto & attributeStreamLayout = vertexMemoryLayout.vertexStreamLayouts[vertexAttribute.streamIndex];
//					attributeStreamLayout.elementSizeInBytes += attributeLayout.sizeInBytes;
//					attributeStreamLayout.activeAttributesMask.set( attributeFlag );
//				}
//			}
//
//			return vertexMemoryLayout;
//		}
//
//	}
//
//
//	MeshDefinitionDefaultP3N3T::SubMeshDefinition assimpLoadSubMeshDefault( aiMesh * pAssimpMesh );
//
//
//	MeshDefinitionDefaultP3N3T AssimpMeshLoader::loadMeshDefault( const std::string & pFilename )
//	{
//		Assimp::Importer aImporter;
//
//		auto * aiSceneObject = aImporter.ReadFile(
//				"assets/meshes/tree/Tree.obj",
//				aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph | aiProcess_Triangulate | aiProcess_FlipWindingOrder | aiProcess_GenSmoothNormals );
//
//		if( !aiSceneObject )
//		{
//			ts3DebugInterrupt();
//			throw 0;
//		}
//
//		MeshDefinitionDefaultP3N3T meshDefinition;
//
//		meshDefinition.subMeshes.resize( aiSceneObject->mNumMeshes );
//		meshDefinition.materials.resize( aiSceneObject->mNumMaterials );
//
//		for( native_uint iMesh = 0; iMesh < aiSceneObject->mNumMeshes; ++iMesh )
//		{
//			auto subMesh = assimpLoadSubMeshDefault( aiSceneObject->mMeshes[iMesh] );
//			meshDefinition.subMeshesTotalIndicesNum += subMesh.meshData.indicesNum;
//			meshDefinition.subMeshesTotalIndicesSizeInBytes += subMesh.meshData.indicesSizeInBytes;
//			meshDefinition.subMeshesTotalVerticesNum += subMesh.meshData.verticesNum;
//			meshDefinition.subMeshesTotalVerticesSizeInBytes += subMesh.meshData.verticesSizeInBytes;
//			meshDefinition.subMeshes[iMesh] = std::move( subMesh );
//		}
//
//		for( native_uint iMaterial = 0; iMaterial < aiSceneObject->mNumMaterials; ++iMaterial )
//		{
//			const auto * materialPtr = aiSceneObject->mMaterials[iMaterial];
//			if( materialPtr->GetTextureCount( aiTextureType_DIFFUSE ) > 0 )
//			{
//				aiString assimpTexturePath;
//				if ( materialPtr->GetTexture( aiTextureType_DIFFUSE, 0, &assimpTexturePath ) == AI_SUCCESS )
//		        {
//					std::string texturePath = "/meshes/tree/";
//			        texturePath.append( assimpTexturePath.C_Str() );
//					const auto pngDataBuffer = _fileLoadCallback( texturePath );
//
//			        MaterialData materialData;
//			        materialData.diffuseTextureData = loadPNGFromMemory( pngDataBuffer.data(), pngDataBuffer.size() );
//
//					if( materialData.diffuseTextureData.empty() )
//					{
//						continue;
//					}
//
//			        meshDefinition.materials[iMaterial] = std::move( materialData );
//		        }
//			}
//		}
//
//		return meshDefinition;
//	}
//
//
//	MeshDefinitionDefaultP3N3T::SubMeshDefinition assimpLoadSubMeshDefault( aiMesh * pAssimpMesh )
//	{
//		MeshDefinitionDefaultP3N3T::SubMeshDefinition subMeshDefinition;
//
//		const auto verticesNum = pAssimpMesh->mNumVertices;
//		const auto facesNum = pAssimpMesh->mNumFaces;
//
//		subMeshDefinition.meshData.initialize( verticesNum, facesNum );
//
//		for( native_uint iVertex = 0; iVertex < pAssimpMesh->mNumVertices; ++iVertex )
//		{
//			auto * targetVertex = subMeshDefinition.meshData.vertexPtr( iVertex );
//
//			targetVertex->color = math::Vec4f( 1.0f, 1.0f, 1.0f, 0.0f );
//
//			const auto & vPosition = pAssimpMesh->mVertices[iVertex];
//			targetVertex->position.x = vPosition.x;
//			targetVertex->position.y = vPosition.y;
//			targetVertex->position.z = vPosition.z;
//
//			if( pAssimpMesh->mNormals )
//			{
//				const auto & vNormal = pAssimpMesh->mNormals[iVertex];
//				targetVertex->normal.x = vNormal.x;
//				targetVertex->normal.y = vNormal.y;
//				targetVertex->normal.z = vNormal.z;
//			}
//
//			if( pAssimpMesh->mTangents )
//			{
//				const auto & vTangent = pAssimpMesh->mTangents[iVertex];
//				targetVertex->tangent.x = vTangent.x;
//				targetVertex->tangent.y = vTangent.y;
//				targetVertex->tangent.z = vTangent.z;
//			}
//
//			if( pAssimpMesh->mBitangents )
//			{
//				const auto & vBiTangent = pAssimpMesh->mBitangents[iVertex];
//				targetVertex->biTangent.x = vBiTangent.x;
//				targetVertex->biTangent.y = vBiTangent.y;
//				targetVertex->biTangent.z = vBiTangent.z;
//			}
//
//			const auto & vTexCoord = pAssimpMesh->mTextureCoords[0][iVertex];
//			targetVertex->texCoord.x = vTexCoord.x;
//			targetVertex->texCoord.y = vTexCoord.y;
//		}
//
//		for( native_uint iFace = 0; iFace < pAssimpMesh->mNumFaces; ++iFace )
//		{
//			const auto & vFace = pAssimpMesh->mFaces[iFace];
//			if( vFace.mNumIndices != 3 )
//			{
//				ts3DebugInterrupt();
//				throw 0;
//			}
//
//			auto * targetIndexBase = subMeshDefinition.meshData.indexPtr( iFace * 3 );
//
//			targetIndexBase[0] = vFace.mIndices[0];
//			targetIndexBase[1] = vFace.mIndices[1];
//			targetIndexBase[2] = vFace.mIndices[2];
//		}
//
//		subMeshDefinition.meshData.vfaNormals = ( pAssimpMesh->mNormals != nullptr );
//		subMeshDefinition.meshData.vfaTangents = ( pAssimpMesh->mTangents != nullptr );
//		subMeshDefinition.meshData.vfaBiTangents = ( pAssimpMesh->mBitangents != nullptr );
//
//		subMeshDefinition.materialIndex = pAssimpMesh->mMaterialIndex;
//
//		return subMeshDefinition;
//	}

}
