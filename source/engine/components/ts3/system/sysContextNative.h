
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

namespace ts3
{
namespace system
{

    /// @brief
    class SysContextNativeProxy
    {
    public:
        SysContextNativeData mNativeData;

    public:
        SysContextNativeProxy( SysContext * pParentObject );
        virtual ~SysContextNativeProxy();
    };

    /// @brief
    TS3_SYSTEM_API_NODISCARD SysContextHandle initializeSysContext( const SysContextNativeProxyCreateInfo & pCreateInfo );

    /// @brief
    TS3_SYSTEM_API void releaseSysContext( SysContextHandle pContext );

} // namespace system
} // namespace ts3

#endif // __TS3_SYSTEM_CORE_SESSION_CONTEXT_NATIVE_H__