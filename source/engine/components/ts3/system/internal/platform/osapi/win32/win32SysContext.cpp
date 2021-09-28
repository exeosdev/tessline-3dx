
#include <ts3/system/sysContextNative.h>

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_WIN32 )
namespace ts3::system
{

    void nativeSysContextInternalInitialize( SysContext & pSysContext, const SysContextCreateInfo & pCreateInfo )
    {
        auto & nativeData = pSysContext.mInternal->nativeDataPriv;
        nativeData.appExecModuleHandle = ::GetModuleHandleA( nullptr );
    }

    void nativeSysContextInternalRelease( SysContext & pSysContext )
    {
        auto & nativeData = pSysContext.mInternal->nativeDataPriv;
        nativeData.appExecModuleHandle = nullptr;
    }

} // namespace ts3::system
#endif // TS3_PCL_TARGET_SYSAPI_WIN32
