
#include <ts3/system/systemContext.h>

namespace ts3
{

	void Context::_sysInitialize( const ContextCreateInfo & pCreateInfo )
	{
		int thrInitStatus = ::XInitThreads();
		if( thrInitStatus == False )
		{
			throw 0;
		}

		auto * xdisplay = ::XOpenDisplay( nullptr );
		if( xdisplay == nullptr )
		{
			throw 0;
		}

		mNativeData.display = xdisplay;
		mNativeData.sessionExtInfo.connectionNumber = XConnectionNumber( xdisplay );
		mNativeData.sessionExtInfo.vendorName = XServerVendor( xdisplay );
		mNativeData.sessionExtInfo.displayString = XDisplayString( xdisplay );
	}

	void Context::_sysRelease() noexcept
	{
		mNativeData.display = nullptr;
		mNativeData.sessionExtInfo.connectionNumber = -1;
		mNativeData.sessionExtInfo.vendorName.clear();
		mNativeData.sessionExtInfo.displayString.clear();
	}

}
