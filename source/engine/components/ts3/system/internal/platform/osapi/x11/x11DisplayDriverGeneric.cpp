
#include <ts3/system/displayDriver.h>
#include <ts3/system/displayManager.h>

namespace ts3
{

	void _x11EnumerateXRRDriverConfiguration( SysDisplayDriverNativeDataGeneric * pDriverNativeData );
	void _x11FreeXRRDriverConfiguration( SysDisplayDriverNativeDataGeneric * pDriverNativeData );

	void SysDisplayDriverGeneric::_sysInitialize()
	{
		if( mDisplayManager->mNativeData.xrrScreenResources == nullptr )
		{
			throw 0;
		}

		mNativeData.generic->xrrVersion = mDisplayManager->mNativeData.xrrVersion;
		mNativeData.generic->display = mDisplayManager->mNativeData.display;
		mNativeData.generic->rootWindow = mDisplayManager->mNativeData.rootWindow;
		mNativeData.generic->xrrScreenResources = mDisplayManager->mNativeData.xrrScreenResources;
		mNativeData.generic->xrrMonitorList = mDisplayManager->mNativeData.xrrMonitorList;
		mNativeData.generic->xrrMonitorsNum = mDisplayManager->mNativeData.xrrMonitorsNum;

		_x11EnumerateXRRDriverConfiguration( mNativeData.generic );
	}

	void SysDisplayDriverGeneric::_sysRelease()
	{
		_x11FreeXRRDriverConfiguration( mNativeData.generic );

		mNativeData.generic->xrrVersion = cvVersionUnknown;
		mNativeData.generic->display = nullptr;
		mNativeData.generic->rootWindow = cvXIDNone;
		mNativeData.generic->xrrScreenResources = nullptr;
		mNativeData.generic->xrrMonitorList = nullptr;
		mNativeData.generic->xrrMonitorsNum = 0;
	}

	void SysDisplayDriverGeneric::_sysResetInternalState()
	{
		_x11FreeXRRDriverConfiguration( mNativeData.generic );
		_x11EnumerateXRRDriverConfiguration( mNativeData.generic );
	}

	void SysDisplayDriverGeneric::_sysEnumAdapterList()
	{
		auto * defaultAdapter = registerAdapter();
		defaultAdapter->adapterDesc.index = 0u;
		defaultAdapter->adapterDesc.name = "DefaultAdapter";
		defaultAdapter->adapterDesc.vendorID = ESysDsmAdapterVendorID::Unknown;
		defaultAdapter->adapterDesc.flags = E_SYS_DSM_ADAPTER_FLAG_PRIMARY_BIT;
	}

	void SysDisplayDriverGeneric::_sysEnumOutputList( SysDsmAdapter & pAdapter )
	{
		auto * xrrDriverData = mNativeData.generic;

		for ( int monitorIndex = 0; monitorIndex < xrrDriverData->xrrMonitorsNum; ++monitorIndex )
		{
			// Output list is fetched during the initialization phase.
			auto & monitorInfo = xrrDriverData->xrrMonitorList[monitorIndex];
			// Iterate over monitor's outputs and find the one used by the system.
			for ( int monitorOutputIndex = 0; monitorOutputIndex < monitorInfo.noutput; ++monitorOutputIndex )
			{
				RROutput xrrOutputID = monitorInfo.outputs[monitorOutputIndex];
				if ( auto * xrrOutputInfo = XRRGetOutputInfo( xrrDriverData->display, xrrDriverData->xrrScreenResources, xrrOutputID ) )
				{
					if ( ( monitorInfo.width != 0 ) && ( monitorInfo.height != 0 ) )
					{
						auto * outputInfo = registerOutput( pAdapter );
						outputInfo->nativeData.generic->xrrOutputID = xrrOutputID;
						outputInfo->nativeData.generic->xrrCrtcID = xrrOutputInfo->crtc;
						outputInfo->outputDesc.flags = 0u;
						outputInfo->outputDesc.name = XGetAtomName( xrrDriverData->display, monitorInfo.name );
						outputInfo->outputDesc.index = monitorOutputIndex;
						outputInfo->outputDesc.screenRect.offset.x = monitorInfo.x;
						outputInfo->outputDesc.screenRect.offset.y = monitorInfo.y;
						outputInfo->outputDesc.screenRect.size.x = static_cast<uint32>( monitorInfo.width );
						outputInfo->outputDesc.screenRect.size.y = static_cast<uint32>( monitorInfo.height );

						if ( monitorInfo.primary != 0 )
						{
							outputInfo->outputDesc.flags.set( E_SYS_DSM_OUTPUT_FLAG_PRIMARY_BIT );
						}
					}

					XRRFreeOutputInfo( xrrOutputInfo );
				}
			}
		}
	}

