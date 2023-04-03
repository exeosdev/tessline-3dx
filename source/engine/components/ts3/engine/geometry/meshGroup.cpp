
#include "mesh.h"
#include "meshGroup.h"
#include "geometryStorage.h"

namespace ts3
{

	MeshGroup::MeshGroup( std::string pGroupName, GeometryStorage * pGeometryStorage )
	: mGroupName( std::move( pGroupName ) )
	, mGeometryDataFormat( pGeometryStorage->mDataFormat )
	, _geometryStorage( pGeometryStorage )
	{}

	MeshGroup::~MeshGroup()
	{
		if( _geometryStorage )
		{
			_geometryStorage->releaseStorage();
			_geometryStorage = nullptr;
		}
	}

	Mesh * MeshGroup::getMesh( size_t pIndex ) const noexcept
	{
		return ( pIndex < _meshes.size() ) ? _meshes[pIndex] : nullptr;
	}

	Mesh * MeshGroup::findMesh( const std::string & pName ) const noexcept
	{
		auto * meshComponentPtr = findMeshComponent( pName );
		if( meshComponentPtr && !meshComponentPtr->isMeshSubComponent() )
		{
			return dynamic_cast_dbg<Mesh *>( meshComponentPtr );
		}

		return nullptr;
	}

	MeshComponent * MeshGroup::findMeshComponent( const std::string & pName ) const noexcept
	{
		auto meshComponentIter = _meshComponents.find( pName );
		if( meshComponentIter != _meshComponents.end() )
		{
			return meshComponentIter->second;
		}

		return nullptr;
	}

	MeshComponent * MeshGroup::findMeshSubComponent( const Mesh & pMesh, const std::string & pName ) const noexcept
	{
		auto * meshComponentPtr = findMeshComponent( pName );
		if( meshComponentPtr && ( meshComponentPtr->parentMesh() == &pMesh ) )
		{
			ts3DebugAssert( meshComponentPtr->isMeshSubComponent() );
			return meshComponentPtr;
		}

		return nullptr;
	}

	Mesh * MeshGroup::addMesh( std::string pName )
	{
		auto & mesh = _meshStorage.emplace_back( *this, std::move( pName ) );
		_meshes.push_back( &mesh );
		return &mesh;
	}

	void MeshGroup::registerMeshComponent( MeshComponent & pMeshComponent )
	{
		_meshComponents[std::string_view( pMeshComponent.name() )] = &pMeshComponent;
	}

}
