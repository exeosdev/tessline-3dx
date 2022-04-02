
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
}

-( void ) doCommandBySelector: ( SEL )pSelector
{
}

@end


@implementation NSOSXWindowView : NSView

-( void ) setNSWindow: ( NSOSXWindow * )pWindow
{
	self->mNSWindow = pWindow;
}

@end
