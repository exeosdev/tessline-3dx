
#include <ts3/system/sysContextNative.h>

namespace ts3::system
{

    void SysContext::_nativeInitialize()
    {
        auto & nativeData = mPrivate->nativeDataPriv;

        int thrInitStatus = ::XInitThreads();
        if( thrInitStatus == False )
        {
            throw 0;
        }

        auto * xDisplay = ::XOpenDisplay( nullptr );
        if( xDisplay == nullptr )
        {
            throw 0;
        }

        nativeData.xSessionData.display = xDisplay;
        nativeData.xSessionData.screenIndex = XDefaultScreen( nativeData.xSessionData.display );
        nativeData.xSessionData.rootWindowXID = XRootWindow( nativeData.xSessionData.display, nativeData.xSessionData.screenIndex );
        nativeData.xSessionData.wmpDeleteWindow = XInternAtom( nativeData.xSessionData.display, "WM_DELETE_WINDOW", False );
        nativeData.xSessionData.sessionInfo.connectionNumber = XConnectionNumber( xDisplay );
        nativeData.xSessionData.sessionInfo.vendorName = XServerVendor( xDisplay );
        nativeData.xSessionData.sessionInfo.displayString = XDisplayString( xDisplay );
    }

    void SysContext::_nativeRelease() noexcept
    {
        auto & nativeData = mPrivate->nativeDataPriv;

        nativeData.xSessionData.display = nullptr;
        nativeData.xSessionData.screenIndex = -1;
        nativeData.xSessionData.rootWindowXID = E_X11_XID_NONE;
        nativeData.xSessionData.wmpDeleteWindow = 0;
        nativeData.xSessionData.sessionInfo.connectionNumber = -1;
        nativeData.xSessionData.sessionInfo.vendorName.clear();
        nativeData.xSessionData.sessionInfo.displayString.clear();
    }

} // namespace ts3::system
