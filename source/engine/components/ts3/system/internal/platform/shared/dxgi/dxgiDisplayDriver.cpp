
#include <ts3/system/displayDriverNative.h>
#include <ts3/stdext/mapUtils.h>
#include <comdef.h> // For _bstr_t

namespace ts3::system
{

    static void _dxgiInitializeDriver( DisplayDriverDXGI & pDriverDXGI );
	static DXGI_FORMAT _dxgiTranslateColorFormatToDXGIFormat( ColorFormat pColorFormat );


	DisplayDriverDXGI::DisplayDriverDXGI( DisplayManager * pDisplayManager )
	: DisplayDriver( pDisplayManager, EDisplayDriverType::DXGI )
	{
	    _dxgiInitializeDriver( *this );
	}

	DisplayDriverDXGI::~DisplayDriverDXGI() = default;

	void DisplayDriverDXGI::_nativeEnumAdapters()
	{
	    auto * dxgiFactory = mPrivate->nativeDataPriv.dxgi->dxgiFactory.Get();

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
	            throw 0;
	        }

	        DXGI_ADAPTER_DESC1 dxgiAdapterDesc;
	        hResult = dxgiAdapter->GetDesc1( &dxgiAdapterDesc );

	        // Again - for a valid adapter, the only error can be E_INVALIDARG if the only parameter is NULL. Still...
	        // https://docs.microsoft.com/en-us/windows/win32/api/dxgi/nf-dxgi-idxgiadapter1-getdesc1
	        if ( FAILED( hResult ) )
	        {
	            throw 0;
	        }

	        auto * adapterObject = addAdapter();
	        adapterObject->mPrivate->nativeDataPriv.dxgi->dxgiAdapter = dxgiAdapter;
	        adapterObject->mPrivate->nativeDataPriv.dxgi->dxgiAdapterDesc = dxgiAdapterDesc;
	        adapterObject->mPrivate->descPriv.name = _bstr_t( dxgiAdapterDesc.Description );

	        if( adapterIndex == 0 )
	        {
	            adapterObject->mPrivate->descPriv.flags.set( E_DISPLAY_ADAPTER_FLAG_PRIMARY_BIT );
	        }

