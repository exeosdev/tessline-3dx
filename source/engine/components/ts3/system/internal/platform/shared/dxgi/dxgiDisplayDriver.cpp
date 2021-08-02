
#include <ts3/system/displayDriver.h>
#include <ts3/stdext/mapUtils.h>
#include <comdef.h> // For _bstr_t

namespace ts3
{

	static DXGI_FORMAT _dxgiTranslateSysColorFormatToDXGIFormat( SysColorFormat pColorFormat );


	SysDisplayDriverDXGI::SysDisplayDriverDXGI( SysDisplayManagerHandle pDisplayManager ) noexcept
	: SysDisplayDriver( pDisplayManager, ESysDsmDisplayDriverType::DXGI )
	{}

	SysDisplayDriverDXGI::~SysDisplayDriverDXGI() noexcept = default;

	ESysDsmDisplayDriverType SysDisplayDriverDXGI::queryDriverID() const noexcept
	{
		return ESysDsmDisplayDriverType::DXGI;
	}

	SysDisplayDriverHandle SysDisplayDriverDXGI::create( SysDisplayManagerHandle pDisplayManager, const SysDisplayDriverCreateInfoDXGI & pCreateInfo )
	{
		auto displayDriver = sysCreateObject<SysDisplayDriverDXGI>( pDisplayManager );
		displayDriver->initialize( pCreateInfo );
		return displayDriver;
	}

	void SysDisplayDriverDXGI::initialize( const SysDisplayDriverCreateInfoDXGI & pCreateInfo )
	{
		auto * dxgiDriverData = mNativeData.dxgi;

		if( dxgiDriverData->dxgiFactory == nullptr )
		{
			Bitmask<UINT> dxgiFactoryCreateFlags = 0;
		#if ( TS3_DEBUG )
			dxgiFactoryCreateFlags.set( DXGI_CREATE_FACTORY_DEBUG );
		#endif

			ComPtr<IDXGIFactory2> dxgiFactory2;
			auto hResult = CreateDXGIFactory2( dxgiFactoryCreateFlags, IID_PPV_ARGS( &dxgiFactory2 ) );
			if( FAILED( hResult ) )
			{
				ComPtr<IDXGIFactory1> dxgiFactory1;
				hResult = CreateDXGIFactory1( IID_PPV_ARGS( &dxgiFactory1 ) );
				if( FAILED( hResult ) )
				{
					throw 0;
				}
				dxgiDriverData->dxgiFactory = dxgiFactory1;
			}

			if( !dxgiDriverData->dxgiFactory )
			{
				dxgiDriverData->dxgiFactory = dxgiFactory2;
			}
		}
	}

	void SysDisplayDriverDXGI::_sysResetInternalState()
	{
	}

