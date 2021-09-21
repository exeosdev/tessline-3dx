
#include <ts3/system/displayNative.h>
#include <ts3/stdext/stringUtils.h>

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_WIN32 )
namespace ts3::system
{

    //
    static BOOL CALLBACK _win32MonitorEnumProc( HMONITOR pMonitorHandle, HDC pHDC, LPRECT pMonitorHandleRect, LPARAM pUserParam );

    // Returns a pointer to an existing adapter with a specified UUID (DeviceKey).
    static DisplayAdapter * _win32FindAdapterByUUID( DisplayDriverGeneric & pDriver, const std::string & pUUID );

    // Returns a pointer to an existing output of a specified adapter with a given output name (DeviceID);
    static DisplayOutput * _win32FindOutputForDisplayDeviceName( DisplayDriverGeneric & pDriver, const char * pDeviceName );

    // Returns a pointer to an existing output of a specified adapter with a given output name (DeviceID);
    static DisplayOutput * _win32FindOutputForDisplayDeviceName( DisplayAdapter & pAdapter, const char * pDeviceName );

    // Returns a name for an output by extracting the output part from the DisplayDevice registry key.
    static std::string _win32GetAdapterOutputName( const std::string & pAdapterRegistryKey );


    void DisplayManager::_nativeCtor()
    {}

    void DisplayManager::_nativeDtor() noexcept
    {}

    void DisplayManager::_nativeQueryMinWindowSize( DisplaySize & pOutSize ) const
    {
        auto cxMin = ::GetSystemMetrics( SM_CXMIN );
        auto cyMin = ::GetSystemMetrics( SM_CYMIN );
        pOutSize.x = static_cast<uint32>( cxMin );
        pOutSize.y = static_cast<uint32>( cyMin );
    }

    void DisplayManager::_nativeQueryDefaultDisplaySize( DisplaySize & pOutSize ) const
    {
        auto cxScreen = ::GetSystemMetrics( SM_CXSCREEN );
        auto cyScreen = ::GetSystemMetrics( SM_CYSCREEN );
        pOutSize.x = static_cast<uint32>( cxScreen );
        pOutSize.y = static_cast<uint32>( cyScreen );
    }


    void DisplayDriverGeneric::_nativeCtor()
    {}

    void DisplayDriverGeneric::_nativeDtor() noexcept
    {}

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
    void DisplayDriverGeneric::_drvEnumDisplayDevices()
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
            auto * adapterObject = _win32FindAdapterByUUID( *this, adapterUUID );

            if( adapterObject == nullptr )
            {
                adapterObject = addAdapter();
                auto & adapterNativeData = dsmGetObjectNativeDataGeneric( *adapterObject );
                auto & adapterDesc = dsmGetObjectDesc( *adapterObject );
                
                adapterNativeData.deviceUUID = std::move( adapterUUID );
                adapterNativeData.deviceName = adapterInfoGDI.DeviceName;
                adapterDesc.name = adapterInfoGDI.DeviceString;

                if( makeBitmask( adapterInfoGDI.StateFlags ).isSet( DISPLAY_DEVICE_ATTACHED_TO_DESKTOP ) )
                {
                    adapterDesc.flags.set( E_DISPLAY_ADAPTER_FLAG_ACTIVE_BIT );
                }
                if( makeBitmask( adapterInfoGDI.StateFlags ).isSet( DISPLAY_DEVICE_PRIMARY_DEVICE ) )
                {
                    adapterDesc.flags.set( E_DISPLAY_ADAPTER_FLAG_PRIMARY_BIT );
                }
            }

