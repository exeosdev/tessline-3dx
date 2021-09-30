
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_FILE_API_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_FILE_API_H__

#include "win32Common.h"
#include <ts3/system/fileCommon.h>

namespace ts3::system
{

    namespace platform
    {

        DWORD win32TranslateFileOpenModeToWin32Access( EFileOpenMode pOpenMode );

        DWORD win32TranslateFileOpenModeToWin32CreationDisposition( EFileOpenMode pOpenMode );

        DWORD win32TranslateFilePointerRefPos( EFilePointerRefPos pFileRefPos );

    }

} // namespace ts3::system

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_FILE_API_H__
