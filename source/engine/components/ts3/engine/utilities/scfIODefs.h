
#pragma once

#ifndef __TS3_ENGINE_SCF_IO_DEFS_H__
#define __TS3_ENGINE_SCF_IO_DEFS_H__

#include "scfCommon.h"


namespace ts3
{

	struct SCFIOEntryDesc
	{
		ESCFEntryType entryType;
		std::string name;
		std::string uid;
	};

	struct SCFIOResourceDesc : public SCFIOEntryDesc
	{
		uint64 dataOffset;
		uint64 dataSize;
	};

	struct SCFIOVirtualFolderDesc : public SCFIOEntryDesc
	{
		uint32 resourcesNum;
		uint32 subFoldersNum;
	};

	namespace gds
	{

		gds_size_t serialize( byte * pOutputBuffer, const SCFIOEntryDesc & pValue );
		gds_size_t deserialize( const byte * pInputDesc, SCFIOEntryDesc & pValue );
		gds_size_t evalByteSize( const SCFIOEntryDesc & pValue );

		gds_size_t serialize( byte * pOutputBuffer, const SCFIOResourceDesc & pValue );
		gds_size_t deserialize( const byte * pInputDesc, SCFIOResourceDesc & pValue );
		gds_size_t evalByteSize( const SCFIOResourceDesc & pValue );

		gds_size_t serialize( byte * pOutputBuffer, const SCFIOVirtualFolderDesc & pValue );
		gds_size_t deserialize( const byte * pInputDesc, SCFIOVirtualFolderDesc & pValue );
		gds_size_t evalByteSize( const SCFIOVirtualFolderDesc & pValue );

	}

}

#endif // __TS3_ENGINE_SCF_IO_DEFS_H__
