
#ifndef __TS3_SYSTEM_DISPLAY_DRIVER_NATIVE_H__
#define __TS3_SYSTEM_DISPLAY_DRIVER_NATIVE_H__

#include "displayDriver.h"
#include "sysContextNative.h"

#include <deque>
#include <unordered_map>

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

    struct DisplayDriverNativeData
    {
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

    struct DisplayVideoModeNativeData
    {
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

    struct DisplayOutputNativeData
    {
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

    struct DisplayAdapterNativeData
    {
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

    struct DisplayDriver::ObjectPrivateData
    {
        DisplayDriverNativeData nativeDataPriv;
        std::deque<DisplayAdapter> adapterStorage;
        DisplayAdapterList adapterList;
        uint32 activeAdaptersNum = 0;
        DisplayAdapter * primaryAdapter = nullptr;
    };

    struct DisplayAdapter::ObjectPrivateData
    {
        DisplayAdapterDesc descPriv;
        DisplayAdapterNativeData nativeDataPriv;
        std::deque<DisplayOutput> outputStorage;
        DisplayOutputList outputList;
        uint32 activeOutputsNum = 0;
        DisplayOutput * primaryOutput = nullptr;
    };

    struct DisplayOutput::ObjectPrivateData
    {
        struct ColorFormatData
        {
            std::deque<DisplayVideoMode> videoModeStorage;
            DisplayVideoModeList videoModeList;
        };

        DisplayOutputDesc descPriv;
        DisplayOutputNativeData nativeDataPriv;
        std::unordered_map<ColorFormat, ColorFormatData> colorFormatMap;
    };

    struct DisplayVideoMode::ObjectPrivateData
    {
        DisplayVideoModeDesc descPriv;
        DisplayVideoModeNativeData nativeDataPriv;
    };

    class DisplayDriverGeneric : public DisplayDriver
    {
    public:
        explicit DisplayDriverGeneric( DisplayManager * pDisplayManager );
        virtual ~DisplayDriverGeneric();

    private:
        virtual void _nativeEnumAdapters() override final;
        virtual void _nativeEnumOutputs( DisplayAdapter & pAdapter ) override final;
        virtual void _nativeEnumVideoModes( DisplayOutput & pOutput, ColorFormat pColorFormat ) override final;
    
        virtual void _nativeDestroyAdapter( DisplayAdapter & pAdapter ) override final;
        virtual void _nativeDestroyOutput( DisplayOutput & pOutput ) override final;
        virtual void _nativeDestroyVideoMode( DisplayVideoMode & pVideoMode ) override final;
    };

#if( TS3_SYSTEM_DSM_DRIVER_TYPE_SUPPORT_DXGI )
    class DisplayDriverDXGI : public DisplayDriver
    {
    public:
        explicit DisplayDriverDXGI( DisplayManager * pDisplayManager );
        virtual ~DisplayDriverDXGI();

    private:
        virtual void _nativeEnumAdapters() override final;
        virtual void _nativeEnumOutputs( DisplayAdapter & pAdapter ) override final;
        virtual void _nativeEnumVideoModes( DisplayOutput & pOutput, ColorFormat pColorFormat ) override final;
    
        virtual void _nativeDestroyAdapter( DisplayAdapter & pAdapter ) override final;
        virtual void _nativeDestroyOutput( DisplayOutput & pOutput ) override final;
        virtual void _nativeDestroyVideoMode( DisplayVideoMode & pVideoMode ) override final;
    };
#endif

#if( TS3_SYSTEM_DSM_DRIVER_TYPE_SUPPORT_SDL )
    class DisplayDriverSDL : public DisplayDriver
    {
    public:
        explicit DisplayDriverSDL( DisplayManager * pDisplayManager );
        virtual ~DisplayDriverSDL();

    private:
        virtual void _nativeEnumAdapters() override final;
        virtual void _nativeEnumOutputs( DisplayAdapter & pAdapter ) override final;
        virtual void _nativeEnumVideoModes( DisplayOutput & pOutput, ColorFormat pColorFormat ) override final;
    
        virtual void _nativeDestroyAdapter( DisplayAdapter & pAdapter ) override final;
        virtual void _nativeDestroyOutput( DisplayOutput & pOutput ) override final;
        virtual void _nativeDestroyVideoMode( DisplayVideoMode & pVideoMode ) override final;
    };
#endif

    union DisplayOutputID
    {
        struct
        {
            dsm_index_t uAdapterIndex;
            dsm_index_t uOutputIndex;
        };
        dsm_output_id_t outputID override finalu;
    };

    union DisplayVideoModeID
    {
        struct
        {
            dsm_output_id_t uOutputID;
            dsm_index_t uColorFormatIndex;
            dsm_index_t uModeIndex;
        };
        dsm_video_mode_id_t modeID override finalu;
    };

    union DisplayVideoSettingsHash
    {
        struct
        {
            uint16 uResWidth;
            uint16 uResHeight;
            uint16 uRefreshRate;
            uint8 uFlags;
            uint8 uColorFormatIndex;
        };
        dsm_video_settings_hash_t hashValue override finalu;
    };

    inline bool operator==( const DisplayVideoSettings & pLhs, const DisplayVideoSettings & pRhs )
    {
        return ( pLhs.resolution == pRhs.resolution ) && ( pLhs.refreshRate == pRhs.refreshRate ) && ( pLhs.flags == pRhs.flags );
    }

    inline bool operator!=( const DisplayVideoSettings & pLhs, const DisplayVideoSettings & pRhs )
    {
        return ( pLhs.resolution != pRhs.resolution ) || ( pLhs.refreshRate != pRhs.refreshRate ) || ( pLhs.flags != pRhs.flags );
    }

    inline bool operator==( const DisplayVideoSettingsFilter & pLhs, const DisplayVideoSettingsFilter & pRhs )
    {
        return ( pLhs.refSettings == pRhs.refSettings ) && ( pLhs.flags == pRhs.flags );
    }

    inline bool operator!=( const DisplayVideoSettingsFilter & pLhs, const DisplayVideoSettingsFilter & pRhs )
    {
        return ( pLhs.refSettings != pRhs.refSettings ) || ( pLhs.flags != pRhs.flags );
    }

    template <typename TpNativeData>
    inline void dsmInitializeNativeData( TpNativeData * pNativeData, EDisplayDriverType pDriverType )
    {
        try
        {
            switch ( pDriverType )
            {
                case EDisplayDriverType::Generic:
                {
                    pNativeData->generic = new typename TpNativeData::GenericType();
                    break;
                }
			#if( TS3_SYSTEM_DSM_DRIVER_TYPE_SUPPORT_DXGI )
			    case EDisplayDriverType::DXGI:
                {
                    pNativeData->dxgi = new typename TpNativeData::DXGIType();
                    break;
                }
			#endif
			#if( TS3_SYSTEM_DSM_DRIVER_TYPE_SUPPORT_SDL )
			    case EDisplayDriverType::SDL:
                {
                    pNativeData->sdl = new typename TpNativeData::SDLType();
                    break;
                }
			#endif
			default:
			{
			    throw 0;
			}
            }
        }
        catch( ... )
        {
        }
    }

    template <typename TpNativeData>
    inline void dsmReleaseNativeData( TpNativeData * pNativeData, EDisplayDriverType pDriverType )
    {
        try
        {
            switch( pDriverType )
            {
                case EDisplayDriverType::Generic:
                {
                    if( pNativeData->generic != nullptr )
                    {
                        delete pNativeData->generic;
                        pNativeData->generic = nullptr;
                    }
                    break;
                }
			#if( TS3_SYSTEM_DSM_DRIVER_TYPE_SUPPORT_DXGI )
			    case EDisplayDriverType::DXGI:
                {
                    if( pNativeData->dxgi != nullptr )
                    {
                        delete pNativeData->dxgi;
                        pNativeData->dxgi = nullptr;
                    }
                    break;
                }
			#endif
			#if( TS3_SYSTEM_DSM_DRIVER_TYPE_SUPPORT_DXGI )
                case EDisplayDriverType::SDL:
                {
                    if( pNativeData->sdl != nullptr )
                    {
                        delete pNativeData->sdl;
                        pNativeData->sdl = nullptr;
                    }
                    break;
                }
			#endif
            }
        }
        catch( ... )
        {
        }
    }

} // namespace ts3::system

#endif // __TS3_SYSTEM_DISPLAY_DRIVER_NATIVE_H__
