
#include "osxDisplaySystem.h"
#include <CoreGraphics/CGDisplayConfiguration.h>

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_OSX )
namespace ts3::system
{

	OSXDisplayManager::OSXDisplayManager( SysContextHandle pSysContext )
	: OSXNativeObject( std::move( pSysContext ) )
	{
		_initializeOSXDisplayManagerState();
	}

	OSXDisplayManager::~OSXDisplayManager() noexcept
	{
		_releaseOSXDisplayManagerState();
	}

	void OSXDisplayManager::_initializeOSXDisplayManagerState()
	{
		CGDisplayCount activeDisplaysNum = 0u;
		auto cgResult = ::CGGetActiveDisplayList( 0, nullptr, &activeDisplaysNum );

		if( cgResult != kCGErrorSuccess )
		{
			ts3DebugInterrupt();
			return;
		}

		if( activeDisplaysNum == 0 )
		{
			return;
		}

		auto activeDisplayList = std::make_unique<CGDirectDisplayID[]>( activeDisplaysNum );
		cgResult = ::CGGetActiveDisplayList( activeDisplaysNum, &( activeDisplayList[0] ), &activeDisplaysNum );

		if( cgResult != kCGErrorSuccess )
		{
			ts3DebugInterrupt();
			return;
		}

		CGDirectDisplayID mainDisplayID = kCGNullDirectDisplay;
		for( CGDisplayCount displayIndex = 0; displayIndex < activeDisplaysNum; ++displayIndex )
		{
			const auto displayID = activeDisplayList[displayIndex];
			if( ::CGDisplayIsMain( displayID ) )
			{
				mainDisplayID = displayID;
				break;
			}
		}

		if( mainDisplayID == kCGNullDirectDisplay )
		{
			mainDisplayID = activeDisplayList[0];
		}

		mNativeData.cgActiveDisplayList = std::move( activeDisplayList );
		mNativeData.cgActiveDisplaysNum = activeDisplaysNum;
		mNativeData.cgMainDisplayID = mainDisplayID;
	}
	
	void OSXDisplayManager::_releaseOSXDisplayManagerState()
	{}

	DisplayDriverHandle OSXDisplayManager::_nativeCreateDisplayDriver()
	{
		return createSysObject<OSXDisplayDriver>( getHandle<OSXDisplayManager>() );
	}

	void OSXDisplayManager::_nativeQueryDefaultDisplaySize( DisplaySize & pOutSize ) const
	{
		if( mNativeData.cgMainDisplayID == kCGNullDirectDisplay )
		{
			throw 0;
		}

		const auto displayRect = ::CGDisplayBounds( mNativeData.cgMainDisplayID );

		pOutSize.x = static_cast<uint32>( displayRect.size.width );
		pOutSize.y = static_cast<uint32>( displayRect.size.height );
	}

	void OSXDisplayManager::_nativeQueryMinWindowSize( DisplaySize & pOutSize ) const
	{
		pOutSize.x = 1;
		pOutSize.y = 1;
	}

} // namespace ts3::system
#endif // TS3_PCL_TARGET_SYSAPI_OSX
