
#pragma once

#ifndef __TS3_ENGINE_SCF_INDEX_H__
#define __TS3_ENGINE_SCF_INDEX_H__

#include "scfEntry.h"

namespace ts3
{

	class SCFIndex
	{
		friend class SCFIndexBuilder;
		friend class SCFIOProxy;
		friend class SCFVirtualFolder;

	public:
		using ResourceDataReadCallback = std::function<uint64( void *, uint64, uint64 )>;

	public:
		SCFIndex();
		~SCFIndex();

		uint64 readResourceData( void * pTarget, uint64 pSize, uint64 pOffset = 0 ) const;

		SCFVirtualFolder & rootFolder() const;

	private:
		SCFVirtualFolder & initRootFolder( SCFVirtualFolderInfo pFolderInfo );

		void addEntry( SCFEntry & pEntry );

		void resetIndex();

	private:
		ResourceDataReadCallback _rdReadCallback;
		std::unique_ptr<SCFVirtualFolder> _rootFolder;
	};

}

#endif // __TS3_ENGINE_SCF_INDEX_H__
