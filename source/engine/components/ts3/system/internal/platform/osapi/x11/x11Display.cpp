
#include <ts3/system/displayManager.h>
#include <ts3/system/systemContext.h>

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_X11 )
namespace ts3
{

	void _x11EnumerateDisplayManagerConfiguration( DisplayManagerNativeData * pNativeData );
	void _x11FreeDisplayManagerConfiguration( DisplayManagerNativeData * pNativeData );

	void DisplayManager::_sysInitialize()
	{
		auto & scNativeData = mContext->mNativeData;

		if( scNativeData.screenIndex == -1 )
		{
			scNativeData.screenIndex = XDefaultScreen( scNativeData.display );
			scNativeData.rootWindow = XRootWindow( scNativeData.display, scNativeData.screenIndex );
			scNativeData.wmpDeleteWindow = XInternAtom( scNativeData.display, "WM_DELETE_WINDOW", False );
		}

		mNativeData.display = scNativeData.display;
		mNativeData.screenIndex = scNativeData.screenIndex;
		mNativeData.rootWindow = scNativeData.rootWindow;

		x11XRRInitializeDriverState( &mNativeData );

		_x11EnumerateDisplayManagerConfiguration( &mNativeData );
	}

	void DisplayManager::_sysRelease() noexcept
	{
		_x11FreeDisplayManagerConfiguration( &mNativeData );

		x11XRRReleaseDriverState( &mNativeData );

		mNativeData.display = nullptr;
		mNativeData.rootWindow = cvXIDNone;
	}

	void DisplayManager::_sysQueryDisplaySize( DisplaySize & pDisplaySize ) const
	{
		// Ideally, we have XRR available here and can fetch the data from XRRMonitorInfo structure.
		// With that approach, we can tell precisely the size of the default monitor (not the entire virtual screen).
		auto * xrrDefaultMonitorInfo = mNativeData.xrrDefaultMonitorInfo;

		if( xrrDefaultMonitorInfo != nullptr )
		{
			// Monitor info for the default monitor. Width and height are the exact dimensions, in pixels.
			pDisplaySize.x = static_cast<uint32>( xrrDefaultMonitorInfo->width );
			pDisplaySize.y = static_cast<uint32>( xrrDefaultMonitorInfo->height );
		}
		else
		{
			// With "raw" X11 only, there is really no good way to get what we want... *screen* refers to the virtual
			// area which (in case of a usual multi-monitor setup) may contain more than one monitor. E.g. for my setup
			// with two 27" 1440p monitors, all X11 functions return the screen size as 5120 x 1440 pixels.
			// TODO: can we somehow get the size of a current default monitor without using XRR/XF86 APIs?
			auto displayWidth = XDisplayWidth( mNativeData.display, mNativeData.screenIndex );
			auto displayHeight = XDisplayHeight( mNativeData.display, mNativeData.screenIndex );
			pDisplaySize.x = static_cast<uint32>( displayWidth );
			pDisplaySize.y = static_cast<uint32>( displayHeight );
		}
	}

	void DisplayManager::_sysQueryMinWindowSize( DisplaySize & pMinWindowSize ) const
	{
		pMinWindowSize.x = 0u;
		pMinWindowSize.y = 0u;
	}


	void _x11EnumerateDisplayManagerConfiguration( DisplayManagerNativeData * pNativeData )
	{
		for ( int monitorIndex = 0; monitorIndex < pNativeData->xrrMonitorsNum; ++monitorIndex )
		{
			auto & monitorInfo = pNativeData->xrrMonitorList[monitorIndex];
			if ( monitorInfo.primary != 0 )
			{
				pNativeData->xrrDefaultMonitorInfo = &monitorInfo;
			}
		}
	}

	void _x11FreeDisplayManagerConfiguration( DisplayManagerNativeData * pNativeData )
	{
		pNativeData->xrrDefaultMonitorInfo = nullptr;
	}

}
#endif
