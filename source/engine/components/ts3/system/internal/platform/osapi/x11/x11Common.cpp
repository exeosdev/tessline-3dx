
#include <ts3/system/sysContextNative.h>

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
