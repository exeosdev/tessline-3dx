
#ifndef __TS3_SYSTEM_DISPLAY_DRIVER_NATIVE_H__
#define __TS3_SYSTEM_DISPLAY_DRIVER_NATIVE_H__

#include "display.h"
#include "sysContextNative.h"
#include <ts3/stdext/utilities.h>

#include <deque>
#include <functional>
#include <map>
#include <unordered_map>

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_ANDROID )
#  include "internal/platform/osapi/android/androidDisplay.h"
#elif( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_WIN32 )
#  include "internal/platform/osapi/win32/win32Display.h"
#elif( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_X11 )
#  include "internal/platform/osapi/x11/x11Display.h"
#endif

#if( TS3_SYSTEM_DSM_DRIVER_TYPE_SUPPORT_DXGI )
#  include "internal/platform/shared/dxgi/dxgiDisplay.h"
#endif

#if( TS3_SYSTEM_DSM_DRIVER_TYPE_SUPPORT_SDL )
#  include "internal/platform/shared/sdl/sdlDisplay.h"
#endif

namespace ts3::system
{

    using DisplayDriverFactoryCallback = std::function<DisplayDriverHandle()>;
    using DisplayDriverFactoryMap = std::map<EDisplayDriverType, DisplayDriverFactoryCallback>;

    struct DisplayManager::ObjectPrivateData
    {
        DisplayManagerNativeData nativeDataPriv;
        DisplayDriverFactoryMap driverFactoryMap;
    };

    template <typename TpNativeData>
    void dsmInitializeNativeData( TpNativeData * pNativeData, EDisplayDriverType pDriverType );

    template <typename TpNativeData>
    void dsmReleaseNativeData( TpNativeData * pNativeData );

    // Driver data structs for display driver-related classes.
    // Unlike in the case of other components, display driver has concrete sub-types instead of the usual 'native' layer.
    // Also, the type can be selected at runtime (it is even possible to create many different drivers at the same time).
    // To make this possible, we add one additional level of indirection for native data too. Instead of being defined
    // at the platform-level, each specific driver backend defines its own local XXXNativeDataYYY and all supported types
    // of ND are stored in common NativeData structs defined here. Each NativeData contains set of pointers for all
    // supported driver types (see below).

    // NativeData for DisplayDriver
    struct DisplayDriverNativeData
    {
        EDisplayDriverType driverType = EDisplayDriverType::Unknown;

        using GenericType = DisplayDriverNativeDataGeneric;
        DisplayDriverNativeDataGeneric * generic = nullptr;

    #if( TS3_SYSTEM_DSM_DRIVER_TYPE_SUPPORT_DXGI )
        using DXGIType = DisplayDriverNativeDataDXGI;
        DisplayDriverNativeDataDXGI * dxgi = nullptr;
    #endif

    #if( TS3_SYSTEM_DSM_DRIVER_TYPE_SUPPORT_SDL )
        using SDLType = DisplayDriverNativeDataSDL;
        DisplayDriverNativeDataSDL * sdl = nullptr;
    #endif
    };

    // NativeData for DisplayAdapter
    struct DisplayAdapterNativeData
    {
        EDisplayDriverType driverType = EDisplayDriverType::Unknown;

        using GenericType = DisplayAdapterNativeDataGeneric;
        DisplayAdapterNativeDataGeneric * generic = nullptr;

    #if( TS3_SYSTEM_DSM_DRIVER_TYPE_SUPPORT_DXGI )
        using DXGIType = DisplayAdapterNativeDataDXGI;
        DisplayAdapterNativeDataDXGI * dxgi = nullptr;
    #endif

    #if( TS3_SYSTEM_DSM_DRIVER_TYPE_SUPPORT_SDL )
        using SDLType = DisplayAdapterNativeDataSDL;
        DisplayAdapterNativeDataSDL * sdl = nullptr;
    #endif
    };

    // NativeData for DisplayOutput
    struct DisplayOutputNativeData
    {
        EDisplayDriverType driverType = EDisplayDriverType::Unknown;

        using GenericType = DisplayOutputNativeDataGeneric;
        DisplayOutputNativeDataGeneric * generic = nullptr;

    #if( TS3_SYSTEM_DSM_DRIVER_TYPE_SUPPORT_DXGI )
        using DXGIType = DisplayOutputNativeDataDXGI;
        DisplayOutputNativeDataDXGI * dxgi = nullptr;
    #endif

