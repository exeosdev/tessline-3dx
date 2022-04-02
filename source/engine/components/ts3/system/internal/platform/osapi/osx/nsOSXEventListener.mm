
#include "nsOSXEventListener.h"
#include "osxEventCore.h"

@implementation NSOSXEventListener

-( void ) bind
{
@autoreleasepool
{
	NSNotificationCenter * nsNotificationCenter = [NSNotificationCenter defaultCenter];

	NSWindow * window = self->mEventSourceNativeData->nsWindow;
	NSView * windowView = [window contentView];

	if( [window delegate] != nil )
	{
		[nsNotificationCenter addObserver:self selector:@selector(windowDidExpose:) name:NSWindowDidExposeNotification object:window];
		[nsNotificationCenter addObserver:self selector:@selector(windowDidMove:) name:NSWindowDidMoveNotification object:window];
		[nsNotificationCenter addObserver:self selector:@selector(windowDidResize:) name:NSWindowDidResizeNotification object:window];
		[nsNotificationCenter addObserver:self selector:@selector(windowDidMiniaturize:) name:NSWindowDidMiniaturizeNotification object:window];
		[nsNotificationCenter addObserver:self selector:@selector(windowDidDeminiaturize:) name:NSWindowDidDeminiaturizeNotification object:window];
		[nsNotificationCenter addObserver:self selector:@selector(windowDidBecomeKey:) name:NSWindowDidBecomeKeyNotification object:window];
		[nsNotificationCenter addObserver:self selector:@selector(windowDidResignKey:) name:NSWindowDidResignKeyNotification object:window];
		[nsNotificationCenter addObserver:self selector:@selector(windowDidChangeBackingProperties:) name:NSWindowDidChangeBackingPropertiesNotification object:window];
		[nsNotificationCenter addObserver:self selector:@selector(windowDidChangeScreenProfile:) name:NSWindowDidChangeScreenProfileNotification object:window];
		[nsNotificationCenter addObserver:self selector:@selector(windowWillEnterFullScreen:) name:NSWindowWillEnterFullScreenNotification object:window];
		[nsNotificationCenter addObserver:self selector:@selector(windowDidEnterFullScreen:) name:NSWindowDidEnterFullScreenNotification object:window];
		[nsNotificationCenter addObserver:self selector:@selector(windowWillExitFullScreen:) name:NSWindowWillExitFullScreenNotification object:window];
		[nsNotificationCenter addObserver:self selector:@selector(windowDidExitFullScreen:) name:NSWindowDidExitFullScreenNotification object:window];
		[nsNotificationCenter addObserver:self selector:@selector(windowDidFailToEnterFullScreen:) name:@"NSWindowDidFailToEnterFullScreenNotification" object:window];
		[nsNotificationCenter addObserver:self selector:@selector(windowDidFailToExitFullScreen:) name:@"NSWindowDidFailToExitFullScreenNotification" object:window];
	}
	else
	{
		[window setDelegate:self];
	}

	[window addObserver:self forKeyPath:@"visible" options:NSKeyValueObservingOptionNew context:nullptr];

	[window setNextResponder:self];
	[window setAcceptsMouseMovedEvents:YES];

	[windowView setNextResponder:self];
	[windowView setAcceptsTouchEvents:YES];
}
}

-( void ) unbind
{
@autoreleasepool
{
	NSNotificationCenter * nsNotificationCenter = [NSNotificationCenter defaultCenter];

	NSWindow * window = self->mEventSourceNativeData->nsWindow;
	NSView * windowView = [window contentView];

	if( [window delegate] != self )
	{
		[nsNotificationCenter removeObserver:self name:NSWindowDidExposeNotification object:window];
		[nsNotificationCenter removeObserver:self name:NSWindowDidMoveNotification object:window];
		[nsNotificationCenter removeObserver:self name:NSWindowDidResizeNotification object:window];
		[nsNotificationCenter removeObserver:self name:NSWindowDidMiniaturizeNotification object:window];
		[nsNotificationCenter removeObserver:self name:NSWindowDidDeminiaturizeNotification object:window];
		[nsNotificationCenter removeObserver:self name:NSWindowDidBecomeKeyNotification object:window];
		[nsNotificationCenter removeObserver:self name:NSWindowDidResignKeyNotification object:window];
		[nsNotificationCenter removeObserver:self name:NSWindowDidChangeBackingPropertiesNotification object:window];
		[nsNotificationCenter removeObserver:self name:NSWindowDidChangeScreenProfileNotification object:window];
		[nsNotificationCenter removeObserver:self name:NSWindowWillEnterFullScreenNotification object:window];
		[nsNotificationCenter removeObserver:self name:NSWindowDidEnterFullScreenNotification object:window];
		[nsNotificationCenter removeObserver:self name:NSWindowWillExitFullScreenNotification object:window];
		[nsNotificationCenter removeObserver:self name:NSWindowDidExitFullScreenNotification object:window];
		[nsNotificationCenter removeObserver:self name:@"NSWindowDidFailToEnterFullScreenNotification" object:window];
		[nsNotificationCenter removeObserver:self name:@"NSWindowDidFailToExitFullScreenNotification" object:window];
	}
	else
	{
		[window setDelegate:nil];
	}

	[window removeObserver:self forKeyPath:@"visible"];

	if( [window nextResponder] == self )
	{
		[window setNextResponder:nil];
	}

	if( [windowView nextResponder] == self )
	{
		[windowView setNextResponder:nil];
	}
}
}

@end
