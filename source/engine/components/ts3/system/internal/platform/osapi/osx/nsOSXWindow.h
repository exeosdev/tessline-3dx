
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_NS_INTERNAL_NS_WINDOW_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_NS_INTERNAL_NS_WINDOW_H__

#include "osxCommon.h"
#include <ts3/system/windowCommon.h>

#import <AppKit/NSWindow.h>

namespace ts3::system
{

	namespace platform
	{

		struct OSXWindowNativeData;

	}

}

/// @brief
@interface NSOSXWindow : NSWindow
{
	@public ts3::system::platform::OSXWindowNativeData * mWindowNativeData;
}

-( BOOL ) canBecomeKeyWindow;

-( BOOL ) canBecomeMainWindow;

-( void ) sendEvent:( NSEvent * ) pEvent;

-( void ) doCommandBySelector:( SEL ) pSelector;

@end


/// @brief
@interface NSOSXWindowView : NSView
{
	NSOSXWindow * mNSWindow;
}

-( void ) setNSWindow:( NSOSXWindow * ) pWindow;

@end

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_NS_INTERNAL_NS_WINDOW_H__
