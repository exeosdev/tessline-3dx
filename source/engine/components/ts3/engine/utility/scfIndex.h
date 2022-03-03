
#pragma once

#ifndef __TS3_ENGINE_SCF_INDEX_H__
#define __TS3_ENGINE_SCF_INDEX_H__

#include "scfEntry.h"

namespace ts3
{

	class SCFIndex
	{
		friend class SCFIOProxy;
		friend class SCFResource;
		friend class SCFVirtualFolder;

	public:
		using ResourceDataReadCallback = std::function<uint64( void *, uint64, uint64 )>;

	public:
		SCFIndex();
		~SCFIndex();

		SCFEntry * findEntryByPath( const std::string & pEntryPath ) const noexcept;

		SCFEntry & getEntryByPath( const std::string & pEntryPath ) const;

		SCFEntry * findEntryByUUID( const std::string & pUUID ) const noexcept;

		SCFEntry & getEntryByUUID( const std::string & pUUID ) const;

		bool enumerateEntries( SCFEntryList & pList, bool pClearList = false ) const;

		bool enumerateEntries( SCFEntryList & pList, const SCFEntryPredicate & pPredicate, bool pClearList = false ) const;

		SCFEntryList enumerateEntries() const;

		SCFEntryList enumerateEntries( const SCFEntryPredicate & pPredicate ) const;

		SCFVirtualFolder & rootFolder() const;

	private:
		SCFVirtualFolder & initRootFolder( SCFVirtualFolderInfo pFolderInfo );

		void addEntry( SCFEntry & pEntry );

		void setResourceDataReadCallback( ResourceDataReadCallback pCallback );

		uint64 readResourceData( void * pTarget, uint64 pSize, uint64 pBaseOffset ) const;

	private:
		ResourceDataReadCallback _rdReadCallback;
		std::unique_ptr<SCFVirtualFolder> _rootFolder;
		std::unordered_map<std::string, SCFEntry *> _entryByUIDMap;
	};

}

#endif // __TS3_ENGINE_SCF_INDEX_H__
