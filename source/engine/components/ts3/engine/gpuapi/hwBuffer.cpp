
#include "hwBuffer.h"

namespace ts3
{

	HWBuffer::HWBuffer()
	: mGPUBufferRef( _gpuBufferRef )
	, mEventProxy( _eventProxy )
	, _eventProxy( *this )
	{}

	HWBuffer::~HWBuffer() = default;

} // namespace ts3
