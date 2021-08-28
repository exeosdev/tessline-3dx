
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

namespace ts3
{
namespace system
{

    class DisplayDriverNativeImpl;
    class DisplayAdapterNativeImpl;
    class DisplayOutputNativeImpl;
    class DisplayVideoModeNativeImpl;

    struct DisplayDriverNativeData
    {
        DisplayDriverNativeDataGeneric * ndGeneric = nullptr;
    #if( TS3_SYSTEM_DSM_DRIVER_TYPE_SUPPORT_DXGI )
        DisplayDriverNativeDataDXGI * ndDXGI = nullptr;
    #endif
    #if( TS3_SYSTEM_DSM_DRIVER_TYPE_SUPPORT_SDL )
        DisplayDriverNativeDataSDL * ndSDL = nullptr;
    #endif
    };

    struct DisplayVideoModeNativeData
    {
        DisplayVideoModeNativeDataGeneric * ndGeneric = nullptr;
    #if( TS3_SYSTEM_DSM_DRIVER_TYPE_SUPPORT_DXGI )
        DisplayVideoModeNativeDataDXGI * ndDXGI = nullptr;
    #endif
    #if( TS3_SYSTEM_DSM_DRIVER_TYPE_SUPPORT_SDL )
        DisplayVideoModeNativeDataSDL * ndSDL = nullptr;
    #endif
    };

    struct DisplayOutputNativeData
    {
        DisplayOutputNativeDataGeneric * ndGeneric = nullptr;
    #if( TS3_SYSTEM_DSM_DRIVER_TYPE_SUPPORT_DXGI )
        DisplayOutputNativeDataDXGI * ndDXGI = nullptr;
    #endif
    #if( TS3_SYSTEM_DSM_DRIVER_TYPE_SUPPORT_SDL )
        DisplayOutputNativeDataSDL * ndSDL = nullptr;
    #endif
    };

    struct DisplayAdapterNativeData
    {
        DisplayAdapterNativeDataGeneric * ndGeneric = nullptr;
    #if( TS3_SYSTEM_DSM_DRIVER_TYPE_SUPPORT_DXGI )
        DisplayAdapterNativeDataDXGI * ndDXGI = nullptr;
    #endif
    #if( TS3_SYSTEM_DSM_DRIVER_TYPE_SUPPORT_SDL )
        DisplayAdapterNativeDataSDL * ndSDL = nullptr;
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
        DisplayVideoModeDesc mDesc;
        DisplayVideoModeNativeData mNativeData;

    public:
        DisplayVideoModeNativeImpl( DisplayOutputNativeImpl * pOutput );
        virtual ~DisplayVideoModeNativeImpl();
    };

    class DisplayOutputNativeImpl : public DisplayOutput
    {
    public:
        DisplayOutputDesc mDesc;
        DisplayOutputNativeData mNativeData;

    public:
        DisplayOutputNativeImpl( DisplayAdapterNativeImpl * pAdapter );
        virtual ~DisplayOutputNativeImpl();

    protected:
        struct VideoModesData
        {
            using VideoModeStorage = std::deque<DisplayVideoModeNativeImpl>;
            using VideoModeIndexArray = std::vector<DisplayVideoModeNativeImpl *>;

            VideoModeStorage storage;
            VideoModeIndexArray indexArray;
        };

        using VideoModesDataMap = std::unordered_map<ColorFormat, VideoModesData>;

        VideoModesDataMap _videoModesDataMap;
    };

    class DisplayAdapterNativeImpl : public DisplayAdapter
    {
    public:
        DisplayAdapterDesc mDesc;
        DisplayAdapterNativeData mNativeData;

    public:
        DisplayAdapterNativeImpl( DisplayDriverNativeImpl * pDisplayDriver );
        virtual ~DisplayAdapterNativeImpl();

    protected:
        struct OutputsData
        {
            using OutputStorage = std::deque<DisplayOutputNativeImpl>;
            using OutputIndexArray = std::vector<DisplayOutputNativeImpl *>;

            OutputStorage storage;
            OutputIndexArray indexArray;
        };

        OutputsData _outputsData;
    };

    class DisplayDriverNativeImpl : public DisplayDriver
    {
    protected:
        DisplayAdapterNativeImpl * addAdapter();

        DisplayOutputNativeImpl * addOutput( DisplayAdapterNativeImpl & pAdapter );

        DisplayVideoModeNativeImpl * addVideoMode( DisplayOutputNativeImpl & pOutputData,
                                                   ColorFormat pColorFormat );

    protected:
        struct AdaptersData
        {
            using AdapterStorage = std::deque<DisplayAdapterNativeImpl>;
            using AdapterIndexArray = std::vector<DisplayAdapterNativeImpl>;

            AdapterStorage storage;
            AdapterIndexArray indexArray;
        };

        AdaptersData _adaptersData;
    };

} // namespace system
} // namespace ts3

#endif // __TS3_SYSTEM_DISPLAY_DRIVER_NATIVE_H__
