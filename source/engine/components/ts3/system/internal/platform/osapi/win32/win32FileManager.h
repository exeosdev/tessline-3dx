
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_FILE_MANAGER_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_FILE_MANAGER_H__

#include "win32Common.h"

namespace ts3::system
{

    struct FileNativeData
    {
        HANDLE fileHandle;
    };

} // namespace ts3::system

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_FILE_MANAGER_H__
