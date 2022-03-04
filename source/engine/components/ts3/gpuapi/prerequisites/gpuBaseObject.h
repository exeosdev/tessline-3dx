
#pragma once

#ifndef __TS3_GPUAPI_GPU_BASE_OBJECT_H__
#define __TS3_GPUAPI_GPU_BASE_OBJECT_H__

namespace ts3
{
namespace gpuapi
{

	class TS3_GPUAPI_CLASS GPUBaseObject : public GPUDeviceChildObject
	{
	public:
		GPUBaseObject( GPUDevice & pGPUDevice );
		virtual ~GPUBaseObject();
	};

} /* namespace ts3 */
} /* namespace gpuapi */

#endif // __TS3_GPUAPI_GPU_BASE_OBJECT_H__
