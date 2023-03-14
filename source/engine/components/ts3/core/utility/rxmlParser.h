
#pragma once

#ifndef __TS3_CORE_RXML_PARSER_H__
#define __TS3_CORE_RXML_PARSER_H__

#include "xmlCommon.h"

namespace ts3
{

	class RxmlParser
	{
	public:
		static RxmlData parseXMLContent( std::string pXMLContent );

		static bool checkNodeNameEquals( RxmlNode * pRxmlNode, const char * pName, size_t pNameLength )
		{
			return strncmp( pRxmlNode->name(), pName, getMinOf( pRxmlNode->name_size(), pNameLength ) ) == 0;
		}

		template <size_t tNameLength>
		static bool checkNodeNameEquals( RxmlNode * pRxmlNode, const char ( &pName )[tNameLength] )
		{
			return checkNodeNameEquals( pRxmlNode, &( pName[0] ), tNameLength );
		}

		static bool checkNodeHasAttribute( RxmlNode * pRxmlNode, const char * pName, size_t pNameLength, bool pAllowEmpty = true )
		{
			const auto * attribNode = pRxmlNode->first_node( pName, pNameLength, true );
			return attribNode && ( pAllowEmpty || ( attribNode->value_size() > 0 ) );
		}

		template <size_t tNameLength>
		static bool checkNodeHasAttribute( RxmlNode * pRxmlNode, const char ( &pName )[tNameLength], bool pAllowEmpty = true )
		{
			return checkNodeHasAttribute( pRxmlNode, &( pName[0] ), tNameLength );
		}

		static bool checkNodeHasValue( RxmlNode * pRxmlNode )
		{
			return pRxmlNode && pRxmlNode->value() && ( pRxmlNode->value_size() > 0 );
		}

		static size_t countAttributes( RxmlNode * pRxmlNode )
		{
			size_t attribsNum = 0;
			for( auto * attrib = pRxmlNode->first_attribute(); attrib != pRxmlNode->last_attribute(); attrib = attrib->next_attribute() )
			{
				++attribsNum;
			}
			return attribsNum;
		}

		static size_t countSubNodes( RxmlNode * pRxmlNode )
		{
			return countSubNodes( pRxmlNode, nullptr, 0 );
		}

		static size_t countSubNodes( RxmlNode * pRxmlNode, const char * pName, size_t pNameLength )
		{
			size_t subNodesNum = 0;
			for( auto * subNode = pRxmlNode->first_node( pName, pNameLength ); subNode != nullptr; )
			{
				++subNodesNum;
				subNode = subNode->next_sibling( pName, pNameLength );
			}
			return subNodesNum;
		}

		static std::string getNodeName( RxmlNode * pRxmlNode )
		{
			if( pRxmlNode && pRxmlNode->name() && ( pRxmlNode->name_size() > 0 ) )
			{
				return std::string{ pRxmlNode->name(), pRxmlNode->name_size() };
			}
			return {};
		}

		static std::string getNodeValue( RxmlNode * pRxmlNode )
		{
			if( pRxmlNode && pRxmlNode->value() && ( pRxmlNode->value_size() > 0 ) )
			{
				return std::string{ pRxmlNode->value(), pRxmlNode->value_size() };
			}
			return {};
		}

		static std::string getAttributeName( RxmlAttribute * pRxmlAttribute )
		{
			if( pRxmlAttribute && pRxmlAttribute->name() && ( pRxmlAttribute->name_size() > 0 ) )
			{
				return std::string{ pRxmlAttribute->name(), pRxmlAttribute->name_size() };
			}
			return {};
		}

		static std::string getAttributeValue( RxmlAttribute * pRxmlAttribute )
		{
			if( pRxmlAttribute && pRxmlAttribute->value() && ( pRxmlAttribute->value_size() > 0 ) )
			{
				return std::string{ pRxmlAttribute->value(), pRxmlAttribute->value_size() };
			}
			return {};
		}
	};

}

#endif // __TS3_CORE_RXML_PARSER_H__
