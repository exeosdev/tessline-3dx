
#pragma once

#ifndef __TS3_ENGINE_SCF_XML_PUBLIC_H__
#define __TS3_ENGINE_SCF_XML_PUBLIC_H__

#include "scfXMLCommon.h"
#include <ts3/system/fileCommon.h>

namespace ts3
{

	class SCFXMLReader
	{
	public:
		explicit SCFXMLReader( system::FileManagerHandle pSysFileManager )
		: _sysFileManager( pSysFileManager )
		{}

		SCFXMLRootNode readFile( const std::string & pFilename );

	private:
		system::FileManagerHandle _sysFileManager;
	};

} // namespace ts3

#endif // __TS3_ENGINE_SCF_XML_PUBLIC_H__
