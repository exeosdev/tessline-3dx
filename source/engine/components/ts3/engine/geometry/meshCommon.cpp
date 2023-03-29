
#include "meshCommon.h"
#include "dataTypesConv.h"
#include <ts3/engine/res/image/pngCommon.h>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/StringUtils.h>

namespace ts3
{

	void MeshData::initializeStorage( uint32 pIndicesNum, uint32 pVerticesNum )
	{
		if( pIndicesNum > 0 )
		{
			const auto indexDataBufferSize = mDataLayout.indexElementSizeInBytes * pIndicesNum;
			_dataStorage.indexDataBuffer.resize( indexDataBufferSize );
		}

		for( uint32 iVertexStream = 0; iVertexStream < gpa::MAX_GEOMETRY_SEPARATE_VERTEX_COMPONENTS_NUM; ++iVertexStream )
		{
			if( mDataLayout.isVertexStreamActive( iVertexStream ) )
			{
				const auto vertexStreamElementSize = mDataLayout.vertexMemoryLayout.vertexStreamLayouts[iVertexStream].elementSizeInBytes;
				const auto vertexStreamDataBufferSize = vertexStreamElementSize * pVerticesNum;
				_dataStorage.vertexDataBuffers[iVertexStream].resize( vertexStreamDataBufferSize );
			}
		}
	}

	MeshDataSubMeshReference & MeshData::addSubMesh( uint32 pIndicesNum, uint32 pVerticesNum )
	{
		MeshDataSubMeshReference subMeshReference;
		subMeshReference.subMeshIndex = 0;
		subMeshReference.indexDataOffsetInElementsNum = 0;
		subMeshReference.vertexDataOffsetInElementsNum = 0;
		subMeshReference.indexDataElementsNum = pIndicesNum;
		subMeshReference.vertexDataElementsNum = pVerticesNum;

		if( !_subMeshes.empty() )
		{
			const auto & lastSubMesh = _subMeshes.back();
			subMeshReference.subMeshIndex = numeric_cast<uint32>( _subMeshes.size() );
			subMeshReference.indexDataOffsetInElementsNum = lastSubMesh.indexDataOffsetInElementsNum + lastSubMesh.indexDataElementsNum;
			subMeshReference.vertexDataOffsetInElementsNum = lastSubMesh.vertexDataOffsetInElementsNum + lastSubMesh.vertexDataElementsNum;
		}

		_subMeshes.push_back( subMeshReference );

		return _subMeshes.back();
	}

	MeshDataBufferWriteRegion MeshData::getIndexDataWriteRegion( const MeshDataSubMeshReference & pSubMeshReference )
	{
		MeshDataBufferWriteRegion writeRegion;
		writeRegion.basePtr = _dataStorage.indexDataBuffer.data() + ( pSubMeshReference.indexDataOffsetInElementsNum * mDataLayout.indexElementSizeInBytes );
		writeRegion.strideInBytes = mDataLayout.indexElementSizeInBytes;
		return writeRegion;
	}

	MeshDataBufferWriteRegion MeshData::getVertexDataWriteRegion( const MeshDataSubMeshReference & pSubMeshReference, uint32 pAttributeIndex )
	{
		const auto & attributeDefinition = mDataLayout.vertexFormat.attributes[pAttributeIndex];
		const auto & attributeStreamDefinition = mDataLayout.vertexMemoryLayout.vertexStreamLayouts[attributeDefinition.streamIndex];

		auto * bufferBasePtr = _dataStorage.vertexDataBuffers[attributeDefinition.streamIndex].data();
		auto * streamBasePtr = bufferBasePtr + ( pSubMeshReference.vertexDataOffsetInElementsNum * attributeStreamDefinition.elementSizeInBytes );

		MeshDataBufferWriteRegion writeRegion;
		writeRegion.basePtr = streamBasePtr + attributeDefinition.relativeOffset;
		writeRegion.strideInBytes = attributeStreamDefinition.elementSizeInBytes;
		return writeRegion;
	}


	MeshDataMetrics getAssimpMeshDataMetrics( const aiScene * pAiScene )
	{
		MeshDataMetrics metrics;
		metrics.subMeshesNum = pAiScene->mNumMeshes;
		metrics.indexDataElementsNum = 0;
		metrics.vertexDataElementsNum = 0;

		for( native_uint iMesh = 0; iMesh < pAiScene->mNumMeshes; ++iMesh )
		{
			const auto * aiMesh = pAiScene->mMeshes[iMesh];
			metrics.indexDataElementsNum += aiMesh->mNumFaces * 3;
			metrics.vertexDataElementsNum += aiMesh->mNumVertices;
		}

		return metrics;
	}

