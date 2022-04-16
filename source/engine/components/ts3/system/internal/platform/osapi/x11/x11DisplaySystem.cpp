
#include "x11DisplaySystem.h"

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_X11 )
namespace ts3::system
{

	namespace platform
	{

		bool _x11CheckColorFormatSupport( XDisplay pXDisplay, int pScreenIndex, EColorFormat pColorFormat );

	}


	X11DisplayManager::X11DisplayManager( SysContextHandle pSysContext )
	: X11NativeObject( std::move( pSysContext ) )
	{
		_initializeX11DisplayManagerState();
	}

	X11DisplayManager::~X11DisplayManager() noexcept
	{
		_releaseX11DisplayManagerState();
	}

	void X11DisplayManager::_initializeX11DisplayManagerState()
	{
		auto & xSessionData = platform::x11GetXSessionData( *this );

		// Get default screen depth for the X screen.
		auto screenDepth = XDefaultDepth( xSessionData.display, xSessionData.screenIndex );

		if ( screenDepth < 32 )
		{
			XVisualInfo visualInfo;
			if ( XMatchVisualInfo( xSessionData.display, xSessionData.screenIndex, 32, TrueColor, &visualInfo ) == True )
			{
				screenDepth = 32;
			}
		}

		mNativeData.screenDepth = screenDepth;

		if( mNativeData.xrrVersion == CX_VERSION_UNKNOWN )
		{
			int xrrVersionMajor = 0;
			int xrrVersionMinor = 0;

			if ( XRRQueryVersion( xSessionData.display, &xrrVersionMajor, &xrrVersionMinor ) == False )
			{
				return;
			}

			mNativeData.xrrVersion.major = static_cast<uint16>( xrrVersionMajor );
			mNativeData.xrrVersion.major = static_cast<uint16>( xrrVersionMinor );
		}

		if ( auto * xrrScreenResources = XRRGetScreenResources( xSessionData.display, xSessionData.rootWindowXID ) )
		{
			int xrrMonitorsNum = 0;
			auto * xrrMonitorList = XRRGetMonitors( xSessionData.display, xSessionData.rootWindowXID, False, &xrrMonitorsNum );

			if ( xrrMonitorList && ( xrrMonitorsNum > 0 ) )
			{
				for ( int monitorIndex = 0; monitorIndex < xrrMonitorsNum; ++monitorIndex )
				{
					auto & xrrMonitorInfo = xrrMonitorList[monitorIndex];
					if ( xrrMonitorInfo.primary != 0 )
					{
						mNativeData.xrrDefaultMonitorInfo = xrrMonitorInfo;
						//break;
					}
				}

				XRRFreeMonitors( xrrMonitorList );
			}

			XRRFreeScreenResources( xrrScreenResources );
		}
	}

	void X11DisplayManager::_releaseX11DisplayManagerState()
	{
		mNativeData.screenDepth = 0u;
	}

	DisplayDriverHandle X11DisplayManager::_nativeCreateDisplayDriver()
	{
		return createSysObject<X11DisplayDriver>( getHandle<X11DisplayManager>() );
	}

	void X11DisplayManager::_nativeQueryDefaultDisplayOffset( DisplayOffset & pOutOffset ) const
	{
		if( mNativeData.xrrVersion != CX_VERSION_UNKNOWN )
		{
			pOutOffset.x = static_cast<int32>( mNativeData.xrrDefaultMonitorInfo.x );
			pOutOffset.y = static_cast<int32>( mNativeData.xrrDefaultMonitorInfo.y );
		}
		else
		{
			pOutOffset.x = 0;
			pOutOffset.y = 0;
		}
	}

	void X11DisplayManager::_nativeQueryDefaultDisplaySize( DisplaySize & pOutSize ) const
	{
		if( mNativeData.xrrVersion != CX_VERSION_UNKNOWN )
		{
			pOutSize.x = static_cast<uint32>( mNativeData.xrrDefaultMonitorInfo.width );
			pOutSize.y = static_cast<uint32>( mNativeData.xrrDefaultMonitorInfo.height );
		}
		else
		{
			auto & xSessionData = platform::x11GetXSessionData( *this );

			const auto displayWidth = XDisplayWidth( xSessionData.display, xSessionData.screenIndex );
			const auto displayHeight = XDisplayHeight( xSessionData.display, xSessionData.screenIndex );

			pOutSize.x = static_cast<uint32>( displayWidth );
			pOutSize.y = static_cast<uint32>( displayHeight );
		}
	}

	void X11DisplayManager::_nativeQueryMinWindowSize( DisplaySize & pOutSize ) const
	{
		pOutSize.x = 0u;
		pOutSize.y = 0u;
	}


