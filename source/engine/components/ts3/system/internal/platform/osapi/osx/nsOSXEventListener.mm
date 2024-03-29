
#include "nsOSXEventListener.h"
#include "osxEventCore.h"
#include <ts3/system/internal/eventCorePrivate.h>

@implementation NSOSXEventListener

-( NSOSXEventListener * ) initForNSWindow:( NSWindow * ) pNSWindow
{
	[super init];
	mNSWindow = pNSWindow;
	mNSView = [mNSWindow contentView];
	return self;
}

-( void ) bind:( OSXEventController * ) pEventController
{
@autoreleasepool
{
	ts3DebugAssert( mNSWindow != nil );

	mEventController = pEventController;

	if( [mNSWindow delegate] != nil )
	{
		NSNotificationCenter * nsNotificationCenter = [NSNotificationCenter defaultCenter];
		[nsNotificationCenter addObserver:self selector:@selector(windowDidExpose:) name:NSWindowDidExposeNotification object:mNSWindow];
		[nsNotificationCenter addObserver:self selector:@selector(windowDidMove:) name:NSWindowDidMoveNotification object:mNSWindow];
		[nsNotificationCenter addObserver:self selector:@selector(windowDidResize:) name:NSWindowDidResizeNotification object:mNSWindow];
		[nsNotificationCenter addObserver:self selector:@selector(windowDidMiniaturize:) name:NSWindowDidMiniaturizeNotification object:mNSWindow];
		[nsNotificationCenter addObserver:self selector:@selector(windowDidDeminiaturize:) name:NSWindowDidDeminiaturizeNotification object:mNSWindow];
		[nsNotificationCenter addObserver:self selector:@selector(windowDidBecomeKey:) name:NSWindowDidBecomeKeyNotification object:mNSWindow];
		[nsNotificationCenter addObserver:self selector:@selector(windowDidResignKey:) name:NSWindowDidResignKeyNotification object:mNSWindow];
		[nsNotificationCenter addObserver:self selector:@selector(windowDidChangeBackingProperties:) name:NSWindowDidChangeBackingPropertiesNotification object:mNSWindow];
		[nsNotificationCenter addObserver:self selector:@selector(windowDidChangeScreenProfile:) name:NSWindowDidChangeScreenProfileNotification object:mNSWindow];
		[nsNotificationCenter addObserver:self selector:@selector(windowWillEnterFullScreen:) name:NSWindowWillEnterFullScreenNotification object:mNSWindow];
		[nsNotificationCenter addObserver:self selector:@selector(windowDidEnterFullScreen:) name:NSWindowDidEnterFullScreenNotification object:mNSWindow];
		[nsNotificationCenter addObserver:self selector:@selector(windowWillExitFullScreen:) name:NSWindowWillExitFullScreenNotification object:mNSWindow];
		[nsNotificationCenter addObserver:self selector:@selector(windowDidExitFullScreen:) name:NSWindowDidExitFullScreenNotification object:mNSWindow];
		//[nsNotificationCenter addObserver:self selector:@selector(windowDidFailToEnterFullScreen:) name:@"NSWindowDidFailToEnterFullScreenNotification" object:mNSWindow];
		//[nsNotificationCenter addObserver:self selector:@selector(windowDidFailToExitFullScreen:) name:@"NSWindowDidFailToExitFullScreenNotification" object:mNSWindow];
	}
	else
	{
		[mNSWindow setDelegate:self];
	}

	[mNSWindow addObserver:self forKeyPath:@"visible" options:NSKeyValueObservingOptionNew context:nullptr];

	[mNSWindow setNextResponder:self];
	[mNSWindow setAcceptsMouseMovedEvents:YES];
}
}

