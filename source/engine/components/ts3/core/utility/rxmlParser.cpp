
#include "rxmlParser.h"

namespace ts3
{

	RxmlData RxmlParser::parseXMLContent( std::string pXMLContent )
	{
		RxmlData rxmlData;

		try
		{
			auto rxmlDocument = std::make_unique<RxmlDocument>();
			rxmlDocument->parse<0>( pXMLContent.data() );

			if( !rxmlDocument->first_node() )
			{
				return nullptr;
			}

			rxmlData.document = std::move( rxmlDocument );
			rxmlData.rootNode = rxmlDocument->first_node();
			rxmlData.textContent = std::move( pXMLContent );

			return rxmlData;
		}
		catch( const rapidxml::parse_error & pException )
		{
			ts3DebugInterrupt();
		}
		catch( ... )
		{
			ts3DebugInterrupt();
		}

		return nullptr;
	}

	std::unique_ptr<const XMLNode> RxmlLoader::loadFromRxmlData( const RxmlData & pRxmlData )
	{
		try
		{
			auto rootXMLNode = std::make_unique<XMLNode>( nullptr );
			_readRxmlNode( pRxmlData.rootNode, *rootXMLNode );
			return rootXMLNode;
		}
		catch( ... )
		{
		}

		return nullptr;
	}

	void RxmlLoader::_readRxmlNode( RxmlNode * pRxmlNode, XMLNode & pXMLNode )
	{
		std::string nodeName{ pRxmlNode->name(), pRxmlNode->name_size() };
		std::string nodeValue{ pRxmlNode->value(), pRxmlNode->value_size() };

		auto attributeMap = _readRxmlNodeAttributes( pRxmlNode, pXMLNode );
		auto subNodeList = _readRxmlNodeSubNodes( pRxmlNode, pXMLNode );

		pXMLNode.setName( std::move( nodeName ) );
		pXMLNode.setValue( std::move( nodeValue ) );
		pXMLNode.replaceAttributeMap( std::move( attributeMap ) );
		pXMLNode.replaceSubNodeList( std::move( subNodeList ) );
	}

	XMLAttributeMap RxmlLoader::_readRxmlNodeAttributes( RxmlNode * pRxmlNode, XMLNode & pXMLNode )
	{
		XMLAttributeMap attributeMap{};
		const auto cvAttributesNum = RxmlParser::countAttributes( pRxmlNode );
		if( cvAttributesNum > 0 )
		{
			auto * rxmlAttribute = pRxmlNode->first_attribute();

			for( uint32 attributeIndex = 0; attributeIndex < cvAttributesNum; ++attributeIndex )
			{
				std::string attributeName{ rxmlAttribute->name(), rxmlAttribute->name_size() };
				std::string attributeValue{ rxmlAttribute->value(), rxmlAttribute->value_size() };
				attributeMap.insert({ std::move( attributeName ), std::move( attributeValue ) });
				rxmlAttribute = rxmlAttribute->next_attribute();
			}
		}
		return attributeMap;
	}

	XMLSubNodeList RxmlLoader::_readRxmlNodeSubNodes( RxmlNode * pRxmlNode, XMLNode & pXMLNode )
	{
		XMLSubNodeList subNodeList{};
		const auto cvSubNodesNum = RxmlParser::countSubNodes( pRxmlNode );
		if( cvSubNodesNum > 0 )
		{
			auto * rxmlSubNode = pRxmlNode->first_node();
			ts3DebugAssert( rxmlSubNode );

			for( uint32 subNodeIndex = 0; subNodeIndex < cvSubNodesNum; ++subNodeIndex )
			{
				auto subNodeIter = subNodeList.emplace( subNodeList.end() );
				auto & subNode = *subNodeIter;

				_readRxmlNode( rxmlSubNode, subNode );

				subNode.setParent( &pXMLNode );

				if( subNodeIndex > 0 )
				{
					auto & previousNode = *( --subNodeIter );
					previousNode.setNextSibling( &subNode );
					subNode.setPrevSibling( &previousNode );
				}
				rxmlSubNode = rxmlSubNode->next_sibling();
			}
		}
		return subNodeList;
	}

}
