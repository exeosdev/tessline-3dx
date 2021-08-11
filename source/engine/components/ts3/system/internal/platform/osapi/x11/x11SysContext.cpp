
#include <ts3/system/sysContextNative.h>

namespace ts3
{
namespace system
{

    void _nativeSysContextInitialize( SysContext & pContext, const SysContextCreateInfo & pCreateInfo )
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

	void _nativeSysContextRelease( SysContext & pContext )
	{
        auto & cscNativeData = pContext.nativeData;

		cscNativeData.display = nullptr;
		cscNativeData.sessionInfo.connectionNumber = -1;
		cscNativeData.sessionInfo.vendorName.clear();
		cscNativeData.sessionInfo.displayString.clear();
	}

} // namespace system
} // namespace ts3
