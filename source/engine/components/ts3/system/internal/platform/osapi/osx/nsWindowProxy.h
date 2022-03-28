
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_OSX_NS_WINDOW_PROXY_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_OSX_NS_WINDOW_PROXY_H__

#include "osxCommon.h"
#include <ts3/system/windowCommon.h>
#include <AppKit/NSWindow.h>

/// @brief
@interface NSWindowProxy : NSWindow

- ( BOOL ) canBecomeKeyWindow;

- ( BOOL ) canBecomeMainWindow;

- ( void ) sendEvent: ( NSEvent * )pEvent;

- ( void ) doCommandBySelector: ( SEL )pSelector;

@end

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_OSX_NS_WINDOW_PROXY_H__
