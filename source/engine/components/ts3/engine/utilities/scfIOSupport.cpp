
#include "scfIOSupport.h"
#include "scfIndexBuilder.h"
#include <ts3/system/fileManager.h>
#include <ts3/core/utility/gdsCoreAPI.h>
#include <ts3/core/utility/gdsTypeSupportStdCore.h>
#include <vector>

namespace ts3
{

    SCFIOProxy::SCFIOProxy( system::FileManagerHandle pSysFileManager )
    : _sysFileManager( std::move( pSysFileManager ) )
    {}

	void SCFIOProxy::saveIndex( const std::string & pFilename, const SCFIndexBuilder & pBuilder )
	{
		auto file = _sysFileManager->openFile( pFilename, system::EFileOpenMode::WriteOverwrite );
		if( !file )
		{
			return;
		}

		FileWriteCallback fileWriteCallback =
		    [file]( const void * pInputData, uint64 pWriteSize ) -> uint64 {
                const auto writeSize = trunc_numeric_cast<system::file_size_t>( pWriteSize );
                return file->writeData( pInputData, writeSize, writeSize );
            };

		ByteBuffer storageBuffer;

		const auto & rootFolder = pBuilder.getRootVirtualFolder();

		writeFolderData( file, rootFolder, storageBuffer, fileWriteCallback );
	}

	void SCFIOProxy::loadIndex( const std::string & pFilename, SCFIndex & pIndex )
	{
		auto file = _sysFileManager->openFile( pFilename, system::EFileOpenMode::ReadOnly );
		if( !file )
		{
			return;
		}

		FileReadCallback fileReadCallback =
		    [file]( void * pOutputBuffer, uint64 pReadSize ) -> uint64 {
                const auto readSize = trunc_numeric_cast<system::file_size_t>( pReadSize );
                return file->readData( pOutputBuffer, readSize, readSize );
		    };

		ByteBuffer storageBuffer;

		SCFIOVirtualFolderDesc scfRootFolderDesc {};
		gds::deserializeExternal( scfRootFolderDesc, fileReadCallback, storageBuffer );

		SCFVirtualFolderInfo folderInfo;
		folderInfo.entryType = ESCFEntryType::VirtualFolder;
		folderInfo.name = std::move( scfRootFolderDesc.name );
		folderInfo.uid = std::move( scfRootFolderDesc.uid );
		folderInfo.resourcesNum = scfRootFolderDesc.resourcesNum;
		folderInfo.subFoldersNum = scfRootFolderDesc.subFoldersNum;

		auto & rootFolder = pIndex.initRootFolder(std::move( folderInfo ) );

		readFolder( file, rootFolder, storageBuffer, fileReadCallback );
	}

	static int cc = 1;
    static ByteBuffer bb {};

    void SCFIOProxy::writeFolderData( system::FileHandle pSysFile,
                                      const SCFVirtualFolderTemplate & pFolder,
                                      ByteBuffer & pStorageBuffer,
                                      const FileWriteCallback & pFileWriteCallback )
	{
		SCFIOVirtualFolderDesc scfVirtualFolderDesc;
		scfVirtualFolderDesc.entryType = ESCFEntryType::VirtualFolder;
		scfVirtualFolderDesc.name = pFolder.name;
		scfVirtualFolderDesc.uid = pFolder.uid;
		scfVirtualFolderDesc.resourcesNum = static_cast<uint32>( pFolder.resourceList.size() );
		scfVirtualFolderDesc.subFoldersNum = static_cast<uint32>( pFolder.subFolderList.size() );

		gds::serializeExternal( pStorageBuffer, scfVirtualFolderDesc, pFileWriteCallback );

		for( const auto & resource : pFolder.resourceList )
		{
		    writeResourceData( pSysFile, resource, pStorageBuffer, pFileWriteCallback );
		}

		for( const auto & subFolder : pFolder.subFolderList )
		{
		    writeFolderData( pSysFile, subFolder, pStorageBuffer, pFileWriteCallback );
		}
	}

