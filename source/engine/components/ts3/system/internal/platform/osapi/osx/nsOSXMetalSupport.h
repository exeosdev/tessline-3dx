
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_OSX_NS_OSX_METAL_VIEW_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_OSX_NS_OSX_METAL_VIEW_H__

#include "nsOSXWindow.h"

#import <CoreGraphics/CGDirectDisplayMetal.h>
#import <QuartzCore/CAMetalLayer.h>

@interface NSOSXMetalView : NSView
{
	@public NSOSXWindow * mNSWindow;
	@public CAMetalLayer * mMetalLayer;
}

-( NSOSXMetalView * ) initForWindow:( NSOSXWindow * )pWindow;

@end

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_OSX_NS_OSX_METAL_VIEW_H__