	X11DisplayDriver::X11DisplayDriver( X11DisplayManagerHandle pDisplayManager )
	: X11NativeObject( std::move( pDisplayManager ), EDisplayDriverType::Generic )
	{
		_initializeX11DisplayDriverState();
	}

	X11DisplayDriver::~X11DisplayDriver() noexcept
	{
		_releaseX11DisplayDriverState();
	}

	void X11DisplayDriver::_initializeX11DisplayDriverState()
	{
		auto & xSessionData = platform::x11GetXSessionData( *this );

		//
		mNativeData.xrrScreenResources = XRRGetScreenResources( xSessionData.display, xSessionData.rootWindowXID );
		if ( mNativeData.xrrScreenResources == nullptr )
		{
			ts3Throw( E_EXC_DEBUG_PLACEHOLDER );
		}

		//
		mNativeData.xrrMonitorList = XRRGetMonitors( xSessionData.display,
		                                             xSessionData.rootWindowXID,
		                                             False,
		                                             &( mNativeData.xrrMonitorsNum ) );

		if ( ( mNativeData.xrrMonitorList == nullptr ) || ( mNativeData.xrrMonitorsNum <= 0 ) )
		{
			ts3Throw( E_EXC_DEBUG_PLACEHOLDER ); // ExsThrowException( EXC_Internal_Error );
		}

		for ( int monitorIndex = 0; monitorIndex < mNativeData.xrrMonitorsNum; ++monitorIndex )
		{
			auto & monitorInfo = mNativeData.xrrMonitorList[monitorIndex];
			if ( monitorInfo.primary != 0 )
			{
				mNativeData.xrrDefaultMonitorInfo = &monitorInfo;
			}
		}

		for( int xrrModeIndex = 0; xrrModeIndex < mNativeData.xrrScreenResources->nmode; ++xrrModeIndex )
		{
			auto & xrrModeInfo = mNativeData.xrrScreenResources->modes[xrrModeIndex];
			mNativeData.xrrModeInfoMap[xrrModeInfo.id] = &xrrModeInfo;
		}
	}
	
	void X11DisplayDriver::_releaseX11DisplayDriverState()
	{
		if ( mNativeData.xrrScreenResources != nullptr )
		{
			XRRFreeScreenResources( mNativeData.xrrScreenResources );
			mNativeData.xrrScreenResources = nullptr;
		}

		if ( mNativeData.xrrMonitorList != nullptr )
		{
			XRRFreeMonitors( mNativeData.xrrMonitorList );
			mNativeData.xrrMonitorList = nullptr;
			mNativeData.xrrMonitorsNum = 0;
		}

		mNativeData.screenDepth = 0;
		mNativeData.resetSessionData();
	}

	void X11DisplayDriver::_nativeEnumDisplayDevices()
	{
		auto & xSessionData = platform::x11GetXSessionData( *this );

		auto adapterObject = createAdapter<X11DisplayAdapter>( *this );

		auto & adapterDesc = getAdapterDescInternal( *adapterObject );
		adapterDesc.name = "XRR Default Adapter";
		adapterDesc.vendorID = EDisplayAdapterVendorID::Unknown;
		adapterDesc.flags.set( E_DISPLAY_ADAPTER_FLAG_ACTIVE_BIT );
		adapterDesc.flags.set( E_DISPLAY_ADAPTER_FLAG_PRIMARY_BIT );

		for ( size_t monitorIndex = 0; monitorIndex < mNativeData.xrrMonitorsNum; ++monitorIndex )
		{
			auto & monitorInfo = mNativeData.xrrMonitorList[monitorIndex];
			//char * monitorName = XGetAtomName( xSessionData.display, monitorInfo.name );

			// Iterate over monitor's outputs and find the one used by the system.
			for ( int monitorOutputIndex = 0; monitorOutputIndex < monitorInfo.noutput; ++monitorOutputIndex )
			{
				auto monitorOutputID = monitorInfo.outputs[monitorOutputIndex];
				auto * monitorOutputInfo = XRRGetOutputInfo( xSessionData.display,
				                                             mNativeData.xrrScreenResources,
				                                             monitorOutputID );

				if ( monitorOutputInfo )
				{
					if ( ( monitorInfo.width != 0 ) && ( monitorInfo.height != 0 ) )
					{
						auto outputObject = createOutput<X11DisplayOutput>( *adapterObject );
						outputObject->mNativeData.xrrOutputID = monitorOutputID;
						outputObject->mNativeData.xrrCrtcID = monitorOutputInfo->crtc;

						auto & outputDesc = getOutputDescInternal( *outputObject );
						outputDesc.name = monitorOutputInfo->name;
						outputDesc.screenRect.offset.x = monitorInfo.x;
						outputDesc.screenRect.offset.y = monitorInfo.y;
						outputDesc.screenRect.size.x = monitorInfo.width;
						outputDesc.screenRect.size.y = monitorInfo.height;

						if ( monitorOutputInfo->connection == RR_Connected )
						{
							outputDesc.flags.set( E_DISPLAY_OUTPUT_FLAG_ACTIVE_BIT );
						}

						if ( monitorInfo.primary != 0 )
						{
							outputDesc.flags.set( E_DISPLAY_OUTPUT_FLAG_PRIMARY_BIT );
						}
					}

					XRRFreeOutputInfo( monitorOutputInfo );
				}
			}
		}
	}

