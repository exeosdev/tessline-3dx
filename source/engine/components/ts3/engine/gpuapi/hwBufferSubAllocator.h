
#ifndef __TS3_ENGINE_HARDWARE_BUFFER_SUB_ALLOCATOR_H__
#define __TS3_ENGINE_HARDWARE_BUFFER_SUB_ALLOCATOR_H__

#include "hwBufferCommon.h"

namespace ts3
{
	
	class HWBufferSubAllocator
	{
	public:
		virtual void beginAlloc() = 0;

		virtual void finishAlloc() = 0;

		virtual bool allocate( gpuapi::gpu_memory_size_t pAllocationSize ) = 0;

	protected:
		enum class InternalAllocatorState : uint32
		{
			Ready,
			Alloc,
			Finished
		};

		InternalAllocatorState _allocatorState;
		std::vector<HWBufferMemoryRef> _hwbMemoryRefList;
	};

	class HWBufferSubAllocator1 : public HWBufferSubAllocator
	{
	public:
		explicit HWBufferSubAllocator1( HWBuffer & pHWBuffer );
		HWBufferSubAllocator1( HWBuffer & pHWBuffer, const gpuapi::GPUMemoryRegion & pSubRegion );

		virtual bool allocate( gpuapi::gpu_memory_size_t pAllocationSize ) override final;
	};

	class HWBufferSubAllocator2 : public HWBufferSubAllocator
	{
	public:
		explicit HWBufferSubAllocator2( HWBufferManager & pHWBufferManager );

		virtual bool allocate( gpuapi::gpu_memory_size_t pAllocationSize ) override final;
	};
	
}

#endif // __TS3_ENGINE_HARDWARE_BUFFER_SUB_ALLOCATOR_H__
