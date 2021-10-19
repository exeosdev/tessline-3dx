
#include "dxgiDisplaySystem.h"
#include <ts3/system/internal/displaySystemPrivate.h>
#include <ts3/stdext/mapUtils.h>
#include <ts3/stdext/stringUtils.h>

#if( TS3_SYSTEM_DSM_DRIVER_TYPE_SUPPORT_DXGI )
namespace ts3::system
{

    namespace platform
    {

        DXGI_FORMAT _dxgiTranslateColorFormatToDXGIFormat( EColorFormat pColorFormat );

    }


    DisplayAdapterDXGI::DisplayAdapterDXGI( DisplayDriverDXGI & pDisplayDriver )
    : DisplayAdapter( pDisplayDriver )
    {}

    DisplayAdapterDXGI::~DisplayAdapterDXGI() noexcept = default;


    DisplayOutputDXGI::DisplayOutputDXGI( DisplayAdapterDXGI & pDisplayAdapter )
    : DisplayOutput( pDisplayAdapter )
    {}

    DisplayOutputDXGI::~DisplayOutputDXGI() noexcept = default;


    DisplayVideoModeDXGI::DisplayVideoModeDXGI( DisplayOutputDXGI & pDisplayOutput )
    : DisplayVideoMode( pDisplayOutput )
    {}

    DisplayVideoModeDXGI::~DisplayVideoModeDXGI() noexcept = default;


	DisplayDriverDXGI::DisplayDriverDXGI( DisplayManagerHandle pDisplayManager )
	: DisplayDriver( pDisplayManager, EDisplayDriverType::DXGI )
	{
	    _initializeDXGIDriverState();
	}

	DisplayDriverDXGI::~DisplayDriverDXGI() noexcept
	{
	    _releaseDXGIDriverState();
	}