	void X11DisplayDriver::_nativeEnumVideoModes( DisplayOutput & pOutput, EColorFormat pColorFormat )
	{
		if( ( pColorFormat != EColorFormat::B8G8R8 ) && ( pColorFormat != EColorFormat::B8G8R8A8 ) )
		{
			return;
		}

		auto & xSessionData = platform::x11GetXSessionData( *this );
		auto * outputX11 = pOutput.queryInterface<X11DisplayOutput>();

		if( !platform::_x11CheckColorFormatSupport( xSessionData.display, xSessionData.screenIndex, pColorFormat ) )
		{
			return;
		}

		auto * outputInfo = XRRGetOutputInfo( xSessionData.display,
		                                      mNativeData.xrrScreenResources,
		                                      outputX11->mNativeData.xrrOutputID );
		if ( outputInfo == nullptr )
		{
			ts3Throw( E_EXC_DEBUG_PLACEHOLDER );
		}

		if ( outputInfo->modes != nullptr )
		{
			dsm_video_settings_hash_t lastSettingsHash = CX_DSM_VIDEO_SETTINGS_HASH_INVALID;

			for ( size_t modeIndex = 0; modeIndex < outputInfo->nmode; ++modeIndex )
			{
				auto outputModeID = outputInfo->modes[modeIndex];
				auto displayModeInfoPtrIter = mNativeData.xrrModeInfoMap.find( outputModeID );
				if ( displayModeInfoPtrIter == mNativeData.xrrModeInfoMap.end() )
				{
					continue;
				}

				auto & displayModeInfo = *( displayModeInfoPtrIter->second );

				if ( ( displayModeInfo.modeFlags & ( RR_DoubleScan | RR_PixelMultiplex ) ) != 0 )
				{
					continue;
				}

				auto rateVHTotal = displayModeInfo.hTotal * displayModeInfo.vTotal;
				auto refreshRate = 0U;

				if ( rateVHTotal != 0 )
				{
					refreshRate = static_cast<uint32_t>( std::round( ( double )displayModeInfo.dotClock / ( double )rateVHTotal ) );
				}

				DisplayVideoSettings videoSettings;
				videoSettings.resolution.x = static_cast<uint32>( displayModeInfo.width );
				videoSettings.resolution.y = static_cast<uint32>( displayModeInfo.height );
				videoSettings.refreshRate = static_cast<uint16>( refreshRate );

				if ( ( displayModeInfo.modeFlags & RR_Interlace ) != 0 )
				{
					videoSettings.flags.set( E_DISPLAY_VIDEO_SETTINGS_FLAG_SCAN_INTERLACED_BIT );
				}
				else
				{
					videoSettings.flags.set( E_DISPLAY_VIDEO_SETTINGS_FLAG_SCAN_PROGRESSIVE_BIT );
				}

				auto settingsHash = dsmComputeVideoSettingsHash( pColorFormat, videoSettings );
				if( settingsHash == lastSettingsHash )
				{
					continue;
				}

				auto videoModeObject = createVideoMode<X11DisplayVideoMode>( *outputX11, pColorFormat );
				videoModeObject->mNativeData.xrrModeID = displayModeInfo.id;
				videoModeObject->mNativeData.xrrModeInfo = &displayModeInfo;

				auto & videoModeDesc = getVideoModeDescInternal( *videoModeObject );
				videoModeDesc.settings = videoSettings;
				videoModeDesc.settingsHash = settingsHash;

				lastSettingsHash = settingsHash;
			}
		}

		XRRFreeOutputInfo( outputInfo );
	}

	EColorFormat X11DisplayDriver::_nativeQueryDefaultSystemColorFormat() const
	{
		return EColorFormat::B8G8R8A8;
	}


	namespace platform
	{

		bool _x11CheckColorFormatSupport( XDisplay pXDisplay, int pScreenIndex, EColorFormat pColorFormat )
		{
			const auto & colorFormatDesc = vsxGetDescForColorFormat( pColorFormat );

			XVisualInfo visualInfo;
			if ( XMatchVisualInfo( pXDisplay, pScreenIndex, colorFormatDesc.size, TrueColor, &visualInfo ) == True )
			{
				return true;
			}
			return false;
		}

	}

} // namespace ts3::system
#endif // TS3_PCL_TARGET_SYSAPI_X11
