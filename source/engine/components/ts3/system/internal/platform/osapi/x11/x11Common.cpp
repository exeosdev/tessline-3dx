
#include <ts3/system/sysContextNative.h>

namespace ts3::system
{

    X11SessionData & nativeX11GetXSessionData( SysContext & pSysContext )
    {
        return pSysContext.mInternal->nativeDataPriv.xSessionData;
    }

} // namespace ts3::system
