
#include "scfIOSupport.h"
#include "scfIndexBuilder.h"
#include <ts3/system/fileManager.h>
#include <ts3/core/utility/gdsCore.h>
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

		InternalFileWriteCallback fileWriteCallback =
			[file]( const void * pInputData, uint64 pWriteSize ) -> uint64 {
				const auto writeSize = trunc_numeric_cast<system::file_size_t>( pWriteSize );
				return file->write( pInputData, writeSize, writeSize );
			};

		DynamicByteArray sharedBuffer;

		const auto & rootFolder = pBuilder.getRootVirtualFolder();

		writeFolderData( file, rootFolder, sharedBuffer, fileWriteCallback );
	}

	void SCFIOProxy::loadIndex( const std::string & pFilename, SCFIndex & pIndex )
	{
		auto file = _sysFileManager->openFile( pFilename, system::EFileOpenMode::ReadOnly );
		if( !file )
		{
			return;
		}

		InternalFileReadCallback fileReadCallback =
			[file]( void * pOutputBuffer, uint64 pReadSize ) -> uint64 {
				const auto readSize = trunc_numeric_cast<system::file_size_t>( pReadSize );
				return file->read( pOutputBuffer, readSize, readSize );
			};

		DynamicByteArray sharedBuffer;

		SCFVirtualFolderInfo scfRootFolderInfo {};
		GdsCore::deserializeExternal( scfRootFolderInfo, fileReadCallback, sharedBuffer );

		auto & rootFolder = pIndex.initRootFolder(std::move( scfRootFolderInfo ) );

		readFolder( file, rootFolder, sharedBuffer, fileReadCallback );

		SCFIndex::ResourceDataReadCallback resourceDataReadCallback =
			[file]( void * pOutputBuffer, uint64 pReadSize, uint64 pBaseOffset ) -> uint64 {
				const auto readSize = trunc_numeric_cast<system::file_size_t>( pReadSize );
				file->setFilePointer( trunc_numeric_cast<system::file_offset_t>( pBaseOffset ) );
				return file->read( pOutputBuffer, readSize, readSize );
		};

		pIndex.setResourceDataReadCallback( std::move( resourceDataReadCallback ) );
	}

	void SCFIOProxy::writeFolderData( system::FileHandle pSysFile,
									  const SCFVirtualFolderTemplate & pFolder,
									  DynamicByteArray & pGdsCache,
									  const InternalFileWriteCallback & pFileWriteCallback )
	{
		SCFVirtualFolderInfo scfFolderInfo;
		scfFolderInfo.entryType = ESCFEntryType::VirtualFolder;
		scfFolderInfo.name = pFolder.name;
		scfFolderInfo.uid = pFolder.uid;
		scfFolderInfo.treeSubLevel = pFolder.treeSubLevel;
		scfFolderInfo.resourcesNum = static_cast<uint32>( pFolder.resourceList.size() );
		scfFolderInfo.subFoldersNum = static_cast<uint32>( pFolder.subFolderList.size() );

		GdsCore::serializeExternal( scfFolderInfo, pFileWriteCallback, pGdsCache );

		for( const auto & resource : pFolder.resourceList )
		{
			writeResourceData( pSysFile, resource, pGdsCache, pFileWriteCallback );
		}

		for( const auto & subFolder : pFolder.subFolderList )
		{
			writeFolderData( pSysFile, subFolder, pGdsCache, pFileWriteCallback );
		}
	}

	void SCFIOProxy::writeResourceData( system::FileHandle pSysFile,
										const SCFResourceTemplate & pResource,
										DynamicByteArray & pGdsCache,
										const InternalFileWriteCallback & pFileWriteCallback )
	{
		SCFResourceInfo scfResourceInfo;
		scfResourceInfo.entryType = ESCFEntryType::VirtualFolder;
		scfResourceInfo.name = pResource.name;
		scfResourceInfo.uid = pResource.uid;
		scfResourceInfo.treeSubLevel = pResource.treeSubLevel;

		const auto currentFilePtrOffset = pSysFile->getFilePointer();
		const auto resourceEntrySize = GdsCore::evalByteSizeWithMetaData( scfResourceInfo );

		scfResourceInfo.dataOffset = currentFilePtrOffset + resourceEntrySize;
		scfResourceInfo.dataSize = pResource.dataSource.byteSize;

		GdsCore::serializeExternal( scfResourceInfo, pFileWriteCallback, pGdsCache );

		const auto sMaxSingleDataWriteSize = 2048;
		
		for( uint64 currentReadPtr = 0; currentReadPtr < scfResourceInfo.dataSize; )
		{
			const auto readSize = pResource.dataSource.readCallback( currentReadPtr, sMaxSingleDataWriteSize, pGdsCache );
			ts3DebugAssert( readSize > 0 );

			pSysFile->write( pGdsCache.data(), pGdsCache.size() );

			currentReadPtr += readSize;
		}
	}

	void SCFIOProxy::readFolder( system::FileHandle pSysFile,
								 SCFVirtualFolder & pFolder,
								 DynamicByteArray & pGdsCache,
								 const InternalFileReadCallback & pFileReadCallback )
	{
		for( uint32 resourceIndex = 0; resourceIndex < pFolder.mFolderInfo.resourcesNum; ++resourceIndex )
		{
			SCFResourceInfo scfResourceInfo;
			GdsCore::deserializeExternal( scfResourceInfo, pFileReadCallback, pGdsCache );

			// The resource path is not serialized, don't forget this line.
			scfResourceInfo.path = pFolder.mFolderInfo.path + "/" + scfResourceInfo.name;

			auto & scfResource = pFolder.addResource( std::move( scfResourceInfo ) );

			const auto skipResourceDataOffset = scfResource.mResourceInfo.dataOffset + scfResource.mResourceInfo.dataSize;
			pSysFile->setFilePointer( static_cast<system::file_offset_t>( skipResourceDataOffset ) );
		}

		for( uint32 subFolderIndex = 0; subFolderIndex < pFolder.mFolderInfo.subFoldersNum; ++subFolderIndex )
		{
			SCFVirtualFolderInfo scfFolderInfo {};
			GdsCore::deserializeExternal( scfFolderInfo, pFileReadCallback, pGdsCache );

			// The folder path is not serialized, don't forget this line.
			scfFolderInfo.path = pFolder.mFolderInfo.path + "/" + scfFolderInfo.name;

			auto & scfFolder = pFolder.addSubFolder( std::move( scfFolderInfo ) );

			readFolder( pSysFile, scfFolder, pGdsCache, pFileReadCallback );
		}
	}

	namespace gds
	{

		gds_size_t serialize( byte * pOutputBuffer, const SCFEntryInfo & pValue )
		{
			return GdsCore::serializeAll( pOutputBuffer,
								 pValue.entryType,
								 pValue.name,
								 pValue.uid,
								 pValue.treeSubLevel );
		}

		gds_size_t deserialize( const byte * pInputData, SCFEntryInfo & pValue )
		{
			return GdsCore::deserializeAll( pInputData,
								   pValue.entryType,
								   pValue.name,
								   pValue.uid,
								   pValue.treeSubLevel );
		}

		gds_size_t evalByteSize( const SCFEntryInfo & pValue )
		{
			return GdsCore::evalByteSizeAll( pValue.entryType,
									pValue.name,
									pValue.uid,
									pValue.treeSubLevel );
		}

		gds_size_t serialize( byte * pOutputBuffer, const SCFResourceInfo & pValue )
		{
			return GdsCore::serializeAll( pOutputBuffer,
								 static_cast<const SCFEntryInfo &>( pValue ),
								 pValue.dataOffset,
								 pValue.dataSize );
		}

		gds_size_t deserialize( const byte * pInputData, SCFResourceInfo & pValue )
		{
			return GdsCore::deserializeAll( pInputData,
								   static_cast<SCFEntryInfo &>( pValue ),
								   pValue.dataOffset,
								   pValue.dataSize );
		}

		gds_size_t evalByteSize( const SCFResourceInfo & pValue )
		{
			return GdsCore::evalByteSizeAll( static_cast<const SCFEntryInfo &>( pValue ),
									pValue.dataOffset,
									pValue.dataSize );
		}

		gds_size_t serialize( byte * pOutputBuffer, const SCFVirtualFolderInfo & pValue )
		{
			return GdsCore::serializeAll( pOutputBuffer,
								 static_cast<const SCFEntryInfo &>( pValue ),
								 pValue.resourcesNum,
								 pValue.subFoldersNum );
		}

		gds_size_t deserialize( const byte * pInputData, SCFVirtualFolderInfo & pValue )
		{
			return GdsCore::deserializeAll( pInputData,
								   static_cast<SCFEntryInfo &>( pValue ),
								   pValue.resourcesNum,
								   pValue.subFoldersNum );
		}

		gds_size_t evalByteSize( const SCFVirtualFolderInfo & pValue )
		{
			return GdsCore::evalByteSizeAll( static_cast<const SCFEntryInfo &>( pValue ),
									pValue.resourcesNum,
									pValue.subFoldersNum );
		}

	}

}
