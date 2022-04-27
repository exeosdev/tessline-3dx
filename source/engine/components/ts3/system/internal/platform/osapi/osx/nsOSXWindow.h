
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_NS_INTERNAL_NS_WINDOW_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_NS_INTERNAL_NS_WINDOW_H__

#include "nsCommon.h"
#include <ts3/system/windowCommon.h>
#import <AppKit/NSWindow.h>

/// @brief
@interface NSOSXWindow : NSWindow

-( BOOL ) canBecomeKeyWindow;

-( BOOL ) canBecomeMainWindow;

-( void ) sendEvent:( NSEvent * )pEvent;

-( void ) doCommandBySelector:( SEL )pSelector;

-( void ) keyDown:( NSEvent * )pEvent;

-( void ) mouseDown:( NSEvent * )pEvent;

-( void ) rightMouseDown:( NSEvent * )pEvent;

@end


/// @brief
@interface NSOSXWindowView : NSView

-( NSOSXWindowView * ) initForWindow:( NSOSXWindow * )pWindow;

-( BOOL ) acceptsFirstResponder;

-( void ) keyDown:( NSEvent * )pEvent;

-( void ) mouseDown:( NSEvent * )pEvent;

-( void ) rightMouseDown:( NSEvent * )pEvent;

@end

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_NS_INTERNAL_NS_WINDOW_H__
