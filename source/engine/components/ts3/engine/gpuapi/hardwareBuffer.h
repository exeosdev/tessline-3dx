
#ifndef __TS3_ENGINE_HARDWARE_BUFFER_H__
#define __TS3_ENGINE_HARDWARE_BUFFER_H__

#include "commonGPUDefs.h"

namespace ts3
{

	class TS3_ENGINE_API HardwareBuffer : public DynamicInterface
	{
	public:
		const GPUBufferRef & mGPUBufferRef;

		HardwareBuffer();
		virtual ~HardwareBuffer();

	private:
		GPUBufferRef _bufferRef;
	};

}

#endif // __TS3_ENGINE_HARDWARE_BUFFER_H__
