
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_FILE_API_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_FILE_API_H__

#include "win32Common.h"
#include <ts3/system/fileCommon.h>

namespace ts3::system
{

    namespace platform
    {

        void win32FAOpenFileGeneric( File & pFile, std::string pFilePath, DWORD pFileAccess, DWORD pOpenMode, DWORD pFileFlags );

        bool win32FACheckDirectoryExists( const std::string & pDirPath );

        bool win32FACheckFileExists( const std::string & pFilePath );

        DWORD win32FATranslateFilePointerRefPos( EFilePointerRefPos pFileRefPos );

    }

} // namespace ts3::system

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_FILE_API_H__
