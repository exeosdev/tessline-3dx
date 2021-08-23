
#ifndef __TS3_SYSTEM_DISPLAY_NATIVE_H__
#define __TS3_SYSTEM_DISPLAY_NATIVE_H__

#include "display.h"
#include "sysContextNative.h"

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_ANDROID )
#  include "internal/platform/osapi/android/androidDisplay.h"
#elif( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_WIN32 )
#  include "internal/platform/osapi/win32/win32Display.h"
#elif( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_X11 )
#  include "internal/platform/osapi/x11/x11Display.h"
#endif

namespace ts3
{
namespace system
{

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

    union DisplayVideoModeHash
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

    class DisplayManagerNativeImpl : public DisplayManager
    {
    public:
        DisplayManagerNativeData mNativeData;

    public:
        explicit DisplayManagerNativeImpl( SysContextHandle pSysContext );
        virtual ~DisplayManagerNativeImpl();

    private:
        virtual void _nativeQueryDefaultDisplaySize( DisplaySize & pOutSize ) const override;
        virtual void _nativeQueryMinWindowSize( DisplaySize & pOutSize ) const override;
    };

    /// @brief
    class DisplayDriverNativeImpl : public DisplayDriver
    {
    public:
        DisplayDriverNativeImpl( DisplayManagerHandle pDisplayManager );
        virtual ~DisplayDriverNativeImpl();

        virtual void _nativeEnumerateAdapters() override;
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

} // namespace system
} // namespace ts3

#endif // __TS3_SYSTEM_DISPLAY_NATIVE_H__
