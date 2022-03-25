
#pragma once

#ifndef __TS3_CORE_XML_COMMON_H__
#define __TS3_CORE_XML_COMMON_H__

#include "../prerequisites.h"
#include <ts3/stdext/stringView.h>
#include <ts3/stdext/utilities.h>
#include <XML/parser/rapidxml.hpp>

namespace ts3
{

	class XMLAttribute;
	class XMLNode;
	class XMLTree;

	using RxmlDocument = rapidxml::xml_document<char>;
	using RxmlNode = rapidxml::xml_node<char>;
	using RxmlAttribute = rapidxml::xml_attribute<char>;

	struct RxmlData
	{
		RxmlData() = default;
		RxmlData( RxmlData && ) = default;
		RxmlData & operator=( RxmlData && ) = default;

		RxmlData( std::nullptr_t )
		: rootNode( nullptr )
		{}

		explicit operator bool() const
		{
			return document && rootNode && !textContent.empty();
		}

		std::unique_ptr<RxmlDocument> document;
		RxmlNode * rootNode;
		std::string textContent;
	};

} // namespace ts3

#endif // __TS3_CORE_XML_COMMON_H__
