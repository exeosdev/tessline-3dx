
#ifndef __TS3_SYSTEM_DISPLAY_DRIVER_NATIVE_H__
#define __TS3_SYSTEM_DISPLAY_DRIVER_NATIVE_H__

#include "displayDriver.h"
#include "sysContextNative.h"

#include <deque>
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

    class DisplayDriverNativeImpl;
    class DisplayAdapterNativeImpl;
    class DisplayOutputNativeImpl;
    class DisplayVideoModeNativeImpl;

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

    union DisplayOutputID
    {
        struct
        {
            dsm_index_t uAdapterIndex;
            dsm_index_t uOutputIndex;
        };
        dsm_output_id_t outputID = 0u;
    };

    union DisplayVideoModeID
    {
        struct
        {
            dsm_output_id_t uOutputID;
            dsm_index_t uColorFormatIndex;
            dsm_index_t uModeIndex;
        };
        dsm_video_mode_id_t modeID = 0u;
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
        dsm_video_settings_hash_t modeHash = 0u;
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

    class DisplayVideoModeNativeImpl : public DisplayVideoMode
    {
    public:
        DisplayVideoModeDesc mDescPriv;
        DisplayVideoModeNativeData mNativeData;

    public:
        explicit DisplayVideoModeNativeImpl( DisplayOutputNativeImpl * pOutput );
        virtual ~DisplayVideoModeNativeImpl();
    };

    class DisplayOutputNativeImpl : public DisplayOutput
    {
    public:
        struct ColorFormatData
        {
            std::deque<DisplayVideoModeNativeImpl> videoModeStorage;
            DisplayVideoModeList videoModeList;
        };

        DisplayOutputDesc mDescPriv;
        DisplayOutputNativeData mNativeData;
        std::unordered_map<ColorFormat, ColorFormatData> mColorFormatMap;

    public:
        explicit DisplayOutputNativeImpl( DisplayAdapterNativeImpl * pAdapter );
        virtual ~DisplayOutputNativeImpl();
    };

    class DisplayAdapterNativeImpl : public DisplayAdapter
    {
    public:
        DisplayAdapterDesc mDescPriv;
        DisplayAdapterNativeData mNativeData;
        std::deque<DisplayOutputNativeImpl> mOutputStorage;
        DisplayOutputList mOutputList;

    public:
        explicit DisplayAdapterNativeImpl( DisplayDriverNativeImpl * pDisplayDriver );
        virtual ~DisplayAdapterNativeImpl();
    };

    class DisplayDriverNativeImpl : public DisplayDriver
    {
    public:
        DisplayDriverNativeData mNativeData;
        std::deque<DisplayAdapterNativeImpl> mAdapterStorage;
        DisplayAdapterList mAdapterList;

    public:
        DisplayDriverNativeImpl( DisplayManager * pDisplayManager,
                                 EDisplayDriverType pDriverType );

        virtual ~DisplayDriverNativeImpl();

    protected:
        DisplayAdapterNativeImpl * addAdapter();
        DisplayOutputNativeImpl * addOutput( DisplayAdapterNativeImpl & pAdapter );
        DisplayVideoModeNativeImpl * addVideoMode( DisplayOutputNativeImpl & pOutputData, ColorFormat pColorFormat );

    private:
        virtual void _nativeEnumAdapters() = 0;
        virtual void _nativeEnumOutputs( DisplayAdapterNativeImpl & pAdapter ) = 0;
        virtual void _nativeEnumVideoModes( DisplayOutputNativeImpl & pOutputData, ColorFormat pColorFormat ) = 0;
    };

    class DisplayDriverNativeImplGeneric : public DisplayDriverNativeImpl
    {
    private:
        virtual void _nativeEnumAdapters() override final;
        virtual void _nativeEnumOutputs( DisplayAdapterNativeImpl & pAdapter ) override final;
        virtual void _nativeEnumVideoModes( DisplayOutputNativeImpl & pOutputData, ColorFormat pColorFormat ) override final;
    };

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
