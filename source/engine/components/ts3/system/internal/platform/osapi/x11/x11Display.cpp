
#include <ts3/system/displayNative.h>

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_X11 )
namespace ts3
{
namespace system
{

	void _x11EnumerateDisplayManagerConfiguration( DsmDisplayManagerNativeData * pNativeData );
	void _x11FreeDisplayManagerConfiguration( DsmDisplayManagerNativeData * pNativeData );

	void _nativeDsmDisplayManagerInitialize( DsmDisplayManager & pDisplayManager )
	{
	    auto & cscNativeData = pDisplayManager.csContext->nativeData;

	    if( cscNativeData.screenIndex == -1 )
	    {
	        cscNativeData.screenIndex = XDefaultScreen( cscNativeData.display );
	        cscNativeData.rootWindow = XRootWindow( cscNativeData.display, cscNativeData.screenIndex );
	        cscNativeData.wmpDeleteWindow = XInternAtom( cscNativeData.display, "WM_DELETE_WINDOW", False );
	    }

	    auto & dmNativeData = pDisplayManager.nativeData;

	    dmNativeData.display = cscNativeData.display;
	    dmNativeData.screenIndex = cscNativeData.screenIndex;
	    dmNativeData.rootWindow = cscNativeData.rootWindow;

	    x11XRRInitializeDriverState( &dmNativeData );

	    _x11EnumerateDisplayManagerConfiguration( &dmNativeData );
	}

	void _nativeDsmDisplayManagerRelease( DsmDisplayManager & pDisplayManager )
	{
	    auto & dmNativeData = pDisplayManager.nativeData;

	    _x11FreeDisplayManagerConfiguration( &dmNativeData );

	    x11XRRReleaseDriverState( &dmNativeData );

	    dmNativeData.display = nullptr;
	    dmNativeData.rootWindow = XID_None;
	}

	void _nativeDsmDisplayManagerQueryDisplaySize( DsmDisplayManager & pDisplayManager, DisplaySize & pOutDisplaySize )
	{
	    const auto & dmNativeData = pDisplayManager.nativeData;

	    // Ideally, we have XRR available here and can fetch the data from XRRMonitorInfo structure.
	    // With that approach, we can tell precisely the size of the default monitor (not the entire virtual screen).
	    auto * xrrDefaultMonitorInfo = dmNativeData.xrrDefaultMonitorInfo;

	    if( xrrDefaultMonitorInfo != nullptr )
	    {
	        // Monitor info for the default monitor. Width and height are the exact dimensions, in pixels.
	        pOutDisplaySize.x = static_cast<uint32>( xrrDefaultMonitorInfo->width );
	        pOutDisplaySize.y = static_cast<uint32>( xrrDefaultMonitorInfo->height );
	    }
	    else
	    {
	        // With "raw" X11 only, there is really no good way to get what we want... *screen* refers to the virtual
	        // area which (in case of a usual multi-monitor setup) may contain more than one monitor. E.g. for my setup
	        // with two 27" 1440p monitors, all X11 functions return the screen size as 5120 x 1440 pixels.
	        // TODO: can we somehow get the size of a current default monitor without using XRR/XF86 APIs?
	        auto displayWidth = XDisplayWidth( dmNativeData.display, dmNativeData.screenIndex );
	        auto displayHeight = XDisplayHeight( dmNativeData.display, dmNativeData.screenIndex );
	        pOutDisplaySize.x = static_cast<uint32>( displayWidth );
	        pOutDisplaySize.y = static_cast<uint32>( displayHeight );
	    }
	}

	void _nativeDsmDisplayManagerQueryMinWindowSize( DsmDisplayManager & pDisplayManager, DisplaySize & pOutMinWindowSize )
	{
	    pMinWindowSize.x = 0u;
	    pMinWindowSize.y = 0u;
	}


	void _x11EnumerateDisplayManagerConfiguration( DsmDisplayManagerNativeData * pNativeData )
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

	void _x11FreeDisplayManagerConfiguration( DsmDisplayManagerNativeData * pNativeData )
	{
		pNativeData->xrrDefaultMonitorInfo = nullptr;
	}

} // namespace system
} // namespace ts3
#endif