	void SysDisplayDriverDXGI::_sysEnumAdapterList()
	{
		auto * dxgiFactory = mNativeData.dxgi->dxgiFactory.Get();

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

			auto * adapterInfo = registerAdapter();
			adapterInfo->nativeData.dxgi->dxgiAdapter = dxgiAdapter;
			adapterInfo->nativeData.dxgi->dxgiAdapterDesc = dxgiAdapterDesc;
			adapterInfo->adapterDesc.name = _bstr_t( dxgiAdapterDesc.Description );

			if( adapterIndex == 0 )
			{
				adapterInfo->adapterDesc.flags.set( E_SYS_DSM_ADAPTER_FLAG_PRIMARY_BIT );
			}

			if( ( dxgiAdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE ) != 0 )
			{
				adapterInfo->adapterDesc.flags.set( E_SYS_DSM_ADAPTER_FLAG_SOFTWARE_BIT );
			}
		}
	}

	void SysDisplayDriverDXGI::_sysEnumOutputList( SysDsmAdapter & pAdapter )
	{
		auto * dxgiAdapter = pAdapter.nativeData.dxgi->dxgiAdapter.Get();

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

			auto * outputInfo = registerOutput( pAdapter );
			outputInfo->nativeData.dxgi->dxgiOutput = dxgiOutput1;
			outputInfo->nativeData.dxgi->dxgiOutputDesc = dxgiOutputDesc;
			outputInfo->outputDesc.name = _bstr_t( dxgiOutputDesc.DeviceName );
			outputInfo->outputDesc.screenRect.offset.x = dxgiOutputDesc.DesktopCoordinates.left;
			outputInfo->outputDesc.screenRect.offset.y = dxgiOutputDesc.DesktopCoordinates.top;
			outputInfo->outputDesc.screenRect.size.x = dxgiOutputDesc.DesktopCoordinates.right - dxgiOutputDesc.DesktopCoordinates.left;
			outputInfo->outputDesc.screenRect.size.y = dxgiOutputDesc.DesktopCoordinates.bottom - dxgiOutputDesc.DesktopCoordinates.top;
		}
	}

	void SysDisplayDriverDXGI::_sysEnumVideoModeList( SysDsmOutput & pOutput, SysColorFormat pFormat )
	{
		auto * dxgiOutput = pOutput.nativeData.dxgi->dxgiOutput.Get();
		auto dxgiFormat = _dxgiTranslateSysColorFormatToDXGIFormat( pFormat );

		//
		if ( dxgiFormat == DXGI_FORMAT_UNKNOWN )
		{
			throw 0;
		}

		UINT displayModesNum = 0;
		//
		auto hResult = dxgiOutput->GetDisplayModeList( dxgiFormat, 0, &displayModesNum, nullptr );

		//
		if ( FAILED( hResult ) )
		{
			throw 0;
		}

		std::vector<DXGI_MODE_DESC> dxgiModeList;
		dxgiModeList.resize( displayModesNum );

		//
		hResult = dxgiOutput->GetDisplayModeList( dxgiFormat, 0, &displayModesNum, dxgiModeList.data() );

		if ( FAILED( hResult ) )
		{
			throw 0;
		}

		sys_dsm_video_settings_hash_t prevSettingsHash = cvSysDsmVideoSettingsHashInvalid;

		for ( auto & dxgiDisplayModeDesc : dxgiModeList )
		{

			SysDsmVideoSettings videoSettings;
			videoSettings.resolution.x = static_cast<uint32>( dxgiDisplayModeDesc.Width );
			videoSettings.resolution.y = static_cast<uint32>( dxgiDisplayModeDesc.Height );
			videoSettings.refreshRate = static_cast<uint16>( dxgiDisplayModeDesc.RefreshRate.Numerator );

			if( dxgiDisplayModeDesc.ScanlineOrdering == DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE )
			{
				videoSettings.flags.set( E_SYS_DSM_VIDEO_SETTINGS_FLAG_SCAN_PROGRESSIVE_BIT );
			}
			else if( dxgiDisplayModeDesc.ScanlineOrdering == DXGI_MODE_SCANLINE_ORDER_LOWER_FIELD_FIRST )
			{
				videoSettings.flags.set( E_SYS_DSM_VIDEO_SETTINGS_FLAG_SCAN_INTERLACED_BIT );
			}
			else if( dxgiDisplayModeDesc.ScanlineOrdering == DXGI_MODE_SCANLINE_ORDER_UPPER_FIELD_FIRST )
			{
				videoSettings.flags.set( E_SYS_DSM_VIDEO_SETTINGS_FLAG_SCAN_INTERLACED_BIT );
			}

			auto settingsHash = sysDsmComputeVideoSettingsHash( pFormat, videoSettings );
			if( settingsHash == prevSettingsHash )
			{
				continue;
			}

			auto * videoMode = registerVideoMode( pOutput, pFormat );
			videoMode->nativeData.dxgi->dxgiModeDesc = dxgiDisplayModeDesc;
			videoMode->modeDesc.settings = videoSettings;
			videoMode->modeDesc.settingsHash = settingsHash;
		}
	}

	DXGI_FORMAT _dxgiTranslateSysColorFormatToDXGIFormat( SysColorFormat pColorFormat )
	{
		static const std::unordered_map<SysColorFormat, DXGI_FORMAT> colorDescMap =
		{
			{ SysColorFormat::B8G8R8       , DXGI_FORMAT_B8G8R8X8_UNORM      },
			{ SysColorFormat::B8G8R8A8     , DXGI_FORMAT_B8G8R8A8_UNORM      },
			{ SysColorFormat::B8G8R8A8SRGB , DXGI_FORMAT_B8G8R8X8_UNORM_SRGB },
			{ SysColorFormat::R8G8B8A8     , DXGI_FORMAT_R8G8B8A8_UNORM      },
			{ SysColorFormat::R8G8B8A8SRGB , DXGI_FORMAT_R8G8B8A8_UNORM_SRGB },
			{ SysColorFormat::R10G10B10A2  , DXGI_FORMAT_R10G10B10A2_UNORM   },
		};
		return getMapValueOrDefault( colorDescMap, pColorFormat, DXGI_FORMAT_UNKNOWN );
	}

}
