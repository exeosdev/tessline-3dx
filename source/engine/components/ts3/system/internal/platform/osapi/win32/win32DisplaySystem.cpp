
#include "win32DisplaySystem.h"
#include <ts3/system/internal/displaySystemPrivate.h>
#include <ts3/stdext/stringUtils.h>

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_WIN32 )
namespace ts3::system
{

	namespace platform
	{

		// Returns a name for an output by extracting the output part from the DisplayDevice registry key.
		std::string _win32GetAdapterOutputName( const std::string & pAdapterRegistryKey );

	}


	Win32DisplayManager::Win32DisplayManager( SysContextHandle pSysContext )
	: DisplayManager( std::move( pSysContext ) )
	{}

	Win32DisplayManager::~Win32DisplayManager() noexcept = default;

	DisplayDriverHandle Win32DisplayManager::_nativeCreateDisplayDriver()
	{
		return createSysObject<Win32DisplayDriver>( getHandle<Win32DisplayManager>() );
	}

    void Win32DisplayManager::_nativeQueryDefaultDisplayOffset( DisplayOffset & pOutOffset ) const
    {
        pOutOffset.x = 0;
        pOutOffset.y = 0;
    }

	void Win32DisplayManager::_nativeQueryDefaultDisplaySize( DisplaySize & pOutSize ) const
	{
		auto cxScreen = ::GetSystemMetrics( SM_CXSCREEN );
		auto cyScreen = ::GetSystemMetrics( SM_CYSCREEN );
		pOutSize.x = static_cast<uint32>( cxScreen );
		pOutSize.y = static_cast<uint32>( cyScreen );
	}

	void Win32DisplayManager::_nativeQueryMinWindowSize( DisplaySize & pOutSize ) const
	{
		auto cxMin = ::GetSystemMetrics( SM_CXMIN );
		auto cyMin = ::GetSystemMetrics( SM_CYMIN );
		pOutSize.x = static_cast<uint32>( cxMin );
		pOutSize.y = static_cast<uint32>( cyMin );
	}


	Win32DisplayDriver::Win32DisplayDriver( DisplayManagerHandle pDisplayManager )
	: Win32NativeObject( std::move( pDisplayManager ), EDisplayDriverType::Generic )
	{}

	Win32DisplayDriver::~Win32DisplayDriver() noexcept = default;