    #if( TS3_SYSTEM_DSM_DRIVER_TYPE_SUPPORT_SDL )
        using SDLType = DisplayOutputNativeDataSDL;
        DisplayOutputNativeDataSDL * sdl = nullptr;
    #endif
    };

    // NativeData for DisplayVideo
    struct DisplayVideoModeNativeData
    {
        EDisplayDriverType driverType = EDisplayDriverType::Unknown;

        using GenericType = DisplayVideoModeNativeDataGeneric;
        DisplayVideoModeNativeDataGeneric * generic = nullptr;

    #if( TS3_SYSTEM_DSM_DRIVER_TYPE_SUPPORT_DXGI )
        using DXGIType = DisplayVideoModeNativeDataDXGI;
        DisplayVideoModeNativeDataDXGI * dxgi = nullptr;
    #endif

    #if( TS3_SYSTEM_DSM_DRIVER_TYPE_SUPPORT_SDL )
        using SDLType = DisplayVideoModeNativeDataSDL;
        DisplayVideoModeNativeDataSDL * sdl = nullptr;
    #endif
    };


    /// @brief Private, implementation-specific data of the DisplayDriver class.
    struct DisplayDriver::ObjectPrivateData
    {
        using NativeDataType = DisplayDriverNativeData;

        //
        DisplayDriver * parentDriver = nullptr;

        // Driver native data.
        // Referenced through a const pointer in the DisplayDriver class.
        DisplayDriverNativeData nativeDataPriv;

        // Storage for holding all adapters. We use deque to guarantee pointer persistence.
        std::deque<DisplayAdapter> adapterInternalStorage;

        // Additional helper list, with all adapters stored as handles/pointers.
        // This is returned directly via the public accessor and allows us to avoid building it on the fly each time.
        DisplayAdapterList adapterList;

        // Numer of active adapters in the system. Equals adapterList.size() (and, thus, adapterInternalStorage.size() too).
        uint32 activeAdaptersNum = 0;

        // Numer of active adapters in the system. Equals adapterList.size() (and, thus, adapterInternalStorage.size() too).
        uint32 combinedActiveOutputsNum = 0;

        // Points to the default/primary adapter in the system. Usually this will be adapterList[0];
        DisplayAdapter * primaryAdapter = nullptr;

        explicit ObjectPrivateData( DisplayDriver * pDriver )
        : parentDriver( pDriver )
        {
            dsmInitializeNativeData( &nativeDataPriv, pDriver->mDriverType );
        }

        ~ObjectPrivateData()
        {
            dsmReleaseNativeData( &nativeDataPriv );
        }
    };


    /// @brief Private, implementation-specific data of the DisplayAdapter class.
    struct DisplayAdapter::ObjectPrivateData
    {
        using DescType = DisplayAdapterDesc;
        using NativeDataType = DisplayAdapterNativeData;

        //
        DisplayAdapter * parentAdapter = nullptr;

        // Ddapter info struct, containing basic adapter properties.
        // Referenced through a const pointer in the DisplayAdapter class.
        DisplayAdapterDesc descPriv;

        // Ddapter native data.
        // Referenced through a const pointer in the DisplayAdapter class.
        DisplayAdapterNativeData nativeDataPriv;

        std::deque<DisplayOutput> outputInternalStorage;
        DisplayOutputList outputList;
        uint32 activeOutputsNum = 0;
        DisplayOutput * primaryOutput = nullptr;

        explicit ObjectPrivateData( DisplayAdapter * pAdapter )
        : parentAdapter( pAdapter )
        {
            dsmInitializeNativeData( &nativeDataPriv, pAdapter->mDriverType );
        }

        ~ObjectPrivateData()
        {
            dsmReleaseNativeData( &nativeDataPriv );
        }
    };

    /// @brief Private, implementation-specific data of the DisplayOutput class.
    struct DisplayOutput::ObjectPrivateData
    {
        using DescType = DisplayOutputDesc;
        using NativeDataType = DisplayOutputNativeData;

        // Per-color-format data.
        // Video modes supported by the system may be different for each color format the display supports.
        // Hence, we associate the information about supported DMs with a separate struct which is created
        // for each ColorFormat supported. This provides correct way to have the right data in the cache.
        struct ColorFormatData
        {
            // The color format this data refers to.
            ColorFormat colorFormat;
            // List of video modes supported for this ColorFormat. Stored as queue - as usual.
            std::deque<DisplayVideoMode> videoModeInternalStorage;
            // Helper list with handles/pointers.
            DisplayVideoModeList videoModeList;
        };

