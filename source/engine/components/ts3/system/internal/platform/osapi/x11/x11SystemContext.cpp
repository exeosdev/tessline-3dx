
#include <ts3/system/systemContext.h>

namespace ts3
{

	void SysContext::_sysInitialize( const SysContextCreateInfo & pCreateInfo )
	{
		int thrInitStatus = XInitThreads();
		if( thrInitStatus == False )
		{
			throw 0;
		}

		auto * xdisplay = XOpenDisplay( nullptr );
		if( xdisplay == nullptr )
		{
			throw 0;
		}

		pSysContext.nativeData->display = xdisplay;
		pSysContext.nativeData->sessionExtInfo.connectionNumber = XConnectionNumber( xdisplay );
		pSysContext.nativeData->sessionExtInfo.vendorName = XServerVendor( xdisplay );
		pSysContext.nativeData->sessionExtInfo.displayString = XDisplayString( xdisplay );
	}

	void SysContext::_sysRelease() noexcept
	{
		pSysContext.nativeData->display = nullptr;
		pSysContext.nativeData->sessionExtInfo.connectionNumber = -1;
		pSysContext.nativeData->sessionExtInfo.vendorName.clear();
		pSysContext.nativeData->sessionExtInfo.displayString.clear();
	}

}
