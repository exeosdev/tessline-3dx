
#include "hwBuffer.h"

namespace ts3
{

	HWBuffer::HWBuffer()
	: mGPUBufferRef( _gpuBufferRef )
	, mEventProxy( _eventProxy )
	, _eventProxy( *this )
	{}

	HWBuffer::~HWBuffer() = default;

	void HWBuffer::setGPUBufferRef( GPUBufferRef pBufferRef )
	{
		_eventProxy.eGPURefUpdate.emit( _gpuBufferRef, pBufferRef );

		_gpuBufferRef = pBufferRef;
	}

}
