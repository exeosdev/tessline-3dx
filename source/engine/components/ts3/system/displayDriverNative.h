
#ifndef __TS3_SYSTEM_DISPLAY_DRIVER_NATIVE_H__
#define __TS3_SYSTEM_DISPLAY_DRIVER_NATIVE_H__

#include "displayDriver.h"
#include "sysContextNative.h"

#include <deque>
#include <unordered_map>
#include <unordered_set>

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_ANDROID )
#  include "internal/platform/osapi/android/androidDisplayDriver.h"
#elif( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_WIN32 )
#  include "internal/platform/osapi/win32/win32DisplayDriver.h"
#elif( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_X11 )
#  include "internal/platform/osapi/x11/x11DisplayDriver.h"
#endif

#if( TS3_SYSTEM_DSM_DRIVER_TYPE_SUPPORT_DXGI )
#  include "internal/platform/shared/dxgi/dxgiDisplayDriver.h"
#endif

#if( TS3_SYSTEM_DSM_DRIVER_TYPE_SUPPORT_SDL )
#  include "internal/platform/shared/sdl/sdlDisplayDriver.h"
#endif

namespace ts3::system
{

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

        explicit DisplayDriverNativeData( EDisplayDriverType pDriverType );
        ~DisplayDriverNativeData();
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

        explicit DisplayAdapterNativeData( EDisplayDriverType pDriverType );
        ~DisplayAdapterNativeData();
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

        explicit DisplayOutputNativeData( EDisplayDriverType pDriverType );
        ~DisplayOutputNativeData();
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

        explicit DisplayVideoModeNativeData( EDisplayDriverType pDriverType );
        ~DisplayVideoModeNativeData();
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

        // Points to the default/primary adapter in the system. Usually this will be adapterList[0];
        DisplayAdapter * primaryAdapter = nullptr;

        ObjectPrivateData( DisplayDriver * pDriver )
        : parentDriver( pDriver )
        , nativeDataPriv( pDriver->mDriverType )
        {}
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

        ObjectPrivateData( DisplayAdapter * pAdapter )
        : parentAdapter( pAdapter )
        , nativeDataPriv( pAdapter->mDriverType )
        {}
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
        std::unordered_map<ColorFormat, ColorFormatData> _colorFormatMap;

        // Returns a ColorFormatData for a specified ColorFormat.
        // If no ColorFormatData is found in the map, a new one is created.
        ColorFormatData & getColorFormatData( ColorFormat pColorFormat )
        {
            auto colorFormatDataRef = _colorFormatMap.find( pColorFormat );
            if( colorFormatDataRef == _colorFormatMap.end() )
            {
                auto insertRes = _colorFormatMap.insert( {pColorFormat, ColorFormatData{}} );
                colorFormatDataRef = insertRes.first;
                colorFormatDataRef->second.colorFormat = pColorFormat;
            }
            return colorFormatDataRef->second;
        }

        // Returns a DisplayVideoModeList for a specified ColorFormat.
        DisplayVideoModeList & getVideoModeList( ColorFormat pColorFormat )
        {
            auto & colorFormatData = getColorFormatData( pColorFormat );
            return colorFormatData.videoModeList;
        }

        // Resets the internal state and data.
        void resetColorFormatMap()
        {
            _colorFormatMap.clear();
        }

        ObjectPrivateData( DisplayOutput * pOutput )
        : parentOutput( pOutput )
        , nativeDataPriv( pOutput->mDriverType )
        {}
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

        ObjectPrivateData( DisplayVideoMode * pVideoMode )
        : parentVideoMode( pVideoMode )
        , nativeDataPriv( pVideoMode->mDriverType )
        {}
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

    template <typename TpDisplayObject>
    inline typename TpDisplayObject::ObjectPrivateData::NativeDataType::DXGIType & dsmGetObjectNativeDataDXGI( TpDisplayObject & pObject )
    {
        return *( pObject.mPrivate->nativeDataPriv.dxgi );
    }

