
#include <ts3/system/sysContextNative.h>

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_WIN32 )
namespace ts3::system
{

    void SysContext::_nativeInitialize()
    {
        auto & nativeData = mPrivate->nativeDataPriv;
        nativeData.appExecModuleHandle = ::GetModuleHandleA( nullptr );
    }

    void SysContext::_nativeRelease() noexcept
    {
        auto & nativeData = mPrivate->nativeDataPriv;
        nativeData.appExecModuleHandle = nullptr;
    }

} // namespace ts3::system
#endif
