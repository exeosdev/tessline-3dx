
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

		SCFVirtualFolderInfo scfRootFolderInfo {};
		gds::deserializeExternal( scfRootFolderInfo, fileReadCallback, storageBuffer );

		auto & rootFolder = pIndex.initRootFolder(std::move( scfRootFolderInfo ) );

		readFolder( file, rootFolder, storageBuffer, fileReadCallback );
	}

    void SCFIOProxy::writeFolderData( system::FileHandle pSysFile,
                                      const SCFVirtualFolderTemplate & pFolder,
                                      ByteBuffer & pStorageBuffer,
                                      const FileWriteCallback & pFileWriteCallback )
	{
        SCFVirtualFolderInfo scfFolderInfo;
		scfFolderInfo.entryType = ESCFEntryType::VirtualFolder;
		scfFolderInfo.name = pFolder.name;
		scfFolderInfo.uid = pFolder.uid;
		scfFolderInfo.treeSubLevel = pFolder.treeSubLevel;
		scfFolderInfo.resourcesNum = static_cast<uint32>( pFolder.resourceList.size() );
		scfFolderInfo.subFoldersNum = static_cast<uint32>( pFolder.subFolderList.size() );

		gds::serializeExternal( pStorageBuffer, scfFolderInfo, pFileWriteCallback );

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
        SCFResourceInfo scfResourceInfo;
		scfResourceInfo.entryType = ESCFEntryType::VirtualFolder;
		scfResourceInfo.name = pResource.name;
		scfResourceInfo.uid = pResource.uid;
		scfResourceInfo.treeSubLevel = pResource.treeSubLevel;

		const auto currentFilePtrOffset = pSysFile->getFilePointer();
		const auto resourceEntrySize = gds::evalByteSizeWithMetaData( scfResourceInfo );

		scfResourceInfo.dataOffset = currentFilePtrOffset + resourceEntrySize;
		scfResourceInfo.dataSize = pResource.dataSource.byteSize;

		gds::serializeExternal( pStorageBuffer, scfResourceInfo, pFileWriteCallback );

		const auto sMaxSingleDataWriteSize = 2048;
		
		for( uint64 currentReadPtr = 0; currentReadPtr < scfResourceInfo.dataSize; )
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
		    SCFResourceInfo scfResourceInfo;
		    gds::deserializeExternal( scfResourceInfo, pFileReadCallback, pStorageBuffer );

		    // The resource path is not serialized, don't forget this line.
		    scfResourceInfo.path = pFolder.mFolderInfo.path + "/" + scfResourceInfo.name;

			auto & scfResource = pFolder.addResource( std::move( scfResourceInfo ) );

			const auto skipResourceDataOffset = scfResource.mResourceInfo.dataOffset + scfResource.mResourceInfo.dataSize;
			pSysFile->setFilePointer( static_cast<system::file_offset_t>( skipResourceDataOffset ) );
		}

		for( uint32 subFolderIndex = 0; subFolderIndex < pFolder.mFolderInfo.subFoldersNum; ++subFolderIndex )
		{
		    SCFVirtualFolderInfo scfFolderInfo {};
			gds::deserializeExternal( scfFolderInfo, pFileReadCallback, pStorageBuffer );

			// The folder path is not serialized, don't forget this line.
			scfFolderInfo.path = pFolder.mFolderInfo.path + "/" + scfFolderInfo.name;

			auto & scfFolder = pFolder.addSubFolder( std::move( scfFolderInfo ) );

			readFolder( pSysFile, scfFolder, pStorageBuffer, pFileReadCallback );
		}
	}

	namespace gds
	{

        gds_size_t serialize( byte * pOutputBuffer, const SCFEntryInfo & pValue )
        {
            return serializeAll( pOutputBuffer,
                                 pValue.entryType,
                                 pValue.name,
                                 pValue.uid,
                                 pValue.treeSubLevel );
        }

        gds_size_t deserialize( const byte * pInputData, SCFEntryInfo & pValue )
        {
            return deserializeAll( pInputData,
                                   pValue.entryType,
                                   pValue.name,
                                   pValue.uid,
                                   pValue.treeSubLevel );
        }

        gds_size_t evalByteSize( const SCFEntryInfo & pValue )
        {
            return evalByteSizeAll( pValue.entryType,
                                    pValue.name,
                                    pValue.uid,
                                    pValue.treeSubLevel );
        }

        gds_size_t serialize( byte * pOutputBuffer, const SCFResourceInfo & pValue )
        {
            return serializeAll( pOutputBuffer,
                                 static_cast<const SCFEntryInfo &>( pValue ),
                                 pValue.dataOffset,
                                 pValue.dataSize );
        }

        gds_size_t deserialize( const byte * pInputData, SCFResourceInfo & pValue )
        {
            return deserializeAll( pInputData,
                                   static_cast<SCFEntryInfo &>( pValue ),
                                   pValue.dataOffset,
                                   pValue.dataSize );
        }

        gds_size_t evalByteSize( const SCFResourceInfo & pValue )
        {
            return evalByteSizeAll( static_cast<const SCFEntryInfo &>( pValue ),
                                    pValue.dataOffset,
                                    pValue.dataSize );
        }

        gds_size_t serialize( byte * pOutputBuffer, const SCFVirtualFolderInfo & pValue )
        {
            return serializeAll( pOutputBuffer,
                                 static_cast<const SCFEntryInfo &>( pValue ),
                                 pValue.resourcesNum,
                                 pValue.subFoldersNum );
        }

        gds_size_t deserialize( const byte * pInputData, SCFVirtualFolderInfo & pValue )
        {
            return deserializeAll( pInputData,
                                   static_cast<SCFEntryInfo &>( pValue ),
                                   pValue.resourcesNum,
                                   pValue.subFoldersNum );
        }

        gds_size_t evalByteSize( const SCFVirtualFolderInfo & pValue )
        {
            return evalByteSizeAll( static_cast<const SCFEntryInfo &>( pValue ),
                                    pValue.resourcesNum,
                                    pValue.subFoldersNum );
        }

	}

}
