
#include <ts3/system/displayNative.h>
#include <ts3/system/windowNative.h>
#include <ts3/stdext/mapUtils.h>

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_ANDROID )
namespace ts3::system
{

    static EColorFormat _androidTranslateAHWBufferFormatToColorFormat( AHardwareBuffer_Format pAHWBufferFormat );


    void DisplayManager::_nativeConstructor()
	{
	}

    void DisplayManager::_nativeDestructor() noexcept
	{
	}

    void DisplayManager::_nativeQueryMinWindowSize( DisplaySize & pOutSize ) const
	{
        auto & aSessionData = nativeAndroidGetASessionData( *mSysContext );
        pOutSize = nativeAndroidQueryNativeWindowSize( aSessionData.aNativeWindow );
	}

    void DisplayManager::_nativeQueryDefaultDisplaySize( DisplaySize & pOutSize ) const
	{
        auto & aSessionData = nativeAndroidGetASessionData( *mSysContext );
        pOutSize = nativeAndroidQueryNativeWindowSize( aSessionData.aNativeWindow );
	}



    void DisplayDriverGeneric::_nativeConstructor()
    {
    }

    void DisplayDriverGeneric::_nativeDestructor() noexcept
    {
    }

    void DisplayDriverGeneric::_drvEnumDisplayDevices()
    {
        auto & driverNativeData = dsmGetObjectNativeDataGeneric( *this );
        auto & aSessionData = driverNativeData.getSessionData();

        auto * adapterObject = addAdapter();
        auto & adapterNativeData = dsmGetObjectNativeDataGeneric( *adapterObject );
        adapterNativeData.setSessionData( aSessionData );
        auto & adapterDesc = dsmGetObjectDesc( *adapterObject );
        adapterDesc.name = "ANDROID_DEFAULT_ADAPTER";
        adapterDesc.vendorID = EDisplayAdapterVendorID::Google;
        adapterDesc.flags.set( E_DISPLAY_ADAPTER_FLAG_ACTIVE_BIT );
        adapterDesc.flags.set( E_DISPLAY_ADAPTER_FLAG_PRIMARY_BIT );

        auto * outputObject = addOutput( *adapterObject );
        auto & outputNativeData = dsmGetObjectNativeDataGeneric( *outputObject );
        outputNativeData.setSessionData( aSessionData );
        auto & outputDesc = dsmGetObjectDesc( *outputObject );
        outputDesc.name = "ANDROID_DEFAULT_DISPLAY";
        outputDesc.screenRect.offset.x = 0;
        outputDesc.screenRect.offset.y = 0;
        outputDesc.screenRect.size = nativeAndroidQueryNativeWindowSize( aSessionData.aNativeWindow );
    }

    void DisplayDriverGeneric::_drvEnumVideoModes( DisplayOutput & pOutput, EColorFormat pColorFormat )
    {
        auto & driverNativeData = dsmGetObjectNativeDataGeneric( *this );
        auto & aSessionData = driverNativeData.getSessionData();

        auto aWindowFormat = ANativeWindow_getFormat( aSessionData.aNativeWindow );
        auto aHWBufferFormat = static_cast<AHardwareBuffer_Format>( aWindowFormat );
        auto colorFormat = _androidTranslateAHWBufferFormatToColorFormat( aHWBufferFormat );

        if( pColorFormat != colorFormat )
        {
            return;
        }

        const auto & outputDesc = dsmGetObjectDesc( pOutput );

        auto * videoModeObject = addVideoMode( pOutput, pColorFormat );
        auto & videoModeNativeData = dsmGetObjectNativeDataGeneric( *videoModeObject );
        videoModeNativeData.setSessionData( aSessionData );
        auto & videoModeDesc = dsmGetObjectDesc( *videoModeObject );
        videoModeDesc.settings.resolution = outputDesc.screenRect.size;
        videoModeDesc.settings.refreshRate = 60;
        videoModeDesc.settings.flags.set( E_DISPLAY_VIDEO_SETTINGS_FLAG_SCAN_PROGRESSIVE_BIT );
        videoModeDesc.settingsHash = dsmComputeVideoSettingsHash( pColorFormat, videoModeDesc.settings );
    }

    EColorFormat DisplayDriverGeneric::_drvQueryDefaultSystemColorFormat() const
    {
        auto & driverNativeData = dsmGetObjectNativeDataGeneric( *this );
        auto & aSessionData = driverNativeData.getSessionData();

        auto aWindowFormat = ANativeWindow_getFormat( aSessionData.aNativeWindow );
        auto aHWBufferFormat = static_cast<AHardwareBuffer_Format>( aWindowFormat );
        auto colorFormat = _androidTranslateAHWBufferFormatToColorFormat( aHWBufferFormat );

        return colorFormat;
    }

    EColorFormat _androidTranslateAHWBufferFormatToColorFormat( AHardwareBuffer_Format pAHWBufferFormat )
    {
        static const std::unordered_map<AHardwareBuffer_Format, EColorFormat> colorDescMap =
        {
            { AHARDWAREBUFFER_FORMAT_R8G8B8A8_UNORM,    EColorFormat::R8G8B8A8 },
            { AHARDWAREBUFFER_FORMAT_R8G8B8X8_UNORM,    EColorFormat::R8G8B8X8 },
            { AHARDWAREBUFFER_FORMAT_R10G10B10A2_UNORM, EColorFormat::R10G10B10A2 }
        };
        return getMapValueOrDefault( colorDescMap, pAHWBufferFormat, EColorFormat::Unknown );
    }

} // namespace ts3::system
#endif // TS3_PCL_TARGET_SYSAPI_ANDROID
