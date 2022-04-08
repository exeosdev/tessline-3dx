
#include "nsOSXWindow.h"

@implementation NSOSXWindow : NSWindow

-( BOOL ) canBecomeKeyWindow
{
	return YES;
}

-( BOOL ) canBecomeMainWindow
{
	return YES;
}

-( void ) sendEvent: ( NSEvent * )pEvent
{
	[super sendEvent:pEvent];
}

-( void ) doCommandBySelector: ( SEL )pSelector
{
}

@end


@implementation NSOSXWindowView : NSView

-( void ) setNSWindow: ( NSOSXWindow * )pWindow
{
	mNSWindow = pWindow;
}

-( BOOL ) acceptsFirstResponder
{
	return NO;
}

//-( void ) keyDown:( NSEvent * ) pEvent
//{
//	ts3DebugInterrupt();
//	[super keyDown:pEvent];
//}

//-( void ) rightMouseDown:( NSEvent * ) pEvent
//{
//	[super rightMouseDown:pEvent];
//}

@end
