
#include "nsOSXMetalSupport.h"

@implementation NSOSXMetalView

-( NSOSXMetalView * ) initForWindow:( NSOSXWindow * )pWindow
{
@autoreleasepool
{
	[super initForWindow:pWindow];

	//
	[self setLayerContentsRedrawPolicy:NSViewLayerContentsRedrawDuringViewResize];

	auto * caMetalLayer = ( CAMetalLayer * )[self layer];
	[caMetalLayer setHidden:NO];

	mMetalLayer = ( CAMetalLayer * )[self layer];

    return self;
}
}

-( BOOL ) acceptsFirstResponder
{
	return YES;
}

-( CALayer * ) makeBackingLayer
{
	return [CAMetalLayer layer];
}

@end
