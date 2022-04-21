
#ifndef __TS3_ENGINE_HARDWARE_BUFFER_H__
#define __TS3_ENGINE_HARDWARE_BUFFER_H__

#include "hwBufferCommon.h"
#include <ts3/core/signals/signalEmitter.h>

namespace ts3
{

	struct HWBufferEventProxy
	{
		template <typename TpEvent>
		using EventEmitterType = EventEmitter<HWBuffer, TpEvent>;

		using EvtLocked = Event<E_EVT_HWB_LOCKED>;
		using EvtUnlocked = Event<E_EVT_HWB_UNLOCKED>;

		EventEmitterType<EvtLocked> eLocked;
		EventEmitterType<EvtUnlocked> eUnlocked;

		explicit HWBufferEventProxy( HWBuffer & pHWBufferRef ) noexcept
		: eLocked( pHWBufferRef )
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

	private:
		GPUBufferRef _gpuBufferRef;
		HWBufferEventProxy _eventProxy;
	};

} // namespace ts3

#endif // __TS3_ENGINE_HARDWARE_BUFFER_H__
