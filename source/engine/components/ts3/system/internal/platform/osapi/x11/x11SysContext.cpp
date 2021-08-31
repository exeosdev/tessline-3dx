
#include <ts3/system/sysContextNative.h>

namespace ts3::system
{

    void SysContext::_nativeInitialize()
    {
        auto & cscNativeData = pContext.nativeData;

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

        cscNativeData.display = xDisplay;
        cscNativeData.sessionInfo.connectionNumber = XConnectionNumber( xDisplay );
        cscNativeData.sessionInfo.vendorName = XServerVendor( xDisplay );
        cscNativeData.sessionInfo.displayString = XDisplayString( xDisplay );
    }

    void SysContext::_nativeRelease()
    {
        auto & cscNativeData = pContext.nativeData;

        cscNativeData.display = nullptr;
        cscNativeData.sessionInfo.connectionNumber = -1;
        cscNativeData.sessionInfo.vendorName.clear();
        cscNativeData.sessionInfo.displayString.clear();
    }

} // namespace ts3::system
