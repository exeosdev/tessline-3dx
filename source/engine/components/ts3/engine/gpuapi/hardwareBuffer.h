
#ifndef __TS3_ENGINE_HARDWARE_BUFFER_H__
#define __TS3_ENGINE_HARDWARE_BUFFER_H__

#include "hardwareBufferCommon.h"
#include <ts3/core/signals/signalEmitter.h>

namespace ts3
{

	struct HardwareBufferEventProxy
	{
		template <typename TpEvent>
		using EventEmitterType = EventEmitter<HardwareBuffer, TpEvent>;

		using EvtLocked = Event<E_EVT_HWB_LOCKED>;
		using EvtUnlocked = Event<E_EVT_HWB_UNLOCKED>;

		EventEmitterType<EvtLocked> eLocked;
		EventEmitterType<EvtUnlocked> eUnlocked;

		explicit HardwareBufferEventProxy( HardwareBuffer & pHWBufferRef ) noexcept
		: eLocked( pHWBufferRef )
		, eUnlocked( pHWBufferRef )
		{}
	};

	class TS3_ENGINE_API HardwareBuffer : public DynamicInterface
	{
	public:
		const GPUBufferRef & mGPUBufferRef;
		const HardwareBufferEventProxy & mEventProxy;

		HardwareBuffer();
		virtual ~HardwareBuffer();

	private:
		GPUBufferRef _gpuBufferRef;
		HardwareBufferEventProxy _eventProxy;
	};

}

#endif // __TS3_ENGINE_HARDWARE_BUFFER_H__