    template <typename TpDisplayObject>
    inline typename TpDisplayObject::ObjectPrivateData::NativeDataType::SDLType & dsmGetObjectNativeDataSDL( TpDisplayObject & pObject )
    {
        return *( pObject.mPrivate->nativeDataPriv.sdl );
    }

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
        explicit DisplayDriverGeneric( DisplayManager * pDisplayManager );
        virtual ~DisplayDriverGeneric();

    private:
        virtual void _nativeEnumDisplayDevices() override final;
        virtual void _nativeEnumVideoModes( DisplayOutput & pOutput, ColorFormat pColorFormat ) override final;
    
        virtual void _nativeDestroyAdapter( DisplayAdapter & pAdapter ) override final;
        virtual void _nativeDestroyOutput( DisplayOutput & pOutput ) override final;
        virtual void _nativeDestroyVideoMode( DisplayVideoMode & pVideoMode ) override final;

        virtual ColorFormat _nativeGetSystemDefaultColorFormat() const override final;
        virtual ArrayView<const ColorFormat> _nativeGetSupportedColorFormatList() const override final;
    };

#if( TS3_SYSTEM_DSM_DRIVER_TYPE_SUPPORT_DXGI )
    class DisplayDriverDXGI : public DisplayDriver
    {
    public:
        explicit DisplayDriverDXGI( DisplayManager * pDisplayManager );
        virtual ~DisplayDriverDXGI();

    private:
        virtual void _nativeEnumDisplayDevices() override final;
        virtual void _nativeEnumVideoModes( DisplayOutput & pOutput, ColorFormat pColorFormat ) override final;
    
        virtual void _nativeDestroyAdapter( DisplayAdapter & pAdapter ) override final;
        virtual void _nativeDestroyOutput( DisplayOutput & pOutput ) override final;
        virtual void _nativeDestroyVideoMode( DisplayVideoMode & pVideoMode ) override final;

        virtual ColorFormat _nativeGetSystemDefaultColorFormat() const override final;
        virtual ArrayView<const ColorFormat> _nativeGetSupportedColorFormatList() const override final;
        
        void _enumAdapterOutputs( DisplayAdapter & pAdapter );
    };
#endif

#if( TS3_SYSTEM_DSM_DRIVER_TYPE_SUPPORT_SDL )
    class DisplayDriverSDL : public DisplayDriver
    {
    public:
        explicit DisplayDriverSDL( DisplayManager * pDisplayManager );
        virtual ~DisplayDriverSDL();

    private:
        virtual void _nativeEnumDisplayDevices() override final {}
        virtual void _nativeEnumVideoModes( DisplayOutput & pOutput, ColorFormat pColorFormat ) override final;
    
        virtual void _nativeDestroyAdapter( DisplayAdapter & pAdapter ) override final;
        virtual void _nativeDestroyOutput( DisplayOutput & pOutput ) override final;
        virtual void _nativeDestroyVideoMode( DisplayVideoMode & pVideoMode ) override final;

        virtual ColorFormat _nativeGetSystemDefaultColorFormat() const override final;
        virtual ArrayView<const ColorFormat> _nativeGetSupportedColorFormatList() const override final;
    };
