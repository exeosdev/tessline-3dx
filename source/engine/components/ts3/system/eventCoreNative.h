
#ifndef __TS3_SYSTEM_EVENT_CORE_NATIVE_H__
#define __TS3_SYSTEM_EVENT_CORE_NATIVE_H__

#include "eventCommon.h"

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_ANDROID )
#  include "internal/platform/osapi/android/androidEventCore.h"
#elif( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_WIN32 )
#  include "internal/platform/osapi/win32/win32EventCore.h"
#elif( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_X11 )
#  include "internal/platform/osapi/x11/x11EventCore.h"
#endif

#endif // __TS3_SYSTEM_EVENT_CORE_NATIVE_H__
