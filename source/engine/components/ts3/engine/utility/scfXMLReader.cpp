
#include "scfXMLReader.h"
#include <ts3/system/fileManager.h>

namespace ts3
{

	template <size_t tpNameLength>
	static bool checkNodeName( SCFXMLNode::RxNode * pRxNode, const char ( &pName )[tpNameLength] )
	{
		return strncmp( pRxNode->name(), pName, getMinOf( pRxNode->name_size(), tpNameLength ) ) == 0;
	}


	SCFXMLRootNode SCFXMLReader::readFile( const std::string & pFilename )
	{
		auto file = _sysFileManager->openFile( pFilename, system::EFileOpenMode::ReadOnly );
		if( !file )
		{
			return nullptr;
		}

		std::string xmlContent;
		file->readAuto( xmlContent );

		if( xmlContent.empty() )
		{
			return nullptr;
		}

		return _parseXMLContent( std::move( xmlContent ) );
	}

	SCFXMLRootNode SCFXMLReader::_parseXMLContent( std::string pXMLContent )
	{
		try
		{
			auto rxDocument = std::make_unique<RxDocument>();
			rxDocument->parse<0>( pXMLContent.data() );

			auto * rxSCFNode = rxDocument->first_node();
			if( !checkNodeName( rxSCFNode, "scf" ) )
			{
				return nullptr;
			}

			auto * rxRootNode = rxSCFNode->first_node();
			if( !checkNodeName( rxRootNode, "root" ) )
			{
				return nullptr;
			}

			SCFXMLRootNode rootNode{ std::move( pXMLContent ), std::move( rxDocument ), rxRootNode };

			_readFolderEntryNodes( rootNode );

			return rootNode;
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

	void SCFXMLReader::_readFolderEntryNodes( SCFXMLFolderNode & pFolderNode )
	{
		auto * folderRxNode = pFolderNode.getRxNode();
		auto & subFolderNodes = pFolderNode._folderNodes;
		auto & resourceNodes = pFolderNode._resourceNodes;

		for( auto * childNode = folderRxNode->first_node(); childNode; childNode = childNode->next_sibling() )
		{
			if( childNode->type() == rapidxml::node_type::node_element )
			{
				if( checkNodeName( childNode, "folder" ) )
				{
					auto * idAttribute = childNode->first_attribute( "id" );
					if( idAttribute )
					{
						subFolderNodes.push_back( SCFXMLFolderNode( &pFolderNode, childNode ) );
						continue;
					}
				}
				if( checkNodeName( childNode, "resource" ) )
				{
					auto * idAttribute = childNode->first_attribute( "id" );
					auto * typeAttribute = childNode->first_attribute( "type" );
					if( idAttribute && typeAttribute )
					{
						resourceNodes.push_back( SCFXMLResourceNode( &pFolderNode, childNode ) );
						continue;
					}
				}
			}

			// Invalid XML definition format
			ts3Throw( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
		}

		std::sort(
			subFolderNodes.begin(),
			subFolderNodes.end(),
			[]( SCFXMLFolderNode & pFirstNode, SCFXMLFolderNode & pSecondNode ) {
				return pFirstNode.id() < pSecondNode.id();
			});

		std::sort(
			resourceNodes.begin(),
			resourceNodes.end(),
			[]( SCFXMLResourceNode & pFirstNode, SCFXMLResourceNode & pSecondNode ) {
				return pFirstNode.id() < pSecondNode.id();
			});

		for( auto & subFolderNode : subFolderNodes )
		{
			_readFolderEntryNodes( subFolderNode );
		}

		for( auto & resourceNode : resourceNodes )
		{
			_readPropertySubNodes( resourceNode );
		}
	}

	void SCFXMLReader::_readPropertySubNodes( SCFXMLPropertyNode & pPropertyNode )
	{
		auto * propertyRxNode = pPropertyNode.getRxNode();
		auto & propertySubNodes = pPropertyNode._subNodes;

		for( auto * childNode = propertyRxNode->first_node(); childNode; childNode = childNode->next_sibling() )
		{
			if( childNode->type() == rapidxml::node_type::node_element )
			{
				propertySubNodes.push_back( SCFXMLPropertyNode( SCFXMLNodeType::Property, &pPropertyNode, childNode ) );
			}
		}

		std::sort(
			propertySubNodes.begin(),
			propertySubNodes.end(),
			[]( SCFXMLPropertyNode & pFirstNode, SCFXMLPropertyNode & pSecondNode ) {
				return pFirstNode.name() < pSecondNode.name();
			});

		for( auto & propertyNode : propertySubNodes )
		{
			_readPropertySubNodes( propertyNode );
		}
	}

}
