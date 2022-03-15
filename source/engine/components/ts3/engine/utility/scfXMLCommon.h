
#pragma once

#ifndef __TS3_ENGINE_SCF_XML_COMMON_H__
#define __TS3_ENGINE_SCF_XML_COMMON_H__

#include "scfCommon.h"
#include <XML/parser/rapidxml.hpp>
#include <vector>

namespace ts3
{

	class SCFXMLNode;
	class SCFXMLResourceNode;
	class SCFXMLReader;

	using SCFXMLNodeList = std::vector<const SCFXMLNode *>;

	enum class SCFXMLNodeType : enum_default_value_t
	{
		Folder,
		Property,
		Resource,
		Root
	};

	class SCFXMLNode
	{
	public:
		using RxNode = rapidxml::xml_node<>;

		SCFXMLNode( SCFXMLNodeType pNodeType, SCFXMLNode * pParent, RxNode * pRxNode );

		explicit operator bool() const;

		SCFXMLNodeType nodeType() const;

		std::string attribute( const char * pName ) const;

		const std::string & name() const;
		const std::string & value() const;
		const std::string & id() const;

		SCFXMLNode * parent() const;

		bool hasParent() const;
		bool isRootNode() const;

	protected:
		RxNode * getRxNode() const;

	private:
		SCFXMLNodeType _nodeType;
		SCFXMLNode * _parentNode;
		RxNode * _rxNode;
		std::string _id;
		std::string _name;
		std::string _value;
	};

	class SCFXMLPropertyNode : public SCFXMLNode
	{
	friend class SCFXMLReader;

	public:
		using PropertyNodeList = std::vector<SCFXMLPropertyNode>;

		SCFXMLPropertyNode( SCFXMLNodeType pNodeType, SCFXMLNode * pParent, RxNode * pRxNode );

		const SCFXMLPropertyNode * subNode( const char * pName ) const;

		SCFXMLNodeList getNodeListRecursive() const;
		SCFXMLNodeList & getNodeListRecursive( SCFXMLNodeList & pOutputList ) const;

	protected:
		PropertyNodeList _subNodes;
	};

	class SCFXMLResourceNode : public SCFXMLPropertyNode
	{
	public:
		SCFXMLResourceNode( SCFXMLNode * pParent, RxNode * pRxNode );

		const std::string & resourceType() const
		{
			return _resourceType;
		}

	private:
		std::string _resourceType;
	};

	class SCFXMLFolderNode : public SCFXMLNode
	{
	friend class SCFXMLReader;

	public:
		using FolderNodeList = std::vector<SCFXMLFolderNode>;
		using ResourceNodeList = std::vector<SCFXMLResourceNode>;

		SCFXMLFolderNode( SCFXMLNode * pParent, RxNode * pRxNode );
		SCFXMLFolderNode( SCFXMLNodeType pNodeType, SCFXMLNode * pParent, RxNode * pRxNode );

		const SCFXMLFolderNode * subFolderNode( const char * pID ) const;

		const SCFXMLResourceNode * resourceNode( const char * pID ) const;

		SCFXMLNodeList getNodeListRecursive() const;
		SCFXMLNodeList & getNodeListRecursive( SCFXMLNodeList & pOutputList ) const;

		const FolderNodeList & getFolderNodes() const
		{
			return _folderNodes;
		}

		const ResourceNodeList & getResourceNodes() const
		{
			return _resourceNodes;
		}

	protected:
		void readFolderEntryNodes();

	private:
		void addFolderNode( RxNode * pRxNode );

		void addResourceNode( RxNode * pRxNode );

		void sortNodes();

	private:
		FolderNodeList _folderNodes;
		ResourceNodeList _resourceNodes;
	};

	class SCFXMLRootNode : public SCFXMLFolderNode
	{
	public:
		using RxDocument = rapidxml::xml_document<>;

		SCFXMLRootNode( std::nullptr_t );
		SCFXMLRootNode( std::string pXMLContent, std::unique_ptr<RxDocument> pRxDocument, RxNode * pRxNode );

	private:
		std::string _xmlContent;
		std::unique_ptr<RxDocument> _rxDocument;
	};

}

#endif // __TS3_ENGINE_SCF_XML_COMMON_H__
