
#pragma once

#ifndef __TS3_ENGINE_SCF_INDEX_BUILDER_H__
#define __TS3_ENGINE_SCF_INDEX_BUILDER_H__

#include "scfIndex.h"
#include <ts3/stdext/byteBuffer.h>
#include <ts3/stdext/sortedArray.h>
#include <ts3/system/fileCommon.h>

namespace ts3
{

	struct SCFInputDataSource
	{
		using DataReadCallback = std::function<uint64( uint64 /* pOffset */, uint64 /* pReadSize */, ByteBuffer & /* pBuffer */ )>;

		uint64 byteSize = 0;

		DataReadCallback readCallback;

		explicit operator bool() const
		{
			return ( byteSize > 0 ) && readCallback;
		}

		static SCFInputDataSource asPlaceholder( uint64 pDataSize );

		static SCFInputDataSource fromFile( system::FileManagerHandle pSysFileManager, const std::string & pFilename );

		static SCFInputDataSource fromMemory( ArrayView<byte> pMemoryView );
	};

	struct SCFEntryTemplate
	{
		std::string name;
		std::string path;
		std::string uid;
	};

	struct SCFResourceTemplate : public SCFEntryTemplate
	{
		SCFInputDataSource dataSource;
	};

	struct SCFVirtualFolderTemplate : public SCFEntryTemplate
	{
		SortedArray<SCFResourceTemplate> resourceList;
		SortedArray<SCFVirtualFolderTemplate> subFolderList;
	};

	inline const std::string cvSCFEntryUIDEmpty {};

	class SCFIndexBuilder
	{
	public:
		SCFIndexBuilder();
		~SCFIndexBuilder();

		const SCFVirtualFolderTemplate * addFolder( const SCFVirtualFolderTemplate * pParentFolder,
													std::string pFolderName,
													std::string pUID = cvSCFEntryUIDEmpty );

		const SCFVirtualFolderTemplate * addFolder( const std::string & pParentLocation,
													std::string pFolderName,
													std::string pUID = cvSCFEntryUIDEmpty );

		const SCFResourceTemplate * addResource( const SCFVirtualFolderTemplate * pParentFolder,
												 std::string pResourceName,
												 SCFInputDataSource pDataSource,
												 std::string pUID = cvSCFEntryUIDEmpty );

		const SCFResourceTemplate * addResource( const std::string & pParentLocation,
												 std::string pResourceName,
												 SCFInputDataSource pDataSource,
												 std::string pUID = cvSCFEntryUIDEmpty );

		bool removeEntry(  const std::string & pLocation );

		const SCFVirtualFolderTemplate & getRootVirtualFolder() const;

		bool checkNameAndUID( const std::string & pName, const std::string & pUID ) const;

	private:
		SCFVirtualFolderTemplate * _findFolderInternal( const std::string & pFolderLocation,
														SCFVirtualFolderTemplate * pRootFolder = nullptr );

		SCFResourceTemplate * _findResourceInternal( const std::string & pResourceLocation,
													 SCFVirtualFolderTemplate * pRootFolder = nullptr );

	private:
		struct PrivateWorkingData;
		std::unique_ptr<SCFIndex> _fileIndex;
		std::unique_ptr<PrivateWorkingData> _privateWorkingData;
	};

}

#endif // __TS3_ENGINE_SCF_INDEX_BUILDER_H__
