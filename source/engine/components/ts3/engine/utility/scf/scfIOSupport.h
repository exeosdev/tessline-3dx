
#pragma once

#ifndef __TS3_ENGINE_SCF_IO_SUPPORT_H__
#define __TS3_ENGINE_SCF_IO_SUPPORT_H__

#include "scfCommon.h"
#include <ts3/stdext/byteArray.h>
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
		SCFIOProxy( system::FileManagerHandle pSysFileManager );

		void saveIndex( const std::string & pFilename, const SCFIndexBuilder & pBuilder );

		void loadIndex( const std::string & pFilename, SCFIndex & pIndex );

	private:
		using InternalFileReadCallback = std::function<uint64( void *, uint64 )>;
		using InternalFileWriteCallback = std::function<uint64( const void *, uint64 )>;

		void writeFolderData( system::FileHandle pSysFile,
		                      const SCFVirtualFolderTemplate & pFolder,
		                      DynamicByteArray & pGdsCache,
		                      const InternalFileWriteCallback & pFileWriteCallback );

		void writeResourceData( system::FileHandle pSysFile,
		                        const SCFResourceTemplate & pResource,
		                        DynamicByteArray & pGdsCache,
		                        const InternalFileWriteCallback & pFileWriteCallback );

		void readFolder( system::FileHandle pSysFile,
		                 SCFVirtualFolder & pFolder,
		                 DynamicByteArray & pGdsCache,
		                 const InternalFileReadCallback & pFileReadCallback );

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

} // namespace ts3

#endif // __TS3_ENGINE_SCF_IO_SUPPORT_H__