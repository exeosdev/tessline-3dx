
#ifndef __TS3_SYSTEM_CORE_SYS_CONTEXT_NATIVE_H__
#define __TS3_SYSTEM_CORE_SYS_CONTEXT_NATIVE_H__

#include "prerequisites.h"

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_ANDROID )
#  include "internal/platform/osapi/android/androidSysContext.h"
#elif( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_WIN32 )
#  include "internal/platform/osapi/win32/win32SysContext.h"
#elif( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_X11 )
#  include "internal/platform/osapi/x11/x11SysContext.h"
#endif

namespace ts3::system
{

    struct SysContextCreateInfo
    {
        platform::SysContextCreateInfoNativeParams nativeParams;
    };

    TS3_SYSTEM_API_NODISCARD SysContextHandle createSysContext( const SysContextCreateInfo & pCreateInfo );

} // namespace ts3::system

#endif // __TS3_SYSTEM_CORE_SYS_CONTEXT_NATIVE_H__
