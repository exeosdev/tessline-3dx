
#include <ts3/system/displayNative.h>
#include <ts3/stdext/mapUtils.h>
#include <ts3/stdext/stringUtils.h>

namespace ts3::system
{

    static void _dxgiEnumAdapterOutputs( DisplayAdapter & pAdapter );

	static DXGI_FORMAT _dxgiTranslateColorFormatToDXGIFormat( ColorFormat pColorFormat );


	void DisplayDriverDXGI::_initialize()
    {
        ts3DebugAssert( mInternal->nativeDataPriv.dxgi );
        auto & dxgiDriverData = *( mInternal->nativeDataPriv.dxgi );

        if( dxgiDriverData.dxgiFactory == nullptr )
        {
            Bitmask<UINT> dxgiFactoryCreateFlags = 0;
        #if ( TS3_DEBUG )
            dxgiFactoryCreateFlags.set( DXGI_CREATE_FACTORY_DEBUG );
        #endif

            ComPtr<IDXGIFactory2> dxgiFactory2;
            auto hResult = ::CreateDXGIFactory2( dxgiFactoryCreateFlags, IID_PPV_ARGS( &dxgiFactory2 ) );

            if( SUCCEEDED( hResult ) )
            {
                dxgiDriverData.dxgiFactory = dxgiFactory2;
            }
            else
            {
                ComPtr<IDXGIFactory1> dxgiFactory1;
                hResult = ::CreateDXGIFactory1( IID_PPV_ARGS( &dxgiFactory1 ) );

                if( FAILED( hResult ) )
                {
                    ts3ThrowAuto( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
                }

                dxgiDriverData.dxgiFactory = dxgiFactory1;
            }
        }
    }

    void DisplayDriverDXGI::_release()
    {}

    void DisplayDriverDXGI::_enumAdapterOutputs( DisplayAdapter & pAdapter )
    {
        auto & adapterNativeData = dsmGetObjectNativeDataDXGI( pAdapter );

        auto * dxgiAdapter = adapterNativeData.dxgiAdapter.Get();

        for( UINT outputIndex = 0u; ; ++outputIndex )
        {
            ComPtr<IDXGIOutput> dxgiOutput;
            auto hResult = dxgiAdapter->EnumOutputs( outputIndex, dxgiOutput.GetAddressOf() );

            // Exactly the same situation: if DXGI_ERROR_NOT_FOUND is returned - no more adapters to enumerate.
            if ( hResult == DXGI_ERROR_NOT_FOUND )
            {
                break;
            }

            // According to the docs, the only possible error is DXGI_ERROR_NOT_FOUND which is returned
            // for an adapter created with D3D_DRIVER_TYPE_WARP D3D device. Not an option here. Still...
            // https://docs.microsoft.com/en-us/windows/win32/api/dxgi/nf-dxgi-idxgiadapter-enumoutputs
            if ( FAILED( hResult ) )
            {
                ts3ThrowAuto( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
            }

            ComPtr<IDXGIOutput1> dxgiOutput1;
            hResult = dxgiOutput->QueryInterface( __uuidof( IDXGIOutput1 ), reinterpret_cast< void** >( dxgiOutput1.GetAddressOf() ) );

            // We require support for the DXGI version 1.1 (Windows 7 and newer). Required for DXGI_ADAPTER_FLAG member.
            if ( FAILED( hResult ) )
            {
                ts3ThrowAuto( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
            }

            DXGI_OUTPUT_DESC dxgiOutputDesc;
            hResult = dxgiOutput1->GetDesc( &dxgiOutputDesc );

            if ( FAILED( hResult ) )
            {
                ts3ThrowAuto( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
            }

            auto * outputObject = addOutput( pAdapter );
            auto & outputNativeData = dsmGetObjectNativeDataDXGI( *outputObject );
            auto & outputDesc = dsmGetObjectDesc( *outputObject );

            outputNativeData.dxgiOutput = dxgiOutput1;
            outputNativeData.dxgiOutputDesc = dxgiOutputDesc;
            outputDesc.name = strUtils::convertStringRepresentation<char>( dxgiOutputDesc.DeviceName );
            outputDesc.screenRect.offset.x = dxgiOutputDesc.DesktopCoordinates.left;
            outputDesc.screenRect.offset.y = dxgiOutputDesc.DesktopCoordinates.top;
            outputDesc.screenRect.size.x = dxgiOutputDesc.DesktopCoordinates.right - dxgiOutputDesc.DesktopCoordinates.left;
            outputDesc.screenRect.size.y = dxgiOutputDesc.DesktopCoordinates.bottom - dxgiOutputDesc.DesktopCoordinates.top;

            if( dxgiOutputDesc.AttachedToDesktop )
            {
                outputDesc.flags.set( E_DISPLAY_OUTPUT_FLAG_ACTIVE_BIT );
            }

            if( dxgiOutputDesc.Monitor )
            {
                // It is almost crazy we need to rely on the old Win32 API within a DXGI realm... but it seems
                // DXGI API does not expose the concept of a "primary (or default) output". Some apps rely on
                // the existence of a default output, so we make sure DXGI has it too.
                MONITORINFOEXA gdiMonitorInfo;
                gdiMonitorInfo.cbSize = sizeof( MONITORINFOEXA );

                if( ::GetMonitorInfoA( dxgiOutputDesc.Monitor, &gdiMonitorInfo ) != FALSE )
                {
                    if( makeBitmask( gdiMonitorInfo.dwFlags ).isSet( MONITORINFOF_PRIMARY ) )
                    {
                        outputDesc.flags.set( E_DISPLAY_OUTPUT_FLAG_PRIMARY_BIT );
                    }
                }
            }
        }
    }

	void DisplayDriverDXGI::_drvEnumDisplayDevices()
	{
	    auto * dxgiFactory = mInternal->nativeDataPriv.dxgi->dxgiFactory.Get();

	    for( UINT adapterIndex = 0u; ; ++adapterIndex )
	    {
	        ComPtr<IDXGIAdapter1> dxgiAdapter;
	        auto hResult = dxgiFactory->EnumAdapters1( adapterIndex, dxgiAdapter.GetAddressOf() );

	        // Adapters are returned in order, starting with index 0. When DXGI_ERROR_NOT_FOUND is returned,
	        // it means all valid adapters have been already queried and there is nothing else to get.
	        if ( hResult == DXGI_ERROR_NOT_FOUND )
	        {
	            break;
	        }

	        // According to the docs, the only possible error is DXGI_ERROR_INVALID_CALL which is returned
	        // if the second parameter is NULL (not possible in our case). Still - handle potential flaws.
	        // https://docs.microsoft.com/en-us/windows/win32/api/dxgi/nf-dxgi-idxgifactory1-enumadapters1
	        if ( FAILED( hResult ) )
	        {
	            ts3ThrowAuto( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
	        }

	        DXGI_ADAPTER_DESC1 dxgiAdapterDesc;
	        hResult = dxgiAdapter->GetDesc1( &dxgiAdapterDesc );

	        // Again - for a valid adapter, the only error can be E_INVALIDARG if the only parameter is NULL. Still...
	        // https://docs.microsoft.com/en-us/windows/win32/api/dxgi/nf-dxgi-idxgiadapter1-getdesc1
	        if ( FAILED( hResult ) )
	        {
	            ts3ThrowAuto( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
	        }

	        auto * adapterObject = addAdapter();
	        auto & adapterNativeData = dsmGetObjectNativeDataDXGI( *adapterObject );
	        auto & adapterDesc = dsmGetObjectDesc( *adapterObject );
	        
	        adapterNativeData.dxgiAdapter = dxgiAdapter;
	        adapterNativeData.dxgiAdapterDesc = dxgiAdapterDesc;
	        adapterDesc.name = strUtils::convertStringRepresentation<char>( dxgiAdapterDesc.Description );
	        adapterDesc.flags.set( E_DISPLAY_ADAPTER_FLAG_ACTIVE_BIT );

	        if( adapterIndex == 0 )
	        {
	            adapterDesc.flags.set( E_DISPLAY_ADAPTER_FLAG_PRIMARY_BIT );
	        }

	        auto dxgiAdapterFlags = makeBitmask( dxgiAdapterDesc.Flags );
	        if( dxgiAdapterFlags.isSet( DXGI_ADAPTER_FLAG_SOFTWARE ) )
	        {
	            adapterDesc.flags.set( E_DISPLAY_ADAPTER_FLAG_SOFTWARE_BIT );
	        }
	        else if( !dxgiAdapterFlags.isSet( DXGI_ADAPTER_FLAG_REMOTE ) )
	        {
	            adapterDesc.flags.set( E_DISPLAY_ADAPTER_FLAG_HARDWARE_BIT );
	        }

	        _enumAdapterOutputs( *adapterObject );
	    }
	}

	void DisplayDriverDXGI::_drvEnumVideoModes( DisplayOutput & pOutput, ColorFormat pColorFormat )
	{
	    auto * dxgiOutput = pOutput.mInternal->nativeDataPriv.dxgi->dxgiOutput.Get();

	    // This should never fail, i.e. the specified format should always yield a value which
	    // is known to the DXGI translation function. We had an issue with 'SystemNative' format,
	    // but this has been resolved by proxy function inside DisplayOutput::PrivateData struct.
	    // If there is a case the call below fails, make sure this function is ALWAYS called with
	    // a "resolved" color format (look for 'dsmResolveSystemColorFormat').
	    auto dxgiFormat = _dxgiTranslateColorFormatToDXGIFormat( pColorFormat );

	    if ( dxgiFormat == DXGI_FORMAT_UNKNOWN )
	    {
	        ts3ThrowAuto( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
	    }

	    UINT displayModesNum = 0;
	    // Passing nullptr as 'pDesc' causes the DXGI runtime to return the total number of supported modes for an adapter.
	    auto hResult = dxgiOutput->GetDisplayModeList( dxgiFormat, 0, &displayModesNum, nullptr );

	    if ( FAILED( hResult ) )
	    {
	        ts3ThrowAuto( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
	    }

	    std::vector<DXGI_MODE_DESC> dxgiModeList;
	    dxgiModeList.resize( displayModesNum );

	    // Query all supported modes and store them into the pre-allocated array.
	    // Any optional filtering and processing is done afterwards, it's more efficient to fetch this in one go.
	    hResult = dxgiOutput->GetDisplayModeList( dxgiFormat, 0, &displayModesNum, dxgiModeList.data() );

	    if ( FAILED( hResult ) )
	    {
	        ts3ThrowAuto( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
	    }

	    // We use hash-based comparison to filter out the same modes - at our level, we are only interested
	    // in settings and color format (and couple flags), so duplications can be safely removed here).
	    dsm_video_settings_hash_t lastSettingsHash = CX_DSM_VIDEO_SETTINGS_HASH_INVALID;

	    for ( auto & dxgiDisplayModeDesc : dxgiModeList )
	    {
	        DisplayVideoSettings videoSettings;
	        videoSettings.resolution.x = static_cast<uint32>( dxgiDisplayModeDesc.Width );
	        videoSettings.resolution.y = static_cast<uint32>( dxgiDisplayModeDesc.Height );
	        auto & refreshRate = dxgiDisplayModeDesc.RefreshRate;
	        videoSettings.refreshRate = static_cast<uint16>( static_cast<float>( refreshRate.Numerator ) / refreshRate.Denominator );

	        if( dxgiDisplayModeDesc.ScanlineOrdering == DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE )
	        {
	            videoSettings.flags.set( E_DISPLAY_VIDEO_SETTINGS_FLAG_SCAN_PROGRESSIVE_BIT );
	        }
	        else if( dxgiDisplayModeDesc.ScanlineOrdering == DXGI_MODE_SCANLINE_ORDER_LOWER_FIELD_FIRST )
	        {
	            videoSettings.flags.set( E_DISPLAY_VIDEO_SETTINGS_FLAG_SCAN_INTERLACED_BIT );
	        }
	        else if( dxgiDisplayModeDesc.ScanlineOrdering == DXGI_MODE_SCANLINE_ORDER_UPPER_FIELD_FIRST )
	        {
	            videoSettings.flags.set( E_DISPLAY_VIDEO_SETTINGS_FLAG_SCAN_INTERLACED_BIT );
	        }

	        auto settingsHash = dsmComputeVideoSettingsHash( pColorFormat, videoSettings );
	        if( settingsHash == lastSettingsHash )
	        {
	            continue;
	        }

	        auto * videoMode = addVideoMode( pOutput, pColorFormat );
	        auto & videoModeNativeData = dsmGetObjectNativeDataDXGI( *videoMode );
	        auto & videoModeDesc = dsmGetObjectDesc( *videoMode );

	        videoModeNativeData.dxgiModeDesc = dxgiDisplayModeDesc;
	        videoModeDesc.settings = videoSettings;
	        videoModeDesc.settingsHash = settingsHash;

	        lastSettingsHash = settingsHash;
	    }
	}

    ColorFormat DisplayDriverDXGI::_drvQueryDefaultSystemColorFormat() const
    {
	    return ColorFormat::B8G8R8A8;
    }


	void _dxgiInitializeDriver( DisplayDriverDXGI & pDriverDXGI )
	{
	}

	DXGI_FORMAT _dxgiTranslateColorFormatToDXGIFormat( ColorFormat pColorFormat )
	{
		static const std::unordered_map<ColorFormat, DXGI_FORMAT> colorDescMap =
		{
			{ ColorFormat::B8G8R8       , DXGI_FORMAT_B8G8R8X8_UNORM      },
			{ ColorFormat::B8G8R8A8     , DXGI_FORMAT_B8G8R8A8_UNORM      },
			{ ColorFormat::B8G8R8A8SRGB , DXGI_FORMAT_B8G8R8X8_UNORM_SRGB },
			{ ColorFormat::R8G8B8A8     , DXGI_FORMAT_R8G8B8A8_UNORM      },
			{ ColorFormat::R8G8B8A8SRGB , DXGI_FORMAT_R8G8B8A8_UNORM_SRGB },
			{ ColorFormat::R8G8B8X8     , DXGI_FORMAT_R8G8B8X8_UNORM      },
			{ ColorFormat::R10G10B10A2  , DXGI_FORMAT_R10G10B10A2_UNORM   },
		};
		return getMapValueOrDefault( colorDescMap, pColorFormat, DXGI_FORMAT_UNKNOWN );
	}

}
