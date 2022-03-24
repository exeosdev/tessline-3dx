
#pragma once

#ifndef __TS3_CORE_XML_TYPES_H__
#define __TS3_CORE_XML_TYPES_H__

#include "xmlCommon.h"

namespace ts3
{

	/// @brief
	class XMLAttribute
	{
	public:
		XMLAttribute( const XMLAttribute & ) = default;
		XMLAttribute & operator=( const XMLAttribute & ) = default;

		XMLAttribute( std::nullptr_t = nullptr );
		explicit XMLAttribute( RxmlAttribute * pRxmlAttribute );

		explicit operator bool() const;

		XMLNode parent() const;

		std::string name() const;

		std::string value() const;

		bool empty() const;

		bool valid() const;

	private:
		RxmlAttribute * _rxmlAttribute;
	};

	/// @brief
	class XMLNode
	{
	public:
		XMLNode( const XMLNode & ) = default;
		XMLNode & operator=( const XMLNode & ) = default;

		XMLNode( std::nullptr_t = nullptr );
		explicit XMLNode( RxmlNode * pRxmlNode );

		explicit operator bool() const;

		XMLAttribute attribute( const StringView<char> & pAttribName ) const;

		XMLNode firstSubNode() const;

		XMLNode firstSubNode( const StringView<char> & pNodeName ) const;

		XMLNode nextSibling() const;

		XMLNode parent() const;

		std::string name() const;

		std::string value() const;

		bool hasAttribute( const StringView<char> & pAttribName ) const;

		bool hasSubNode( const StringView<char> & pNodeName ) const;

		bool hasSubNodes() const;

		bool empty() const;

		bool valid() const;

	private:
		RxmlNode * _rxmlNode;
	};

	/// @brief
	class XMLTree
	{
	public:
		XMLTree( XMLTree && ) = default;
		XMLTree & operator=( XMLTree && ) = default;

		XMLTree( std::nullptr_t = nullptr );
		explicit XMLTree( RxmlData pRxmlData );

		explicit operator bool() const;

		XMLNode rootNode() const;

		bool empty() const;

		bool valid() const;

	private:
		RxmlData _rxmlData;
	};

	/// @brief
	class XMLReader
	{
	public:
		static XMLTree readXMLContent( std::string pXMLContent );
	};
	
}

#endif // __TS3_CORE_XML_TYPES_H__
