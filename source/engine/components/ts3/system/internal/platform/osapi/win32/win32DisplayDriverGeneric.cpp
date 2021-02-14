
#include <ts3/system/displayDriver.h>

#include <functional>
#include <comdef.h> // For _bstr_t

namespace ts3
{

	// This file is heavily documented - Microsoft has never been making our lives easy!
	//
	// -- Note on adapters enumeration:
	// Without the awesome DXGI, EnumDisplayDevices is the only reliable way of enumerating the display stuff.
	// With its first parameter set to NULL, it queries the information about an adapter with the specified index.
	// Adapter is identified as a registry key, however - it is tied with its connected monitors. Example:
	// In my current setup (AMD Radeon RX580 and two 1440p monitors connected via DisplayPort), EnumDisplayDevices
	// gives me two entries with the following keys (DISPLAY_DEVICEA::DeviceKey):
	// 1) \\Registry\\Machine\\System\\CurrentControlSet\\Control\\Video\\{79BD17DD-B591-11EA-B520-AC9E17ECDDE5}\\0000
	// 2) \\Registry\\Machine\\System\\CurrentControlSet\\Control\\Video\\{79BD17DD-B591-11EA-B520-AC9E17ECDDE5}\\0001
	// So, to enumerate adapters properly, we must check the UUID of the adapter to not duplicate the entries.
	// See SysDisplayDriverGenericImplProxy::nativeEnumAdapterList below.
	//
	// -- Note on outputs enumeration:
	// Our definition of an output is: "a physical display connected to one of the active adapters". So, in my case
	// (a single AMD GPU with two monitors connected) I should get two outputs. Although EnumDisplayDevices can also
	// be used to get that (if first param is not NULL, but a name of an adapter, it enumerates connected outputs),
	// the information it gives is rather poor (limited to output's name) and there seem to be no obvious way to get
	// an extended info using that name. So, instead, we enumerate all monitors in the system using EnumDisplayMonitors
	// and then use the associated device's name to properly add it.

	SysDsmAdapter * _win32FindAdapterByUUID( SysDisplayDriverGeneric & pDisplayDriver, const std::string & pUUID );
	SysDsmOutput * _win32FindOutputForDeviceID( SysDsmAdapter * pAdapter, const char * pDeviceID );
	std::string _win32GetAdapterOutputName( const std::string & pAdapterRegistryKey );

	void SysDisplayDriverGeneric::_sysResetInternalState()
	{
	}

	void SysDisplayDriverGeneric::_sysEnumAdapterList()
	{
		// Represents information about a display device in the system. String properties have the following meaning:
		// ::DeviceID - PCI-specific ID, not really interesting
		// ::DeviceString - name of the adapter, i.e. "Radeon RX580 Series"
		// ::DeviceName - detailed name of the device, i.e. "\\\\.\\DISPLAY1" or "\\\\.\\DISPLAY2". For adapters, this is the name of a connected display.
		// ::DeviceKey - device's key in the registry, i.e. "\\Registry\\Machine\\System\\CurrentControlSet\\Control\\Video\\{79BD17DD-B591-11EA-B520-AC9E17ECDDE5}\\0000"
		DISPLAY_DEVICEA gdiDeviceInfo;
		gdiDeviceInfo.cb = sizeof( DISPLAY_DEVICEA );

		for( UINT displayDeviceIndex = 0; ; ++displayDeviceIndex )
		{
			// Enumerate next display device in the system. Multiple devices may refer to the same physical adapter, i.e.:
			// "\\Registry\\Machine\\System\\CurrentControlSet\\Control\\Video\\{79BD17DD-B591-11EA-B520-AC9E17ECDDE5}\\0000"
			// "\\Registry\\Machine\\System\\CurrentControlSet\\Control\\Video\\{79BD17DD-B591-11EA-B520-AC9E17ECDDE5}\\0001"
			// are two different physical outputs (0000 and 0001) connected to the same adapter (UUID={79BD17DD-B591-11EA-B520-AC9E17ECDDE5}).
			BOOL result = ::EnumDisplayDevicesA( nullptr, displayDeviceIndex, &gdiDeviceInfo, 0 );

			// Display devices are indexed starting from 0 up to DEVICES_NUM - 1.
			// If EnumDisplayDevicesA returns FALSE, it means we enumerated all of them already.
			if( result == FALSE )
			{
				break;
			}

			// We care only about adapters with active outputs (i.e. monitors used in the current display topology).
			if( ( gdiDeviceInfo.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP ) == 0 )
			{
				continue;
			}

			// Extract adapter UUID from the registry key. Devices referring to the same adapter have the same adapter UUID.
			auto adapterUUID = getUUIDString( gdiDeviceInfo.DeviceKey );
			// Check if that adapter has been already added to the list of adapters (if there was already another device which referred to it).
			auto * adapterInfo = _win32FindAdapterByUUID( *this, adapterUUID );

			if( adapterInfo == nullptr )
			{
				adapterInfo = registerAdapter();
				adapterInfo->nativeData.generic->adapterUUID = std::move( adapterUUID );
                adapterInfo->nativeData.generic->adapterName = gdiDeviceInfo.DeviceString;
                adapterInfo->nativeData.generic->displayDeviceID = gdiDeviceInfo.DeviceName;
				adapterInfo->nativeData.generic->gdiDeviceInfo = gdiDeviceInfo;
				adapterInfo->adapterDesc.name = _bstr_t( gdiDeviceInfo.DeviceString );

				if( ( gdiDeviceInfo.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE ) == DISPLAY_DEVICE_PRIMARY_DEVICE )
				{
					adapterInfo->adapterDesc.flags.set( E_SYS_DSM_ADAPTER_FLAG_PRIMARY_BIT );
				}
			}
		}
	}

