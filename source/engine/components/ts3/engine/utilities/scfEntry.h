
#pragma once

#ifndef __TS3_ENGINE_SCF_ENTRY_H__
#define __TS3_ENGINE_SCF_ENTRY_H__

#include "scfCommon.h"
#include <ts3/stdext/memoryBuffer.h>

namespace ts3
{

	enum class ESCFFindMode : uint32
	{
		Direct,
		Recursive,
		Default = static_cast<uint32>( Recursive )
	};

	class SCFEntry
	{
	public:
		const SCFEntryInfo * const mInfo;
		SCFVirtualFolder * const mParentFolder;

	public:
		SCFEntry( const SCFEntryInfo & pInfo, SCFVirtualFolder * pParentFolder );

		SCFVirtualFolder & asVirtualFolder();
		const SCFVirtualFolder & asVirtualFolder() const;

		SCFVirtualFolder * asVirtualFolderPtr() noexcept;
		const SCFVirtualFolder * asVirtualFolderPtr() const noexcept;

		SCFResource & asResource();
		const SCFResource & asResource() const;

		SCFResource * asResourcePtr() noexcept;
		const SCFResource * asResourcePtr() const noexcept;

		constexpr bool isVirtualFolder() const;
		constexpr bool isResource() const;
	};

	class SCFResource : public SCFEntry
	{
	public:
		SCFResourceInfo const mResourceInfo;

	public:
		uint64 readData( void * pTarget, uint64 pSize, uint64 pOffset = 0 ) const;

		uint64 readData( MemoryBuffer & pTarget, uint64 pSize = CX_MAX_SIZE, uint64 pOffset = 0 ) const;

		uint64 readData( std::vector<byte> & pTarget, uint64 pSize = CX_MAX_SIZE, uint64 pOffset = 0 ) const;
	};

	class SCFVirtualFolder : public SCFEntry
	{
	public:
		SCFVirtualFolderInfo const mVirtualFolderInfo;

	public:
		using EntryPredicate = std::function<bool( const SCFEntry & )>;
		using EntryList = std::vector<const SCFEntry *>;

		const SCFEntry * operator[]( const std::string & pEntryPath ) const noexcept;

		const SCFEntry * findEntry( const std::string & pEntryPath, ESCFFindMode pFindMode = ESCFFindMode::Default ) const noexcept;
		const SCFEntry & getEntry( const std::string & pEntryPath, ESCFFindMode pFindMode = ESCFFindMode::Default ) const;

		const SCFVirtualFolder * findVirtualFolder( const std::string & pVFPath, ESCFFindMode pFindMode = ESCFFindMode::Default ) const noexcept;
		const SCFVirtualFolder & getVirtualFolder( const std::string & pVFPath, ESCFFindMode pFindMode = ESCFFindMode::Default ) const;

		const SCFResource * findResource( const std::string & pResourcePath, ESCFFindMode pFindMode = ESCFFindMode::Default ) const noexcept;
		const SCFResource & getResource( const std::string & pResourcePath, ESCFFindMode pFindMode = ESCFFindMode::Default ) const;

		bool enumerateEntries( EntryList & pList ) const;

		bool enumerateEntries( EntryList & pList, const EntryPredicate & pPredicate ) const;

		EntryList enumerateEntries() const;

		EntryList enumerateEntries( const EntryPredicate & pPredicate ) const;

	protected:
		//void addResource(  )

	private:
		SCFEntry * findEntryInternal( const std::string & pEntryPath, ESCFFindMode pFindMode ) const noexcept;
		SCFEntry * findEntryInternalDirect( const std::string & pEntryPath ) const noexcept;
		SCFEntry * findEntryInternalRecursive( const std::string & pEntryName ) const noexcept;

	protected:
		using ResourceList = std::vector<SCFResource>;
		using SubVirtualFolderList = std::vector<SCFVirtualFolder>;
		using EntryMap = std::map<std::string, SCFEntry *>;

		ResourceList _resourceList;
		SubVirtualFolderList _subVirtualFolderList;
		EntryList _entryList;
		EntryMap  _entryMap;
	};

}

#endif // __TS3_ENGINE_SCF_ENTRY_H__
