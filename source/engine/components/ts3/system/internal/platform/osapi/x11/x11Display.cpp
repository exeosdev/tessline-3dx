
#include <ts3/system/displayNative.h>

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_X11 )
namespace ts3::system
{

    bool nativeX11CheckColorFormatSupport( XDisplay pXDisplay, int pScreenIndex, EColorFormat pColorFormat )
    {
        const auto & colorFormatDesc = vsxGetDescForColorFormat( pColorFormat );

        XVisualInfo visualInfo;
        if ( XMatchVisualInfo( pXDisplay, pScreenIndex, colorFormatDesc.size, TrueColor, &visualInfo ) == True )
        {
            return true;
        }
        return false;
    }

    void DisplayManager::_nativeConstructor()
    {
        auto & xSessionData = nativeX11GetXSessionData( *mSysContext );

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

        mInternal->nativeDataPriv.setSessionData( xSessionData );
        mInternal->nativeDataPriv.screenDepth = static_cast<uint16>( screenDepth );
    }

    void DisplayManager::_nativeDestructor() noexcept
    {
        mInternal->nativeDataPriv.screenDepth = 0u;
        mInternal->nativeDataPriv.resetSessionData();
    }

    void DisplayManager::_nativeQueryMinWindowSize( DisplaySize & pOutSize ) const
    {
        pOutSize.x = 0u;
        pOutSize.y = 0u;
    }

    void DisplayManager::_nativeQueryDefaultDisplaySize( DisplaySize & pOutSize ) const
    {
        auto & xSessionData = nativeX11GetXSessionData( *mSysContext );

        // Ideally, we have XRR available here and can fetch the data from XRRMonitorInfo structure.
        // With that approach, we can tell precisely the size of the default monitor (not the entire virtual screen).
        // auto * xrrDefaultMonitorInfo = mInternal->nativeDataPriv.xrrDefaultMonitorInfo;

        // if( xrrDefaultMonitorInfo != nullptr )
        // {
        //     // Monitor info for the default monitor. Width and height are the exact dimensions, in pixels.
        //     pOutSize.x = static_cast<uint32>( xrrDefaultMonitorInfo->width );
        //     pOutSize.y = static_cast<uint32>( xrrDefaultMonitorInfo->height );
        // }
        // else
        {
            // auto & driverNativeData = dsmGetObjectNativeDataGeneric( *this );
            // With "raw" X11 only, there is really no good way to get what we want... *screen* refers to the virtual
            // area which (in case of a usual multi-monitor setup) may contain more than one monitor. E.g. for my setup
            // with two 27" 1440p monitors, all X11 functions return the screen size as 5120 x 1440 pixels.
            // TODO: can we somehow get the size of a current default monitor without using XRR/XF86 APIs?
            auto displayWidth = XDisplayWidth( xSessionData.display,
                                               xSessionData.screenIndex );

            auto displayHeight = XDisplayHeight( xSessionData.display,
                                                 xSessionData.screenIndex );

            pOutSize.x = static_cast<uint32>( displayWidth );
            pOutSize.y = static_cast<uint32>( displayHeight );
        }
    }


    void DisplayDriverGeneric::_nativeConstructor()
    {
        auto & xSessionData = nativeX11GetXSessionData( *mSysContext );

        auto & driverNativeData = dsmGetObjectNativeDataGeneric( *this );
        driverNativeData.setSessionData( xSessionData );
        driverNativeData.screenDepth = mDisplayManager->mNativeData->screenDepth;
        
        if( driverNativeData.xrrVersion == cvVersionUnknown )
        {
            int xrrVersionMajor = 0;
            int xrrVersionMinor = 0;

            if ( XRRQueryVersion( xSessionData.display, &xrrVersionMajor, &xrrVersionMinor ) == False )
            {
                ts3Throw( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER ); // ExsThrowException( EXC_Not_Supported );
            }

            driverNativeData.xrrVersion.major = static_cast<uint16>( xrrVersionMajor );
            driverNativeData.xrrVersion.major = static_cast<uint16>( xrrVersionMinor );
        }

        //
        driverNativeData.xrrScreenResources = XRRGetScreenResources( xSessionData.display,
                                                                     xSessionData.rootWindowXID );
        if ( driverNativeData.xrrScreenResources == nullptr )
        {
            ts3Throw( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER ); // ExsThrowException( EXC_Internal_Error );
        }

        //
        driverNativeData.xrrMonitorList = XRRGetMonitors( xSessionData.display,
                                                          xSessionData.rootWindowXID,
                                                          False,
                                                          &( driverNativeData.xrrMonitorsNum ) );

        if ( ( driverNativeData.xrrMonitorList == nullptr ) || ( driverNativeData.xrrMonitorsNum <= 0 ) )
        {
            ts3Throw( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER ); // ExsThrowException( EXC_Internal_Error );
        }

        for ( int monitorIndex = 0; monitorIndex < driverNativeData.xrrMonitorsNum; ++monitorIndex )
        {
            auto & monitorInfo = driverNativeData.xrrMonitorList[monitorIndex];
            if ( monitorInfo.primary != 0 )
            {
                driverNativeData.xrrDefaultMonitorInfo = &monitorInfo;
            }
        }

        for( int xrrModeIndex = 0; xrrModeIndex < driverNativeData.xrrScreenResources->nmode; ++xrrModeIndex )
        {
            auto & xrrModeInfo = driverNativeData.xrrScreenResources->modes[xrrModeIndex];
            driverNativeData.xrrModeInfoMap[xrrModeInfo.id] = &xrrModeInfo;
        }
    }

