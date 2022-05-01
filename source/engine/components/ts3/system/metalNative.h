
#ifndef __TS3_SYSTEM_METAL_NATIVE_H__
#define __TS3_SYSTEM_METAL_NATIVE_H__

#include "metalDriver.h"

#import <Metal/MTLDevice.h>
#import <Metal/MTLCommandQueue.h>
#import <Metal/MTLRenderPass.h>
#import <QuartzCore/CAMetalLayer.h>

namespace ts3::system
{

	struct MetalDeviceData
	{
		id<MTLDevice> mtlDevice;
		id<MTLCommandQueue> mtlMainCmdQueue;
	};

	struct MetalDisplaySurfaceData
	{
		CAMetalLayer * caMetalLayer = nil;

		MTLRenderPassDescriptor * mtlClearRPDescriptor = nil;
	};

	struct MetalSystemDriverData
	{
	};

}

#endif // __TS3_SYSTEM_METAL_NATIVE_H__
