
#ifndef __TS3_ENGINE_HARDWARE_BUFFER_COMMON_H__
#define __TS3_ENGINE_HARDWARE_BUFFER_COMMON_H__

#include "commonGPUDefs.h"
#include <ts3/core/signals/signalCommon.h>

#define ts3MemoryFlagU64Set( pFlag ) ( ( ( uint64 ) pFlag ) << 32 )

namespace ts3
{

	ts3DeclareClassHandle( HardwareBuffer );
	ts3DeclareClassHandle( HardwareGeometryBuffer );
	ts3DeclareClassHandle( HardwareVertexBuffer );
	ts3DeclareClassHandle( HardwareIndexBuffer );

	using hardware_buffer_flags_value_t = uint64;

	enum EEventHWBCodes : event_code_value_t
	{
		E_EVT_HWB_BASE_ID = 0x7F,
		E_EVT_HWB_GPU_BUFFER_REF_UPDATED,
		E_EVT_HWB_LOCKED,
		E_EVT_HWB_UNLOCKED,
	};

	enum class EHardwareBufferType : uint32
	{
		HBTConstantBuffer         = static_cast<uint32>( gpuapi::EGPUBufferTarget::ConstantBuffer ),
		HBTVertexBuffer           = static_cast<uint32>( gpuapi::EGPUBufferTarget::VertexBuffer ),
		HBTIndexBuffer            = static_cast<uint32>( gpuapi::EGPUBufferTarget::IndexBuffer ),
		HBTShaderUAVBuffer        = static_cast<uint32>( gpuapi::EGPUBufferTarget::ShaderUAVBuffer ),
		HBTTransferSourceBuffer   = static_cast<uint32>( gpuapi::EGPUBufferTarget::TransferSourceBuffer ),
		HBTTransferTargetBuffer   = static_cast<uint32>( gpuapi::EGPUBufferTarget::TransferTargetBuffer ),
		HBTUnknown                = static_cast<uint32>( gpuapi::EGPUBufferTarget::Unknown ),
	};

	enum class EHardwareBufferUsageMode : hardware_buffer_flags_value_t
	{
		ConstantBufferDynamic       = gpuapi::E_GPU_BUFFER_BIND_FLAG_CONSTANT_BUFFER_BIT |
		                              gpuapi::E_GPU_RESOURCE_CONTENT_FLAG_DYNAMIC_BIT |
		                              bshLeft<uint64, 32>( gpuapi::E_MEMORY_ACCESS_FLAG_CPU_WRITE_BIT ),

		ConstantBufferImmutable     = gpuapi::E_GPU_BUFFER_BIND_FLAG_CONSTANT_BUFFER_BIT |
		                              gpuapi::E_GPU_RESOURCE_CONTENT_FLAG_IMMUTABLE_BIT,

		ConstantBufferStatic        = gpuapi::E_GPU_BUFFER_BIND_FLAG_CONSTANT_BUFFER_BIT |
		                              gpuapi::E_GPU_RESOURCE_CONTENT_FLAG_STATIC_BIT,

		ConstantBufferWriteDiscard  = gpuapi::E_GPU_BUFFER_BIND_FLAG_CONSTANT_BUFFER_BIT |
		                              gpuapi::E_GPU_RESOURCE_CONTENT_FLAG_DYNAMIC_BIT |
		                              bshLeft<uint64, 32>( gpuapi::E_MEMORY_MAP_FLAG_WRITE_INVALIDATE_BIT ),

		IndexBufferDynamic          = gpuapi::E_GPU_BUFFER_BIND_FLAG_INDEX_BUFFER_BIT |
		                              gpuapi::E_GPU_RESOURCE_CONTENT_FLAG_DYNAMIC_BIT |
		                              bshLeft<uint64, 32>( gpuapi::E_MEMORY_ACCESS_FLAG_CPU_WRITE_BIT ),

