
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_OSX_NS_COMMON_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_OSX_NS_COMMON_H__

#include "osxCommon.h"
#import <AppKit/NSApplication.h>
#import <AppKit/NSWindow.h>

namespace ts3::system
{

	ts3SysDeclareHandle( OSXDisplayManager );
	ts3SysDeclareHandle( OSXDisplayDriver );
	ts3SysDeclareHandle( OSXEventController );
	ts3SysDeclareHandle( OSXWindowManager );

}

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_OSX_NS_COMMON_H__
