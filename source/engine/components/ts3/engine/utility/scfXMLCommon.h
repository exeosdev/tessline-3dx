
#pragma once

#ifndef __TS3_ENGINE_SCF_XML_COMMON_H__
#define __TS3_ENGINE_SCF_XML_COMMON_H__

#include "scfCommon.h"
#include <ts3/core/utility/xmlTypes.h>

namespace ts3
{

	class SCFXMLResourceNode
	{
	public:
		using PropertyNodeList = std::vector<XMLNode>;

		SCFXMLResourceNode( std::nullptr_t = nullptr );

		const std::string & resourceID() const;

		const std::string & resourceType() const;

		const XMLNode * property( const StringView<char> & pPropertyName ) const;

		const PropertyNodeList & getPropertyNodes() const;

		bool hasProperty( const StringView<char> & pPropertyName ) const;

		static SCFXMLResourceNode initFromXMLNode( XMLNode pXMLNode );

	private:
		using PropertyNodeMap = std::map<std::string, XMLNode *, StringViewCmpLess<char>>;

		static PropertyNodeList _readProperties( const XMLNode & pXMLNode, size_t pPropertyNodesNum );

		static PropertyNodeMap _buildPropertyNodeMap( PropertyNodeList & pPropertyNodes );

	private:
		XMLNode _xmlNode;
		std::string _id;
		std::string _type;
		PropertyNodeList _propertyNodeList;
		PropertyNodeMap _propertyNodeMap;
	};

	class SCFXMLFolderNode
	{
	public:
		using ResourceNodeList = std::vector<SCFXMLResourceNode>;
		using SubFolderNodeList = std::vector<SCFXMLFolderNode>;

		SCFXMLFolderNode( std::nullptr_t = nullptr );

		const std::string & folderName() const;

		const SCFXMLResourceNode * resource( const StringView<char> & pResourceID ) const;

		const SCFXMLFolderNode * subFolder( const StringView<char> & pSubFolderName ) const;

		const ResourceNodeList & getResourceNodes() const;

		const SubFolderNodeList & getSubFolderNodes() const;

		bool hasResource( const StringView<char> & pResourceID ) const;

		bool hasSubFolder( const StringView<char> & pSubFolderName ) const;

		static SCFXMLFolderNode initFromXMLNode( XMLNode pXMLNode );

	private:
		using ResourceNodeMap = std::map<std::string, SCFXMLResourceNode *, StringViewCmpLess<char>>;
		using SubFolderNodeMap = std::map<std::string, SCFXMLFolderNode *, StringViewCmpLess<char>>;

		static ResourceNodeList _readResources( const XMLNode & pXMLNode, size_t pResourceNodesNum );

		static ResourceNodeMap _buildResourceNodeMap( ResourceNodeList & pResourceNodes );

		static SubFolderNodeList _readSubFolders( const XMLNode & pXMLNode, size_t pSubFolderNodesNum );

		static SubFolderNodeMap _buildSubFolderNodeMap( SubFolderNodeList & pSubFolderNodes );

	private:
		XMLNode _xmlNode;
		std::string _name;
		ResourceNodeList _resourceNodeList;
		ResourceNodeMap _resourceNodeMap;
		SubFolderNodeList _subFolderNodeList;
		SubFolderNodeMap _subFolderNodeMap;
	};

}

#endif // __TS3_ENGINE_SCF_XML_COMMON_H__
