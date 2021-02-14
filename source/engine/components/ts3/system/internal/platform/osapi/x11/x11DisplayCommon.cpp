
#include "x11DisplayCommon.h"

namespace ts3
{

	void x11XRRInitializeDriverState( XRRCommonDriverData * pXRRDriverData )
	{
		if( pXRRDriverData->xrrVersion == cvVersionUnknown )
		{
			int xrrVersionMajor = 0;
			int xrrVersionMinor = 0;

			if ( XRRQueryVersion( pXRRDriverData->display, &xrrVersionMajor, &xrrVersionMinor ) == False )
			{
				throw 0; // ExsThrowException( EXC_Not_Supported );
			}

			pXRRDriverData->xrrVersion.major = static_cast<uint16>( xrrVersionMajor );
			pXRRDriverData->xrrVersion.major = static_cast<uint16>( xrrVersionMinor );
		}

		x11XRREnumScreenResources( pXRRDriverData );
	}

	void x11XRRReleaseDriverState( XRRCommonDriverData * pXRRDriverData )
	{
		x11XRRFreeScreenResources( pXRRDriverData );
	}

	void x11XRREnumScreenResources( XRRCommonDriverData * pXRRDriverData )
	{
		ts3DebugAssert( pXRRDriverData->xrrScreenResources == nullptr );
		ts3DebugAssert( pXRRDriverData->xrrMonitorList == nullptr );

		//
		pXRRDriverData->xrrScreenResources = XRRGetScreenResources( pXRRDriverData->display, pXRRDriverData->rootWindow );
		if ( pXRRDriverData->xrrScreenResources == nullptr )
		{
			throw 0; // ExsThrowException( EXC_Internal_Error );
		}

		//
		pXRRDriverData->xrrMonitorList = XRRGetMonitors( pXRRDriverData->display, pXRRDriverData->rootWindow, False, &( pXRRDriverData->xrrMonitorsNum ) );
		if ( ( pXRRDriverData->xrrMonitorList == nullptr ) || ( pXRRDriverData->xrrMonitorsNum <= 0 ) )
		{
			throw 0; // ExsThrowException( EXC_Internal_Error );
		}
	}

	void x11XRRFreeScreenResources( XRRCommonDriverData * pXRRDriverData )
	{
		if ( pXRRDriverData->xrrScreenResources != nullptr )
		{
			XRRFreeScreenResources( pXRRDriverData->xrrScreenResources );
			pXRRDriverData->xrrScreenResources = nullptr;
		}

		if ( pXRRDriverData->xrrMonitorList != nullptr )
		{
			XRRFreeMonitors( pXRRDriverData->xrrMonitorList );
			pXRRDriverData->xrrMonitorList = nullptr;
			pXRRDriverData->xrrMonitorsNum = 0;
		}
	}

	void x11XRRResetDriverState( XRRCommonDriverData * pXRRDriverData )
	{
		x11XRRFreeScreenResources( pXRRDriverData );
		x11XRREnumScreenResources( pXRRDriverData );
	}

}
