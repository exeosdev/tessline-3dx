
#ifndef __TS3_SYSTEM_DISPLAY_MANAGER_NATIVE_H__
#define __TS3_SYSTEM_DISPLAY_MANAGER_NATIVE_H__

#include "displayCommon.h"
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

    union DisplayManagerNativeData
    {
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

} // namespace system
} // namespace ts3

#endif // __TS3_SYSTEM_DISPLAY_MANAGER_NATIVE_H__
