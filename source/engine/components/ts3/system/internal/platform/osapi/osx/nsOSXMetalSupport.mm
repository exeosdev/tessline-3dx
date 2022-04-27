
#include "nsOSXMetalSupport.h"

@implementation NSOSXMetalView

-( NSOSXMetalView * ) initForWindow:( NSOSXWindow * )pWindow
{
@autoreleasepool
{
	const auto windowRect = [pWindow contentRectForFrameRect:[pWindow frame]];

	[super initWithFrame:windowRect];

	auto * caMetalLayer = [CAMetalLayer layer];

	[self setWantsLayer:YES];
	[self setLayer: caMetalLayer];

	mMetalLayer = caMetalLayer;
	mNSWindow = pWindow;
}
}

@end