            for( UINT adapterOutputIndex = 0; ; ++adapterOutputIndex )
            {
                //
                enumResult = ::EnumDisplayDevicesA( adapterInfoGDI.DeviceName, adapterOutputIndex, &outputInfoGDI, 0 );
                if( enumResult == FALSE )
                {
                    break;
                }

                auto * outputObject = addOutput( *adapterObject );

                auto & outputNativeData = dsmGetObjectNativeDataGeneric( *outputObject );
                outputNativeData.displayDeviceName = adapterInfoGDI.DeviceName;
                outputNativeData.outputID = outputInfoGDI.DeviceName;

                // NOTE: 'DISPLAY_DEVICE_PRIMARY_DEVICE' flag is not set in case of output devices (unlike adapters).
                // Primary output can be detected by analysing at monitor flags and looking for MONITORINFOF_PRIMARY.
                // See _win32MonitorEnumProc function above where this gets done.
                if( makeBitmask( outputInfoGDI.StateFlags ).isSet( DISPLAY_DEVICE_ATTACHED_TO_DESKTOP ) )
                {
                    outputObject->mPrivate->descPriv.flags.set( E_DISPLAY_OUTPUT_FLAG_ACTIVE_BIT );
                }
            }
        }

        ::EnumDisplayMonitors( nullptr, nullptr, _win32MonitorEnumProc, reinterpret_cast<LPARAM>( this ) );
    }

    // Monitor enumeration function. Called for each monitor in the system (as a part of _nativeEnumOutputs).
    // This function is called after all output devices in the system have been enumerated and added to the
    // internal list inside the adapter. IMPORTANT: again, this gets called *within* _nativeEnumOutputs()
    // function *per each adapter* (that's why we pass DisplayAdapter* and check for monitors connected to it.
    BOOL CALLBACK _win32MonitorEnumProc( HMONITOR pMonitorHandle, HDC pHDC, LPRECT pMonitorHandleRect, LPARAM pUserParam )
    {
        // Adapter object for which we are enumerating outputs/monitors.
        auto * displayDriver = reinterpret_cast<DisplayDriverGeneric *>( pUserParam );

        MONITORINFOEXA gdiMonitorInfo;
        gdiMonitorInfo.cbSize = sizeof( MONITORINFOEXA );

        if( ::GetMonitorInfoA( pMonitorHandle, &gdiMonitorInfo ) != FALSE )
        {
            // 'szDevice' of the MONITORINFOEXA matches the DISPLAY_DEVICEA::DeviceName property of the output.
            // Thanks to that, we can obtain the output this monitor refers to.
            auto * outputObject = _win32FindOutputForDisplayDeviceName( *displayDriver, gdiMonitorInfo.szDevice );
            auto & outputNativeData = dsmGetObjectNativeDataGeneric( *outputObject );
            auto & outputDesc = dsmGetObjectDesc( *outputObject );

            outputNativeData.gdiMonitorHandle = pMonitorHandle;
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

        return TRUE;
    }

    void DisplayDriverGeneric::_drvEnumVideoModes( DisplayOutput & pOutput, ColorFormat pColorFormat )
    {
        const auto & outputNativeData = dsmGetObjectNativeDataGeneric( pOutput );
        const auto & colorFormatDesc = vsxGetDescForColorFormat( pColorFormat );

        if( colorFormatDesc.colorSpace != ColorSpace::Linear )
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
            auto * displayDeviceNameStr = outputNativeData.displayDeviceName.c_str();

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

            auto * videoModeObject = addVideoMode( pOutput, pColorFormat );
            auto & videoModeNativeData = dsmGetObjectNativeDataGeneric( *videoModeObject );
            auto & videoModeDesc = dsmGetObjectDesc( *videoModeObject );
            
            videoModeNativeData.gdiModeInfo = gdiDevMode;
            videoModeDesc.settings = videoSettings;
            videoModeDesc.settingsHash = settingsHash;

            lastSettingsHash = settingsHash;
        }
    }

    ColorFormat DisplayDriverGeneric::_drvQueryDefaultSystemColorFormat() const
    {
        return ColorFormat::B8G8R8A8;
    }


    DisplayAdapter * _win32FindAdapterByUUID( DisplayDriverGeneric & pDriver, const std::string & pUUID )
    {
        auto adapterIter = std::find_if(
            pDriver.mPrivate->adapterInternalStorage.begin(),
            pDriver.mPrivate->adapterInternalStorage.end(),
            [&pUUID]( const DisplayAdapter & pAdapter ) -> bool {
                return pAdapter.mNativeData->generic->deviceUUID == pUUID;
            });

        return ( adapterIter != pDriver.mPrivate->adapterInternalStorage.end() ) ? &( *adapterIter ) : nullptr;
    }

    DisplayOutput * _win32FindOutputForDisplayDeviceName( DisplayDriverGeneric & pDriver, const char * pDeviceName )
    {
        for( auto & adapter : pDriver.mPrivate->adapterInternalStorage )
        {
            if( auto * adapterOutput = _win32FindOutputForDisplayDeviceName( adapter, pDeviceName ) )
            {
                return adapterOutput;
            }
        }

        return nullptr;
    }

    DisplayOutput * _win32FindOutputForDisplayDeviceName( DisplayAdapter & pAdapter, const char * pDeviceName )
    {
        auto outputIter = std::find_if(
            pAdapter.mPrivate->outputInternalStorage.begin(),
            pAdapter.mPrivate->outputInternalStorage.end(),
            [pDeviceName]( const DisplayOutput & pOutput ) -> bool {
                return pOutput.mNativeData->generic->displayDeviceName == pDeviceName;
            });

        return ( outputIter != pAdapter.mPrivate->outputInternalStorage.end() ) ? &( *outputIter ) : nullptr;
    }

    std::string _win32GetAdapterOutputName( const std::string & pAdapterRegistryKey )
    {
        auto lastSepPos = pAdapterRegistryKey.find_last_of( '\\', 0 );
        return pAdapterRegistryKey.substr( 0, lastSepPos );
    }
    
} // namespace ts3::system
#endif
