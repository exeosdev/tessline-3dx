
#include <ts3/system/sysContextNative.h>

namespace ts3::system
{

    ASessionData & nativeAndroidGetASessionData( SysContext & pSysContext )
    {
        return pSysContext.mInternal->nativeDataPriv.aSessionData;
    }

    ASessionData & nativeAndroidGetASessionData( const AndroidNativeDataCommon & pNativeData )
    {
        ts3DebugAssert( pNativeData.aSessionDataPtr );
        return *( pNativeData.aSessionDataPtr );
    }

} // namespace ts3::system
