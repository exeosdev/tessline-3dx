
#ifndef __TS3_ENGINE_HARDWARE_BUFFER_MANAGER_H__
#define __TS3_ENGINE_HARDWARE_BUFFER_MANAGER_H__

#include "hwBuffer.h"
#include <ts3/stdext/memory.h>

namespace ts3
{

	using gpuapi_buffer_ref_id_t = uint64;

	inline constexpr auto CX_GPUAPI_BUFFER_REF_ID_AUTO = Limits<gpuapi_buffer_ref_id_t>::maxValue;

	struct HWBufferCreateInfo
	{
		union Metrics
		{
			HWBufferMetrics uGeneric;
			HWVertexBufferMetrics uVertexBuffer;
			HWIndexBufferMetrics uIndexBuffer;
		};

		// Primary type of the buffer
		EHWBufferType baseType = EHWBufferType::HBTUnknown;
		Bitmask<hardware_buffer_flags_value_t> flags = 0;
		Metrics metrics;
		gpuapi::GPUBufferInitDataDesc initData;

		HWBufferCreateInfo()
		{
			memZero( metrics );
		}
	};

	class HWBufferManager
	{
	public:
		struct GPUBufferState
		{
			using HWBufferList = std::list<HWBufferWeakHandle>;
			gpuapi_buffer_ref_id_t bufferRefID;
			gpuapi::GPUBufferHandle gpuBuffer;
			HWBufferList allocatedHWBufferList;
			gpuapi::gpu_memory_size_t availableMemorySize;
			gpuapi::gpu_memory_size_t currentAllocOffset;
		};

		ResultCode allocateGPUBufferExplicit( gpuapi_buffer_ref_id_t pGPUBufferRefID,
											  const gpuapi::GPUBufferCreateInfo & pGPUBufferCreateInfo );

		HWVertexBufferHandle createVertexBuffer( const HWBufferCreateInfo & pHWBCreateInfo );

		HWVertexBufferHandle createVertexBufferEx( gpuapi_buffer_ref_id_t pGBUBufferRefID,
		                                           const HWBufferCreateInfo & pHWBCreateInfo );

		HWIndexBufferHandle createIndexBuffer( const HWBufferCreateInfo & pHWBCreateInfo );

		HWIndexBufferHandle createIndexBufferEx( gpuapi_buffer_ref_id_t pGBUBufferRefID,
		                                         const HWBufferCreateInfo & pHWBCreateInfo );

		TS3_PCL_ATTR_NO_DISCARD GPUBufferUsageInfo getGPUBufferInfo( gpuapi_buffer_ref_id_t pGPUBufferRefID ) const;

		static memory_align_t queryAlignmentRequirementsForBuffer( EHWBufferType pBufferType,
		                                                           gpuapi::gpu_memory_size_t pBufferSize,
		                                                           Bitmask<gpuapi::gpu_memory_flags_value_t> pMemoryFlags = 0 );

	private:
		gpuapi::GPUBufferHandle _createGPUBuffer( gpuapi_buffer_ref_id_t pGPUBufferRefID,
		                                          const HWBufferCreateInfo & pHWBCreateInfo );

		GPUBufferRef _reserveGPUBufferRegion( gpuapi_buffer_ref_id_t pGPUBufferRefID,
		                                      gpuapi::gpu_memory_size_t pSize,
		                                      memory_align_t pAlignment = 0 );

		static void _validateBufferCreateInfo( EHWBufferType pBufferType, HWBufferCreateInfo & pHWBCreateInfo );

	private:
		using GPUBufferMap = std::unordered_map<gpuapi_buffer_ref_id_t, GPUBufferState>;
		gpuapi::GPUDeviceHandle _gpuDevice;
		GPUBufferMap _gpuBufferMap;
	};

}

#endif // __TS3_ENGINE_HARDWARE_BUFFER_MANAGER_H__
