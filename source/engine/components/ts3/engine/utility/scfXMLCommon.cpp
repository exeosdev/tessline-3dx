
#include "scfXMLReader.h"

namespace ts3
{

	SCFXMLNode::SCFXMLNode( SCFXMLNodeType pNodeType, SCFXMLNode * pParent, RxNode * pRxNode )
	: _nodeType( pNodeType )
	, _parentNode( pParent )
	, _rxNode( pRxNode )
	{
		if( _rxNode )
		{
			_name.assign( _rxNode->name(), _rxNode->name_size() );
			_value.assign( _rxNode->value(), _rxNode->value_size() );

			if( auto * idAttribute = _rxNode->first_attribute( "id" ) )
			{
				_id.assign( idAttribute->value(), idAttribute->value_size() );
			}
		}
	}

	SCFXMLNode::operator bool() const
	{
		return _rxNode != nullptr;
	}

	SCFXMLNodeType SCFXMLNode::nodeType() const
	{
		return _nodeType;
	}

	std::string SCFXMLNode::attribute( const char * pName ) const
	{
		if( auto * rxAttrib = _rxNode->first_attribute( pName ) )
		{
			return std::string( rxAttrib->value(), rxAttrib->value_size() );
		}
		return {};
	}

	const std::string & SCFXMLNode::name() const
	{
		return _name;
	}

	const std::string & SCFXMLNode::value() const
	{
		return _value;
	}

	const std::string & SCFXMLNode::id() const
	{
		return _id;
	}

	SCFXMLNode * SCFXMLNode::parent() const
	{
		return _parentNode;
	}

	bool SCFXMLNode::hasParent() const
	{
		return _parentNode != nullptr;
	}

	bool SCFXMLNode::isRootNode() const
	{
		return _nodeType == SCFXMLNodeType::Root;
	}

	SCFXMLNode::RxNode * SCFXMLNode::getRxNode() const
	{
		return _rxNode;
	}


	SCFXMLPropertyNode::SCFXMLPropertyNode( SCFXMLNodeType pNodeType, SCFXMLNode * pParent, RxNode * pRxNode )
	: SCFXMLNode( pNodeType, pParent, pRxNode )
	{}

	const SCFXMLPropertyNode * SCFXMLPropertyNode::subNode( const char * pName ) const
	{
		auto nodeIter = std::lower_bound(
			_subNodes.begin(),
			_subNodes.end(),
			pName,
			[]( const SCFXMLPropertyNode & pNode, const char * pNodeName ) -> bool {
				return pNode.name() < pNodeName;
			});

		if( ( nodeIter != _subNodes.end() ) && ( nodeIter->name() == pName ) )
		{
			return &( *nodeIter );
		}

		return nullptr;
	}

	SCFXMLNodeList SCFXMLPropertyNode::getNodeListRecursive() const
	{
		SCFXMLNodeList resultList;
		getNodeListRecursive( resultList );
		return resultList;
	}

	SCFXMLNodeList & SCFXMLPropertyNode::getNodeListRecursive( SCFXMLNodeList & pOutputList ) const
	{
		for( auto & propertyNode : _subNodes )
		{
			pOutputList.push_back( &propertyNode );
			propertyNode.getNodeListRecursive( pOutputList );
		}

		return pOutputList;
	}


	SCFXMLResourceNode::SCFXMLResourceNode( SCFXMLNode * pParent, RxNode * pRxNode )
	: SCFXMLPropertyNode( SCFXMLNodeType::Resource, pParent, pRxNode )
	{
		if( pRxNode )
		{
			if( auto * typeAttribute = pRxNode->first_attribute( "type" ) )
			{
				_resourceType.assign( typeAttribute->value(), typeAttribute->value_size() );
			}
		}
	}


	SCFXMLFolderNode::SCFXMLFolderNode( SCFXMLNode * pParent, RxNode * pRxNode )
	: SCFXMLNode( SCFXMLNodeType::Folder, pParent, pRxNode )
	{}

	SCFXMLFolderNode::SCFXMLFolderNode( SCFXMLNodeType pNodeType, SCFXMLNode * pParent, RxNode * pRxNode )
	: SCFXMLNode( pNodeType, pParent, pRxNode )
	{}

	const SCFXMLFolderNode * SCFXMLFolderNode::subFolderNode( const char * pID ) const
	{
		auto nodeIter = std::lower_bound(
			_folderNodes.begin(),
			_folderNodes.end(),
			pID,
			[]( const SCFXMLFolderNode & pNode, const char * pFolderID ) -> bool {
				return pNode.id() < pFolderID;
			});

		if( ( nodeIter != _folderNodes.end() ) && ( nodeIter->id() == pID ) )
		{
			return &( *nodeIter );
		}

		return nullptr;
	}

	const SCFXMLResourceNode * SCFXMLFolderNode::resourceNode( const char * pID ) const
	{
		auto nodeIter = std::lower_bound(
			_resourceNodes.begin(),
			_resourceNodes.end(),
			pID,
			[]( const SCFXMLResourceNode & pNode, const char * pResourceID ) -> bool {
				return pNode.id() < pResourceID;
			});

		if( ( nodeIter != _resourceNodes.end() ) && ( nodeIter->id() == pID ) )
		{
			return &( *nodeIter );
		}

		return nullptr;
	}

	SCFXMLNodeList SCFXMLFolderNode::getNodeListRecursive() const
	{
		SCFXMLNodeList resultList;
		getNodeListRecursive( resultList );
		return resultList;
	}

	SCFXMLNodeList & SCFXMLFolderNode::getNodeListRecursive( SCFXMLNodeList & pOutputList ) const
	{
		for( const auto & resourceNode : _resourceNodes )
		{
			pOutputList.push_back( &resourceNode );
			resourceNode.getNodeListRecursive( pOutputList );
		}

		for( const auto & subFolderNode : _folderNodes )
		{
			pOutputList.push_back( &subFolderNode );
			subFolderNode.getNodeListRecursive( pOutputList );
		}

		return pOutputList;
	}


	SCFXMLRootNode::SCFXMLRootNode( std::nullptr_t )
	: SCFXMLFolderNode( SCFXMLNodeType::Root, nullptr, nullptr )
	{}

	SCFXMLRootNode::SCFXMLRootNode( std::string pXMLContent, std::unique_ptr<RxDocument> pRxDocument, RxNode * pRxNode )
	: SCFXMLFolderNode( SCFXMLNodeType::Root, nullptr, pRxNode )
	, _xmlContent( std::move( pXMLContent ) )
	, _rxDocument( std::move( pRxDocument ) )
	{}

}
