
#ifndef __TS3_SYSTEM_FILE_MANAGER_NATIVE_H__
#define __TS3_SYSTEM_FILE_MANAGER_NATIVE_H__

#include "fileManager.h"
#include <list>
#include <map>

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_ANDROID )
#  include "internal/platform/osapi/android/androidFileManager.h"
#elif( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_WIN32 )
#  include "internal/platform/osapi/win32/win32FileManager.h"
#elif( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_X11 )
#  include "internal/platform/osapi/x11/x11FileManager.h"
#endif

namespace ts3::system
{

    struct File::ObjectInternalData
    {
        FileNativeData nativeDataPriv;
        file_str_t name;
        file_str_t fullPath;
    };

    struct FileManager::ObjectInternalData
    {
    };

}

#endif //__TS3_SYSTEM_FILE_MANAGER_NATIVE_H__
