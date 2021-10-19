
#include "androidDisplaySystem.h"
#include "androidWindowSystem.h"
#include <ts3/stdext/mapUtils.h>
#include <map>

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_ANDROID )
namespace ts3::system
{

    namespace platform
    {

        EColorFormat _androidTranslateAHWBufferFormatToColorFormat( AHardwareBuffer_Format pAHWBufferFormat );

    }


    AndroidDisplayAdapter::AndroidDisplayAdapter( AndroidDisplayDriver & pDisplayDriver )
    : AndroidNativeObject( pDisplayDriver )
    {}

    AndroidDisplayAdapter::~AndroidDisplayAdapter() noexcept = default;


    AndroidDisplayOutput::AndroidDisplayOutput( AndroidDisplayAdapter & pDisplayAdapter )
    : AndroidNativeObject( pDisplayAdapter )
    {}

    AndroidDisplayOutput::~AndroidDisplayOutput() noexcept = default;


    AndroidDisplayVideoMode::AndroidDisplayVideoMode( AndroidDisplayOutput & pDisplayOutput )
    : AndroidNativeObject( pDisplayOutput )
    {}

    AndroidDisplayVideoMode::~AndroidDisplayVideoMode() noexcept = default;


    AndroidDisplayManager::AndroidDisplayManager( SysContextHandle pSysContext )
    : AndroidNativeObject( std::move( pSysContext ) )
    {}

    AndroidDisplayManager::~AndroidDisplayManager() noexcept = default;

    DisplayDriverHandle AndroidDisplayManager::_nativeCreateDisplayDriver()
    {
        return createSysObject<AndroidDisplayDriver>( getHandle<AndroidDisplayManager>() );
    }

    void AndroidDisplayManager::_nativeQueryDefaultDisplaySize( DisplaySize & pOutSize ) const
    {
        auto & aSessionData = platform::androidGetASessionData( *this );
        pOutSize = platform::androidQueryNativeWindowSize( aSessionData.aNativeWindow );
    }

    void AndroidDisplayManager::_nativeQueryMinWindowSize( DisplaySize & pOutSize ) const
    {
        auto & aSessionData = platform::androidGetASessionData( *this );
        pOutSize = platform::androidQueryNativeWindowSize( aSessionData.aNativeWindow );
    }


    AndroidDisplayDriver::AndroidDisplayDriver( AndroidDisplayManagerHandle pDisplayManager )
    : AndroidNativeObject( std::move( pDisplayManager ), EDisplayDriverType::Generic )
    {}

    AndroidDisplayDriver::~AndroidDisplayDriver() noexcept = default;

    void AndroidDisplayDriver::_nativeEnumDisplayDevices()
    {
        auto adapterObject = createAdapter<AndroidDisplayAdapter>( *this );
        auto & adapterDesc = adapterObject->getAdapterDescInternal();
        adapterDesc.name = "ANDROID_DEFAULT_ADAPTER";
        adapterDesc.vendorID = EDisplayAdapterVendorID::Google;
        adapterDesc.flags.set( E_DISPLAY_ADAPTER_FLAG_ACTIVE_BIT );
        adapterDesc.flags.set( E_DISPLAY_ADAPTER_FLAG_PRIMARY_BIT );

        auto outputObject = adapterObject->createOutput<AndroidDisplayOutput>( *adapterObject );
        auto & outputDesc = outputObject->getOutputDescInternal();
        outputDesc.name = "ANDROID_DEFAULT_DISPLAY";
        outputDesc.screenRect.offset.x = 0;
        outputDesc.screenRect.offset.y = 0;
        outputDesc.screenRect.size = mDisplayManager->queryDefaultDisplaySize();
    }

    void AndroidDisplayDriver::_nativeEnumVideoModes( DisplayOutput & pOutput, EColorFormat pColorFormat )
    {
        auto & aSessionData = platform::androidGetASessionData( *this );

        auto aWindowFormat = ANativeWindow_getFormat( aSessionData.aNativeWindow );
        auto aHWBufferFormat = static_cast<AHardwareBuffer_Format>( aWindowFormat );
        auto colorFormat = platform::_androidTranslateAHWBufferFormatToColorFormat( aHWBufferFormat );

        if( pColorFormat != colorFormat )
        {
            return;
        }

        auto * androidDisplayOutput = pOutput.queryInterface<AndroidDisplayOutput>();
        auto videoModeObject = androidDisplayOutput->createVideoMode<AndroidDisplayVideoMode>( *androidDisplayOutput, pColorFormat );

        auto & videoModeDesc = videoModeObject->getModeDescInternal();
        videoModeDesc.settings.resolution = pOutput.getOutputDesc().screenRect.size;
        videoModeDesc.settings.refreshRate = 60;
        videoModeDesc.settings.flags.set( E_DISPLAY_VIDEO_SETTINGS_FLAG_SCAN_PROGRESSIVE_BIT );
        videoModeDesc.settingsHash = dsmComputeVideoSettingsHash( pColorFormat, videoModeDesc.settings );
    }

    EColorFormat AndroidDisplayDriver::_nativeQueryDefaultSystemColorFormat() const
    {
        auto & aSessionData = platform::androidGetASessionData( *this );

        auto aWindowFormat = ANativeWindow_getFormat( aSessionData.aNativeWindow );
        auto aHWBufferFormat = static_cast<AHardwareBuffer_Format>( aWindowFormat );
        auto colorFormat = platform::_androidTranslateAHWBufferFormatToColorFormat( aHWBufferFormat );

        return colorFormat;
    }

    namespace platform
    {

        EColorFormat _androidTranslateAHWBufferFormatToColorFormat( AHardwareBuffer_Format pAHWBufferFormat )
        {
            static const std::map<AHardwareBuffer_Format, EColorFormat> colorDescMap =
            {
                { AHARDWAREBUFFER_FORMAT_R8G8B8A8_UNORM,    EColorFormat::R8G8B8A8 },
                { AHARDWAREBUFFER_FORMAT_R8G8B8X8_UNORM,    EColorFormat::R8G8B8X8 },
                { AHARDWAREBUFFER_FORMAT_R10G10B10A2_UNORM, EColorFormat::R10G10B10A2 }
            };
            return getMapValueOrDefault( colorDescMap, pAHWBufferFormat, EColorFormat::Unknown );
        }

    }

} // namespace ts3::system
#endif // TS3_PCL_TARGET_SYSAPI_ANDROID
