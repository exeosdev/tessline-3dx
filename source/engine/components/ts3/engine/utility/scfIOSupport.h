
#pragma once

#ifndef __TS3_ENGINE_SCF_IO_SUPPORT_H__
#define __TS3_ENGINE_SCF_IO_SUPPORT_H__

#include "scfCommon.h"
#include <ts3/stdext/byteBuffer.h>
#include <ts3/system/fileCommon.h>

namespace ts3
{

	struct SCFResourceTemplate;
	struct SCFVirtualFolderTemplate;

	class SCFIndex;
	class SCFIndexBuilder;

	class SCFIOProxy
	{
	public:
	    using FileReadCallback = std::function<uint64( void *, uint64 )>;
	    using FileWriteCallback = std::function<uint64( const void *, uint64 )>;

	    SCFIOProxy( system::FileManagerHandle pSysFileManager );

		void saveIndex( const std::string & pFilename, const SCFIndexBuilder & pBuilder );

		void loadIndex( const std::string & pFilename, SCFIndex & pIndex );

	private:
		void writeFolderData( system::FileHandle pSysFile,
                              const SCFVirtualFolderTemplate & pFolder,
                              ByteBuffer & pStorageBuffer,
                              const FileWriteCallback & pFileWriteCallback );

		void writeResourceData( system::FileHandle pSysFile,
                                const SCFResourceTemplate & pResource,
                                ByteBuffer & pStorageBuffer,
                                const FileWriteCallback & pFileWriteCallback );

		void readFolder( system::FileHandle pSysFile,
                         SCFVirtualFolder & pFolder,
                         ByteBuffer & pStorageBuffer,
                         const FileReadCallback & pFileReadCallback );

	private:
		system::FileManagerHandle _sysFileManager;
	};

	namespace gds
	{

	    gds_size_t serialize( byte * pOutputBuffer, const SCFEntryInfo & pValue );
	    gds_size_t deserialize( const byte * pInputDesc, SCFEntryInfo & pValue );
	    gds_size_t evalByteSize( const SCFEntryInfo & pValue );

	    gds_size_t serialize( byte * pOutputBuffer, const SCFResourceInfo & pValue );
	    gds_size_t deserialize( const byte * pInputDesc, SCFResourceInfo & pValue );
	    gds_size_t evalByteSize( const SCFResourceInfo & pValue );

	    gds_size_t serialize( byte * pOutputBuffer, const SCFVirtualFolderInfo & pValue );
	    gds_size_t deserialize( const byte * pInputDesc, SCFVirtualFolderInfo & pValue );
	    gds_size_t evalByteSize( const SCFVirtualFolderInfo & pValue );

	}

}

#endif // __TS3_ENGINE_SCF_IO_SUPPORT_H__
