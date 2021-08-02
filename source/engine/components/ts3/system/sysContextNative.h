
#ifndef __TS3_SYSTEM_SYS_CONTEXT_NATIVE_H__
#define __TS3_SYSTEM_SYS_CONTEXT_NATIVE_H__

#include "sysContext.h"

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_ANDROID )
#  include "internal/platform/osapi/android/androidtemContext.h"
#elif( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_WIN32 )
#  include "internal/platform/osapi/win32/win32temContext.h"
#elif( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_X11 )
#  include "internal/platform/osapi/x11/x11temContext.h"
#endif

namespace ts3
{
namespace system
{

    enum EContextCreateFlags : uint32
    {
        E_CONTEXT_CREATE_FLAG_INIT_DEFAULT = 0
    };

    struct ContextCreateInfo : public ContextNativeCreateInfo
    {
        Bitmask<EContextCreateFlags> flags = E_CONTEXT_CREATE_FLAG_INIT_DEFAULT;
    };

    /// @brief
    class Context
    {
        friend ContextHandle sysCreateContext( const ContextCreateInfo & pCreateInfo );

    public:
        ContextNativeData mNativeData;

        Context() noexcept;
        ~Context() noexcept;

    private:
        void _sysInitialize( const ContextCreateInfo & pCreateInfo );
        void _sysRelease() noexcept;
    };

    ContextHandle sysCreateContext( const ContextCreateInfo & pCreateInfo );

} // namespace system
} // namespace ts3

#endif // __TS3_SYSTEM_SYS_CONTEXT_NATIVE_H__
