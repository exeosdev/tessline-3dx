
#pragma once

#ifndef __TS3_ENGINE_SCF_ENTRY_H__
#define __TS3_ENGINE_SCF_ENTRY_H__

#include "scfCommon.h"

namespace ts3
{

    class ByteBuffer;
    class MemoryBuffer;
    class DynamicMemoryBuffer;

    using SCFEntryPredicate = std::function<bool( const SCFEntry & )>;
    using SCFEntryList = std::vector<SCFEntry *>;

	enum class ESCFFindMode : uint32
	{
		Direct,
		Recursive,
		Default = static_cast<uint32>( Recursive )
	};

	class SCFEntry
	{
	public:
	    SCFIndex * const mIndex;
		SCFVirtualFolder * const mParentFolder;
		const SCFEntryInfo * const mInfo;

	public:
		SCFEntry( const SCFEntry & ) = delete;
		SCFEntry & operator=( const SCFEntry & ) = delete;

		SCFEntry( SCFIndex & pIndex, const SCFEntryInfo * pInfo );
		SCFEntry( SCFVirtualFolder & pParentFolder, const SCFEntryInfo * pInfo );

		virtual ~SCFEntry();

		SCFVirtualFolder & asVirtualFolder();
		const SCFVirtualFolder & asVirtualFolder() const;

		SCFVirtualFolder * asVirtualFolderPtr() noexcept;
		const SCFVirtualFolder * asVirtualFolderPtr() const noexcept;

		SCFResource & asResource();
		const SCFResource & asResource() const;

		SCFResource * asResourcePtr() noexcept;
		const SCFResource * asResourcePtr() const noexcept;

		constexpr bool isResource() const
		{
		    return mInfo->entryType == ESCFEntryType::Resource;
		}

		constexpr bool isVirtualFolder() const
		{
		    return mInfo->entryType == ESCFEntryType::VirtualFolder;
		}
	};

	class SCFResource : public SCFEntry
	{
	public:
		SCFResourceInfo const mResourceInfo;

	public:
		SCFResource( const SCFResource & ) = delete;
		SCFResource & operator=( const SCFResource & ) = delete;

		SCFResource( SCFVirtualFolder & pParentFolder, SCFResourceInfo pInfo );

		uint64 readData( void * pTarget, uint64 pCapacity ) const;
		uint64 readData( ByteBuffer & pTarget ) const;
		uint64 readData( DynamicMemoryBuffer & pTarget ) const;
		uint64 readData( std::string & pTarget ) const;
		uint64 readData( std::vector<byte> & pTarget ) const;

		uint64 readSubData( void * pTarget, uint64 pCapacity, uint64 pReadSize, uint64 pResOffset = 0 ) const;
		uint64 readSubData( ByteBuffer & pTarget, uint64 pReadSize, uint64 pResOffset = 0 ) const;
		uint64 readSubData( MemoryBuffer & pTarget, uint64 pReadSize, uint64 pResOffset = 0 ) const;
		uint64 readSubData( std::vector<byte> & pTarget, uint64 pReadSize, uint64 pResOffset = 0 ) const;
	};

	class SCFVirtualFolder : public SCFEntry
	{
		friend class SCFIOProxy;

	public:
		SCFVirtualFolderInfo const mFolderInfo;

	public:
		SCFVirtualFolder( const SCFVirtualFolder & ) = delete;
		SCFVirtualFolder & operator=( const SCFVirtualFolder & ) = delete;

		SCFVirtualFolder( SCFIndex & pIndex, SCFVirtualFolderInfo pInfo );
		SCFVirtualFolder( SCFVirtualFolder & pParentFolder, SCFVirtualFolderInfo pInfo );

		SCFEntry * operator[]( const std::string & pEntryPath ) const noexcept;

		SCFEntry & at( const std::string & pEntryPath ) const;

		SCFEntry * findEntry( const std::string & pEntryPath, ESCFFindMode pFindMode ) const noexcept;

		SCFEntry & getEntry( const std::string & pEntryPath, ESCFFindMode pFindMode ) const;

		SCFVirtualFolder * findVirtualFolder( const std::string & pFolderPath, ESCFFindMode pFindMode ) const noexcept;

		SCFVirtualFolder & getVirtualFolder( const std::string & pFolderPath, ESCFFindMode pFindMode ) const;

		SCFResource * findResource( const std::string & pResourcePath, ESCFFindMode pFindMode ) const noexcept;

		SCFResource & getResource( const std::string & pResourcePath, ESCFFindMode pFindMode ) const;

		bool enumerateEntries( SCFEntryList & pList, ESCFFindMode pFindMode, bool pClearList = false ) const;

		bool enumerateEntries( SCFEntryList & pList, ESCFFindMode pFindMode, const SCFEntryPredicate & pPredicate, bool pClearList = false ) const;

		SCFEntryList enumerateEntries( ESCFFindMode  ) const;

		SCFEntryList enumerateEntries( ESCFFindMode pFindMode, const SCFEntryPredicate & pPredicate ) const;

	private:
		SCFResource & addResource( SCFResourceInfo pResourceInfo );

		SCFVirtualFolder & addSubFolder( SCFVirtualFolderInfo pFolderInfo );

		SCFEntry * findEntryInternal( const std::string & pEntryPath, ESCFFindMode pFindMode ) const noexcept;

		SCFEntry * findEntryInternalDirect( const std::string & pEntryPath ) const noexcept;

		SCFEntry * findEntryInternalRecursive( const std::string & pEntryName ) const noexcept;

	protected:
		using ResourcePtr = std::unique_ptr<SCFResource>;
		using VirtualFolderPtr = std::unique_ptr<SCFVirtualFolder>;
		std::vector<ResourcePtr> _resourceList;
		std::vector<VirtualFolderPtr> _subFolderList;
		std::map<std::string, SCFEntry *>  _entryNameMap;
	};

}

#endif // __TS3_ENGINE_SCF_ENTRY_H__
