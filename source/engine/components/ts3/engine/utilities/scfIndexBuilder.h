
#pragma once

#ifndef __TS3_ENGINE_SCF_INDEX_BUILDER_H__
#define __TS3_ENGINE_SCF_INDEX_BUILDER_H__

#include "scfIndex.h"
#include <ts3/stdext/sortedArray.h>
#include <ts3/core/utility/dataProvider.h>

namespace ts3
{

	enum class SCFResourceInputDataType
	{
		Unknown,
		DataProvider,
		DataSource,
		EmptyPlaceholder
	};

	struct SCFResourceInputData
	{
		struct DataProvider
		{
			std::function<uint64()> sizeInfoCallback;
			std::function<uint64(void *, uint64, uint64)> readCallback;

			explicit operator bool() const
			{
				return sizeInfoCallback && readCallback;
			}
		};

		struct DataSource
		{
			const void * dataPtr = nullptr;
			uint64 dataSize = 0;

			explicit operator bool() const
			{
				return dataPtr && ( dataSize > 0 );
			}
		};

		SCFResourceInputDataType dataType = SCFResourceInputDataType::Unknown;
		uint64 placeholderSize = 0;
		DataProvider provider;
		DataSource source;

		explicit operator bool() const
		{
			return dataType != SCFResourceInputDataType::Unknown;
		}
	};

	struct SCFEntryTemplate
	{
		std::string name;
		std::string path;
		std::string uid;
	};

	struct SCFResourceTemplate : public SCFEntryTemplate
	{
		uint64 size;
		SCFResourceInputData inputData;
	};

	struct SCFVirtualFolderTemplate : public SCFEntryTemplate
	{
		SortedArray<SCFResourceTemplate> resourceList;
		SortedArray<SCFVirtualFolderTemplate> subFolderList;
	};

	class SCFIndexBuilder
	{
	public:

	public:
		SCFIndexBuilder();
		~SCFIndexBuilder();

		const SCFVirtualFolderTemplate * addFolder( const SCFVirtualFolderTemplate * pParentFolder,
													std::string pFolderName,
													std::string pUID );

		const SCFVirtualFolderTemplate * addFolder( const std::string & pParentLocation,
													std::string pFolderName,
													std::string pUID );

		const SCFResourceTemplate * addResource( const SCFVirtualFolderTemplate * pParentFolder,
												 std::string pResourceName,
												 std::string pUID );

		const SCFResourceTemplate * addResource( const std::string & pParentLocation,
												 std::string pResourceName,
												 std::string pUID );

		bool removeEntry(  const std::string & pLocation );

		bool resetResourceInputData( const std::string & pResourceLocation );

		bool setResourceDataPlaceholderSize( const SCFResourceTemplate * pResource, uint64 pDataSize );

		bool setResourceDataProvider( const SCFResourceTemplate * pResource,
									  SCFResourceInputData::DataProvider pDataProvider );

		bool setResourceDataSource( const SCFResourceTemplate * pResource,
									SCFResourceInputData::DataSource pDataSource );

		const SCFVirtualFolderTemplate & getRootVirtualFolder() const;

		bool checkNameAndUID( const std::string & pName, const std::string & pUID ) const;

	private:
		SCFVirtualFolderTemplate * _findFolderInternal( const std::string & pFolderLocation,
														SCFVirtualFolderTemplate * pRootFolder = nullptr );

		SCFResourceTemplate * _findResourceInternal( const std::string & pResourceLocation,
													 SCFVirtualFolderTemplate * pRootFolder = nullptr );

		bool _setResourceInputData( const SCFResourceTemplate * pResource, SCFResourceInputData pInputData );

		bool checkNameAndUID( const std::string & pName, const std::string & pUID );

		static void processLocationString( std::string & pLocationString );

	private:
		struct PrivateWorkingData;
		std::unique_ptr<SCFIndex> _fileIndex;
		std::unique_ptr<PrivateWorkingData> _privateWorkingData;
	};

}

#endif // __TS3_ENGINE_SCF_INDEX_BUILDER_H__
