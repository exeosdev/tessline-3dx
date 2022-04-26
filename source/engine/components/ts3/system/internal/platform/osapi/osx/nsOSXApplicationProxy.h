
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_OSX_NS_OSX_APPLICATION_PROXY_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_OSX_NS_OSX_APPLICATION_PROXY_H__

#include "nsCommon.h"
#include <ts3/system/windowCommon.h>

@interface NSApplication( NSAppleMenu )
-( void ) setAppleMenu: (NSMenu *)pMenu;
@end

/// @brief
@interface NSOSXApplicationProxy : NSApplication
{
	ts3::system::OSXEventController * mEventController;
}

-( void ) terminate: ( id )pSender;

-( void ) sendEvent: ( NSEvent * )pEvent;

+( void ) registerUserDefaults;

@end


@interface NSOSXApplicationDelegate : NSObject<NSApplicationDelegate>
{
	ts3::system::OSXSysContext * mOSXSysContext;
}

-( void ) setOSXSysContext: ( ts3::system::OSXSysContext * )pOSXSysContext;

-( id ) init;

-( void ) dealloc;

-( void ) applicationDidBecomeActiveNotificationHandler:( NSNotification * )pNotification;

-( void ) currentLocaleDidChangeNotificationHandler:( NSNotification * )pNotification;

-( void ) windowWillCloseNotificationHandler:( NSNotification * )pNotification;

-( void ) applicationWillFinishLaunching:( NSNotification * )pNotification;

-( void ) applicationDidFinishLaunching:( NSNotification * )pNotification;

-( BOOL ) applicationShouldTerminateAfterLastWindowClosed:( NSApplication * )pApplication;

-( NSApplicationTerminateReply ) applicationShouldTerminate:( NSApplication * )pSender;

-( void ) applicationWillTerminate:( NSNotification * )pNotification;

@end

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_OSX_NS_OSX_APPLICATION_PROXY_H__