	void DisplayDriverDXGI::_initializeDXGIDriverState()
    {
        if( mNativeData.dxgiFactory == nullptr )
        {
            Bitmask<UINT> dxgiFactoryCreateFlags = 0;
        #if ( TS3_DEBUG )
            dxgiFactoryCreateFlags.set( DXGI_CREATE_FACTORY_DEBUG );
        #endif

            ComPtr<IDXGIFactory2> dxgiFactory2;
            auto hResult = ::CreateDXGIFactory2( dxgiFactoryCreateFlags, IID_PPV_ARGS( &dxgiFactory2 ) );

            if( SUCCEEDED( hResult ) )
            {
                mNativeData.dxgiFactory = dxgiFactory2;
            }
            else
            {
                ComPtr<IDXGIFactory1> dxgiFactory1;
                hResult = ::CreateDXGIFactory1( IID_PPV_ARGS( &dxgiFactory1 ) );

                if( FAILED( hResult ) )
                {
                    ts3Throw( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
                }

                mNativeData.dxgiFactory = dxgiFactory1;
            }
        }
    }

    void DisplayDriverDXGI::_releaseDXGIDriverState() noexcept
    {}

    void DisplayDriverDXGI::_enumAdapterOutputs( DisplayAdapter & pAdapter )
    {
	    auto * adapterDXGI = pAdapter.queryInterface<DisplayAdapterDXGI>();

	    auto * dxgiAdapterInterface = adapterDXGI->mNativeData.dxgiAdapter.Get();

        for( UINT outputIndex = 0u; ; ++outputIndex )
        {
            ComPtr<IDXGIOutput> dxgiOutput;
            auto hResult = dxgiAdapterInterface->EnumOutputs( outputIndex, dxgiOutput.GetAddressOf() );

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
                ts3Throw( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
            }

            ComPtr<IDXGIOutput1> dxgiOutput1;
            hResult = dxgiOutput->QueryInterface( __uuidof( IDXGIOutput1 ), reinterpret_cast< void** >( dxgiOutput1.GetAddressOf() ) );

            // We require support for the DXGI version 1.1 (Windows 7 and newer). Required for DXGI_ADAPTER_FLAG member.
            if ( FAILED( hResult ) )
            {
                ts3Throw( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
            }

            DXGI_OUTPUT_DESC dxgiOutputDesc;
            hResult = dxgiOutput1->GetDesc( &dxgiOutputDesc );

            if ( FAILED( hResult ) )
            {
                ts3Throw( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
            }

            auto outputObject = adapterDXGI->createOutput<DisplayOutputDXGI>( *adapterDXGI );
            outputObject->mNativeData.dxgiOutput = dxgiOutput1;
            outputObject->mNativeData.dxgiOutputDesc = dxgiOutputDesc;
            
            auto & outputDesc = outputObject->getOutputDescInternal();
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
                // It is almost crazy we need to rely on the old  API within a DXGI realm... but it seems
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

	void DisplayDriverDXGI::_nativeEnumDisplayDevices()
	{
	    auto * dxgiFactory =  mNativeData.dxgiFactory.Get();

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
	            ts3Throw( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
	        }

	        DXGI_ADAPTER_DESC1 dxgiAdapterDesc;
	        hResult = dxgiAdapter->GetDesc1( &dxgiAdapterDesc );

	        // Again - for a valid adapter, the only error can be E_INVALIDARG if the only parameter is NULL. Still...
	        // https://docs.microsoft.com/en-us/windows/win32/api/dxgi/nf-dxgi-idxgiadapter1-getdesc1
	        if ( FAILED( hResult ) )
	        {
	            ts3Throw( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
	        }

	        auto adapterObject = createAdapter<DisplayAdapterDXGI>( *this );
	        adapterObject->mNativeData.dxgiAdapter = dxgiAdapter;
	        adapterObject->mNativeData.dxgiAdapterDesc = dxgiAdapterDesc;
	        
	        auto & adapterDesc = adapterObject->getAdapterDescInternal();
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

	void DisplayDriverDXGI::_nativeEnumVideoModes( DisplayOutput & pOutput, EColorFormat pColorFormat )
	{
	    auto * outputDXGI = pOutput.queryInterface<DisplayOutputDXGI>();

	    auto * dxgiOutputInterface = outputDXGI->mNativeData.dxgiOutput.Get();

	    // This should never fail, i.e. the specified format should always yield a value which
	    // is known to the DXGI translation function. We had an issue with 'SystemNative' format,
	    // but this has been resolved by proxy function inside DisplayOutput::PrivateData struct.
	    // If there is a case the call below fails, make sure this function is ALWAYS called with
	    // a "resolved" color format (look for 'dsmResolveSystemColorFormat').
	    auto dxgiFormat = platform::_dxgiTranslateColorFormatToDXGIFormat( pColorFormat );

	    if ( dxgiFormat == DXGI_FORMAT_UNKNOWN )
	    {
	        ts3Throw( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
	    }

	    UINT displayModesNum = 0;
	    // Passing nullptr as 'pDesc' causes the DXGI runtime to return the total number of supported modes for an adapter.
	    auto hResult = dxgiOutputInterface->GetDisplayModeList( dxgiFormat, 0, &displayModesNum, nullptr );

	    if ( FAILED( hResult ) )
	    {
	        ts3Throw( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
	    }

	    std::vector<DXGI_MODE_DESC> dxgiModeList;
	    dxgiModeList.resize( displayModesNum );

	    // Query all supported modes and store them into the pre-allocated array.
	    // Any optional filtering and processing is done afterwards, it's more efficient to fetch this in one go.
	    hResult = dxgiOutputInterface->GetDisplayModeList( dxgiFormat, 0, &displayModesNum, dxgiModeList.data() );

	    if ( FAILED( hResult ) )
	    {
	        ts3Throw( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
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

	        auto videoModeObject = outputDXGI->createVideoMode<DisplayVideoModeDXGI>( *outputDXGI, pColorFormat );
	        videoModeObject->mNativeData.dxgiModeDesc = dxgiDisplayModeDesc;
	        
	        auto & videoModeDesc = videoModeObject->getModeDescInternal();
	        videoModeDesc.settings = videoSettings;
	        videoModeDesc.settingsHash = settingsHash;

	        lastSettingsHash = settingsHash;
	    }
	}

    EColorFormat DisplayDriverDXGI::_nativeQueryDefaultSystemColorFormat() const
    {
	    return EColorFormat::B8G8R8A8;
    }


    namespace platform
    {

        DXGI_FORMAT _dxgiTranslateColorFormatToDXGIFormat( EColorFormat pColorFormat )
        {
            static const std::unordered_map<EColorFormat, DXGI_FORMAT> colorDescMap =
            {
                { EColorFormat::B8G8R8       , DXGI_FORMAT_B8G8R8X8_UNORM      },
                { EColorFormat::B8G8R8A8     , DXGI_FORMAT_B8G8R8A8_UNORM      },
                { EColorFormat::B8G8R8A8SRGB , DXGI_FORMAT_B8G8R8X8_UNORM_SRGB },
                { EColorFormat::R8G8B8A8     , DXGI_FORMAT_R8G8B8A8_UNORM      },
                { EColorFormat::R8G8B8A8SRGB , DXGI_FORMAT_R8G8B8A8_UNORM_SRGB },
                { EColorFormat::R8G8B8X8     , DXGI_FORMAT_UNKNOWN             },
                { EColorFormat::R10G10B10A2  , DXGI_FORMAT_R10G10B10A2_UNORM   },
            };
            return getMapValueOrDefault( colorDescMap, pColorFormat, DXGI_FORMAT_UNKNOWN );
        }

    }

} // namespace ts3::system
#endif // TS3_SYSTEM_DSM_DRIVER_TYPE_SUPPORT_DXGI
