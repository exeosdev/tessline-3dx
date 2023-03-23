
#include "scfXMLCommon.h"
#include <ts3/stdext/stlHelperAlgo.h>
#include <ts3/core/exception.h>
#include <ts3/engine/exception.h>

namespace ts3
{

	SCFXMLNode::SCFXMLNode( std::nullptr_t )
	: _nodeType( ESCFXMLNodeType::Unknown )
	{}

	ESCFXMLNodeType SCFXMLNode::nodeType() const
	{
		return _nodeType;
	}

	const std::string & SCFXMLNode::nodeName() const
	{
		return _nodeName;
	}

	const std::string & SCFXMLNode::nodeTextValue() const
	{
		return _nodeTextValue;
	}

	bool SCFXMLNode::empty() const
	{
		return _xmlNode.empty();
	}


	SCFXMLResourceNode::SCFXMLResourceNode( std::nullptr_t )
	{}

	const std::string & SCFXMLResourceNode::resourceID() const
	{
		return _id;
	}

	const std::string & SCFXMLResourceNode::resourceType() const
	{
		return _type;
	}

	XMLAttribute SCFXMLResourceNode::attribute( const StringView<char> & pAttribName ) const
	{
		return _xmlNode.attribute( pAttribName );
	}

	XMLNode SCFXMLResourceNode::dataNode( const StringView<char> & pNodeName ) const
	{
		return _xmlNode.firstSubNode( pNodeName );
	}

	bool SCFXMLResourceNode::hasDataNode( const StringView<char> & pNodeName ) const
	{
		return _xmlNode.hasSubNode( pNodeName );
	}

	SCFXMLResourceNode SCFXMLResourceNode::initFromXMLNode( XMLNode pXMLNode )
	{
		SCFXMLResourceNode resultNode{};
		initFromXMLNode( std::move( pXMLNode ), resultNode );
		return resultNode;
	}

	void SCFXMLResourceNode::initFromXMLNode( XMLNode pXMLNode, SCFXMLResourceNode & pInitNode )
	{
		auto xmlNodeName = pXMLNode.name();

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

		pInitNode._xmlNode = pXMLNode;
		pInitNode._nodeType = ESCFXMLNodeType::Resource;
		pInitNode._nodeName = std::move( xmlNodeName );
		pInitNode._nodeTextValue = pXMLNode.value();
		pInitNode._id = idAttribute.value();
		pInitNode._type = typeAttribute.value();
	}


	SCFXMLFolderNode::SCFXMLFolderNode( std::nullptr_t )
	{}

	const std::string & SCFXMLFolderNode::folderName() const
	{
		return _folderName;
	}

	const SCFXMLResourceNode * SCFXMLFolderNode::resource( const StringView<char> & pResourceID ) const
	{
		return stdx::getMapValueRefOrDefault( _resourceNodeMap, pResourceID, nullptr );
	}

	const SCFXMLFolderNode * SCFXMLFolderNode::subFolder( const StringView<char> & pSubFolderName ) const
	{
		return stdx::getMapValueRefOrDefault( _subFolderNodeMap, pSubFolderName, nullptr );
	}

	SCFXMLNodeList SCFXMLFolderNode::getNodeList( bool pRecursive ) const
	{
		SCFXMLNodeList nodeList{};

		if( !pRecursive )
		{
			const auto totalNodesNum = _resourceNodeList.size() + _subFolderNodeList.size();
			nodeList.reserve( nodeList.size() + totalNodesNum );
		}

		getNodeList( nodeList, pRecursive );

		return nodeList;
	}

	SCFXMLNodeList & SCFXMLFolderNode::getNodeList( SCFXMLNodeList & pOutputList, bool pRecursive ) const
	{
		for( const auto & resourceNode : _resourceNodeList )
		{
			pOutputList.push_back( &resourceNode );
		}

		for( const auto & subFolderNode : _subFolderNodeList )
		{
			pOutputList.push_back( &subFolderNode );
			if( pRecursive )
			{
				subFolderNode.getNodeList( pOutputList, true );
			}
		}

		return pOutputList;
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
		SCFXMLFolderNode resultNode{};
		initFromXMLNode( std::move( pXMLNode ), resultNode );
		return resultNode;
	}