    void DisplayDriverGeneric::_nativeDestructor() noexcept
    {
        auto & driverNativeData = dsmGetObjectNativeDataGeneric( *this );

        if ( driverNativeData.xrrScreenResources != nullptr )
        {
            XRRFreeScreenResources( driverNativeData.xrrScreenResources );
            driverNativeData.xrrScreenResources = nullptr;
        }

        if ( driverNativeData.xrrMonitorList != nullptr )
        {
            XRRFreeMonitors( driverNativeData.xrrMonitorList );
            driverNativeData.xrrMonitorList = nullptr;
            driverNativeData.xrrMonitorsNum = 0;
        }

        driverNativeData.screenDepth = 0;
        driverNativeData.resetSessionData();
    }

    void DisplayDriverGeneric::_drvEnumDisplayDevices()
    {
        auto & driverNativeData = dsmGetObjectNativeDataGeneric( *this );
        auto & xSessionData = driverNativeData.getSessionData();

        auto * adapterObject = addAdapter();
        auto & adapterNativeData = dsmGetObjectNativeDataGeneric( *adapterObject );
        adapterNativeData.setSessionData( xSessionData );
        auto & adapterDesc = dsmGetObjectDesc( *adapterObject );
        adapterDesc.name = "XRR Default Adapter";
        adapterDesc.vendorID = EDisplayAdapterVendorID::Unknown;
        adapterDesc.flags.set( E_DISPLAY_ADAPTER_FLAG_ACTIVE_BIT );
        adapterDesc.flags.set( E_DISPLAY_ADAPTER_FLAG_PRIMARY_BIT );

        for ( size_t monitorIndex = 0; monitorIndex < driverNativeData.xrrMonitorsNum; ++monitorIndex )
        {
            auto & monitorInfo = driverNativeData.xrrMonitorList[monitorIndex];
            //char * monitorName = XGetAtomName( xSessionData.display, monitorInfo.name );

            // Iterate over monitor's outputs and find the one used by the system.
            for ( int monitorOutputIndex = 0; monitorOutputIndex < monitorInfo.noutput; ++monitorOutputIndex )
            {
                auto monitorOutputID = monitorInfo.outputs[monitorOutputIndex];
                auto * monitorOutputInfo = XRRGetOutputInfo( xSessionData.display,
                                                             driverNativeData.xrrScreenResources,
                                                             monitorOutputID );

                if ( monitorOutputInfo )
                {
                    if ( ( monitorInfo.width != 0 ) && ( monitorInfo.height != 0 ) )
                    {
                        auto * outputObject = addOutput( *adapterObject );
                        auto & outputNativeData = dsmGetObjectNativeDataGeneric( *outputObject );
                        outputNativeData.setSessionData( xSessionData );
                        outputNativeData.xrrOutputID = monitorOutputID;
                        outputNativeData.xrrCrtcID = monitorOutputInfo->crtc;
                        auto & outputDesc = dsmGetObjectDesc( *outputObject );
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

    void DisplayDriverGeneric::_drvEnumVideoModes( DisplayOutput & pOutput, EColorFormat pColorFormat )
    {
        if( ( pColorFormat != EColorFormat::B8G8R8 ) && ( pColorFormat != EColorFormat::B8G8R8A8 ) )
        {
            return;
        }

        auto & driverNativeData = dsmGetObjectNativeDataGeneric( *this );
        auto & outputNativeData = dsmGetObjectNativeDataGeneric( pOutput );
        auto & xSessionData = outputNativeData.getSessionData();

        if( !nativeX11CheckColorFormatSupport( xSessionData.display, xSessionData.screenIndex, pColorFormat ) )
        {
            return;
        }

        auto * outputInfo = XRRGetOutputInfo( xSessionData.display,
                                              driverNativeData.xrrScreenResources,
                                              outputNativeData.xrrOutputID );
        if ( outputInfo == nullptr )
        {
            ts3Throw( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
        }

        if ( outputInfo->modes != nullptr )
        {
            dsm_video_settings_hash_t lastSettingsHash = CX_DSM_VIDEO_SETTINGS_HASH_INVALID;

            for ( size_t modeIndex = 0; modeIndex < outputInfo->nmode; ++modeIndex )
            {
                auto outputModeID = outputInfo->modes[modeIndex];
                auto displayModeInfoPtrIter = driverNativeData.xrrModeInfoMap.find( outputModeID );
                if ( displayModeInfoPtrIter == driverNativeData.xrrModeInfoMap.end() )
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

                auto * videoModeObject = addVideoMode( pOutput, pColorFormat );
                auto & videoModeNativeData = dsmGetObjectNativeDataGeneric( *videoModeObject );
                videoModeNativeData.setSessionData( xSessionData );
                videoModeNativeData.xrrModeID = displayModeInfo.id;
                videoModeNativeData.xrrModeInfo = &displayModeInfo;
                auto & videoModeDesc = dsmGetObjectDesc( *videoModeObject );
                videoModeDesc.settings = videoSettings;
                videoModeDesc.settingsHash = settingsHash;

                lastSettingsHash = settingsHash;
            }
        }

        XRRFreeOutputInfo( outputInfo );
    }

    EColorFormat DisplayDriverGeneric::_drvQueryDefaultSystemColorFormat() const
    {
        return EColorFormat::B8G8R8A8;
    }

} // namespace ts3::system
#endif // TS3_PCL_TARGET_SYSAPI_X11
