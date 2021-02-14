
#ifndef __TS3_ENGINE_GPUAPI_COMMON_H__
#define __TS3_ENGINE_GPUAPI_COMMON_H__

#include <ts3/gpuapi/resources/gpuBufferCommon.h>

namespace ts3
{

	using gpu_memory_align_t = gpuapi::memory_align_t;
	using gpu_memory_offset_t = gpuapi::memory_offset_t;
	using gpu_memory_size_t = gpuapi::memory_size_t;

	enum class EHardwareBufferClass
	{
		HBCGeometry,
		HBCIndirect,
		HBCShader,
		HBCStreamOut,
		HBCTransfer,
	};

	inline constexpr uint64 ecDeclareHardwareBufferType( EHardwareBufferClass pHBC, gpuapi::EGPUBufferTarget pGPUBT )
	{
		return ( ( ( uint64 ) pHBC ) << 32 ) | ( ( uint64 ) pGPUBT );
	}

	enum class EHardwareBufferType : uint64
	{
		HBTVertex = ecDeclareHardwareBufferType( EHardwareBufferClass::HBCGeometry,
		                                         gpuapi::EGPUBufferTarget::VertexBuffer ),

		HBTIndex = ecDeclareHardwareBufferType( EHardwareBufferClass::HBCGeometry,
		                                        gpuapi::EGPUBufferTarget::VertexBuffer ),

		HBTIndirectDraw = ecDeclareHardwareBufferType( EHardwareBufferClass::HBCGeometry,
		                                               gpuapi::EGPUBufferTarget::VertexBuffer ),

		HBTStreamOutput = ecDeclareHardwareBufferType( EHardwareBufferClass::HBCGeometry,
		                                               gpuapi::EGPUBufferTarget::VertexBuffer ),

		HBTUnorderedAccess = ecDeclareHardwareBufferType( EHardwareBufferClass::HBCGeometry,
		                                                  gpuapi::EGPUBufferTarget::VertexBuffer ),
	};

	inline constexpr EHardwareBufferClass ecGetHardwareBufferClass( EHardwareBufferType pHBT )
	{
		return ( EHardwareBufferClass )( ( ( uint64 ) pHBT ) >> 32 );
	}

	inline constexpr gpuapi::EGPUBufferTarget ecGetHardwareBufferGPUBufferTarget( EHardwareBufferType pHBT )
	{
		return ( gpuapi::EGPUBufferTarget )( ( ( uint64 ) pHBT ) & 0xFFFFFFFF );
	}

	struct GPUBufferRef
	{
		gpuapi::GPUBufferHandle buffer = nullptr;
		gpuapi::MemoryRegion usedRegion = { 0, gpuapi::cxMemorySizeMax };

		explicit operator bool() const
		{
			return buffer && !usedRegion.empty();
		}
	};

}

#endif // __TS3_ENGINE_GPUAPI_COMMON_H__
