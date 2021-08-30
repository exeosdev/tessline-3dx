
#ifndef __TS3_SYSTEM_CORE_SESSION_CONTEXT_NATIVE_H__
#define __TS3_SYSTEM_CORE_SESSION_CONTEXT_NATIVE_H__

#include "sysContext.h"

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_ANDROID )
#  include "internal/platform/osapi/android/androidSysContext.h"
#elif( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_WIN32 )
#  include "internal/platform/osapi/win32/win32SysContext.h"
#elif( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_X11 )
#  include "internal/platform/osapi/x11/x11SysContext.h"
#endif

namespace ts3::system
{

    struct SysContext::ContextPrivateData
    {
        SysContextNativeData nativeDataPriv;
    };

    /// @brief
    TS3_SYSTEM_API_NODISCARD SysContextHandle creInitializeSystemContext( const SysContextNativeCreateInfo & pCreateInfo );

    /// @brief
    TS3_SYSTEM_API void releaseSysContext( SysContextHandle pContext );

} // namespace ts3::system

#endif // __TS3_SYSTEM_CORE_SESSION_CONTEXT_NATIVE_H__
