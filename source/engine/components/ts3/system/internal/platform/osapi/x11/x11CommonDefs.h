
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_X11_COMMON_DEFS_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_X11_COMMON_DEFS_H__

#include <X11/X.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <string>

namespace ts3
{
namespace system
{

    // Having these pretty common words replaced with a numbers tends
    // to be an issue, so we use some explicit enum constants instead
    // and remove them from the global scope.
    enum : XID
    {
        XID_Always = Always,
        XID_None = None,
        XID_Success = Success
    };

    #undef Always
    #undef None
    #undef Success

} // namespace system
} // namespace ts3

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_X11_COMMON_DEFS_H__
