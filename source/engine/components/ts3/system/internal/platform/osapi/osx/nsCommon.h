
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_OSX_NS_COMMON_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_OSX_NS_COMMON_H__

#include <ts3/system/prerequisites.h>
#include <objc/objc.h>

#import <AppKit/NSWindow.h>
#import <AppKit/NSApplication.h>

// NSApplication(NSScripting) provides [orderedWindows] property.
#import <AppKit/NSApplicationScripting.h>

namespace ts3::system
{

    // Required to create the alias below.
    namespace platform
    {}

    class OSXSysContext;

	ts3SysDeclareHandle( OSXDisplayManager  );
	ts3SysDeclareHandle( OSXDisplayDriver   );
	ts3SysDeclareHandle( OSXEventController );
	ts3SysDeclareHandle( OSXWindowManager   );

}

namespace platform = ts3::system::platform;

using OSXSysContext = ts3::system::OSXSysContext;
using OSXDisplayManager = ts3::system::OSXDisplayManager;
using OSXDisplayDriver = ts3::system::OSXDisplayDriver;
using OSXEventController = ts3::system::OSXEventController;
using OSXWindowManager = ts3::system::OSXWindowManager;

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_OSX_NS_COMMON_H__
