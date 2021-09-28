
#include <ts3/system/sysContextNative.h>

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_ANDROID )
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
#endif // TS3_PCL_TARGET_SYSAPI_ANDROID
