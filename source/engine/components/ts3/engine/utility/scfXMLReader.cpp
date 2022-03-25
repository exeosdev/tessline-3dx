
#include "scfXMLReader.h"
#include <ts3/system/fileManager.h>

namespace ts3
{

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

		try
		{
			auto xmlTree = XMLReader::readXMLContent( std::move ( xmlContent ) );
			if( !xmlTree.valid() || xmlTree.empty() )
			{
				return nullptr;
			}

			return SCFXMLRootNode::initFromXMLTree( std::move( xmlTree ) );
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

}