-( void ) unbind
{
@autoreleasepool
{
	if( [mNSWindow delegate] != self )
	{
		NSNotificationCenter * nsNotificationCenter = [NSNotificationCenter defaultCenter];
		[nsNotificationCenter removeObserver:self name:NSWindowDidExposeNotification object:mNSWindow];
		[nsNotificationCenter removeObserver:self name:NSWindowDidMoveNotification object:mNSWindow];
		[nsNotificationCenter removeObserver:self name:NSWindowDidResizeNotification object:mNSWindow];
		[nsNotificationCenter removeObserver:self name:NSWindowDidMiniaturizeNotification object:mNSWindow];
		[nsNotificationCenter removeObserver:self name:NSWindowDidDeminiaturizeNotification object:mNSWindow];
		[nsNotificationCenter removeObserver:self name:NSWindowDidBecomeKeyNotification object:mNSWindow];
		[nsNotificationCenter removeObserver:self name:NSWindowDidResignKeyNotification object:mNSWindow];
		[nsNotificationCenter removeObserver:self name:NSWindowDidChangeBackingPropertiesNotification object:mNSWindow];
		[nsNotificationCenter removeObserver:self name:NSWindowDidChangeScreenProfileNotification object:mNSWindow];
		[nsNotificationCenter removeObserver:self name:NSWindowWillEnterFullScreenNotification object:mNSWindow];
		[nsNotificationCenter removeObserver:self name:NSWindowDidEnterFullScreenNotification object:mNSWindow];
		[nsNotificationCenter removeObserver:self name:NSWindowWillExitFullScreenNotification object:mNSWindow];
		[nsNotificationCenter removeObserver:self name:NSWindowDidExitFullScreenNotification object:mNSWindow];
		//[nsNotificationCenter removeObserver:self name:@"NSWindowDidFailToEnterFullScreenNotification" object:mNSWindow];
		//[nsNotificationCenter removeObserver:self name:@"NSWindowDidFailToExitFullScreenNotification" object:mNSWindow];
	}
	else
	{
		[mNSWindow setDelegate:nil];
	}

	[mNSWindow removeObserver:self forKeyPath:@"visible"];

	if( [mNSWindow nextResponder] == self )
	{
		[mNSWindow setNextResponder:nil];
	}

	if( [mNSView nextResponder] == self )
	{
		[mNSView setNextResponder:nil];
	}

	mEventController = nullptr;
}
}

-( void ) observeValueForKeyPath:( NSString * ) pKeyPath
          ofObject:( id ) pObject
          change:( NSDictionary * ) pChange
          context:( void * ) pContext
{
	if( (  pObject == mNSWindow ) && [pKeyPath isEqualToString:@"visible"] )
	{
		int newVisibility = [[pChange objectForKey:@"new"] intValue];
		if( newVisibility != 0 )
		{
			// visible
		}
		else
		{
			// hidden
		}
	}
}

-( void ) pauseVisibleObservation
{
}

-( void ) resumeVisibleObservation
{
}

#define ts3DbgEventLog(...) // ts3DebugOutput(__VA_ARGS__)

-( void ) keyDown:( NSEvent * ) pEvent
{
	ts3DbgEventLog("keyDown");
	nativeEventDispatch( *mEventController, platform::NativeEventType( pEvent ) );
}

-( void ) keyUp:( NSEvent * ) pEvent
{
	ts3DbgEventLog("keyUp");
	nativeEventDispatch( *mEventController, platform::NativeEventType( pEvent ) );
}

-( void ) flagsChanged:( NSEvent * ) pEvent
{
	ts3DbgEventLog("flagsChanged");
	nativeEventDispatch( *mEventController, platform::NativeEventType( pEvent ) );
}

-( void ) mouseEntered:( NSEvent * ) pEvent
{
	ts3DbgEventLog("mouseEntered");
	nativeEventDispatch( *mEventController, platform::NativeEventType( pEvent ) );
}

-( void ) mouseExited:( NSEvent * ) pEvent
{
	ts3DbgEventLog("mouseExited");
	nativeEventDispatch( *mEventController, platform::NativeEventType( pEvent ) );
}

-( void ) mouseMoved:( NSEvent * ) pEvent
{
	ts3DbgEventLog("mouseMoved");
	nativeEventDispatch( *mEventController, platform::NativeEventType( pEvent ) );
}

-( void ) mouseDown:( NSEvent * ) pEvent
{
	ts3DbgEventLog("mouseDown");
	nativeEventDispatch( *mEventController, platform::NativeEventType( pEvent ) );
}

-( void ) mouseUp:( NSEvent * ) pEvent
{
	ts3DbgEventLog("mouseUp");
	nativeEventDispatch( *mEventController, platform::NativeEventType( pEvent ) );
}

