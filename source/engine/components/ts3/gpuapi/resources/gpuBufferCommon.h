
#pragma once

#ifndef __TS3_GPUAPI_GPU_BUFFER_COMMON_H__
#define __TS3_GPUAPI_GPU_BUFFER_COMMON_H__

#include "commonGPUResourceDefs.h"

namespace ts3::gpuapi
{

	ts3DeclareClassHandle( GPUBuffer );

	enum EGPUBufferDataCopyFlags : uint32
	{
		E_GPU_BUFFER_DATA_COPY_FLAG_MODE_APPEND_BIT     = E_GPU_MEMORY_MAP_FLAG_WRITE_APPEND_BIT,
		E_GPU_BUFFER_DATA_COPY_FLAG_MODE_INVALIDATE_BIT = E_GPU_MEMORY_MAP_FLAG_WRITE_INVALIDATE_BIT,
		E_GPU_BUFFER_DATA_COPY_FLAGS_DEFAULT = 0,
	};

	/// @brief Bind flags for buffer resources. Used to specify how buffers are bound to the pipeline.
	///
	/// Note:
	/// Certain bind targets may also imply something about how the resource will be used in general. For example,
	/// vertex and index buffers should naturally have E_GPU_RESOURCE_USAGE_FLAG_VERTEX_STREAM_BIT set as well as a
	/// part of their properties. To ensure that is the case (and to reduce the amount of flags specified at creation
	/// time), these bind flags also have all required USAGE bits set as well.
	enum EGPUBufferBindFlags : resource_flags_value_t
	{
		// Bind flag for using a buffer as a constant buffer in one or more shader stages.
		// Such buffer is accessible via one of the valid CB input registers.
		// Implies SHADER_INPUT usage bit.
		E_GPU_BUFFER_BIND_FLAG_CONSTANT_BUFFER_BIT          = 0x010000 | E_GPU_RESOURCE_USAGE_FLAG_SHADER_INPUT_BIT,

		// Bind flag for using a buffer as a source for vertex data fetched during execution of all rendering commands.
		// Such buffer is bound to the pipeline via TODO.
		// Implies VERTEX_STREAM usage bit.
		E_GPU_BUFFER_BIND_FLAG_VERTEX_BUFFER_BIT            = 0x020000 | E_GPU_RESOURCE_USAGE_FLAG_VERTEX_STREAM_BIT,

		// Bind flag for using a buffer as a source for index data fetched during execution of indexed rendering commands.
		// Such buffer is bound to the pipeline via TODO.
		// Implies VERTEX_STREAM usage bit.
		E_GPU_BUFFER_BIND_FLAG_INDEX_BUFFER_BIT             = 0x040000 | E_GPU_RESOURCE_USAGE_FLAG_VERTEX_STREAM_BIT,

		// Bind flag for using a buffer as a target storage for stream output functionality.
		E_GPU_BUFFER_BIND_FLAG_STREAM_OUTPUT_BUFFER_BIT     = 0x080000,

		// Bind flag for using a buffer as a general read-only resource.
		E_GPU_BUFFER_BIND_FLAG_SHADER_INPUT_BUFFER_BIT      = 0x100000 | E_GPU_RESOURCE_USAGE_FLAG_SHADER_INPUT_BIT,
		// Implies SHADER_INPUT usage bit.

		// Bind flag for using a buffer as a general read/write resource supporting unordered access.
		E_GPU_BUFFER_BIND_FLAG_SHADER_UAV_BUFFER_BIT        = 0x200000 | E_GPU_RESOURCE_USAGE_FLAG_SHADER_UAV_BIT,
		// Implies SHADER_UAV usage bit.

		// Bind flag for using a buffer as a source for arguments to indirect dispatch commands.
		E_GPU_BUFFER_BIND_FLAG_INDIRECT_DISPATCH_BUFFER_BIT = 0x400000,

		// Bind flag for using a buffer as a source for arguments to indirect draw commands.
		E_GPU_BUFFER_BIND_FLAG_INDIRECT_DRAW_BUFFER_BIT     = 0x800000,

		// Bind flag for using a buffer as a source in transfer operations. @See E_GPU_RESOURCE_USAGE_FLAG_TRANSFER_SOURCE_BIT.
		E_GPU_BUFFER_BIND_FLAG_TRANSFER_SOURCE_BUFFER_BIT   = E_GPU_RESOURCE_USAGE_FLAG_TRANSFER_SOURCE_BIT,

		// Bind flag for using a buffer as a target in transfer operations. @See E_GPU_RESOURCE_USAGE_FLAG_TRANSFER_TARGET_BIT.
		E_GPU_BUFFER_BIND_FLAG_TRANSFER_TARGET_BUFFER_BIT   = E_GPU_RESOURCE_USAGE_FLAG_TRANSFER_TARGET_BIT,

		// Mask with all valid BIND_FLAG bits set.
		E_GPU_BUFFER_BIND_MASK_ALL = 0xFF0000 | E_GPU_RESOURCE_USAGE_MASK_ALL,
	};

