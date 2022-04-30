
#ifndef __TS3_SYSTEM_PLATFORM_SHARED_MTL_COMMON_H__
#define __TS3_SYSTEM_PLATFORM_SHARED_MTL_COMMON_H__

#include <ts3/system/metalCommon.h>

#import <CoreGraphics/CGDirectDisplayMetal.h>
#import <Metal/MTLPixelFormat.h>
#import <QuartzCore/CAMetalLayer.h>

namespace ts3::system
{

    namespace platform
    {

        TS3_FUNC_NO_DISCARD MTLPixelFormat mtlChoosePixelFormatForVisualConfig( const VisualConfig & pVisualConfig );

    }

}

#endif // __TS3_SYSTEM_PLATFORM_SHARED_MTL_COMMON_H__
