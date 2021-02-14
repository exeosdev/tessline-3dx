
#include <ts3/system/displayManagerNative.h>

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_X11 )
namespace ts3
{

	void _x11EnumerateDisplayManagerConfiguration( SysDisplayManagerNativeData * pNativeData );
	void _x11FreeDisplayManagerConfiguration( SysDisplayManagerNativeData * pNativeData );

	void SysDisplayManager::_sysInitialize()
	{
		auto * scNativeData = mSystemContext->nativeData;
		scNativeData->display = scNativeData->display;
		scNativeData->screenIndex = XDefaultScreen( scNativeData->display );
		scNativeData->rootWindow = XRootWindow( scNativeData->display, scNativeData->screenIndex );

		pDisplayManager.nativeData->display = scNativeData->display;
		pDisplayManager.nativeData->screenIndex = scNativeData->screenIndex;
		pDisplayManager.nativeData->rootWindow = scNativeData->rootWindow;

		x11XRRInitializeDriverState( pDisplayManager.nativeData );

		_x11EnumerateDisplayManagerConfiguration( pDisplayManager.nativeData );
	}

	void SysDisplayManager::_sysRelease() noexcept
	{
	}

	void SysDisplayManager::_sysQueryDisplaySize( SysDisplaySize & pDisplaySize ) const
	{
	}

	void SysDisplayManager::_sysQueryMinWindowSize( SysDisplaySize & pMinWindowSize ) const
	{
	}

	void nativeDisplayManagerInitialize( SysDisplayManager & pDisplayManager )
	{
	}

	void SysDisplayManagerImplProxy::nativeReleaseDisplayManager( SysDisplayManager & pDisplayManager )
	{
		_x11FreeDisplayManagerConfiguration( pDisplayManager.nativeData );

		x11XRRReleaseDriverState( pDisplayManager.nativeData );

		pDisplayManager.nativeData->display = nullptr;
		pDisplayManager.nativeData->rootWindow = cvXIDNone;

		auto * scNativeData = pDisplayManager.systemContext->nativeData;
		scNativeData->display = nullptr;
		scNativeData->screenIndex = -1;
		scNativeData->rootWindow = cvXIDNone;
	}

	void SysDisplayManagerImplProxy::nativeQueryDisplaySize( const SysDisplayManager & pDisplayManager, SysDisplaySize & pDisplaySize )
	{
		// Ideally, we have XRR available here and can fetch the data from XRRMonitorInfo structure.
		// With that approach, we can tell precisely the size of the default monitor (not the entire virtual screen).
		auto * xrrDefaultMonitorInfo = pDisplayManager.nativeData->xrrDefaultMonitorInfo;

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
			auto displayWidth = XDisplayWidth( pDisplayManager.nativeData->display, pDisplayManager.nativeData->screenIndex );
			auto displayHeight = XDisplayHeight( pDisplayManager.nativeData->display, pDisplayManager.nativeData->screenIndex );
			pDisplaySize.x = static_cast<uint32>( displayWidth );
			pDisplaySize.y = static_cast<uint32>( displayHeight );
		}
	}

	void SysDisplayManagerImplProxy::nativeQueryMinWindowSize( const SysDisplayManager & pDisplayManager, SysDisplaySize & pMinWindowSize )
	{
		pMinWindowSize.x = 0u;
		pMinWindowSize.y = 0u;
	}


	void _x11EnumerateDisplayManagerConfiguration( SysDisplayManagerNativeData * pNativeData )
	{
		for ( size_t monitorIndex = 0; monitorIndex < pNativeData->xrrMonitorsNum; ++monitorIndex )
		{
			auto & monitorInfo = pNativeData->xrrMonitorList[monitorIndex];
			if ( monitorInfo.primary != 0 )
			{
				pNativeData->xrrDefaultMonitorInfo = &monitorInfo;
			}
		}
	}

	void _x11FreeDisplayManagerConfiguration( SysDisplayManagerNativeData * pNativeData )
	{
		pNativeData->xrrDefaultMonitorInfo = nullptr;
	}

}
#endif