-( void ) mouseDragged:( NSEvent * ) pEvent
{
	ts3DbgEventLog("mouseDragged");
	nativeEventDispatch( *mEventController, platform::NativeEventType( pEvent ) );
}

-( void ) rightMouseDown:( NSEvent * ) pEvent
{
	ts3DbgEventLog("rightMouseDown");
	nativeEventDispatch( *mEventController, platform::NativeEventType( pEvent ) );
}

-( void ) rightMouseUp:( NSEvent * ) pEvent
{
	ts3DbgEventLog("rightMouseUp");
	nativeEventDispatch( *mEventController, platform::NativeEventType( pEvent ) );
}

-( void ) rightMouseDragged:( NSEvent * ) pEvent
{
	ts3DbgEventLog("rightMouseDragged");
	nativeEventDispatch( *mEventController, platform::NativeEventType( pEvent ) );
}

-( void ) otherMouseDown:( NSEvent * ) pEvent
{
	ts3DbgEventLog("otherMouseDown");
	nativeEventDispatch( *mEventController, platform::NativeEventType( pEvent ) );
}

-( void ) otherMouseUp:( NSEvent * ) pEvent
{
	ts3DbgEventLog("otherMouseUp");
	nativeEventDispatch( *mEventController, platform::NativeEventType( pEvent ) );
}

-( void ) otherMouseDragged:( NSEvent * ) pEvent
{
	ts3DbgEventLog("otherMouseDragged");
	nativeEventDispatch( *mEventController, platform::NativeEventType( pEvent ) );
}

-( void ) scrollWheel:( NSEvent * ) pEvent
{
	ts3DbgEventLog("scrollWheel");
	nativeEventDispatch( *mEventController, platform::NativeEventType( pEvent ) );
}

-( void ) tabletPoint:( NSEvent * ) pEvent
{
	nativeEventDispatch( *mEventController, platform::NativeEventType( pEvent ) );
}

-( void ) tabletProximity:( NSEvent * ) pEvent
{
	nativeEventDispatch( *mEventController, platform::NativeEventType( pEvent ) );
}

-( void ) touchesBeganWithEvent:( NSEvent * ) pEvent
{
}

-( void ) touchesMovedWithEvent:( NSEvent * ) pEvent
{
}

-( void ) touchesEndedWithEvent:( NSEvent * ) pEvent
{
}

-( void ) touchesCancelledWithEvent:( NSEvent * ) pEvent
{
}

-( void ) handleTouches:( NSTouchPhase ) pTouchPhase withEvent:( NSEvent * ) pEvent
{
}

-( void ) windowDidExpose:( NSNotification * ) pNotification
{
}

-( void ) windowDidMove:( NSNotification * ) pNotification
{
}

-( void ) windowDidResize:( NSNotification * ) pNotification
{
}

-( void ) windowDidMiniaturize:( NSNotification * ) pNotification
{
}

-( void ) windowDidDeminiaturize:( NSNotification * ) pNotification
{
}

-( void ) windowDidBecomeKey:( NSNotification * ) pNotification
{
}

-( void ) windowDidResignKey:( NSNotification * ) pNotification
{
}

-( void ) windowDidChangeBackingProperties:( NSNotification * ) pNotification
{
}

-( void ) windowDidChangeScreenProfile:( NSNotification * ) pNotification
{
}

-( void ) windowWillEnterFullScreen:( NSNotification * ) pNotification
{
}

-( void ) windowDidEnterFullScreen:( NSNotification * ) pNotification
{
}

-( void ) windowWillExitFullScreen:( NSNotification * ) pNotification
{
}

-( void ) windowDidExitFullScreen:( NSNotification * ) pNotification
{
}

-( void ) windowWillClose:( NSNotification * ) pNotification
{
	nativeEventDispatch( *mEventController, platform::NativeEventType( platform::OSXEventIDWindowWillClose, pNotification ) );
}

-( BOOL ) windowShouldClose:( id ) pSender
{
	return YES;
}

-( NSApplicationPresentationOptions ) window:( NSWindow * ) pWindow
                                      willUseFullScreenPresentationOptions:( NSApplicationPresentationOptions ) pProposedOptions
{
	return pProposedOptions;
}

@end