	void SCFIOProxy::writeResourceData( system::FileHandle pSysFile,
                                        const SCFResourceTemplate & pResource,
                                        ByteBuffer & pStorageBuffer,
                                        const FileWriteCallback & pFileWriteCallback )
	{
		SCFIOResourceDesc scfResourceDesc;
		scfResourceDesc.entryType = ESCFEntryType::VirtualFolder;
		scfResourceDesc.name = pResource.name;
		scfResourceDesc.uid = pResource.uid;

		const auto currentFilePtrOffset = pSysFile->getFilePointer();
		const auto resourceEntrySize = gds::evalByteSizeWithMetaData( scfResourceDesc );

		scfResourceDesc.dataOffset = currentFilePtrOffset + resourceEntrySize;
		scfResourceDesc.dataSize = pResource.dataSource.byteSize;

		gds::serializeExternal( pStorageBuffer, scfResourceDesc, pFileWriteCallback );

		const auto sMaxSingleDataWriteSize = 2048;
		
		for( uint64 currentReadPtr = 0; currentReadPtr < scfResourceDesc.dataSize; )
		{
		    const auto readSize = pResource.dataSource.readCallback( currentReadPtr, sMaxSingleDataWriteSize, pStorageBuffer );
		    ts3DebugAssert( readSize > 0 );

		    pSysFile->writeData( pStorageBuffer.dataPtr(), pStorageBuffer.size() );

		    currentReadPtr += readSize;
		}
	}

	void SCFIOProxy::readFolder( system::FileHandle pSysFile,
                                 SCFVirtualFolder & pFolder,
                                 ByteBuffer & pStorageBuffer,
                                 const FileReadCallback & pFileReadCallback )
	{
		for( uint32 resourceIndex = 0; resourceIndex < pFolder.mFolderInfo.resourcesNum; ++resourceIndex )
		{
			SCFIOResourceDesc scfResourceDesc;
			gds::deserializeExternal( scfResourceDesc, pFileReadCallback, pStorageBuffer );

			SCFResourceInfo scfResourceInfo;
			scfResourceInfo.entryType = ESCFEntryType::Resource;
			scfResourceInfo.name = std::move( scfResourceDesc.name );
			scfResourceInfo.uid = std::move( scfResourceDesc.uid );
			scfResourceInfo.path = pFolder.mFolderInfo.path + "/" + scfResourceInfo.name;
			scfResourceInfo.dataOffset = scfResourceDesc.dataOffset;
			scfResourceInfo.dataSize = scfResourceDesc.dataSize;

			auto & scfResource = pFolder.addResource( std::move( scfResourceInfo ) );

			pSysFile->setFilePointer( static_cast<system::file_offset_t>( scfResource.mResourceInfo.dataOffset + scfResource.mResourceInfo.dataSize ) );
		}

		for( uint32 subFolderIndex = 0; subFolderIndex < pFolder.mFolderInfo.subFoldersNum; ++subFolderIndex )
		{
			SCFIOVirtualFolderDesc scfFolderDesc {};
			gds::deserializeExternal( scfFolderDesc, pFileReadCallback, pStorageBuffer );

			SCFVirtualFolderInfo scfFolderInfo;
			scfFolderInfo.entryType = ESCFEntryType::VirtualFolder;
			scfFolderInfo.name = std::move( scfFolderDesc.name );
			scfFolderInfo.uid = std::move( scfFolderDesc.uid );
			scfFolderInfo.path = pFolder.mFolderInfo.path + "/" + scfFolderInfo.name;
			scfFolderInfo.resourcesNum = scfFolderDesc.resourcesNum;
			scfFolderInfo.subFoldersNum = scfFolderDesc.subFoldersNum;

			auto & scfFolder = pFolder.addSubFolder( std::move( scfFolderInfo ) );

			readFolder( pSysFile, scfFolder, pStorageBuffer, pFileReadCallback );
		}
	}

}