		IndexBufferImmutable        = gpuapi::E_GPU_BUFFER_BIND_FLAG_INDEX_BUFFER_BIT |
		                              gpuapi::E_GPU_RESOURCE_CONTENT_FLAG_IMMUTABLE_BIT,

		IndexBufferStatic           = gpuapi::E_GPU_BUFFER_BIND_FLAG_INDEX_BUFFER_BIT |
		                              gpuapi::E_GPU_RESOURCE_CONTENT_FLAG_STATIC_BIT,

		IndexBufferStreamAppend     = gpuapi::E_GPU_BUFFER_BIND_FLAG_INDEX_BUFFER_BIT |
		                              gpuapi::E_GPU_RESOURCE_CONTENT_FLAG_DYNAMIC_BIT |
		                              bshLeft<uint64, 32>( gpuapi::E_MEMORY_MAP_FLAG_WRITE_APPEND_BIT ),

		IndexBufferStreamOverwrite  = gpuapi::E_GPU_BUFFER_BIND_FLAG_INDEX_BUFFER_BIT |
		                              gpuapi::E_GPU_RESOURCE_CONTENT_FLAG_DYNAMIC_BIT |
		                              bshLeft<uint64, 32>( gpuapi::E_MEMORY_ACCESS_FLAG_CPU_WRITE_BIT ),

		VertexBufferDynamic         = gpuapi::E_GPU_BUFFER_BIND_FLAG_VERTEX_BUFFER_BIT |
		                              gpuapi::E_GPU_RESOURCE_CONTENT_FLAG_DYNAMIC_BIT |
		                              bshLeft<uint64, 32>( gpuapi::E_MEMORY_ACCESS_FLAG_CPU_WRITE_BIT ),

		VertexBufferImmutable       = gpuapi::E_GPU_BUFFER_BIND_FLAG_INDEX_BUFFER_BIT |
		                              gpuapi::E_GPU_RESOURCE_CONTENT_FLAG_IMMUTABLE_BIT,

		VertexBufferStatic          = gpuapi::E_GPU_BUFFER_BIND_FLAG_VERTEX_BUFFER_BIT |
		                              gpuapi::E_GPU_RESOURCE_CONTENT_FLAG_STATIC_BIT,

		VertexBufferStreamAppend    = gpuapi::E_GPU_BUFFER_BIND_FLAG_VERTEX_BUFFER_BIT |
		                              gpuapi::E_GPU_RESOURCE_CONTENT_FLAG_DYNAMIC_BIT |
		                              bshLeft<uint64, 32>( gpuapi::E_MEMORY_MAP_FLAG_WRITE_APPEND_BIT ),

		VertexBufferStreamOverwrite = gpuapi::E_GPU_BUFFER_BIND_FLAG_VERTEX_BUFFER_BIT |
		                              gpuapi::E_GPU_RESOURCE_CONTENT_FLAG_DYNAMIC_BIT |
		                              bshLeft<uint64, 32>( gpuapi::E_MEMORY_MAP_FLAG_WRITE_APPEND_BIT ),
	};

	template <typename TpBufferUsage>
	inline constexpr gpuapi::memory_flags_value_t ecGetHWBufferUsageGPUMemoryFlags( TpBufferUsage pBufferUsage )
	{
		return static_cast<gpuapi::memory_flags_value_t>( ( ( ( hardware_buffer_flags_value_t ) pBufferUsage ) >> 32 ) & Limits<uint32>::maxValue );
	}

	template <typename TpBufferUsage>
	inline constexpr gpuapi::resource_flags_value_t ecGetHWBufferUsageGPUResourceFlags( TpBufferUsage pBufferUsage )
	{
		return static_cast<gpuapi::resource_flags_value_t>( ( ( hardware_buffer_flags_value_t ) pBufferUsage ) & Limits<uint32>::maxValue );
	}

}

#endif // __TS3_ENGINE_HARDWARE_BUFFER_COMMON_H__