	// -- Note on adapters enumeration:
	// Without the awesome DXGI, EnumDisplayDevices is the only reliable way of enumerating the display stuff.
	// With its first parameter set to NULL, it queries the information about an adapter with the specified index.
	// Adapter is identified as a registry key, however - it is tied with its connected monitors. Example:
	// In my current setup (AMD Radeon RX580 and two 1440p monitors connected via DisplayPort), EnumDisplayDevices
	// gives me two entries with the following keys (DISPLAY_DEVICEA::DeviceKey):
	// 1) \\Registry\\Machine\\System\\CurrentControlSet\\Control\\Video\\{79BD17DD-B591-11EA-B520-AC9E17ECDDE5}\\0000
	// 2) \\Registry\\Machine\\System\\CurrentControlSet\\Control\\Video\\{79BD17DD-B591-11EA-B520-AC9E17ECDDE5}\\0001
	// So, to enumerate adapters properly, we must check the UUID of the adapter to not duplicate the entries.
	// See SysDisplayDriverImplProxy::nativeEnumAdapterList below.
	void Win32DisplayDriver::_nativeEnumDisplayDevices()
	{
		// Represents information about a display device in the system. String properties have the following meaning:
		// ::DeviceID - PCI-specific ID, not really interesting
		// ::DeviceString - name of the adapter, e.g. "Radeon RX580 Series"
		// ::DeviceName - detailed name of the device, e.g. "\\\\.\\DISPLAY1". For adapters, this is the name of a connected display
		// ::DeviceKey - device's key in the registry, e.g. "\\Registry\\Machine\\System\\CurrentControlSet\\Control\\Video\\{SOME_UUID}"

		DISPLAY_DEVICEA adapterInfoGDI;
		adapterInfoGDI.cb = sizeof( DISPLAY_DEVICEA );

		DISPLAY_DEVICEA outputInfoGDI;
		outputInfoGDI.cb = sizeof( DISPLAY_DEVICEA );

		for( UINT displayDeviceIndex = 0; ; ++displayDeviceIndex )
		{
			// Enumerate next display device in the system. Multiple devices may refer to the same physical adapter, i.e.:
			// "\\Registry\\Machine\\System\\CurrentControlSet\\Control\\Video\\{79BD17DD-B591-11EA-B520-AC9E17ECDDE5}\\0000"
			// "\\Registry\\Machine\\System\\CurrentControlSet\\Control\\Video\\{79BD17DD-B591-11EA-B520-AC9E17ECDDE5}\\0001"
			// are two different physical outputs (0000 and 0001) connected to the same adapter (UUID={79BD17DD-B591-11EA-B520-AC9E17ECDDE5}).
			BOOL enumResult = ::EnumDisplayDevicesA( nullptr, displayDeviceIndex, &adapterInfoGDI, 0 );

			// Display devices are indexed starting from 0 up to DEVICES_NUM - 1.
			// If EnumDisplayDevicesA returns FALSE, it means we enumerated all of them already.
			if( enumResult == FALSE )
			{
				break;
			}

			// Extract adapter UUID from the registry key. Devices referring to the same adapter have the same adapter UUID.
			auto adapterUUID = getUUIDString( adapterInfoGDI.DeviceKey );
			// Check if that adapter has been already added to the list of adapters (if there was already another device which referred to it).
			auto adapterObject = _findAdapterByUUID( adapterUUID );

			if( adapterObject == nullptr )
			{
				auto newAdapterObject = createAdapter<Win32DisplayAdapter>( *this );
				newAdapterObject->mNativeData.deviceUUID = std::move( adapterUUID );
				newAdapterObject->mNativeData.deviceName = adapterInfoGDI.DeviceName;

				auto & adapterDesc = getAdapterDescInternal( *newAdapterObject );
				adapterDesc.name = adapterInfoGDI.DeviceString;

				if( makeBitmask( adapterInfoGDI.StateFlags ).isSet( DISPLAY_DEVICE_ATTACHED_TO_DESKTOP ) )
				{
					adapterDesc.flags.set( E_DISPLAY_ADAPTER_FLAG_ACTIVE_BIT );
				}
				if( makeBitmask( adapterInfoGDI.StateFlags ).isSet( DISPLAY_DEVICE_PRIMARY_DEVICE ) )
				{
					adapterDesc.flags.set( E_DISPLAY_ADAPTER_FLAG_PRIMARY_BIT );
				}

				adapterObject = std::move( newAdapterObject );
			}

			for( UINT adapterOutputIndex = 0; ; ++adapterOutputIndex )
			{
				//
				enumResult = ::EnumDisplayDevicesA( adapterInfoGDI.DeviceName, adapterOutputIndex, &outputInfoGDI, 0 );
				if( enumResult == FALSE )
				{
					break;
				}

				auto outputObject = createOutput<Win32DisplayOutput>( *adapterObject );
				outputObject->mNativeData.displayDeviceName = adapterInfoGDI.DeviceName;
				outputObject->mNativeData.outputID = outputInfoGDI.DeviceName;

				auto & outputDesc = getOutputDescInternal( *outputObject );
				// NOTE: 'DISPLAY_DEVICE_PRIMARY_DEVICE' flag is not set in case of output devices (unlike adapters).
				// Primary output can be detected by analysing at monitor flags and looking for MONITORINFOF_PRIMARY.
				// See _win32MonitorEnumProc function above where this gets done.
				if( makeBitmask( outputInfoGDI.StateFlags ).isSet( DISPLAY_DEVICE_ATTACHED_TO_DESKTOP ) )
				{
					outputDesc.flags.set( E_DISPLAY_OUTPUT_FLAG_ACTIVE_BIT );
				}
			}
		}

		::EnumDisplayMonitors( nullptr, nullptr, _win32MonitorEnumProc, reinterpret_cast<LPARAM>( this ) );
	}

	// Monitor enumeration function. Called for each monitor in the system (as a part of _nativeEnumOutputs).
	// This function is called after all output devices in the system have been enumerated and added to the
	// internal list inside the adapter. IMPORTANT: again, this gets called *within* _nativeEnumOutputs()
	// function *per each adapter* (that's why we pass DisplayAdapter* and check for monitors connected to it.
	BOOL CALLBACK Win32DisplayDriver::_win32MonitorEnumProc( HMONITOR pMonitorHandle, HDC pHDC, LPRECT pMonitorRect, LPARAM pUserParam )
	{
		// Adapter object for which we are enumerating outputs/monitors.
		auto * win32DisplayDriver = reinterpret_cast<Win32DisplayDriver *>( pUserParam );

		MONITORINFOEXA gdiMonitorInfo;
		gdiMonitorInfo.cbSize = sizeof( MONITORINFOEXA );

		if( ::GetMonitorInfoA( pMonitorHandle, &gdiMonitorInfo ) != FALSE )
		{
			// 'szDevice' of the MONITORINFOEXA matches the DISPLAY_DEVICEA::DeviceName property of the output.
			// Thanks to that, we can obtain the output this monitor refers to.
			if( auto outputObject = win32DisplayDriver->_findAnyOutputForDisplayDeviceName( gdiMonitorInfo.szDevice ) )
			{
				auto * win32OutputObject = outputObject->queryInterface<Win32DisplayOutput>();
				win32OutputObject->mNativeData.gdiMonitorHandle = pMonitorHandle;

				auto & outputDesc = getOutputDescInternal( *win32OutputObject );
				outputDesc.name = strUtils::convertStringRepresentation<char>( gdiMonitorInfo.szDevice );
				outputDesc.screenRect.offset.x = gdiMonitorInfo.rcMonitor.left;
				outputDesc.screenRect.offset.y = gdiMonitorInfo.rcMonitor.top;
				outputDesc.screenRect.size.x = gdiMonitorInfo.rcMonitor.right - gdiMonitorInfo.rcMonitor.left;
				outputDesc.screenRect.size.y = gdiMonitorInfo.rcMonitor.bottom - gdiMonitorInfo.rcMonitor.top;

				if( makeBitmask( gdiMonitorInfo.dwFlags ).isSet( MONITORINFOF_PRIMARY ) )
				{
					outputDesc.flags.set( E_DISPLAY_OUTPUT_FLAG_PRIMARY_BIT );
				}
			}
		}

		return TRUE;
	}

