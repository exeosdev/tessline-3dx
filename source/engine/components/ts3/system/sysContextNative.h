
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

    /// @brief
    enum ESysContextCreateFlags : uint32
    {
        E_CORE_SESSION_CONTEXT_CREATE_FLAG_INIT_DEFAULT = 0
    };

    /// @brief
    struct SysContextCreateInfo
    {
        SysContextCreateInfoNativeParams nativeParams;
        Bitmask<ESysContextCreateFlags> flags = E_CORE_SESSION_CONTEXT_CREATE_FLAG_INIT_DEFAULT;
    };

    struct SysContext::ObjectInternalData
    {
        SysContextNativeData nativeDataPriv;

        EventControllerHandle sharedEventController = nullptr;
    };

    void nativeSysContextInternalInitialize( SysContext & pSysContext, const SysContextCreateInfo & pCreateInfo );

    void nativeSysContextInternalRelease( SysContext & pSysContext );

    TS3_PCL_ATTR_NO_DISCARD inline system::SysContextHandle nativeSysContextCreate( const SysContextCreateInfo & pCreateInfo )
    {
        auto sysContext = std::make_shared<SysContext>();
        nativeSysContextInternalInitialize( *sysContext, pCreateInfo );
        return sysContext;
    }

    inline void nativeSysContextDestroy( SysContext & pSysContext )
    {
        ts3DebugAssert( pSysContext.mInternal );
        ts3DebugAssert( pSysContext.mNativeData );

        nativeSysContextInternalRelease( pSysContext );
    }

} // namespace ts3::system

#endif // __TS3_SYSTEM_CORE_SESSION_CONTEXT_NATIVE_H__
