
#include "nsOSXWindow.h"
#include "osxWindowSystem.h"

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

	// Set this view as the window's main content view.
	[mNSWindow setContentView:self];

	// Bind the view as the responder. All defined event handlers inside this view
	// will get called first before (optionally) being forwarded to the window itself.
	[mNSWindow makeFirstResponder:self];

	// Next responder for this view is the window.
	// E.g. calling [super keyDown:pEvent] in the view will forward the even to the window handler.
	[self setNextResponder:mNSWindow];

	// Enable touch events support.
	[self setAcceptsTouchEvents:YES];

	mNSWindow = pWindow;
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
