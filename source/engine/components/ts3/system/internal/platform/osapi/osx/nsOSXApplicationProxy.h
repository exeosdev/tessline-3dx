
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_OSX_NS_OSX_APPLICATION_PROXY_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_OSX_NS_OSX_APPLICATION_PROXY_H__

#include "nsCommon.h"

/// NSApplication::setAppleMenu seems to be gone from the
@interface NSApplication( NSAppleMenu )
-( void ) setAppleMenu:( NSMenu * )pMenu;
@end

/// @brief
@interface NSOSXApplicationProxy : NSApplication

-( void ) terminate:( id )pSender;

-( void ) sendEvent:( NSEvent * )pEvent;

+( void ) registerUserDefaults;

@end


@interface NSOSXApplicationDelegate : NSObject<NSApplicationDelegate>
{
	@public OSXSysContext * mSysContext;
}

-( id ) initWithSysContext: ( OSXSysContext * )pSysContext;

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
