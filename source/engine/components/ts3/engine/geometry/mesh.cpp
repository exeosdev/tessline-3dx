
#include "meshGroup.h"

namespace ts3
{

	MeshComponent::MeshComponent( Mesh & pParentMesh, std::string pName, GeometryRefHandle pGeometryRef )
	: _meshGroup( pParentMesh.meshGroup() )
	, _parentMesh( &pParentMesh )
	, _name( std::move( pName ) )
	, _geometryRef( pGeometryRef )
	{
		ts3DebugAssert( _meshGroup );
		_meshGroup->registerMeshComponent( *this );
	}

	MeshComponent::MeshComponent( MeshGroup & pMeshGroup, std::string pName, GeometryRefHandle pGeometryRef )
	: _meshGroup( &pMeshGroup )
	, _parentMesh( nullptr )
	, _name( std::move( pName ) )
	, _geometryRef( pGeometryRef )
	{
		ts3DebugAssert( _meshGroup );
		_meshGroup->registerMeshComponent( *this );
	}

	MeshComponent::~MeshComponent() = default;

	EMeshObjectType MeshComponent::getMeshObjectType() const noexcept
	{
		return EMeshObjectType::SubComponent;
	}


	Mesh::Mesh( MeshGroup & pMeshGroup, std::string pName )
	: MeshComponent( pMeshGroup, std::move( pName ), &_meshGeometryRef )
	, _meshGeometryRef( pMeshGroup.getGeometryStorage()  )
	{}

	Mesh::~Mesh() = default;

	EMeshObjectType Mesh::getMeshObjectType() const noexcept
	{
		return EMeshObjectType::Mesh;
	}

	MeshComponent * Mesh::findSubComponent( const std::string & pName ) const noexcept
	{
		return _meshGroup->findMeshSubComponent( *this, pName );
	}

	MeshComponent * Mesh::addMeshComponent( std::string pName, GeometryRefHandle pGeometryRef )
	{
		auto & meshSubComponent = _meshSubComponents.emplace_back( *this, std::move( pName ), pGeometryRef );
		_meshSubComponentsIndex.push_back( &meshSubComponent );
		_meshGeometryRef.dataReference.append( pGeometryRef->dataReference );
		return &meshSubComponent;
	}

}
