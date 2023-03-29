
#pragma once

#ifndef __TS3_ENGINE_CORE_ENGINE_STATE_H__
#define __TS3_ENGINE_CORE_ENGINE_STATE_H__

#include <ts3/system/sysContext.h>

namespace ts3
{

	class CoreEngineState
	{
	public:
		system::SysContextHandle mSysContext;

		gpuapi::GPUDeviceHandle mGPUDevice;

	public:
		CoreEngineState( system::SysContextHandle pSysContext, gpuapi::GPUDeviceHandle pGPUDevice )
		: mSysContext( pSysContext )
		, mGPUDevice( pGPUDevice )
		{}
	};

} // namespace ts3

#endif // __TS3_ENGINE_CORE_ENGINE_STATE_H__
