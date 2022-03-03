
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

	using scf_resource_type_tag_t = uint64;
	using scf_version_value_t = uint64;
	using SCFResourceUID = std::string;

	enum ESCFFormatVersion : uint32
	{
		E_SCF_FORMAT_VERSION_V1 = 1,
		E_SCF_FORMAT_VERSION_EX_CURRENT = E_SCF_FORMAT_VERSION_V1,
	};

	enum class ESCFEntryType : uint32
	{
		Resource,
		VirtualFolder
	};

	inline const std::string & toString( ESCFEntryType pEntryType )
	{
	    static const std::unordered_map<ESCFEntryType, std::string> sMap = {
	        { ESCFEntryType::Resource, "Resource" },
	        { ESCFEntryType::VirtualFolder, "VirtualFolder" }
	    };
	    return sMap.at( pEntryType );
	}

	struct SCFEntryInfo
	{
		ESCFEntryType entryType;
		std::string name;
		std::string path;
		std::string uid;
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

}

#endif // __TS3_ENGINE_SCF_COMMON_H__