	void Win32DisplayDriver::_nativeEnumVideoModes( DisplayOutput & pOutput, EColorFormat pColorFormat )
	{
		auto * outputWin32 = pOutput.queryInterface<Win32DisplayOutput>();

		const auto & colorFormatDesc = vsxGetDescForColorFormat( pColorFormat );

		if( colorFormatDesc.colorSpace != EColorSpace::Linear )
		{
			return;
		}

		if( ( colorFormatDesc.rgba.u8Red != 8 ) || ( colorFormatDesc.rgba.u8Green != 8 ) || ( colorFormatDesc.rgba.u8Blue != 8 ) )
		{
			return;
		}

		DEVMODEA gdiDevMode;
		gdiDevMode.dmSize = sizeof( DEVMODEA );
		gdiDevMode.dmDriverExtra = 0;

		dsm_video_settings_hash_t lastSettingsHash = CX_DSM_VIDEO_SETTINGS_HASH_INVALID;

		for( UINT displayModeIndex = 0; ; ++displayModeIndex )
		{
			auto * displayDeviceNameStr = outputWin32->mNativeData.displayDeviceName.c_str();

			BOOL edsResult = ::EnumDisplaySettingsExA( displayDeviceNameStr, displayModeIndex, &gdiDevMode, 0 );

			if( edsResult == FALSE )
			{
				break;
			}

			if( gdiDevMode.dmBitsPerPel != colorFormatDesc.size )
			{
				continue;
			}

			DisplayVideoSettings videoSettings;
			videoSettings.resolution.x = static_cast<uint32>( gdiDevMode.dmPelsWidth );
			videoSettings.resolution.y = static_cast<uint32>( gdiDevMode.dmPelsHeight );
			videoSettings.refreshRate = static_cast<uint16>( gdiDevMode.dmDisplayFrequency );

			if( makeBitmask( gdiDevMode.dmDisplayFlags ).isSet( DM_INTERLACED ) )
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

			auto videoModeObject = createVideoMode<Win32DisplayVideoMode>( *outputWin32, pColorFormat );
			videoModeObject->mNativeData.gdiModeInfo = gdiDevMode;

			auto & videoModeDesc = getVideoModeDescInternal( *videoModeObject );
			videoModeDesc.settings = videoSettings;
			videoModeDesc.settingsHash = settingsHash;

			lastSettingsHash = settingsHash;
		}
	}

	EColorFormat Win32DisplayDriver::_nativeQueryDefaultSystemColorFormat() const
	{
		return EColorFormat::B8G8R8A8;
	}


	SysHandle<Win32DisplayAdapter> Win32DisplayDriver::_findAdapterByUUID( const std::string & pUUID )
	{
		auto displayAdapter = findAdapter( [&pUUID]( const DisplayAdapter & pAdapter ) -> bool {
			auto * win32Adapter = pAdapter.queryInterface<Win32DisplayAdapter>();
			return win32Adapter->mNativeData.deviceUUID == pUUID;
		} );
		return displayAdapter ? displayAdapter->getHandle<Win32DisplayAdapter>() : nullptr;
	}

	SysHandle<Win32DisplayOutput> Win32DisplayDriver::_findAdapterOutputForDisplayDeviceName( DisplayAdapter & pAdapter,
	                                                                                       const char * pDeviceName )
	{
		auto displayOutput = pAdapter.findOutput( [pDeviceName]( const DisplayOutput & pOutput ) -> bool {
			auto * win32Output = pOutput.queryInterface<Win32DisplayOutput>();
			return win32Output->mNativeData.displayDeviceName == pDeviceName;
		} );
		return displayOutput ? displayOutput->getHandle<Win32DisplayOutput>() : nullptr;
	}

	SysHandle<Win32DisplayOutput> Win32DisplayDriver::_findAnyOutputForDisplayDeviceName( const char * pDeviceName )
	{
		for( auto & adapter : _privateData->adapterInstanceList )
		{
			if( auto adapterOutput = _findAdapterOutputForDisplayDeviceName( *adapter, pDeviceName ) )
			{
				return adapterOutput;
			}
		}

		return nullptr;
	}


	namespace platform
	{

		std::string _win32GetAdapterOutputName( const std::string & pAdapterRegistryKey )
		{
			auto lastSepPos = pAdapterRegistryKey.find_last_of( '\\', 0 );
			return pAdapterRegistryKey.substr( 0, lastSepPos );
		}

	}
	
} // namespace ts3::system
#endif // TS3_PCL_TARGET_SYSAPI_WIN32