	// Monitor enumeration function. Called for each monitor in the system (as a part of EnumDisplayMonitors).
	// Actual initialization must be performed here - EnumDisplayOutput just wraps things up and returns.
	static BOOL CALLBACK _win32MonitorEnumProc( HMONITOR hMonitor, HDC hDC, LPRECT monitorRect, LPARAM enumProcParam )
	{
		auto * adapterInfo = reinterpret_cast<SysDsmAdapter *>( enumProcParam );

		MONITORINFOEXA gdiMonitorInfo;
		gdiMonitorInfo.cbSize = sizeof( MONITORINFOEXA );

		if( ::GetMonitorInfoA( hMonitor, &gdiMonitorInfo ) != FALSE )
		{
			// Output name matches the DISPLAY_DEVICEA::DeviceName property of its adapter.
			// Find
            auto * outputInfo = _win32FindOutputForDeviceID( adapterInfo, gdiMonitorInfo.szDevice );

			if( outputInfo != nullptr )
			{
				outputInfo->nativeData.generic->gdiOutputMonitor = hMonitor;
				outputInfo->outputDesc.name = _bstr_t( gdiMonitorInfo.szDevice );
				outputInfo->outputDesc.screenRect.offset.x = gdiMonitorInfo.rcMonitor.left;
				outputInfo->outputDesc.screenRect.offset.y = gdiMonitorInfo.rcMonitor.top;
				outputInfo->outputDesc.screenRect.size.x = gdiMonitorInfo.rcMonitor.right - gdiMonitorInfo.rcMonitor.left;
				outputInfo->outputDesc.screenRect.size.y = gdiMonitorInfo.rcMonitor.bottom - gdiMonitorInfo.rcMonitor.top;
			}
		}

		return TRUE;
	}

	void SysDisplayDriverGeneric::_sysEnumOutputList( SysDsmAdapter & pAdapter )
	{
        DISPLAY_DEVICEA gdiOutputInfo;
        gdiOutputInfo.cb = sizeof( DISPLAY_DEVICEA );

        for( UINT displayOutputIndex = 0; ; ++displayOutputIndex )
        {
            //
            auto * displayDeviceID = pAdapter.nativeData.generic->displayDeviceID.c_str();
            //
            BOOL result = ::EnumDisplayDevicesA( displayDeviceID, displayOutputIndex, &gdiOutputInfo, 0 );

            if( result == FALSE )
            {
                break;
            }

            if( ( gdiOutputInfo.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP ) == 0 )
            {
                continue;
            }

            auto * outputInfo = registerOutput( pAdapter );
            outputInfo->nativeData.generic->displayDeviceID = displayDeviceID;
            outputInfo->nativeData.generic->outputID = gdiOutputInfo.DeviceName;

            if( ( gdiOutputInfo.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE ) == DISPLAY_DEVICE_PRIMARY_DEVICE )
            {
                outputInfo->outputDesc.flags.set( E_SYS_DSM_OUTPUT_FLAG_PRIMARY_BIT );
            }
        }

		BOOL edmResult = ::EnumDisplayMonitors( nullptr, nullptr, _win32MonitorEnumProc, reinterpret_cast<LPARAM>( &pAdapter ) );

		if( edmResult == FALSE )
		{
			throw 0;
		}
	}