	void assimpImportMesh( const aiMesh * pAiMesh, MeshData & pMeshData )
	{
		auto & meshReference = pMeshData.addSubMesh( pAiMesh->mNumFaces * 3, pAiMesh->mNumVertices );

		if( pAiMesh->mNumFaces > 0 )
		{
			const auto indexDataWriteRegion = pMeshData.getIndexDataWriteRegion( meshReference );
			const auto conversionFunction = gmutil::getGeometryConversionFunction<math::Vec3u32>( gpuapi::EBaseDataType::Uint32 );
			auto * writePtr = indexDataWriteRegion.basePtr;

			for( native_uint iFace = 0; iFace < pAiMesh->mNumFaces; ++iFace )
			{
				const auto & vFace = pAiMesh->mFaces[iFace];
				conversionFunction( vFace.mIndices, writePtr, 3 );
				writePtr += indexDataWriteRegion.strideInBytes;
			}
		}

		for( uint32 iAttribute = 0; iAttribute < gpa::MAX_GEOMETRY_VERTEX_ATTRIBUTES_NUM; ++iAttribute )
		{
			if( pMeshData.mDataLayout.isAttributeValid( iAttribute ) )
			{
				const auto & attributeDefinition = pMeshData.mDataLayout.vertexFormat.attributes[iAttribute];
				const auto attributeDataWriteRegion = pMeshData.getVertexDataWriteRegion( meshReference, iAttribute );

				switch( attributeDefinition.attributeID )
				{
					case EVertexAttributeID::Position:
					{
						const auto conversionFunction = gmutil::getGeometryConversionFunction<math::Vec3d>( attributeDefinition.format.baseType );
						auto * writePtr = attributeDataWriteRegion.basePtr;

						for( native_uint iVertex = 0; iVertex < pAiMesh->mNumVertices; ++iVertex )
						{
							const auto & vPosition = pAiMesh->mVertices[iVertex];
							conversionFunction( &vPosition, writePtr, attributeDefinition.format.componentsNum );
							writePtr += attributeDataWriteRegion.strideInBytes;
						}

						break;
					}
					case EVertexAttributeID::Normal:
					{
						const auto conversionFunction = gmutil::getGeometryConversionFunction<math::Vec3d>( attributeDefinition.format.baseType );
						auto * writePtr = attributeDataWriteRegion.basePtr;

						for( native_uint iVertex = 0; iVertex < pAiMesh->mNumVertices; ++iVertex )
						{
							const auto & vNormal = pAiMesh->mNormals[iVertex];
							conversionFunction( &vNormal, writePtr, attributeDefinition.format.componentsNum );
							writePtr += attributeDataWriteRegion.strideInBytes;
						}

						break;
					}
					case EVertexAttributeID::TexCoord0:
					{
						const auto conversionFunction = gmutil::getGeometryConversionFunction<math::Vec3d>( attributeDefinition.format.baseType );
						auto * writePtr = attributeDataWriteRegion.basePtr;

						for( native_uint iVertex = 0; iVertex < pAiMesh->mNumVertices; ++iVertex )
						{
							const auto & vTexCoord0 = pAiMesh->mTextureCoords[0][iVertex];
							conversionFunction( &vTexCoord0, writePtr, attributeDefinition.format.componentsNum );
							writePtr += attributeDataWriteRegion.strideInBytes;
						}

						break;
					}
				}
			}
		}
	}

	MeshData assimpImportScene( const aiScene * pAiScene, const MeshVertexFormat & pMeshVertexFormat )
	{
		const auto meshDataMetrics = getAssimpMeshDataMetrics( pAiScene );

		MeshDataLayout meshDataLayout;
		meshDataLayout.indexFormat = gpuapi::EIndexDataFormat::Uint32;
		meshDataLayout.indexElementSizeInBytes = gpuapi::cxdefs::getIndexDataFormatByteSize( meshDataLayout.indexFormat );
		meshDataLayout.vertexFormat = pMeshVertexFormat;
		meshDataLayout.vertexMemoryLayout = gmutil::generateMeshVertexMemoryLayoutForVertexFormat( pMeshVertexFormat );

		MeshData meshData{ meshDataLayout };
		meshData.initializeStorage( meshDataMetrics.indexDataElementsNum, meshDataMetrics.vertexDataElementsNum );

		for( native_uint iMesh = 0; iMesh < pAiScene->mNumMeshes; ++iMesh )
		{
			const auto * aiMesh = pAiScene->mMeshes[iMesh];
			assimpImportMesh( aiMesh, meshData );
		}

		return meshData;
	}


