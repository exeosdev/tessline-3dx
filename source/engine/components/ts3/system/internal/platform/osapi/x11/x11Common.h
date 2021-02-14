
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_X11_COMMON_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_X11_COMMON_H__

#include <X11/X.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>

namespace ts3
{

    inline constexpr XID cvXIDAlways = Always;
    inline constexpr XID cvXIDNone = None;
    inline constexpr XID cvXIDSuccess= Success;

}

#undef Always
#undef None
#undef Success

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_X11_COMMON_H__
