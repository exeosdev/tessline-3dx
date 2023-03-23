
#include "meshCommon.h"
#include <ts3/engine/res/image/pngCommon.h>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/StringUtils.h>

namespace ts3
{

	MeshDefinitionDefaultP3N3T::SubMeshDefinition assimpLoadSubMeshDefault( aiMesh * pAssimpMesh );


	MeshDefinitionDefaultP3N3T AssimpMeshLoader::loadMeshDefault( const std::string & pFilename )
	{
		Assimp::Importer aImporter;

		auto * aiSceneObject = aImporter.ReadFile(
				"assets/meshes/tree/Tree.obj",
				aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph | aiProcess_Triangulate | aiProcess_FlipWindingOrder );

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
			const auto & vPosition = pAssimpMesh->mVertices[iVertex];
			const auto & vNormal = pAssimpMesh->mNormals[iVertex];
			const auto & vTexCoord = pAssimpMesh->mTextureCoords[0][iVertex];

			auto * targetVertex = subMeshDefinition.meshData.vertexPtr( iVertex );

			targetVertex->position.x = vPosition.x;
			targetVertex->position.y = vPosition.y;
			targetVertex->position.z = vPosition.z;

			targetVertex->color = math::Vec4f( 1.0f, 1.0f, 1.0f, 0.0f );

			targetVertex->normal.x = vNormal.x;
			targetVertex->normal.y = vNormal.y;
			targetVertex->normal.z = vNormal.z;

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

		subMeshDefinition.materialIndex = pAssimpMesh->mMaterialIndex;

		return subMeshDefinition;
	}

}
