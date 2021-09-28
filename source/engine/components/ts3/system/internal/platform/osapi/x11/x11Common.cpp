
#include <ts3/system/sysContextNative.h>

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_X11 )
namespace ts3::system
{

    X11SessionData & nativeX11GetXSessionData( SysContext & pSysContext )
    {
        return pSysContext.mInternal->nativeDataPriv.xSessionData;
    }

    X11SessionData & nativeX11GetXSessionData( const X11NativeDataCommon & pNativeData )
    {
        ts3DebugAssert( pNativeData.xSessionDataPtr );
        return *( pNativeData.xSessionDataPtr );
    }

} // namespace ts3::system
#endif // TS3_PCL_TARGET_SYSAPI_X11
