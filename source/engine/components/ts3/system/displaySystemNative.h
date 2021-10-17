
#ifndef __TS3_SYSTEM_INTERNAL_DISPLAY_SYSTEM_NATIVE_H__
#define __TS3_SYSTEM_INTERNAL_DISPLAY_SYSTEM_NATIVE_H__

#include <ts3/system/displayCommon.h>

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_ANDROID )
#  include "platform/osapi/android/androidDisplaySystem.h"
#elif( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_WIN32 )
#  include "internal/platform/osapi/win32/win32DisplaySystem.h"
#elif( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_X11 )
#  include "internal/platform/osapi/x11/x11DisplaySystem.h"
#endif

#if( TS3_SYSTEM_DSM_DRIVER_TYPE_SUPPORT_DXGI )
#  include "internal/platform/shared/dxgi/dxgiDisplaySystem.h"
#endif

#if( TS3_SYSTEM_DSM_DRIVER_TYPE_SUPPORT_SDL )
#  include "internal/platform/shared/sdl/sdlDisplaySystem.h"
#endif

#endif // __TS3_SYSTEM_INTERNAL_DISPLAY_SYSTEM_NATIVE_H__
