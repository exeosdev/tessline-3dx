
#include <ts3/system/displayDriverNative.h>

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_WIN32 )
namespace ts3::system
{

    //	void DisplayManager::_sysQueryDisplaySize( DisplaySize & pDisplaySize ) const
    //	{
    //		auto screenWidth = ::GettemMetrics( SM_CXSCREEN );
    //		auto screenHeight = ::GettemMetrics( SM_CYSCREEN );
    //		pDisplaySize.x = static_cast<uint32>( screenWidth );
    //		pDisplaySize.y = static_cast<uint32>( screenHeight );
    //	}
    //
    //	void DisplayManager::_sysQueryMinWindowSize( DisplaySize & pMinWindowSize ) const
    //	{
    //		auto minWindowWidth = ::GettemMetrics( SM_CXMIN );
    //		auto minWindowHeight = ::GettemMetrics( SM_CYMIN );
    //		pMinWindowSize.x = static_cast<uint32>( minWindowWidth );
    //		pMinWindowSize.y = static_cast<uint32>( minWindowHeight );
    //	}

    // Returns a pointer to an existing adapter with a specified UUID (DeviceKey).
    DisplayAdapterNativeImpl * _win32FindAdapterByUUID( DisplayDriverNativeImplGeneric & pDriver, const std::string & pUUID );

    // Returns a pointer to an existing output of a specified adapter with a given output name (DeviceID);
    DisplayOutputNativeImpl * _win32FindOutputForDeviceID( DisplayAdapterNativeImpl & pAdapter, const char * pDeviceID );

    // Returns a name for an output by extracting the output part from the DisplayDevice registry key.
    std::string _win32GetAdapterOutputName( const std::string & pAdapterRegistryKey );


    DisplayDriverNativeImplGeneric::DisplayDriverNativeImplGeneric( DisplayManager * pDisplayManager )
    : DisplayDriverNativeImpl( pDisplayManager, EDisplayDriverType::Generic )
    {}

    DisplayDriverNativeImplGeneric::~DisplayDriverNativeImplGeneric() = default;

    void DisplayDriverNativeImplGeneric::_drvInitialize()
    {}

