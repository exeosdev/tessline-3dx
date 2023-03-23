
#pragma once

#ifndef __TS3_ENGINE_SCF_COMMON_H__
#define __TS3_ENGINE_SCF_COMMON_H__

#include "../prerequisites.h"

namespace ts3
{

	class SCFIndex;
	class SCFIOProxy;
	class SCFEntry;
	class SCFResource;
	class SCFVirtualFolder;

	enum class ESCFEntryType : uint32
	{
		Resource,
		VirtualFolder
	};

	ts3TypeInfoEnumDeclare( ESCFEntryType );

	struct SCFEntryInfo
	{
		ESCFEntryType entryType;
		std::string name;
		std::string path;
		std::string uid;
		uint32 treeSubLevel = 0;
	};

	struct SCFResourceInfo : public SCFEntryInfo
	{
		uint64 dataOffset;
		uint64 dataSize;
	};

	struct SCFVirtualFolderInfo : public SCFEntryInfo
	{
		uint32 resourcesNum;
		uint32 subFoldersNum;
	};

} // namespace ts3

#endif // __TS3_ENGINE_SCF_COMMON_H__
