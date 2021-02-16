
#include "hardwareBuffer.h"

namespace ts3
{

	HardwareBuffer::HardwareBuffer()
	: mGPUBufferRef( _gpuBufferRef )
	, mEventProxy( _eventProxy )
	, _eventProxy( *this )
	{}

	HardwareBuffer::~HardwareBuffer() = default;

}
