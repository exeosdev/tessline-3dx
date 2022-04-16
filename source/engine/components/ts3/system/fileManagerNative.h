
#ifndef __TS3_SYSTEM_FILE_MANAGER_NATIVE_H__
#define __TS3_SYSTEM_FILE_MANAGER_NATIVE_H__

#include "fileCommon.h"

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_ANDROID )
#  include "internal/platform/osapi/android/androidFileManager.h"
#elif( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_WIN32 )
#  include "internal/platform/osapi/win32/win32FileManager.h"
#elif( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_X11 )
#  include "internal/platform/osapi/x11/x11FileManager.h"
#endif

#endif //__TS3_SYSTEM_FILE_MANAGER_NATIVE_H__