    void DisplayDriverNativeImplGeneric::_drvRelease()
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
    void DisplayDriverNativeImplGeneric::_drvEnumAdapters()
    {
        // Represents information about a display device in the system. String properties have the following meaning:
        // ::DeviceID - PCI-specific ID, not really interesting
        // ::DeviceString - name of the adapter, e.g. "Radeon RX580 Series"
        // ::DeviceName - detailed name of the device, e.g. "\\\\.\\DISPLAY1". For adapters, this is the name of a connected display.
        // ::DeviceKey - device's key in the registry, e.g. "\\Registry\\Machine\\System\\CurrentControlSet\\Control\\Video\\{SOME_UUID}"
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
            auto * adapterObject = _win32FindAdapterByUUID( *this, adapterUUID );

            if( adapterObject == nullptr )
            {
                adapterObject = addAdapter();
                adapterObject->mNativeData.generic->adapterUUID = std::move( adapterUUID );
                adapterObject->mNativeData.generic->adapterName = gdiDeviceInfo.DeviceString;
                adapterObject->mNativeData.generic->displayDeviceID = gdiDeviceInfo.DeviceName;
                adapterObject->mNativeData.generic->gdiDeviceInfo = gdiDeviceInfo;
                adapterObject->mDescPriv.name = _bstr_t( gdiDeviceInfo.DeviceString );

                if( ( gdiDeviceInfo.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE ) == DISPLAY_DEVICE_PRIMARY_DEVICE )
                {
                    adapterObject->mDescPriv.flags.set( E_DISPLAY_ADAPTER_FLAG_PRIMARY_BIT );
                }
            }
        }
    }

    // Monitor enumeration function. Called for each monitor in the system (as a part of EnumDisplayMonitors).
    // Actual initialization must be performed here - EnumDisplayOutput just wraps things up and returns.
    static BOOL CALLBACK _win32MonitorEnumProc( HMONITOR hMonitor, HDC hDC, LPRECT monitorRect, LPARAM enumProcParam )
    {
        auto * adapterObject = reinterpret_cast<DisplayAdapterNativeImpl *>( enumProcParam );

        MONITORINFOEXA gdiMonitorInfo;
        gdiMonitorInfo.cbSize = sizeof( MONITORINFOEXA );

        if( ::GetMonitorInfoA( hMonitor, &gdiMonitorInfo ) != FALSE )
        {
            // Output name matches the DISPLAY_DEVICEA::DeviceName property of its adapter.
            // Find
            auto * outputObject = _win32FindOutputForDeviceID( *adapterObject, gdiMonitorInfo.szDevice );

            if( outputObject != nullptr )
            {
                outputObject->mNativeData.generic->gdiOutputMonitor = hMonitor;
                outputObject->mDescPriv.name = _bstr_t( gdiMonitorInfo.szDevice );
                outputObject->mDescPriv.screenRect.offset.x = gdiMonitorInfo.rcMonitor.left;
                outputObject->mDescPriv.screenRect.offset.y = gdiMonitorInfo.rcMonitor.top;
                outputObject->mDescPriv.screenRect.size.x = gdiMonitorInfo.rcMonitor.right - gdiMonitorInfo.rcMonitor.left;
                outputObject->mDescPriv.screenRect.size.y = gdiMonitorInfo.rcMonitor.bottom - gdiMonitorInfo.rcMonitor.top;
            }
        }

        return TRUE;
    }

    // -- Note on outputs enumeration:
    // Our definition of an output is: "a physical display connected to one of the active adapters". So, in my case
    // (a single AMD GPU with two monitors connected) I should get two outputs. Although EnumDisplayDevices can also
    // be used to get that (if first param is not NULL, but a name of an adapter, it enumerates connected outputs),
    // the information it gives is rather poor (limited to output's name) and there seem to be no obvious way to get
    // an extended info using that name. So, instead, we enumerate all monitors in the system using EnumDisplayMonitors
    // and then use the associated device's name to properly add it.
    void DisplayDriverNativeImplGeneric::_drvEnumOutputs( DisplayAdapterNativeImpl & pAdapter )
    {
        DISPLAY_DEVICEA gdiOutputInfo;
        gdiOutputInfo.cb = sizeof( DISPLAY_DEVICEA );

        for( UINT displayOutputIndex = 0; ; ++displayOutputIndex )
        {
            //
            auto * displayDeviceID = pAdapter.mNativeData.generic->displayDeviceID.c_str();
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

            auto * outputObject = addOutput( pAdapter );
            outputObject->mNativeData.generic->displayDeviceID = displayDeviceID;
            outputObject->mNativeData.generic->outputID = gdiOutputInfo.DeviceName;

            if( ( gdiOutputInfo.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE ) == DISPLAY_DEVICE_PRIMARY_DEVICE )
            {
                outputObject->mDescPriv.flags.set( E_DISPLAY_OUTPUT_FLAG_PRIMARY_BIT );
            }
        }

        BOOL edmResult = ::EnumDisplayMonitors( nullptr, nullptr, _win32MonitorEnumProc, reinterpret_cast<LPARAM>( &pAdapter ) );

        if( edmResult == FALSE )
        {
            throw 0;
        }
    }

    void DisplayDriverNativeImplGeneric::_drvEnumVideoModes( DisplayOutputNativeImpl & pOutput, ColorFormat pColorFormat )
    {
        DEVMODEA displayMode;
        displayMode.dmSize = sizeof( DEVMODEA );
        displayMode.dmDriverExtra = 0;

        dsm_video_settings_hash_t prevSettingsHash = CX_DSM_VIDEO_SETTINGS_HASH_INVALID;

        for( UINT displayModeIndex = 0; ; ++displayModeIndex )
        {
            auto * outputName = pOutput.mNativeData.generic->displayDeviceID.c_str();
            BOOL edsResult = ::EnumDisplaySettingsExA( outputName, displayModeIndex, &displayMode, 0 );

            if( edsResult == FALSE )
            {
                break;
            }

            DisplayVideoSettings videoSettings;
            videoSettings.resolution.x = static_cast<uint32>( displayMode.dmPelsWidth );
            videoSettings.resolution.y = static_cast<uint32>( displayMode.dmPelsHeight );
            videoSettings.refreshRate = static_cast<uint16>( displayMode.dmDisplayFrequency );

            if( ( displayMode.dmDisplayFlags & DM_INTERLACED ) == DM_INTERLACED )
            {
                videoSettings.flags.set( E_DISPLAY_VIDEO_SETTINGS_FLAG_SCAN_INTERLACED_BIT );
            }
            else
            {
                videoSettings.flags.set( E_DISPLAY_VIDEO_SETTINGS_FLAG_SCAN_PROGRESSIVE_BIT );
            }

            auto settingsHash = dsmComputeVideoSettingsHash( pColorFormat, videoSettings );
            if( settingsHash == prevSettingsHash )
            {
                continue;
            }

            auto * videoModeObject = addVideoMode( pOutput, pColorFormat );
            videoModeObject->mNativeData.generic->gdiModeInfo = displayMode;
            videoModeObject->mDescPriv.settings = videoSettings;
            videoModeObject->mDescPriv.settingsHash = settingsHash;

            prevSettingsHash = settingsHash;
        }
    }

    DisplayAdapterNativeImpl * _win32FindAdapterByUUID( DisplayDriverNativeImplGeneric & pDriver, const std::string & pUUID )
    {
        auto adapterIter = std::find_if( pDriver.mAdapterStorage.begin(),
                                         pDriver.mAdapterStorage.end(),
                                         [&pUUID]( const DisplayAdapterNativeImpl & pAdapter ) -> bool {
                                             return pAdapter.mNativeData.generic->adapterUUID == pUUID;
                                         });

        return ( adapterIter != pDriver.mAdapterStorage.end() ) ? &( *adapterIter ) : nullptr;
    }

    DisplayOutputNativeImpl * _win32FindOutputForDeviceID( DisplayAdapterNativeImpl & pAdapter, const char * pDeviceID )
    {
        auto outputIter = std::find_if( pAdapter.mOutputStorage.begin(),
                                        pAdapter.mOutputStorage.end(),
                                        [pDeviceID]( const DisplayOutputNativeImpl & pOutput ) -> bool {
                                            return pOutput.mNativeData.generic->displayDeviceID == pDeviceID;
                                        });

        return ( outputIter != pAdapter.mOutputStorage.end() ) ? &( *outputIter ) : nullptr;
    }

    std::string _win32GetAdapterOutputName( const std::string & pAdapterRegistryKey )
    {
        auto lastSepPos = pAdapterRegistryKey.find_last_of( '\\', 0 );
        return pAdapterRegistryKey.substr( 0, lastSepPos );
    }
    
} // namespace ts3::system
#endif