        //
        DisplayOutput * parentOutput = nullptr;

        // Output info struct, containing basic output properties.
        // Referenced through a const pointer in the DisplayOutput class.
        DisplayOutputDesc descPriv;

        // Output native data.
        // Referenced through a const pointer in the DisplayOutput class.
        DisplayOutputNativeData nativeDataPriv;

        // The main data. Stores a ColorFormatData strut for each ColorFormat supported/enumerated.
        // This map should  not be used directly - instead there are dedicated functions provided below.
        std::unordered_map<ColorFormat, ColorFormatData> colorFormatMap;

        explicit ObjectPrivateData( DisplayOutput * pOutput )
        : parentOutput( pOutput )
        {
            dsmInitializeNativeData( &nativeDataPriv, pOutput->mDriverType );
        }

        ~ObjectPrivateData()
        {
            dsmReleaseNativeData( &nativeDataPriv );
        }
    };

    /// @brief Private, implementation-specific data of the DisplayVideoMode class.
    struct DisplayVideoMode::ObjectPrivateData
    {
        using DescType = DisplayVideoModeDesc;
        using NativeDataType = DisplayVideoModeNativeData;

        //
        DisplayVideoMode * parentVideoMode = nullptr;

        // Video mode info struct, containing basic mode properties.
        // Referenced through a const pointer in the DisplayVideoMode class.
        DisplayVideoModeDesc descPriv;

        // Video mode native data.
        // Referenced through a const pointer in the DisplayVideoMode class.
        DisplayVideoModeNativeData nativeDataPriv;

        explicit ObjectPrivateData( DisplayVideoMode * pVideoMode )
        : parentVideoMode( pVideoMode )
        {
            dsmInitializeNativeData( &nativeDataPriv, pVideoMode->mDriverType );
        }

        ~ObjectPrivateData()
        {
            dsmReleaseNativeData( &nativeDataPriv );
        }
    };

    // Proxy functions to get the correct NativeData member, specific to a driver, without going through all internals.
    // Used heavily in all drivers, allowing them to specify the following:
    // >> auto * adapterDXGINativeData = dsmGetNativeDataDXGI( someAdapter );
    // Instead of:
    // >> auto * adapterDXGINativeData = pAdapter.mPrivate->nativeDataPriv.dxgi;
    // This enables complete transparency and immunity to potential structure changes.
    // TpDisplayObject can be either:
    // - DisplayDriver
    // - DisplayAdapter
    // - DisplayOutput
    // - DisplayVideoMode

    template <typename TpDisplayObject>
    inline typename TpDisplayObject::ObjectPrivateData::NativeDataType::GenericType & dsmGetObjectNativeDataGeneric( TpDisplayObject & pObject )
    {
        return *( pObject.mPrivate->nativeDataPriv.generic );
    }

#if( TS3_SYSTEM_DSM_DRIVER_TYPE_SUPPORT_DXGI )
    template <typename TpDisplayObject>
    inline typename TpDisplayObject::ObjectPrivateData::NativeDataType::DXGIType & dsmGetObjectNativeDataDXGI( TpDisplayObject & pObject )
    {
        return *( pObject.mPrivate->nativeDataPriv.dxgi );
    }
#endif

#if( TS3_SYSTEM_DSM_DRIVER_TYPE_SUPPORT_SDL )
    template <typename TpDisplayObject>
    inline typename TpDisplayObject::ObjectPrivateData::NativeDataType::SDLType & dsmGetObjectNativeDataSDL( TpDisplayObject & pObject )
    {
        return *( pObject.mPrivate->nativeDataPriv.sdl );
    }
#endif

    // Additional proxy function for obtaining the private 'desc' structure of an object.
    // TpDisplayObject can be either:
    // - DisplayAdapter
    // - DisplayOutput
    // - DisplayVideoMode
    template <typename TpDisplayObject>
    inline typename TpDisplayObject::ObjectPrivateData::DescType & dsmGetObjectDesc( TpDisplayObject & pObject )
    {
        return pObject.mPrivate->descPriv;
    }

    class DisplayDriverGeneric : public DisplayDriver
    {
    public:
        explicit DisplayDriverGeneric( DisplayManager * pDisplayManager )
        : DisplayDriver( pDisplayManager, EDisplayDriverType::Generic )
        {
            _nativeCtor();
        }

        virtual ~DisplayDriverGeneric() noexcept
        {
            _nativeDtor();
        }

