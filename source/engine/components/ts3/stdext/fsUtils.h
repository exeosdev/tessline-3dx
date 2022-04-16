
#ifndef __TS3_STDEXT_FS_UTILS_H__
#define __TS3_STDEXT_FS_UTILS_H__

#include "bitmask.h"
#include <string>

namespace ts3
{

	class FSUtilityAPI
	{
	public:
		enum EFlags : uint32
		{
			E_SPLIT_PATH_FLAG_ASSUME_DIRECTORY_BIT = 0x0001,

			E_SPLIT_PATH_FLAG_ASSUME_FILE_BIT = 0x0002,

			E_SPLIT_PATH_FLAGS_DEFAULT = E_SPLIT_PATH_FLAG_ASSUME_DIRECTORY_BIT,

			E_SPLIT_PATH_MASK_ALL =
				E_SPLIT_PATH_FLAG_ASSUME_DIRECTORY_BIT |
				E_SPLIT_PATH_FLAG_ASSUME_FILE_BIT
		};

		struct FilePathInfo
		{
			std::string directory;
			std::string fileName;
		};

		FSUtilityAPI() = delete;
		~FSUtilityAPI() = delete;

		static std::string normalizePath( const std::string & pPath );

		static FilePathInfo splitPath( std::string pFilePath, Bitmask<EFlags> pFlags = E_SPLIT_PATH_FLAGS_DEFAULT );

		static bool isFilenameWithExtension( const std::string & pFilename );
	};

}

#endif // __TS3_STDEXT_FS_UTILS_H__
