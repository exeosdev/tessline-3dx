
#include "scfXMLCommon.h"
#include <ts3/stdext/mapUtils.h>
#include <ts3/core/exception.h>

namespace ts3
{

	SCFXMLResourceNode::SCFXMLResourceNode( std::nullptr_t )
	: _xmlNode( nullptr )
	{}

	const std::string & SCFXMLResourceNode::resourceID() const
	{
		return _id;
	}

	const std::string & SCFXMLResourceNode::resourceType() const
	{
		return _type;
	}

	const XMLNode * SCFXMLResourceNode::property( const StringView<char> & pPropertyName ) const
	{
		return stdx::getMapValueRefOrDefault( _propertyNodeMap, pPropertyName, nullptr );
	}

	const SCFXMLResourceNode::PropertyNodeList & SCFXMLResourceNode::getPropertyNodes() const
	{
		return _propertyNodeList;
	}

	bool SCFXMLResourceNode::hasProperty( const StringView<char> & pPropertyName ) const
	{
		return _propertyNodeMap.find( pPropertyName ) != _propertyNodeMap.end();
	}

	SCFXMLResourceNode SCFXMLResourceNode::initFromXMLNode( XMLNode pXMLNode )
	{
		const auto xmlNodeName = pXMLNode.name();

		if( xmlNodeName != "resource" )
		{
			ts3ThrowDesc( E_EXC_DEBUG_PLACEHOLDER, "SCFXML node is not valid resource node: <" + xmlNodeName + ">" );
		}

		const auto idAttribute = pXMLNode.attribute( "id" );
		const auto typeAttribute = pXMLNode.attribute( "type" );

		if( idAttribute.empty() || typeAttribute.empty() )
		{
			ts3ThrowDesc( E_EXC_DEBUG_PLACEHOLDER, "SCFXML resource node without 'id' and/or 'type' attribute(s)" );
		}

		SCFXMLResourceNode resourceNode{};
		resourceNode._xmlNode = pXMLNode;
		resourceNode._id = idAttribute.value();
		resourceNode._type = typeAttribute.value();

		const auto subNodesNum = pXMLNode.countSubNodes();
		if( subNodesNum > 0 )
		{
			auto propertyNodes = _readProperties( pXMLNode, subNodesNum );
			ts3DebugAssert( !propertyNodes.empty() );

			auto propertyMap = _buildPropertyNodeMap( propertyNodes );
			ts3DebugAssert( !propertyMap.empty() );

			resourceNode._propertyNodeList = std::move( propertyNodes );
			resourceNode._propertyNodeMap = std::move( propertyMap );
		}

		return resourceNode;
	}

	SCFXMLResourceNode::PropertyNodeList SCFXMLResourceNode::_readProperties( const XMLNode & pXMLNode, size_t pPropertyNodesNum )
	{
		PropertyNodeList propertyNodes{};
		propertyNodes.reserve( pPropertyNodesNum );

		for( auto propertyXMLNode = pXMLNode.firstSubNode(); propertyXMLNode.valid(); )
		{
			propertyNodes.push_back( propertyXMLNode );
			propertyXMLNode = propertyXMLNode.nextSibling();
		}

		return propertyNodes;
	}

	SCFXMLResourceNode::PropertyNodeMap SCFXMLResourceNode::_buildPropertyNodeMap( PropertyNodeList & pPropertyNodes )
	{
		ts3DebugAssert( !pPropertyNodes.empty() );

		PropertyNodeMap propertyMap{};
		for( auto & propertyNode : pPropertyNodes )
		{
			propertyMap[propertyNode.name()] = &propertyNode;
		}

		return propertyMap;
	}


	SCFXMLFolderNode::SCFXMLFolderNode( std::nullptr_t )
	: _xmlNode( nullptr )
	{}

	const std::string & SCFXMLFolderNode::folderName() const
	{
		return _name;
	}

	const SCFXMLResourceNode * SCFXMLFolderNode::resource( const StringView<char> & pResourceID ) const
	{
		return stdx::getMapValueRefOrDefault( _resourceNodeMap, pResourceID, nullptr );
	}

	const SCFXMLFolderNode * SCFXMLFolderNode::subFolder( const StringView<char> & pSubFolderName ) const
	{
		return stdx::getMapValueRefOrDefault( _subFolderNodeMap, pSubFolderName, nullptr );
	}

	const SCFXMLFolderNode::ResourceNodeList & SCFXMLFolderNode::getResourceNodes() const
	{
		return _resourceNodeList;
	}

	const SCFXMLFolderNode::SubFolderNodeList & SCFXMLFolderNode::getSubFolderNodes() const
	{
		return _subFolderNodeList;
	}

	bool SCFXMLFolderNode::hasResource( const StringView<char> & pResourceID ) const
	{
		return _resourceNodeMap.find( pResourceID ) != _resourceNodeMap.end();
	}