#endif

    union DisplayOutputIDGen
    {
        struct
        {
            dsm_index_t uAdapterIndex;
            dsm_index_t uOutputIndex;
        };

        dsm_output_id_t outputID = 0;
    };

    union DisplayVideoModeIDGen
    {
        struct
        {
            dsm_output_id_t uOutputID;
            dsm_index_t uColorFormatIndex;
            dsm_index_t uModeIndex;
        };

        dsm_video_mode_id_t modeID = 0u;
    };

    union DisplayVideoSettingsHashGen
    {
        struct
        {
            uint16 uResWidth;
            uint16 uResHeight;
            uint16 uRefreshRate;
            uint8 uFlags;
            uint8 uColorFormatIndex;
        };

        dsm_video_settings_hash_t hashValue = 0u;
    };

    inline dsm_index_t dsmExtractOutputIDAdapterIndex( dsm_output_id_t pOutputID )
    {
        DisplayOutputIDGen outputIDGen;
        outputIDGen.outputID = pOutputID;
        return outputIDGen.uAdapterIndex;
    }

    inline dsm_index_t dsmExtractOutputIDOutputIndex( dsm_output_id_t pOutputID )
    {
        DisplayOutputIDGen outputIDGen;
        outputIDGen.outputID = pOutputID;
        return outputIDGen.uOutputIndex;
    }


    template <typename TpNativeData>
    inline void dsmInitializeNativeData( TpNativeData * pNativeData, EDisplayDriverType pDriverType )
    {
        if( pDriverType == EDisplayDriverType::Generic )
        {
            pNativeData->generic = new typename TpNativeData::GenericType();
        }
    #if( TS3_SYSTEM_DSM_DRIVER_TYPE_SUPPORT_DXGI )
        else if( pDriverType == EDisplayDriverType::DXGI )
        {
            pNativeData->dxgi = new typename TpNativeData::DXGIType();
        }
    #endif
    #if( TS3_SYSTEM_DSM_DRIVER_TYPE_SUPPORT_SDL )
        else if( pDriverType == EDisplayDriverType::SDL )
        {
            pNativeData->sdl = new typename TpNativeData::SDLType();
        }
    #endif
        else
        {
            ts3DebugInterrupt();
        }
    }

    template <typename TpNativeData>
    inline void dsmReleaseNativeData( TpNativeData * pNativeData, EDisplayDriverType pDriverType )
    {
        if( ( pDriverType == EDisplayDriverType::Generic ) && ( pNativeData->generic != nullptr ) )
        {
            delete pNativeData->generic;
            pNativeData->generic = nullptr;
        }
    #if( TS3_SYSTEM_DSM_DRIVER_TYPE_SUPPORT_DXGI )
        else if( ( pDriverType == EDisplayDriverType::DXGI ) && ( pNativeData->dxgi != nullptr ) )
        {
            delete pNativeData->dxgi;
            pNativeData->dxgi = nullptr;
        }
    #endif
    #if( TS3_SYSTEM_DSM_DRIVER_TYPE_SUPPORT_SDL )
        else if( ( pDriverType == EDisplayDriverType::SDL ) && ( pNativeData->sdl != nullptr ) )
        {
            delete pNativeData->sdl;
            pNativeData->sdl = nullptr;
        }
    #endif
        else
        {
            ts3DebugInterrupt();
        }
    }


    inline DisplayDriverNativeData::DisplayDriverNativeData( EDisplayDriverType pDriverType )
    : driverType( pDriverType )
    {
        dsmInitializeNativeData( this, pDriverType );
    }

    inline DisplayDriverNativeData::~DisplayDriverNativeData()
    {
        dsmReleaseNativeData( this, driverType );
    }


    inline DisplayAdapterNativeData::DisplayAdapterNativeData( EDisplayDriverType pDriverType )
    : driverType( pDriverType )
    {
        dsmInitializeNativeData( this, pDriverType );
    }

    inline DisplayAdapterNativeData::~DisplayAdapterNativeData()
    {
        dsmReleaseNativeData( this, driverType );
    }


    inline DisplayOutputNativeData::DisplayOutputNativeData( EDisplayDriverType pDriverType )
    : driverType( pDriverType )
    {
        dsmInitializeNativeData( this, pDriverType );
    }

    inline DisplayOutputNativeData::~DisplayOutputNativeData()
    {
        dsmReleaseNativeData( this, driverType );
    }


    inline DisplayVideoModeNativeData::DisplayVideoModeNativeData( EDisplayDriverType pDriverType )
    : driverType( pDriverType )
    {
        dsmInitializeNativeData( this, pDriverType );
    }

    inline DisplayVideoModeNativeData::~DisplayVideoModeNativeData()
    {
        dsmReleaseNativeData( this, driverType );
    }

} // namespace ts3::system

#endif // __TS3_SYSTEM_DISPLAY_DRIVER_NATIVE_H__