	void SysDisplayDriverGeneric::_sysEnumVideoModeList( SysDsmOutput & pOutput, SysColorFormat pFormat )
	{
		auto * xrrDriverData = mNativeData.generic;
		auto * xrrOutputData = pOutput.nativeData.generic;

		if( auto * xrrOutputInfo = XRRGetOutputInfo( xrrDriverData->display, xrrDriverData->xrrScreenResources, xrrOutputData->xrrOutputID ) )
		{
			if ( xrrOutputInfo->modes != nullptr )
			{
				for ( int modeIndex = 0; modeIndex < xrrOutputInfo->nmode; ++modeIndex )
				{
					RRMode xrrModeID = xrrOutputInfo->modes[modeIndex];

					auto displayModeInfoPtrIter = xrrDriverData->xrrDisplayModeInfoMap.find( xrrModeID );
					if ( displayModeInfoPtrIter == xrrDriverData->xrrDisplayModeInfoMap.end() )
					{
						continue;
					}

					auto * xrrModeInfo = displayModeInfoPtrIter->second;

					auto rateVHTotal = xrrModeInfo->hTotal * xrrModeInfo->vTotal;
					auto refreshRate = 0U;

					if ( rateVHTotal != 0 )
					{
						refreshRate = static_cast<uint32_t>( std::round( ( double )xrrModeInfo->dotClock / ( double )rateVHTotal ) );
					}

					auto * displayMode = registerVideoMode( pOutput, pFormat );
					displayMode->nativeData.generic->xrrModeID = xrrModeID;
					displayMode->nativeData.generic->xrrModeInfo = xrrModeInfo;
					displayMode->modeDesc.index = static_cast<sys_dsm_index_t>( modeIndex );
					displayMode->modeDesc.format = pFormat;
					displayMode->modeDesc.settings.resolution.x = xrrModeInfo->width;
					displayMode->modeDesc.settings.resolution.y = xrrModeInfo->height;
					displayMode->modeDesc.settings.refreshRate = static_cast<uint16>( refreshRate );
					displayMode->modeDesc.settings.flags = E_SYS_DSM_VIDEO_SETTINGS_FLAG_SCAN_PROGRESSIVE_BIT;

					if ( ( xrrModeInfo->modeFlags & RR_DoubleScan ) != 0 )
					{
						displayMode->modeDesc.settings.refreshRate *= 2;
						displayMode->modeDesc.settings.flags.unset( E_SYS_DSM_VIDEO_SETTINGS_FLAG_SCAN_PROGRESSIVE_BIT );
					}

					if ( ( xrrModeInfo->modeFlags & RR_Interlace ) != 0 )
					{
						displayMode->modeDesc.settings.refreshRate /= 2;
						displayMode->modeDesc.settings.flags.set( E_SYS_DSM_VIDEO_SETTINGS_FLAG_SCAN_INTERLACED_BIT );
					}
				}
			}

			XRRFreeOutputInfo( xrrOutputInfo );
		}
	}


	void _x11EnumerateXRRDriverConfiguration( SysDisplayDriverNativeDataGeneric * pDriverNativeData )
	{
		for ( int modeIndex = 0; modeIndex < pDriverNativeData->xrrScreenResources->nmode; ++modeIndex )
		{
			auto & modeInfo = pDriverNativeData->xrrScreenResources->modes[modeIndex];
			pDriverNativeData->xrrDisplayModeInfoMap[modeInfo.id] = &modeInfo;
		}
	}

	void _x11FreeXRRDriverConfiguration( SysDisplayDriverNativeDataGeneric * pDriverNativeData )
	{
		pDriverNativeData->xrrDisplayModeInfoMap.clear();
	}

}
