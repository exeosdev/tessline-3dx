
#ifndef __TS3_SYSTEM_CORE_SESSION_CONTEXT_NATIVE_H__
#define __TS3_SYSTEM_CORE_SESSION_CONTEXT_NATIVE_H__

#include "coreSessionContext.h"

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_ANDROID )
#  include "internal/platform/osapi/android/androidCoreSessionContext.h"
#elif( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_WIN32 )
#  include "internal/platform/osapi/win32/win32CoreSessionContext.h"
#elif( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_X11 )
#  include "internal/platform/osapi/x11/x11CoreSessionContext.h"
#endif

namespace ts3
{
namespace system
{

    /// @brief
    struct CoreSessionContext
    {
        CoreSessionContextNativeData nativeData;
    };

} // namespace system
} // namespace ts3

#endif // __TS3_SYSTEM_CORE_SESSION_CONTEXT_NATIVE_H__
