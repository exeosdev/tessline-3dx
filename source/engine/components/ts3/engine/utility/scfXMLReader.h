
#pragma once

#ifndef __TS3_ENGINE_SCF_XML_READER_H__
#define __TS3_ENGINE_SCF_XML_READER_H__

#include "scfXMLCommon.h"
#include <ts3/system/fileCommon.h>

namespace ts3
{

	class SCFXMLReader
	{
	public:
		using RxDocument = SCFXMLRootNode::RxDocument;

		SCFXMLReader( system::FileManagerHandle pSysFileManager )
		: _sysFileManager( pSysFileManager )
		{}

		SCFXMLRootNode readFile( const std::string & pFilename );

	private:
		static SCFXMLRootNode _parseXMLContent( std::string pXMLContent );

		static void _readFolderEntryNodes( SCFXMLFolderNode & pFolderNode );

		static void _readPropertySubNodes( SCFXMLPropertyNode & pPropertyNode );

	private:
		system::FileManagerHandle _sysFileManager;
	};

}

#endif // __TS3_ENGINE_SCF_XML_READER_H__
