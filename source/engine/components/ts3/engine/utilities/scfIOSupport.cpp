
#include "scfIOSupport.h"
#include "scfIndexBuilder.h"
#include <ts3/system/fileManager.h>
#include <ts3/stdext/gdsUtilities.h>
#include <ts3/stdext/gdsStdCore.h>
#include <ts3/stdext/gdsStdSTL.h>
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

		ByteBuffer storageBuffer;

		const auto & rootFolder = pBuilder.getRootVirtualFolder();

		writeFolderData( file, rootFolder, storageBuffer );
	}

	void SCFIOProxy::loadIndex( const std::string & pFilename, SCFIndex & pIndex )
	{
		auto file = _sysFileManager->openFile( pFilename, system::EFileOpenMode::ReadOnly );
		if( !file )
		{
			return;
		}

		ByteBuffer storageBuffer;

		SCFIOVirtualFolderDesc scfRootFolderDesc {};
		readSerializedValueWithMetaInfo( file, scfRootFolderDesc, storageBuffer );

		SCFVirtualFolderInfo folderInfo;
		folderInfo.entryType = ESCFEntryType::VirtualFolder;
		folderInfo.name = std::move( scfRootFolderDesc.name );
		folderInfo.uid = std::move( scfRootFolderDesc.uid );
		folderInfo.resourcesNum = scfRootFolderDesc.resourcesNum;
		folderInfo.subFoldersNum = scfRootFolderDesc.subFoldersNum;

		auto & rootFolder = pIndex.initRootFolder(std::move( folderInfo ) );

		readFolder( file, rootFolder, storageBuffer );
	}

	static int cc = 1;
    static ByteBuffer bb {};

	void SCFIOProxy::writeFolderData( system::FileHandle pSysFile, const SCFVirtualFolderTemplate & pFolder, ByteBuffer & pStorageBuffer )
	{
		SCFIOVirtualFolderDesc scfVirtualFolderDesc;
		scfVirtualFolderDesc.entryType = ESCFEntryType::VirtualFolder;
		scfVirtualFolderDesc.name = pFolder.name;
		scfVirtualFolderDesc.uid = pFolder.uid;
		scfVirtualFolderDesc.resourcesNum = static_cast<uint32>( pFolder.resourceList.size() );
		scfVirtualFolderDesc.subFoldersNum = static_cast<uint32>( pFolder.subFolderList.size() );

		gds::serializeAuto( pStorageBuffer, scfVirtualFolderDesc );
		pSysFile->writeData( pStorageBuffer.dataPtr(), pStorageBuffer.size() );

		for( const auto & resource : pFolder.resourceList )
		{
			writeResourceData( pSysFile, resource, pStorageBuffer );
		}

		for( const auto & subFolder : pFolder.subFolderList )
		{
			writeFolderData( pSysFile, subFolder, pStorageBuffer );
		}
	}

	void SCFIOProxy::writeResourceData( system::FileHandle pSysFile, const SCFResourceTemplate & pResource, ByteBuffer & pStorageBuffer )
	{
		SCFIOResourceDesc scfResourceDesc;
		scfResourceDesc.entryType = ESCFEntryType::VirtualFolder;
		scfResourceDesc.name = pResource.name;
		scfResourceDesc.uid = pResource.uid;

		const auto currentFilePtrOffset = pSysFile->getFilePointer();
		const auto resourceEntrySize = gds::evalByteSizeAuto( scfResourceDesc );

		scfResourceDesc.dataOffset = currentFilePtrOffset + resourceEntrySize;
		scfResourceDesc.dataSize = pResource.dataSource.byteSize;

		gds::serializeAuto( pStorageBuffer, scfResourceDesc );
		pSysFile->writeData( pStorageBuffer.dataPtr(), pStorageBuffer.size() );

		const auto sMaxSingleDataWriteSize = 2048;
		
		for( uint64 currentReadPtr = 0; currentReadPtr < scfResourceDesc.dataSize; )
		{
		    const auto readSize = pResource.dataSource.readCallback( currentReadPtr, sMaxSingleDataWriteSize, pStorageBuffer );
		    ts3DebugAssert( readSize > 0 );

		    pSysFile->writeData( pStorageBuffer.dataPtr(), pStorageBuffer.size() );

		    currentReadPtr += readSize;
		}
	}

	void SCFIOProxy::readFolder( system::FileHandle pSysFile, SCFVirtualFolder & pFolder, ByteBuffer & pStorageBuffer )
	{
		for( uint32 resourceIndex = 0; resourceIndex < pFolder.mFolderInfo.resourcesNum; ++resourceIndex )
		{
			SCFIOResourceDesc scfResourceDesc;
			readSerializedValueWithMetaInfo( pSysFile, scfResourceDesc, pStorageBuffer );

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
			readSerializedValueWithMetaInfo( pSysFile, scfFolderDesc, pStorageBuffer );

			SCFVirtualFolderInfo scfFolderInfo;
			scfFolderInfo.name = std::move( scfFolderDesc.name );
			scfFolderInfo.uid = std::move( scfFolderDesc.uid );
			scfFolderInfo.path = pFolder.mFolderInfo.path + "/" + scfFolderInfo.name;
			scfFolderInfo.resourcesNum = scfFolderDesc.resourcesNum;
			scfFolderInfo.subFoldersNum = scfFolderDesc.subFoldersNum;

			auto & scfFolder = pFolder.addSubFolder( std::move( scfFolderInfo ) );

			readFolder( pSysFile, scfFolder, pStorageBuffer );
		}
	}

	template <typename TpValue>
	void SCFIOProxy::readSerializedValueWithMetaInfo( system::FileHandle pSysFile, TpValue & pValue, ByteBuffer & pStorageBuffer )
	{
		const auto metaInfoSize = gds::getValueMetaInfoSize();
		const auto currentFileOffset = pSysFile->getFilePointer();

		const auto & bbref = bb;

		pSysFile->readData( pStorageBuffer, metaInfoSize );
		pSysFile->setFilePointer( currentFileOffset );

		const auto gdsMetaInfo = gds::readValueMetaInfo( pStorageBuffer.dataPtr() );

		pStorageBuffer.resize( trunc_numeric_cast<size_t>( gdsMetaInfo.outputBlockSize ) );
		pSysFile->readData( pStorageBuffer, static_cast<system::file_size_t>( gdsMetaInfo.outputBlockSize ) );

		gds::deserializeAuto( pStorageBuffer.dataPtr(), pValue );
	}

}