	void SCFXMLFolderNode::initFromXMLNode( XMLNode pXMLNode, SCFXMLFolderNode & pInitNode )
	{
		auto xmlNodeName = pXMLNode.name();

		if( xmlNodeName != "folder" )
		{
			ts3ThrowDesc( E_EXC_DEBUG_PLACEHOLDER, "SCFXML node is not valid folder node: <" + xmlNodeName + ">" );
		}

		const auto nameAttribute = pXMLNode.attribute( "name" );

		if( nameAttribute.empty() )
		{
			ts3ThrowDesc( E_EXC_DEBUG_PLACEHOLDER, "SCFXML folder node without 'name' attribute" );
		}

		pInitNode._xmlNode = pXMLNode;
		pInitNode._nodeType = ESCFXMLNodeType::Folder;
		pInitNode._nodeName = std::move( xmlNodeName );
		pInitNode._nodeTextValue = pXMLNode.value();
		pInitNode._folderName = nameAttribute.value();

		_initContent( pXMLNode, pInitNode );
	}

	void SCFXMLFolderNode::_initContent( const XMLNode & pXMLNode, SCFXMLFolderNode & pInitNode )
	{
		const auto resourceNodesNum = pXMLNode.countSubNodes( "resource" );
		if( resourceNodesNum > 0 )
		{
			auto resourceNodes = _readResources( pXMLNode, resourceNodesNum );
			ts3DebugAssert( !resourceNodes.empty() );

			auto resourceMap = _buildResourceNodeMap( resourceNodes );
			ts3DebugAssert( !resourceMap.empty() );

			pInitNode._resourceNodeList = std::move( resourceNodes );
			pInitNode._resourceNodeMap = std::move( resourceMap );
		}

		const auto subFolderNodesNum = pXMLNode.countSubNodes( "folder" );
		if( subFolderNodesNum > 0 )
		{
			auto subFolderNodes = _readSubFolders( pXMLNode, subFolderNodesNum );
			ts3DebugAssert( !subFolderNodes.empty() );

			auto subFolderMap = _buildSubFolderNodeMap( subFolderNodes );
			ts3DebugAssert( !subFolderNodes.empty() );

			pInitNode._subFolderNodeList = std::move( subFolderNodes );
			pInitNode._subFolderNodeMap = std::move( subFolderMap );
		}
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
			resourceXMLNode = resourceXMLNode.nextSibling( "resource" );
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
			subFolderXMLNode = subFolderXMLNode.nextSibling( "folder" );
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


	SCFXMLRootNode::SCFXMLRootNode( std::nullptr_t )
	: SCFXMLFolderNode( nullptr )
	{}

	SCFXMLRootNode SCFXMLRootNode::initFromXMLTree( XMLTree pXMLTree )
	{
		SCFXMLRootNode resultNode{};
		initFromXMLTree( std::move( pXMLTree ), resultNode );
		return resultNode;
	}

	void SCFXMLRootNode::initFromXMLTree( XMLTree pXMLTree, SCFXMLRootNode & pInitNode )
	{
		const auto scfNode = pXMLTree.rootNode();
		if( scfNode.name() != "scf" )
		{
			ts3Throw( E_EXC_ESM_MAIN_SCF_ERROR );
		}

		const auto rootNode = scfNode.firstSubNode();
		if( rootNode.name() != "root" )
		{
			ts3Throw( E_EXC_ESM_MAIN_SCF_ERROR );
		}

		pInitNode._xmlNode = rootNode;
		pInitNode._folderName.clear();
		pInitNode._xmlTree = std::move( pXMLTree );

		_initContent( rootNode, pInitNode );
	}

} // namespace ts3
