
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_POSIX_FILE_MANAGER_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_POSIX_FILE_MANAGER_H__

#include <cstdio>
#include <cstring>

namespace ts3::system
{

    struct FileNativeData
    {
        FILE * filePtr;
    };

} // namespace ts3::system

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_POSIX_FILE_MANAGER_H__
