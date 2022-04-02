
#ifndef __TS3_SYSTEM_WINDOW_NATIVE_H__
#define __TS3_SYSTEM_WINDOW_NATIVE_H__

#include "windowSystem.h"
#include <list>

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_ANDROID )
#  include "internal/platform/osapi/android/androidWindow.h"
#elif( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_OSX )
#  include "internal/platform/osapi/osx/osxWindowSystem.h"
#elif( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_WIN32 )
#  include "internal/platform/osapi/win32/win32WindowSystem.h"
#elif( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_X11 )
#  include "internal/platform/osapi/x11/x11WindowSystem.h"
#endif

namespace ts3::system
{



} // namespace ts3::system

#endif // __TS3_SYSTEM_WINDOW_NATIVE_H__
