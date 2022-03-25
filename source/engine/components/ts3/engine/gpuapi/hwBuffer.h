
#ifndef __TS3_ENGINE_HARDWARE_BUFFER_H__
#define __TS3_ENGINE_HARDWARE_BUFFER_H__

#include "hwBufferCommon.h"
#include <ts3/core/signals/signalEmitter.h>
#include <ts3/stdext/bitmaskAtomic.h>

namespace ts3
{

	struct HWBufferEventProxy
	{
		template <typename TpEvent>
		using EventEmitterType = EventEmitter<HWBuffer, TpEvent>;

		using EvtGPURefUpdate = Event<E_EVT_HWB_COMMON_GPU_REF_UPDATE_BEGIN, GPUBufferRef &, GPUBufferRef &>;
		using EvtLocked = Event<E_EVT_HWB_COMMON_LOCKED>;
		using EvtUnlocked = Event<E_EVT_HWB_COMMON_UNLOCKED>;

		EventEmitterType<EvtGPURefUpdate> eGPURefUpdate;
		EventEmitterType<EvtLocked> eLocked;
		EventEmitterType<EvtUnlocked> eUnlocked;

		explicit HWBufferEventProxy( HWBuffer & pHWBufferRef ) noexcept
		: eGPURefUpdate( pHWBufferRef )
		, eLocked( pHWBufferRef )
		, eUnlocked( pHWBufferRef )
		{}
	};

	class TS3_ENGINE_API HWBuffer : public DynamicInterface
	{
	public:
		const GPUBufferRef & mGPUBufferRef;
		const HWBufferEventProxy & mEventProxy;

		HWBuffer();
		virtual ~HWBuffer();

		void lock();

		void unlock();

	friendapi:
		void setGPUBufferRef( GPUBufferRef pBufferRef );

	private:
		GPUBufferRef _gpuBufferRef;
		HWBufferEventProxy _eventProxy;
		AtomicBitmask<uint32> _lockFlag;
	};

}

#endif // __TS3_ENGINE_HARDWARE_BUFFER_H__
