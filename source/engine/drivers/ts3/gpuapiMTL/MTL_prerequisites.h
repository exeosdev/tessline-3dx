
#ifndef __TS3DRIVER_GPUAPI_MTLCOMMON_PREREQUISITES_H__
#define __TS3DRIVER_GPUAPI_MTLCOMMON_PREREQUISITES_H__

#include <ts3/gpuapi/memory/commonGPUMemoryDefs.h>
#include <ts3/system/metalDriver.h>
#include <ts3/system/metalNative.h>

#include <Metal/MTLDefines.h>
#include <Metal/MTLDevice.h>
#include <Metal/MTLEvent.h>
#include <Metal/MTLFence.h>

#include "prerequisites/MTL_featureLevel.h"

namespace ts3::gpuapi
{

	ts3DeclareClassHandle( MetalCommandList );
	ts3DeclareClassHandle( MetalCommandSystem );
	ts3DeclareClassHandle( MetalGPUDevice );
	ts3DeclareClassHandle( MetalGPUDriver );
	ts3DeclareClassHandle( MetalPresentationLayer );
	
} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_MTLCOMMON_PREREQUISITES_H__