	MeshData AssimpMeshImporter::importMeshDefault( const std::string & pFilename )
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

		MeshVertexFormat meshVertexFormatDefault;
		meshVertexFormatDefault.activeAttributesMask =
				E_VERTEX_ATTRIBUTE_FLAG_POSITION_BIT |
				E_VERTEX_ATTRIBUTE_FLAG_NORMAL_BIT |
				E_VERTEX_ATTRIBUTE_FLAG_TEX_COORD_0_BIT;
		meshVertexFormatDefault.attributes[0] = {
				EVertexAttributeID::Position, { gpuapi::EBaseDataType::Float32, 3 }, 0, 0
		};
		meshVertexFormatDefault.attributes[1] = {
				EVertexAttributeID::Normal, { gpuapi::EBaseDataType::Float32, 3 }, 0, 12
		};
		meshVertexFormatDefault.attributes[2] = {
				EVertexAttributeID::TexCoord0, { gpuapi::EBaseDataType::Float32, 2 }, 0, 24
		};

		return assimpImportScene( aiSceneObject, meshVertexFormatDefault );
	}


	namespace gmutil
	{

		MeshVertexMemoryLayout generateMeshVertexMemoryLayoutForVertexFormat( const MeshVertexFormat & pMeshVertexFormat )
		{
			MeshVertexMemoryLayout vertexMemoryLayout;

			for( const auto & vertexAttribute : pMeshVertexFormat.attributes )
			{
				const auto attributeFlag = cxdefs::getVertexAttributeFlags( vertexAttribute.attributeID );
				if( pMeshVertexFormat.activeAttributesMask.isSet( attributeFlag ) )
				{
					const auto attributeIndex = cxdefs::getVertexAttributeIndex( vertexAttribute.attributeID );

					auto & attributeLayout = vertexMemoryLayout.attributeLayouts[attributeIndex];
					const auto baseTypeSizeInBytes = gpuapi::cxdefs::getBaseDataTypeByteSize( vertexAttribute.format.baseType );
					attributeLayout.baseOffsetInBytes = vertexAttribute.relativeOffset;
					attributeLayout.sizeInBytes = baseTypeSizeInBytes * vertexAttribute.format.componentsNum;

					auto & attributeStreamLayout = vertexMemoryLayout.vertexStreamLayouts[vertexAttribute.streamIndex];
					attributeStreamLayout.elementSizeInBytes += attributeLayout.sizeInBytes;
					attributeStreamLayout.activeAttributesMask.set( attributeFlag );
				}
			}

			return vertexMemoryLayout;
		}

	}


	MeshDefinitionDefaultP3N3T::SubMeshDefinition assimpLoadSubMeshDefault( aiMesh * pAssimpMesh );


	MeshDefinitionDefaultP3N3T AssimpMeshLoader::loadMeshDefault( const std::string & pFilename )
	{
		Assimp::Importer aImporter;

		auto * aiSceneObject = aImporter.ReadFile(
				"assets/meshes/tree/Tree.obj",
				aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph | aiProcess_Triangulate | aiProcess_FlipWindingOrder | aiProcess_GenSmoothNormals );

		if( !aiSceneObject )
		{
			ts3DebugInterrupt();
			throw 0;
		}

		MeshDefinitionDefaultP3N3T meshDefinition;

		meshDefinition.subMeshes.resize( aiSceneObject->mNumMeshes );
		meshDefinition.materials.resize( aiSceneObject->mNumMaterials );

		for( native_uint iMesh = 0; iMesh < aiSceneObject->mNumMeshes; ++iMesh )
		{
			auto subMesh = assimpLoadSubMeshDefault( aiSceneObject->mMeshes[iMesh] );
			meshDefinition.subMeshesTotalIndicesNum += subMesh.meshData.indicesNum;
			meshDefinition.subMeshesTotalIndicesSizeInBytes += subMesh.meshData.indicesSizeInBytes;
			meshDefinition.subMeshesTotalVerticesNum += subMesh.meshData.verticesNum;
			meshDefinition.subMeshesTotalVerticesSizeInBytes += subMesh.meshData.verticesSizeInBytes;
			meshDefinition.subMeshes[iMesh] = std::move( subMesh );
		}

		for( native_uint iMaterial = 0; iMaterial < aiSceneObject->mNumMaterials; ++iMaterial )
		{
			const auto * materialPtr = aiSceneObject->mMaterials[iMaterial];
			if( materialPtr->GetTextureCount( aiTextureType_DIFFUSE ) > 0 )
			{
				aiString assimpTexturePath;
				if ( materialPtr->GetTexture( aiTextureType_DIFFUSE, 0, &assimpTexturePath ) == AI_SUCCESS )
		        {
					std::string texturePath = "/meshes/tree/";
			        texturePath.append( assimpTexturePath.C_Str() );
					const auto pngDataBuffer = _fileLoadCallback( texturePath );

			        MaterialData materialData;
			        materialData.diffuseTextureData = loadPNGFromMemory( pngDataBuffer.data(), pngDataBuffer.size() );

					if( materialData.diffuseTextureData.empty() )
					{
						continue;
					}

			        meshDefinition.materials[iMaterial] = std::move( materialData );
		        }
			}
		}

		return meshDefinition;
	}


	MeshDefinitionDefaultP3N3T::SubMeshDefinition assimpLoadSubMeshDefault( aiMesh * pAssimpMesh )
	{
		MeshDefinitionDefaultP3N3T::SubMeshDefinition subMeshDefinition;

		const auto verticesNum = pAssimpMesh->mNumVertices;
		const auto facesNum = pAssimpMesh->mNumFaces;

		subMeshDefinition.meshData.initialize( verticesNum, facesNum );

		for( native_uint iVertex = 0; iVertex < pAssimpMesh->mNumVertices; ++iVertex )
		{
			auto * targetVertex = subMeshDefinition.meshData.vertexPtr( iVertex );

			targetVertex->color = math::Vec4f( 1.0f, 1.0f, 1.0f, 0.0f );

			const auto & vPosition = pAssimpMesh->mVertices[iVertex];
			targetVertex->position.x = vPosition.x;
			targetVertex->position.y = vPosition.y;
			targetVertex->position.z = vPosition.z;

			if( pAssimpMesh->mNormals )
			{
				const auto & vNormal = pAssimpMesh->mNormals[iVertex];
				targetVertex->normal.x = vNormal.x;
				targetVertex->normal.y = vNormal.y;
				targetVertex->normal.z = vNormal.z;
			}

			if( pAssimpMesh->mTangents )
			{
				const auto & vTangent = pAssimpMesh->mTangents[iVertex];
				targetVertex->tangent.x = vTangent.x;
				targetVertex->tangent.y = vTangent.y;
				targetVertex->tangent.z = vTangent.z;
			}

			if( pAssimpMesh->mBitangents )
			{
				const auto & vBiTangent = pAssimpMesh->mBitangents[iVertex];
				targetVertex->biTangent.x = vBiTangent.x;
				targetVertex->biTangent.y = vBiTangent.y;
				targetVertex->biTangent.z = vBiTangent.z;
			}

			const auto & vTexCoord = pAssimpMesh->mTextureCoords[0][iVertex];
			targetVertex->texCoord.x = vTexCoord.x;
			targetVertex->texCoord.y = vTexCoord.y;
		}

		for( native_uint iFace = 0; iFace < pAssimpMesh->mNumFaces; ++iFace )
		{
			const auto & vFace = pAssimpMesh->mFaces[iFace];
			if( vFace.mNumIndices != 3 )
			{
				ts3DebugInterrupt();
				throw 0;
			}

			auto * targetIndexBase = subMeshDefinition.meshData.indexPtr( iFace * 3 );

			targetIndexBase[0] = vFace.mIndices[0];
			targetIndexBase[1] = vFace.mIndices[1];
			targetIndexBase[2] = vFace.mIndices[2];
		}

		subMeshDefinition.meshData.vfaNormals = ( pAssimpMesh->mNormals != nullptr );
		subMeshDefinition.meshData.vfaTangents = ( pAssimpMesh->mTangents != nullptr );
		subMeshDefinition.meshData.vfaBiTangents = ( pAssimpMesh->mBitangents != nullptr );

		subMeshDefinition.materialIndex = pAssimpMesh->mMaterialIndex;

		return subMeshDefinition;
	}

}