	bool SCFXMLFolderNode::hasSubFolder( const StringView<char> & pSubFolderName ) const
	{
		return _subFolderNodeMap.find( pSubFolderName ) != _subFolderNodeMap.end();
	}

	SCFXMLFolderNode SCFXMLFolderNode::initFromXMLNode( XMLNode pXMLNode )
	{
		const auto xmlNodeName = pXMLNode.name();

		if( xmlNodeName != "folder" )
		{
			ts3ThrowDesc( E_EXC_DEBUG_PLACEHOLDER, "SCFXML node is not valid folder node: <" + xmlNodeName + ">" );
		}

		const auto nameAttribute = pXMLNode.attribute( "name" );

		if( nameAttribute.empty() )
		{
			ts3ThrowDesc( E_EXC_DEBUG_PLACEHOLDER, "SCFXML folder node without 'name' attribute" );
		}

		SCFXMLFolderNode folderNode{};
		folderNode._xmlNode = pXMLNode;
		folderNode._name = nameAttribute.value();

		const auto resourceNodesNum = pXMLNode.countSubNodes( "resource" );
		if( resourceNodesNum > 0 )
		{
			auto resourceNodes = _readResources( pXMLNode, resourceNodesNum );
			ts3DebugAssert( !resourceNodes.empty() );

			auto resourceMap = _buildResourceNodeMap( resourceNodes );
			ts3DebugAssert( !resourceMap.empty() );

			folderNode._resourceNodeList = std::move( resourceNodes );
			folderNode._resourceNodeMap = std::move( resourceMap );
		}

		const auto subFolderNodesNum = pXMLNode.countSubNodes( "folder" );
		if( subFolderNodesNum > 0 )
		{
			auto subFolderNodes = _readSubFolders( pXMLNode, subFolderNodesNum );
			ts3DebugAssert( !subFolderNodes.empty() );

			auto subFolderMap = _buildSubFolderNodeMap( subFolderNodes );
			ts3DebugAssert( !subFolderNodes.empty() );

			folderNode._subFolderNodeList = std::move( subFolderNodes );
			folderNode._subFolderNodeMap = std::move( subFolderMap );
		}

		return folderNode;
	}

	SCFXMLFolderNode::ResourceNodeList SCFXMLFolderNode::_readResources( const XMLNode & pXMLNode, size_t pResourceNodesNum )
	{
		ts3DebugAssert( pResourceNodesNum > 0 );

		ResourceNodeList resourceNodes{};
		resourceNodes.reserve( pResourceNodesNum );

		for( auto resourceXMLNode = pXMLNode.firstSubNode( "resource" ); resourceXMLNode.valid(); )
		{
			auto scfResourceNode = SCFXMLResourceNode::initFromXMLNode( resourceXMLNode );
			resourceNodes.push_back( std::move( scfResourceNode ) );
			resourceXMLNode = resourceXMLNode.nextSibling();
		}

		return resourceNodes;
	}

	SCFXMLFolderNode::ResourceNodeMap SCFXMLFolderNode::_buildResourceNodeMap( ResourceNodeList & pResourceNodes )
	{
		ts3DebugAssert( !pResourceNodes.empty() );

		ResourceNodeMap resourceMap{};
		for( auto & resourceNode : pResourceNodes )
		{
			resourceMap[resourceNode.resourceID()] = &resourceNode;
		}

		return resourceMap;
	}

	SCFXMLFolderNode::SubFolderNodeList SCFXMLFolderNode::_readSubFolders( const XMLNode & pXMLNode, size_t pSubFolderNodesNum )
	{
		ts3DebugAssert( pSubFolderNodesNum > 0 );

		SubFolderNodeList subFolderNodes{};
		subFolderNodes.reserve( pSubFolderNodesNum );

		for( auto subFolderXMLNode = pXMLNode.firstSubNode( "folder" ); subFolderXMLNode.valid(); )
		{
			auto scfSubFolderNode = SCFXMLFolderNode::initFromXMLNode( subFolderXMLNode );
			subFolderNodes.push_back( std::move( scfSubFolderNode ) );
			subFolderXMLNode = subFolderXMLNode.nextSibling();
		}

		return subFolderNodes;
	}

	SCFXMLFolderNode::SubFolderNodeMap SCFXMLFolderNode::_buildSubFolderNodeMap( SubFolderNodeList & pSubFolderNodes )
	{
		ts3DebugAssert( !pSubFolderNodes.empty() );

		SubFolderNodeMap subFolderMap{};
		for( auto & subFolderNode : pSubFolderNodes )
		{
			subFolderMap[subFolderNode.folderName()] = &subFolderNode;
		}

		return subFolderMap;
	}

}
