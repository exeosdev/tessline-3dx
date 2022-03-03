
#pragma once

#ifndef __TS3_ENGINE_SCF_IO_SUPPORT_H__
#define __TS3_ENGINE_SCF_IO_SUPPORT_H__

#include "scfIODefs.h"
#include <ts3/stdext/byteBuffer.h>
#include <ts3/system/fileCommon.h>

namespace ts3
{

	struct SCFIOResourceDesc;
	struct SCFIOVirtualFolderDesc;
	struct SCFResourceTemplate;
	struct SCFVirtualFolderTemplate;

	class SCFIndex;
	class SCFIndexBuilder;

	class SCFIOProxy
	{
	public:
	    SCFIOProxy( system::FileManagerHandle pSysFileManager );

		void saveIndex( const std::string & pFilename, const SCFIndexBuilder & pBuilder );

		void loadIndex( const std::string & pFilename, SCFIndex & pIndex );

	private:
		void writeFolderData( system::FileHandle pSysFile, const SCFVirtualFolderTemplate & pFolder, ByteBuffer & pStorageBuffer );

		void writeResourceData( system::FileHandle pSysFile, const SCFResourceTemplate & pResource, ByteBuffer & pStorageBuffer );

		void readFolder( system::FileHandle pSysFile, SCFVirtualFolder & pFolder, ByteBuffer & pStorageBuffer );

		template <typename TpValue>
		void readSerializedValueWithMetaInfo( system::FileHandle pSysFile, TpValue & pValue, ByteBuffer & pStorageBuffer );

	private:
		system::FileManagerHandle _sysFileManager;
	};

}

#endif // __TS3_ENGINE_SCF_IO_SUPPORT_H__