	void SysDisplayDriverGeneric::_sysEnumVideoModeList( SysDsmOutput & pOutput, SysColorFormat pFormat )
	{
	    DEVMODEA displayMode;
	    displayMode.dmSize = sizeof( DEVMODEA );
		displayMode.dmDriverExtra = 0;

        sys_dsm_video_settings_hash_t prevSettingsHash = cvSysDsmVideoSettingsHashInvalid;

        for( UINT displayModeIndex = 0; ; ++displayModeIndex )
        {
            auto * outputName = pOutput.nativeData.generic->displayDeviceID.c_str();
            BOOL edsResult = ::EnumDisplaySettingsExA( outputName, displayModeIndex, &displayMode, 0 );

            if( edsResult == FALSE )
            {
                break;
            }

            SysDsmVideoSettings videoSettings;
            videoSettings.resolution.x = static_cast<uint32>( displayMode.dmPelsWidth );
            videoSettings.resolution.y = static_cast<uint32>( displayMode.dmPelsHeight );
            videoSettings.refreshRate = static_cast<uint16>( displayMode.dmDisplayFrequency );

            if( ( displayMode.dmDisplayFlags & DM_INTERLACED ) == DM_INTERLACED )
            {
                videoSettings.flags.set( E_SYS_DSM_VIDEO_SETTINGS_FLAG_SCAN_INTERLACED_BIT );
            }
            else
            {
                videoSettings.flags.set( E_SYS_DSM_VIDEO_SETTINGS_FLAG_SCAN_PROGRESSIVE_BIT );
            }

            auto settingsHash = sysDsmComputeVideoSettingsHash( pFormat, videoSettings );
            if( settingsHash == prevSettingsHash )
            {
                continue;
            }

            auto * videoModeInfo = registerVideoMode( pOutput, pFormat );
	        videoModeInfo->nativeData.generic->gdiModeInfo = displayMode;
	        videoModeInfo->modeDesc.settings = videoSettings;
	        videoModeInfo->modeDesc.settingsHash = settingsHash;

            prevSettingsHash = settingsHash;
        }
	}

	SysDsmAdapter * _win32FindAdapterByUUID( SysDisplayDriverGeneric & pDisplayDriver, const std::string & pUUID )
	{
		auto * adapter = pDisplayDriver.findAdapterIf( [&pUUID]( const SysDsmAdapter * pAdapter ) -> bool {
			return pAdapter->nativeData.generic->adapterUUID == pUUID;
		} );
		return ( adapter != nullptr ) ? static_cast<SysDsmAdapter *>( adapter->driverReserved ) : nullptr;
	}

	SysDsmOutput * _win32FindOutputForDeviceID( SysDsmAdapter * pAdapter, const char * pDeviceID )
	{
		auto * outputDesc = pAdapter->driver->findOutputIf( pAdapter->adapterDesc.index, [pDeviceID]( const SysDsmOutput * pOutput ) -> bool {
			return pOutput->nativeData.generic->displayDeviceID == pDeviceID;
		} );
		return ( outputDesc != nullptr ) ? static_cast<SysDsmOutput *>( outputDesc->driverReserved ) : nullptr;
	}

	std::string _win32GetAdapterOutputName( const std::string & pAdapterRegistryKey )
	{
		auto lastSepPos = pAdapterRegistryKey.find_last_of( '\\', 0 );
		auto outputName = pAdapterRegistryKey.substr( 0, lastSepPos );
		return outputName;
	}

}