	        if( ( dxgiAdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE ) != 0 )
	        {
	            adapterObject->mPrivate->descPriv.flags.set( E_DISPLAY_ADAPTER_FLAG_SOFTWARE_BIT );
	        }
	    }
	}

	void DisplayDriverDXGI::_nativeEnumOutputs( DisplayAdapter & pAdapter )
	{
	    auto * dxgiAdapter = pAdapter.mPrivate->nativeDataPriv.dxgi->dxgiAdapter.Get();

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
	        // for an adapter which is a parent for D3D_DRIVER_TYPE_WARP D3D device. Not an option here. Still...
	        // https://docs.microsoft.com/en-us/windows/win32/api/dxgi/nf-dxgi-idxgiadapter-enumoutputs
	        if ( FAILED( hResult ) )
	        {
	            throw 0;
	        }

	        ComPtr<IDXGIOutput1> dxgiOutput1;
	        hResult = dxgiOutput->QueryInterface( __uuidof( IDXGIOutput1 ), reinterpret_cast< void** >( dxgiOutput1.GetAddressOf() ) );

	        // We require support for the DXGI version 1.1 (Windows 7 and newer). Required for DXGI_ADAPTER_FLAG member.
	        if ( FAILED( hResult ) )
	        {
	            throw 0;
	        }

	        DXGI_OUTPUT_DESC dxgiOutputDesc;
	        hResult = dxgiOutput1->GetDesc( &dxgiOutputDesc );

	        if ( FAILED( hResult ) )
	        {
	            throw 0;
	        }

	        auto * outputObject = addOutput( pAdapter );
	        outputObject->mPrivate->nativeDataPriv.dxgi->dxgiOutput = dxgiOutput1;
	        outputObject->mPrivate->nativeDataPriv.dxgi->dxgiOutputDesc = dxgiOutputDesc;
	        outputObject->mPrivate->descPriv.name = _bstr_t( dxgiOutputDesc.DeviceName );
	        outputObject->mPrivate->descPriv.screenRect.offset.x = dxgiOutputDesc.DesktopCoordinates.left;
	        outputObject->mPrivate->descPriv.screenRect.offset.y = dxgiOutputDesc.DesktopCoordinates.top;
	        outputObject->mPrivate->descPriv.screenRect.size.x = dxgiOutputDesc.DesktopCoordinates.right - dxgiOutputDesc.DesktopCoordinates.left;
	        outputObject->mPrivate->descPriv.screenRect.size.y = dxgiOutputDesc.DesktopCoordinates.bottom - dxgiOutputDesc.DesktopCoordinates.top;
	    }
	}

	void DisplayDriverDXGI::_nativeEnumVideoModes( DisplayOutput & pOutput, ColorFormat pColorFormat )
	{
	    auto * dxgiOutput = pOutput.mPrivate->nativeDataPriv.dxgi->dxgiOutput.Get();
	    auto dxgiFormat = _dxgiTranslateColorFormatToDXGIFormat( pColorFormat );

	    if ( dxgiFormat == DXGI_FORMAT_UNKNOWN )
	    {
	        throw 0;
	    }

	    UINT displayModesNum = 0;
	    // Passing nullptr as 'pDesc' causes the DXGI runtime to return the total number of supported modes for an adapter.
	    auto hResult = dxgiOutput->GetDisplayModeList( dxgiFormat, 0, &displayModesNum, nullptr );

	    if ( FAILED( hResult ) )
	    {
	        throw 0;
	    }

	    std::vector<DXGI_MODE_DESC> dxgiModeList;
	    dxgiModeList.resize( displayModesNum );

	    // Query all supported modes and store them into the pre-allocated array.
	    // Any optional filtering and processing is done afterwards, it's more efficient to fetch this in one go.
	    hResult = dxgiOutput->GetDisplayModeList( dxgiFormat, 0, &displayModesNum, dxgiModeList.data() );

	    if ( FAILED( hResult ) )
	    {
	        throw 0;
	    }

	    // We use hash-based comparison to filter out the same modes - at our level, we are only interested
	    // in settings and color format (and couple flags), so duplications can be safely removed here).
	    dsm_video_settings_hash_t prevSettingsHash = CX_DSM_VIDEO_SETTINGS_HASH_INVALID;

	    for ( auto & dxgiDisplayModeDesc : dxgiModeList )
	    {

	        DisplayVideoSettings videoSettings;
	        videoSettings.resolution.x = static_cast<uint32>( dxgiDisplayModeDesc.Width );
	        videoSettings.resolution.y = static_cast<uint32>( dxgiDisplayModeDesc.Height );
	        videoSettings.refreshRate = static_cast<uint16>( dxgiDisplayModeDesc.RefreshRate.Numerator );

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
	        if( settingsHash == prevSettingsHash )
	        {
	            continue;
	        }

	        auto * videoMode = addVideoMode( pOutput, pColorFormat );
	        videoMode->mPrivate->nativeDataPriv.dxgi->dxgiModeDesc = dxgiDisplayModeDesc;
	        videoMode->mPrivate->descPriv.settings = videoSettings;
	        videoMode->mPrivate->descPriv.settingsHash = settingsHash;
	    }
	}

	void DisplayDriverDXGI::_nativeDestroyAdapter( DisplayAdapter & pAdapter )
	{}

	void DisplayDriverDXGI::_nativeDestroyOutput( DisplayOutput & pOutput )
	{}

	void DisplayDriverDXGI::_nativeDestroyVideoMode( DisplayVideoMode & pVideoMode )
	{}


	void _dxgiInitializeDriver( DisplayDriverDXGI & pDriverDXGI )
	{
	    ts3DebugAssert( pDriverDXGI.mPrivate->nativeDataPriv.dxgi );
	    auto & dxgiDriverData = *( pDriverDXGI.mPrivate->nativeDataPriv.dxgi );

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
	                throw 0;
	            }

	            dxgiDriverData.dxgiFactory = dxgiFactory1;
	        }
	    }
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
			{ ColorFormat::R10G10B10A2  , DXGI_FORMAT_R10G10B10A2_UNORM   },
		};
		return getMapValueOrDefault( colorDescMap, pColorFormat, DXGI_FORMAT_UNKNOWN );
	}

}
