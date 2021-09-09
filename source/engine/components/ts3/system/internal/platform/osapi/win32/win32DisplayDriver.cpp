
#include <ts3/system/displayDriverNative.h>
#include <ts3/stdext/stringUtils.h>

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_WIN32 )
namespace ts3::system
{

    // Returns a pointer to an existing adapter with a specified UUID (DeviceKey).
    static DisplayAdapter * _win32FindAdapterByUUID( DisplayDriverGeneric & pDriver, const std::string & pUUID );

    // Returns a pointer to an existing output of a specified adapter with a given output name (DeviceID);
    static DisplayOutput * _win32FindOutputForDeviceID( DisplayAdapter & pAdapter, const char * pDeviceID );

    // Returns a name for an output by extracting the output part from the DisplayDevice registry key.
    static std::string _win32GetAdapterOutputName( const std::string & pAdapterRegistryKey );


    DisplayDriverGeneric::DisplayDriverGeneric( DisplayManager * pDisplayManager )
    : DisplayDriver( pDisplayManager, EDisplayDriverType::Generic )
    {}

    DisplayDriverGeneric::~DisplayDriverGeneric() = default;

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
    void DisplayDriverGeneric::_nativeEnumAdapters()
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

            const std::string oiDeviceID  = gdiDeviceInfo.DeviceID;
            const std::string oiDeviceKey = gdiDeviceInfo.DeviceKey;
            const std::string oiDeviceName = gdiDeviceInfo.DeviceName;
            const std::string oiDeviceStr = gdiDeviceInfo.DeviceString;

            // Extract adapter UUID from the registry key. Devices referring to the same adapter have the same adapter UUID.
            auto adapterUUID = getUUIDString( gdiDeviceInfo.DeviceKey );

            // Check if that adapter has been already added to the list of adapters (if there was already another device which referred to it).
            auto * adapterObject = _win32FindAdapterByUUID( *this, adapterUUID );

