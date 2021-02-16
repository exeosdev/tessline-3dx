
#ifndef __TS3_ENGINE_HARDWARE_BUFFER_MANAGER_H__
#define __TS3_ENGINE_HARDWARE_BUFFER_MANAGER_H__

#include "hardwareBuffer.h"
#include <ts3/stdext/memory.h>

namespace ts3
{

	using gpuapi_buffer_ref_id_t = uint64;

	struct HardwareBufferCreateInfo
	{
		union Metrics
		{
			GenericHWBufferMetrics uGeneric;
			VertexHWBufferMetrics uVertexBuffer;
			IndexHWBufferMetrics uIndexBuffer;
		};

		EHardwareBufferType baseType = EHardwareBufferType::HBTUnknown;
		Bitmask<hardware_buffer_flags_value_t> flags = 0;
		Metrics metrics;
		gpuapi::GPUBufferInitDataDesc initData;

		HardwareBufferCreateInfo()
		{
			memZero( metrics );
		}
	};

	class HardwareBufferManager
	{
	public:
		struct GPUBufferState
		{
			using HWBufferList = std::list<HardwareBufferWeakHandle>;
			gpuapi_buffer_ref_id_t bufferRefID;
			gpuapi::GPUBufferHandle gpuBuffer;
			HWBufferList allocatedHWBufferList;
			gpuapi::memory_size_t availableMemorySize;
			gpuapi::memory_offset_t currentAllocOffset;
		};

		Result allocateGPUBufferExplicit( gpuapi_buffer_ref_id_t pGPUBufferRefID,
		                                  const gpuapi::GPUBufferCreateInfo & pGPUBufferCreateInfo );

		HardwareVertexBufferHandle createVertexBuffer( const HardwareBufferCreateInfo & pHWBCreateInfo );

		HardwareVertexBufferHandle createVertexBufferEx( gpuapi_buffer_ref_id_t pGBUBufferRefID,
		                                                 const HardwareBufferCreateInfo & pHWBCreateInfo );

		HardwareIndexBufferHandle createIndexBuffer( const HardwareBufferCreateInfo & pHWBCreateInfo );

		HardwareIndexBufferHandle createIndexBufferEx( gpuapi_buffer_ref_id_t pGBUBufferRefID,
		                                               const HardwareBufferCreateInfo & pHWBCreateInfo );

		TS3_PCL_ATTR_NO_DISCARD GPUBufferUsageInfo getGPUBufferInfo( gpuapi_buffer_ref_id_t pGPUBufferRefID ) const;

		static gpuapi::memory_align_t queryAlignmentRequirementsForBuffer( EHardwareBufferType pBufferType,
		                                                                   gpuapi::memory_size_t pBufferSize,
		                                                                   Bitmask<gpuapi::memory_flags_value_t> pMemoryFlags = 0 );

	private:
		gpuapi::GPUBufferHandle _createGPUBuffer( gpuapi_buffer_ref_id_t pGPUBufferRefID,
		                                          const HardwareBufferCreateInfo & pHWBCreateInfo );

		GPUBufferRef _reserveGPUBufferRegion( gpuapi_buffer_ref_id_t pGPUBufferRefID,
		                                      gpuapi::memory_size_t pSize,
		                                      gpuapi::memory_align_t pAlignment = 0 );

		static void _validateBufferCreateInfo( EHardwareBufferType pBufferType, HardwareBufferCreateInfo & pHWBCreateInfo );

	private:
		using GPUBufferMap = std::unordered_map<gpuapi_buffer_ref_id_t, GPUBufferState>;
		gpuapi::GPUDeviceHandle _gpuDevice;
		GPUBufferMap _gpuBufferMap;
	};

}

#endif // __TS3_ENGINE_HARDWARE_BUFFER_MANAGER_H__
