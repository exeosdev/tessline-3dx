
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_OSX_NS_OSX_METAL_VIEW_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_OSX_NS_OSX_METAL_VIEW_H__

#include "nsOSXWindow.h"

#import <CoreGraphics/CGDirectDisplayMetal.h>
#import <QuartzCore/CAMetalLayer.h>

@protocol NSOSXMetalViewDelegate<NSObject>

-( void ) drawableResize:( CGSize )pLayerSize;

-( void ) renderToMetalLayer:( nonnull CAMetalLayer * )pCAMetalLayer;

@end

@interface NSOSXMetalView : NSOSXWindowView //NSView<CALayerDelegate>
{
	@public CAMetalLayer * mMetalLayer;
}

-( NSOSXMetalView * ) initForWindow:( NSOSXWindow * )pWindow;

-( BOOL ) acceptsFirstResponder;

-( CALayer * ) makeBackingLayer;

@end

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_OSX_NS_OSX_METAL_VIEW_H__
