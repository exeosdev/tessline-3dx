
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_OSX_NS_INTERNAL_EVENT_LISTENER_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_OSX_NS_INTERNAL_EVENT_LISTENER_H__

#include "nsCommon.h"
#include <ts3/system/eventCommon.h>

#import <AppKit/NSWindow.h>

/// @brief
@interface NSOSXEventListener : NSResponder<NSWindowDelegate>
{
	@public NSWindow * mNSWindow;
	@public NSView * mNSView;
	@public OSXEventController * mEventController;
}

-( NSOSXEventListener * ) initForNSWindow:( NSWindow * ) pNSWindow;

-( void ) bind:( OSXEventController * ) pEventController;
-( void ) unbind;

-( void ) keyDown:( NSEvent * ) pEvent;
-( void ) keyUp:( NSEvent * ) pEvent;

-( void ) mouseEntered:( NSEvent * ) pEvent;
-( void ) mouseExited:( NSEvent * ) pEvent;
-( void ) mouseMoved:( NSEvent * ) pEvent;
-( void ) mouseDown:( NSEvent * ) pEvent;
-( void ) mouseUp:( NSEvent * ) pEvent;
-( void ) mouseDragged:( NSEvent * ) pEvent;
-( void ) rightMouseDown:( NSEvent * ) pEvent;
-( void ) rightMouseUp:( NSEvent * ) pEvent;
-( void ) rightMouseDragged:( NSEvent * ) pEvent;
-( void ) otherMouseDown:( NSEvent * ) pEvent;
-( void ) otherMouseUp:( NSEvent * ) pEvent;
-( void ) otherMouseDragged:( NSEvent * ) pEvent;
-( void ) scrollWheel:( NSEvent * ) pEvent;

-( void ) tabletPoint:( NSEvent * ) pEvent;
-( void ) tabletProximity:( NSEvent * ) pEvent;

-( void ) touchesBeganWithEvent:( NSEvent * ) pEvent;
-( void ) touchesMovedWithEvent:( NSEvent * ) pEvent;
-( void ) touchesEndedWithEvent:( NSEvent * ) pEvent;
-( void ) touchesCancelledWithEvent:( NSEvent * ) pEvent;

-( void ) handleTouches:( NSTouchPhase ) pTouchPhase withEvent:( NSEvent * ) pEvent;

-( void ) windowDidExpose:( NSNotification * ) pNotification;
-( void ) windowDidMove:( NSNotification * ) pNotification;
-( void ) windowDidResize:( NSNotification * ) pNotification;
-( void ) windowDidMiniaturize:( NSNotification * ) pNotification;
-( void ) windowDidDeminiaturize:( NSNotification * ) pNotification;
-( void ) windowDidBecomeKey:( NSNotification * ) pNotification;
-( void ) windowDidResignKey:( NSNotification * ) pNotification;
-( void ) windowDidChangeBackingProperties:( NSNotification * ) pNotification;
-( void ) windowDidChangeScreenProfile:( NSNotification * ) pNotification;
-( void ) windowWillEnterFullScreen:( NSNotification * ) pNotification;
-( void ) windowDidEnterFullScreen:( NSNotification * ) pNotification;
-( void ) windowWillExitFullScreen:( NSNotification * ) pNotification;
-( void ) windowDidExitFullScreen:( NSNotification * ) pNotification;
-( void ) windowWillClose:( NSNotification * ) pNotification;
-( BOOL ) windowShouldClose:( id ) pSender;

-( NSApplicationPresentationOptions ) window:( NSWindow * ) pWindow
                                      willUseFullScreenPresentationOptions:( NSApplicationPresentationOptions ) pProposedOptions;
@end

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_OSX_NS_INTERNAL_EVENT_LISTENER_H__