            if( adapterObject == nullptr )
            {
                adapterObject = addAdapter();
                adapterObject->mPrivate->nativeDataPriv.generic->adapterUUID = std::move( adapterUUID );
                adapterObject->mPrivate->nativeDataPriv.generic->adapterName = gdiDeviceInfo.DeviceString;
                adapterObject->mPrivate->nativeDataPriv.generic->displayDeviceID = gdiDeviceInfo.DeviceName;
                adapterObject->mPrivate->nativeDataPriv.generic->gdiDeviceInfo = gdiDeviceInfo;
                adapterObject->mPrivate->descPriv.name = strUtils::convertStringRepresentation<char>( gdiDeviceInfo.DeviceString );

                auto gdiAdapterFlags = makeBitmask( gdiDeviceInfo.StateFlags );
                if( gdiAdapterFlags.isSet( DISPLAY_DEVICE_ATTACHED_TO_DESKTOP ) )
                {
                    adapterObject->mPrivate->descPriv.flags.set( E_DISPLAY_ADAPTER_FLAG_ACTIVE_BIT );
                }
                if( gdiAdapterFlags.isSet( DISPLAY_DEVICE_PRIMARY_DEVICE ) )
                {
                    adapterObject->mPrivate->descPriv.flags.set( E_DISPLAY_ADAPTER_FLAG_PRIMARY_BIT );
                }
            }
        }
    }

    // Monitor enumeration function. Called for each monitor in the system (as a part of _nativeEnumOutputs).
    // This function is called after all output devices in the system have been enumerated and added to the
    // internal list inside the adapter. IMPORTANT: again, this gets called *within* _nativeEnumOutputs()
    // function *per each adapter* (that's why we pass DisplayAdapter* and check for monitors connected to it.
    BOOL CALLBACK DisplayDriverGeneric::_win32MonitorEnumProc( HMONITOR hMonitor, HDC hDC, LPRECT monitorRect, LPARAM enumProcParam )
    {
        // Adapter object for which we are enumerating outputs/monitors.
        auto * adapterObject = reinterpret_cast<DisplayAdapter *>( enumProcParam );

        MONITORINFOEXA gdiMonitorInfo;
        gdiMonitorInfo.cbSize = sizeof( MONITORINFOEXA );

        if( ::GetMonitorInfoA( hMonitor, &gdiMonitorInfo ) != FALSE )
        {
            // 'szDevice' of the MONITORINFOEXA matches the DISPLAY_DEVICEA::DeviceName property of the output.
            // Thanks to that, we can obtain the output this monitor refers to.
            auto * outputObject = _win32FindOutputForDeviceID( *adapterObject, gdiMonitorInfo.szDevice );

            if( !outputObject )
            {
                auto * thisPtr = static_cast<DisplayDriverGeneric *>( adapterObject->mDisplayDriver );
                outputObject = thisPtr->addOutput( *adapterObject );

                auto & outputNativeData = dsmGetObjectNativeDataGeneric( *outputObject );
                outputNativeData.outputID = gdiMonitorInfo.szDevice;

                auto & adapterNativeData = dsmGetObjectNativeDataGeneric( *adapterObject );
                outputNativeData.displayDeviceID = adapterNativeData.displayDeviceID;
            }

            outputObject->mPrivate->nativeDataPriv.generic->gdiOutputMonitor = hMonitor;
            outputObject->mPrivate->descPriv.name = strUtils::convertStringRepresentation<char>( gdiMonitorInfo.szDevice );
            outputObject->mPrivate->descPriv.screenRect.offset.x = gdiMonitorInfo.rcMonitor.left;
            outputObject->mPrivate->descPriv.screenRect.offset.y = gdiMonitorInfo.rcMonitor.top;
            outputObject->mPrivate->descPriv.screenRect.size.x = gdiMonitorInfo.rcMonitor.right - gdiMonitorInfo.rcMonitor.left;
            outputObject->mPrivate->descPriv.screenRect.size.y = gdiMonitorInfo.rcMonitor.bottom - gdiMonitorInfo.rcMonitor.top;

            if( makeBitmask( gdiMonitorInfo.dwFlags ).isSet( MONITORINFOF_PRIMARY ) )
            {
                outputObject->mPrivate->descPriv.flags.set( E_DISPLAY_OUTPUT_FLAG_PRIMARY_BIT );
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
    void DisplayDriverGeneric::_nativeEnumOutputs( DisplayAdapter & pAdapter )
    {
        auto & adapterNativeData = dsmGetObjectNativeDataGeneric( pAdapter );
        //
        auto * displayDeviceID = adapterNativeData.displayDeviceID.c_str();

        DISPLAY_DEVICEA gdiOutputInfo;
        gdiOutputInfo.cb = sizeof( DISPLAY_DEVICEA );

        for( UINT displayOutputIndex = 0; ; ++displayOutputIndex )
        {
            //
            BOOL result = ::EnumDisplayDevicesA( displayDeviceID, displayOutputIndex, &gdiOutputInfo, 0 );

            if( result == FALSE )
            {
                break;
            }

            auto * outputObject = addOutput( pAdapter );
            auto & outputNativeData = dsmGetObjectNativeDataGeneric( *outputObject );
            outputNativeData.displayDeviceID = displayDeviceID;
            outputNativeData.outputID = gdiOutputInfo.DeviceName;

            const std::string oiDeviceID  = gdiOutputInfo.DeviceID;
            const std::string oiDeviceKey = gdiOutputInfo.DeviceKey;
            const std::string oiDeviceName = gdiOutputInfo.DeviceName;
            const std::string oiDeviceStr = gdiOutputInfo.DeviceString;

            auto outputFlags = makeBitmask( gdiOutputInfo.StateFlags );
            if( outputFlags.isSet( DISPLAY_DEVICE_ATTACHED_TO_DESKTOP ) )
            {
                outputObject->mPrivate->descPriv.flags.set( E_DISPLAY_OUTPUT_FLAG_ACTIVE_BIT );
            }
            // NOTE: 'DISPLAY_DEVICE_PRIMARY_DEVICE' flag is not set in case of output devices (unlike adapters).
            // Primary output can be detected by analysing at monitor flags and looking for MONITORINFOF_PRIMARY.
            // See _win32MonitorEnumProc function above where this gets done.
        }

        BOOL edmResult = ::EnumDisplayMonitors( nullptr, nullptr, _win32MonitorEnumProc, reinterpret_cast<LPARAM>( &pAdapter ) );

        if( edmResult == FALSE )
        {
            throw 0;
        }
    }

    void DisplayDriverGeneric::_nativeEnumVideoModes( DisplayOutput & pOutput, ColorFormat pColorFormat )
    {
        auto & outputNativeData = dsmGetObjectNativeDataGeneric( pOutput );
        auto * outputName = outputNativeData.displayDeviceID.c_str();

        const auto & colorFormatDesc = vsxGetDescForColorFormat( pColorFormat );

        DEVMODEA gdiDevMode;
        gdiDevMode.dmSize = sizeof( DEVMODEA );
        gdiDevMode.dmDriverExtra = 0;

        dsm_video_settings_hash_t prevSettingsHash = CX_DSM_VIDEO_SETTINGS_HASH_INVALID;

        for( UINT displayModeIndex = 0; ; ++displayModeIndex )
        {
            BOOL edsResult = ::EnumDisplaySettingsExA( outputName, displayModeIndex, &gdiDevMode, 0 );

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
            if( settingsHash == prevSettingsHash )
            {
                continue;
            }

            auto * videoModeObject = addVideoMode( pOutput, pColorFormat );

            auto & videoModeNativeData = dsmGetObjectNativeDataGeneric( *videoModeObject );
            videoModeNativeData.gdiModeInfo = gdiDevMode;

            auto & videoModeDesc = dsmGetObjectDesc( *videoModeObject );
            videoModeDesc.settings = videoSettings;
            videoModeDesc.settingsHash = settingsHash;

            prevSettingsHash = settingsHash;
        }
    }
    
    void DisplayDriverGeneric::_nativeDestroyAdapter( DisplayAdapter & pAdapter )
    {
        ( pAdapter );
    }

    void DisplayDriverGeneric::_nativeDestroyOutput( DisplayOutput & pOutput )
    {
        ( pOutput );
    }

    void DisplayDriverGeneric::_nativeDestroyVideoMode( DisplayVideoMode & pVideoMode )
    {
        ( pVideoMode );
    }

    ColorFormat DisplayDriverGeneric::_nativeGetSystemDefaultColorFormat() const
    {
        return ColorFormat::B8G8R8A8;
    }

    ArrayView<const ColorFormat> DisplayDriverGeneric::_nativeGetSupportedColorFormatList() const
    {
        static const ColorFormat sColorFormatArray[] =
        {
            ColorFormat::B8G8R8,
            ColorFormat::B8G8R8A8,
        };
        return bindArrayView( sColorFormatArray );
    }


    DisplayAdapter * _win32FindAdapterByUUID( DisplayDriverGeneric & pDriver, const std::string & pUUID )
    {
        auto adapterIter = std::find_if(
            pDriver.mPrivate->adapterInternalStorage.begin(),
            pDriver.mPrivate->adapterInternalStorage.end(),
            [&pUUID]( const DisplayAdapter & pAdapter ) -> bool {
                return pAdapter.mNativeData->generic->adapterUUID == pUUID;
            });

        return ( adapterIter != pDriver.mPrivate->adapterInternalStorage.end() ) ? &( *adapterIter ) : nullptr;
    }

    DisplayOutput * _win32FindOutputForDeviceID( DisplayAdapter & pAdapter, const char * pDeviceID )
    {
        auto outputIter = std::find_if(
            pAdapter.mPrivate->outputInternalStorage.begin(),
            pAdapter.mPrivate->outputInternalStorage.end(),
            [pDeviceID]( const DisplayOutput & pOutput ) -> bool {
                return pOutput.mNativeData->generic->displayDeviceID == pDeviceID;
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
