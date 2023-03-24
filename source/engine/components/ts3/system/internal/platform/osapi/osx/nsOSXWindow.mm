
#include "nsOSXWindow.h"
#include "osxWindowSystem.h"

#import <QuartzCore/CALayer.h>

@implementation NSOSXWindow : NSWindow

-( BOOL ) canBecomeKeyWindow
{
	return YES;
}

-( BOOL ) canBecomeMainWindow
{
	return YES;
}

-( void ) sendEvent:( NSEvent * )pEvent
{
	[super sendEvent:pEvent];
}

-( void ) doCommandBySelector:( SEL )pSelector
{
}

-( void ) keyDown:( NSEvent * )pEvent
{
	[super keyDown:pEvent];
}

-( void ) mouseDown:( NSEvent * )pEvent
{
	[super mouseDown:pEvent];
}

-( void ) rightMouseDown:( NSEvent * )pEvent
{
	[super rightMouseDown:pEvent];
}

@end


@implementation NSOSXWindowView : NSView

-( NSOSXWindowView * ) initForWindow:( NSOSXWindow * )pWindow
{
	const auto windowRect = [pWindow contentRectForFrameRect:[pWindow frame]];

	[super initWithFrame:windowRect];

	[self setWantsLayer:YES];

	// Set this view as the window's main content view.
	[pWindow setContentView:self];

	// Bind the view as the responder. All defined event handlers inside this view
	// will get called first before (optionally) being forwarded to the window itself.
	[pWindow makeFirstResponder:self];

	// Next responder for this view is the window.
	// E.g. calling [super keyDown:pEvent] in the view will forward the even to the window handler.
	[self setNextResponder:pWindow];

	// Enable touch events support.
	if( platform::osxCheckAppKitFrameworkVersion( NSAppKitVersionNumber10_12_2 ) )
	{
		[self setAllowedTouchTypes:NSTouchTypeMaskDirect];
	}
	else
	{
		[self setAcceptsTouchEvents:YES];
	}

	mNSWindow = pWindow;

	return self;
}

-( BOOL ) acceptsFirstResponder
{
	return YES;
}

-( void ) keyDown:( NSEvent * )pEvent
{
	[super keyDown:pEvent];
}

-( void ) mouseDown:( NSEvent * )pEvent
{
	[super mouseDown:pEvent];
}

-( void ) rightMouseDown:( NSEvent * )pEvent
{
	[super rightMouseDown:pEvent];
}

@end