	/// @brief Represents all valid targets for buffer resources. Corresponding E_GPU_BUFFER_BIND_FLAGs are used as values.
	enum class EGPUBufferTarget : uint32
	{
		Unknown                = 0,
		ConstantBuffer         = E_GPU_BUFFER_BIND_FLAG_CONSTANT_BUFFER_BIT,
		VertexBuffer           = E_GPU_BUFFER_BIND_FLAG_VERTEX_BUFFER_BIT,
		IndexBuffer            = E_GPU_BUFFER_BIND_FLAG_INDEX_BUFFER_BIT,
		StreamOutputBuffer     = E_GPU_BUFFER_BIND_FLAG_STREAM_OUTPUT_BUFFER_BIT,
		ShaderInputBuffer      = E_GPU_BUFFER_BIND_FLAG_SHADER_INPUT_BUFFER_BIT,
		ShaderUAVBuffer        = E_GPU_BUFFER_BIND_FLAG_SHADER_UAV_BUFFER_BIT,
		IndirectDispatchBuffer = E_GPU_BUFFER_BIND_FLAG_INDIRECT_DISPATCH_BUFFER_BIT,
		IndirectDrawBuffer     = E_GPU_BUFFER_BIND_FLAG_INDIRECT_DRAW_BUFFER_BIT,
		TransferSourceBuffer   = E_GPU_BUFFER_BIND_FLAG_TRANSFER_SOURCE_BUFFER_BIT,
		TransferTargetBuffer   = E_GPU_BUFFER_BIND_FLAG_TRANSFER_TARGET_BUFFER_BIT,
	};

	struct GPUBufferInitDataDesc : public ResourceInputDataDesc
	{
	};

	struct GPUBufferInputDataDesc : public ResourceInputDataDesc
	{
	};

	struct GPUBufferCreateInfo : public ResourceCreateInfo
	{
		// Total size of the buffer, in bytes. The buffer, if created, will have at least the specified size.
		// Additional alignment rules and memory requirements may cause the buffer to be larger than requested.
		gpu_memory_size_t bufferSize = 0;

		// Initial target for the buffer. Some drivers, like GL, require binding to a target in order to actually
		// initialize the buffer. If the buffer has multiple bind targets specified (which is perfectly fine), this
		// variable controls which one the buffer will be bound to in order to initialize it. Following rules apply:
		// 1) If the target is NOT EGPUBufferTarget::Unknown, its corresponding E_GPU_BUFFER_BIND_FLAG must have been set
		//    in ResourceCreateInfo::resourceFlags. Otherwise, the creation of a buffer will fail.
		// 2) If the target is EGPUBufferTarget::Unknown, the driver will pick the first compatible target, in the order
		//    of definition within EGPUBufferTarget enum.
		EGPUBufferTarget initialTarget = EGPUBufferTarget::Unknown;

		// Initial data for the buffer. If the buffer is created with E_GPU_RESOURCE_CONTENT_FLAG_IMMUTABLE_BIT,
		// this has to contain a valid data specification.
		GPUBufferInitDataDesc initDataDesc;
	};

	struct GPUBufferDataCopyDesc
	{
		Bitmask<EGPUBufferDataCopyFlags> flags = E_GPU_BUFFER_DATA_COPY_FLAGS_DEFAULT;
	};

	struct GPUBufferSubDataCopyDesc
	{
		GPUMemoryRegion sourceBufferRegion;
		gpu_memory_size_t targetBufferOffset;
		Bitmask<EGPUBufferDataCopyFlags> flags = E_GPU_BUFFER_DATA_COPY_FLAGS_DEFAULT;
	};

	struct GPUBufferDataUploadDesc
	{
		ResourceInputDataDesc inputDataDesc;
		Bitmask<EGPUBufferDataCopyFlags> flags = E_GPU_BUFFER_DATA_COPY_FLAGS_DEFAULT;
	};

	struct GPUBufferSubDataUploadDesc
	{
		GPUMemoryRegion bufferRegion;
		ResourceInputDataDesc inputDataDesc;
		Bitmask<EGPUBufferDataCopyFlags> flags = E_GPU_BUFFER_DATA_COPY_FLAGS_DEFAULT;
	};

	namespace rcutil
	{

		TS3_GPUAPI_API_NO_DISCARD gpu_memory_size_t queryGPUBufferByteSize( GPUBufferHandle pGPUBuffer );

		TS3_GPUAPI_API_NO_DISCARD bool checkGPUBufferRegion(
				GPUBufferHandle pGPUBuffer,
				gpu_memory_size_t pOffset,
				gpu_memory_size_t pSize );

		TS3_GPUAPI_API_NO_DISCARD bool checkGPUBufferRegion(
				GPUBufferHandle pGPUBuffer,
				const GPUMemoryRegion & pRegion );

		TS3_GPUAPI_API_NO_DISCARD GPUMemoryRegion validateGPUBufferRegion(
				GPUBufferHandle pGPUBuffer,
				gpu_memory_size_t pOffset,
				gpu_memory_size_t pSize );

		TS3_GPUAPI_API_NO_DISCARD GPUMemoryRegion validateGPUBufferRegion(
				GPUBufferHandle pGPUBuffer,
				const GPUMemoryRegion & pRegion );

	}

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_GPU_BUFFER_COMMON_H__