    private:
        void _nativeCtor();
        void _nativeDtor() noexcept;

        virtual void _drvEnumDisplayDevices() override final;
        virtual void _drvEnumVideoModes( DisplayOutput & pOutput, ColorFormat pColorFormat ) override final;
        virtual ColorFormat _drvQueryDefaultSystemColorFormat() const override final;
    };

#if( TS3_SYSTEM_DSM_DRIVER_TYPE_SUPPORT_DXGI )
    class DisplayDriverDXGI : public DisplayDriver
    {
    public:
        explicit DisplayDriverDXGI( DisplayManager * pDisplayManager )
        : DisplayDriver( pDisplayManager, EDisplayDriverType::DXGI )
        {
            _initialize();
        }

        virtual ~DisplayDriverDXGI()
        {
            _release();
        }

    private:
        void _initialize();
        void _release();

        void _enumAdapterOutputs( DisplayAdapter & pAdapter );

        virtual void _drvEnumDisplayDevices() override final;
        virtual void _drvEnumVideoModes( DisplayOutput & pOutput, ColorFormat pColorFormat ) override final;
        virtual ColorFormat _drvQueryDefaultSystemColorFormat() const override final;
    };
#endif

#if( TS3_SYSTEM_DSM_DRIVER_TYPE_SUPPORT_SDL )
    class DisplayDriverSDL : public DisplayDriver
    {
    public:
        explicit DisplayDriverSDL( DisplayManager * pDisplayManager )
        : DisplayDriver( pDisplayManager, EDisplayDriverType::SDL )
        {
            _initialize();
        }

        virtual ~DisplayDriverSDL()
        {
            _release();
        }

    private:
        void _initialize();
        void _release();

        virtual void _drvEnumDisplayDevices() override final;
        virtual void _drvEnumVideoModes( DisplayOutput & pOutput, ColorFormat pColorFormat ) override final;
        virtual ColorFormat _drvQueryDefaultSystemColorFormat() const override final;
    };
#endif


    template <typename TpNativeData>
    inline void dsmInitializeNativeData( TpNativeData * pNativeData, EDisplayDriverType pDriverType )
    {
        if( pDriverType == EDisplayDriverType::Generic )
        {
            pNativeData->generic = new typename TpNativeData::GenericType();
            pNativeData->driverType = EDisplayDriverType::Generic;
        }
    #if( TS3_SYSTEM_DSM_DRIVER_TYPE_SUPPORT_DXGI )
        else if( pDriverType == EDisplayDriverType::DXGI )
        {
            pNativeData->dxgi = new typename TpNativeData::DXGIType();
            pNativeData->driverType = EDisplayDriverType::DXGI;
        }
    #endif
    #if( TS3_SYSTEM_DSM_DRIVER_TYPE_SUPPORT_SDL )
        else if( pDriverType == EDisplayDriverType::SDL )
        {
            pNativeData->sdl = new typename TpNativeData::SDLType();
            pNativeData->driverType = EDisplayDriverType::SDL;
        }
    #endif
        else
        {
            ts3DebugInterrupt();
        }
    }

    template <typename TpNativeData>
    inline void dsmReleaseNativeData( TpNativeData * pNativeData )
    {
        if( ( pNativeData->driverType == EDisplayDriverType::Generic ) && ( pNativeData->generic != nullptr ) )
        {
            delete pNativeData->generic;
            pNativeData->generic = nullptr;
            pNativeData->driverType = EDisplayDriverType::Unknown;
        }
    #if( TS3_SYSTEM_DSM_DRIVER_TYPE_SUPPORT_DXGI )
        else if( ( pNativeData->driverType == EDisplayDriverType::DXGI ) && ( pNativeData->dxgi != nullptr ) )
        {
            delete pNativeData->dxgi;
            pNativeData->dxgi = nullptr;
            pNativeData->driverType = EDisplayDriverType::Unknown;
        }
    #endif
    #if( TS3_SYSTEM_DSM_DRIVER_TYPE_SUPPORT_SDL )
        else if( ( pNativeData->driverType == EDisplayDriverType::SDL ) && ( pNativeData->sdl != nullptr ) )
        {
            delete pNativeData->sdl;
            pNativeData->sdl = nullptr;
            pNativeData->driverType = EDisplayDriverType::Unknown;
        }
    #endif
        else
        {
            ts3DebugInterrupt();
        }
    }

} // namespace ts3::system

#endif // __TS3_SYSTEM_DISPLAY_DRIVER_NATIVE_H__
