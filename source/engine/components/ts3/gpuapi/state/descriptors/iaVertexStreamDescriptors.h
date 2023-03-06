
#pragma once

#ifndef __TS3_GPUAPI_VERTEX_STREAM_DESCRIPTORS_H__
#define __TS3_GPUAPI_VERTEX_STREAM_DESCRIPTORS_H__

#include "ts3/gpuapi/resources/gpuBufferReference.h"
#include "ts3/gpuapi/state/inputAssemblerCommon.h"

namespace ts3::gpuapi
{

	class TS3_GPUAPI_API IAVertexStreamBaseDescriptor
	{
	public:
		IAVertexStreamBaseDescriptor() = default;
		IAVertexStreamBaseDescriptor( const GPUBufferReference & pBufferRef, gpu_memory_size_t pRelativeOffset = 0 );

		TS3_ATTR_NO_DISCARD gpu_memory_size_t dataOffset() const noexcept
		{
			return _sourceBuffer.getRefSubRegion().offset + _relativeOffset;
		}

		TS3_ATTR_NO_DISCARD gpu_memory_size_t relativeOffset() const noexcept
		{
			return _relativeOffset;
		}

		TS3_ATTR_NO_DISCARD GPUBuffer * sourceBuffer() const noexcept
		{
			return _sourceBuffer.getRefBuffer().get();
		}

		TS3_ATTR_NO_DISCARD bool empty() const noexcept
		{
			return _sourceBuffer.empty();
		}

		TS3_ATTR_NO_DISCARD bool valid() const noexcept
		{
			return _sourceBuffer.valid();
		}

		void reset()
		{
			_sourceBuffer.reset();
		}

	private:
		GPUBufferReference _sourceBuffer;
		gpu_memory_size_t _relativeOffset;
	};

	class TS3_GPUAPI_API IAVertexBufferDescriptor : public IAVertexStreamBaseDescriptor
	{
	public:
		IAVertexBufferDescriptor() = default;
		~IAVertexBufferDescriptor() = default;

		IAVertexBufferDescriptor( const IAVertexBufferDescriptor & ) = default;
		IAVertexBufferDescriptor & operator=( const IAVertexBufferDescriptor & ) = default;

		IAVertexBufferDescriptor( const GPUBufferReference & pVertexBufferRef, gpu_memory_size_t pVertexStride );

		TS3_ATTR_NO_DISCARD gpu_memory_size_t vertexStride() const noexcept
		{
			return _vertexStride;
		}

	private:
		gpu_memory_size_t _vertexStride;
	};

	class alignas(8) TS3_GPUAPI_API IAIndexBufferDescriptor : public IAVertexStreamBaseDescriptor
	{
	public:
		IAIndexBufferDescriptor() = default;
		~IAIndexBufferDescriptor() = default;

		IAIndexBufferDescriptor( const IAIndexBufferDescriptor & ) = default;
		IAIndexBufferDescriptor & operator=( const IAIndexBufferDescriptor & ) = default;

		IAIndexBufferDescriptor( const GPUBufferReference & pBufferRef, EIndexDataFormat pIndexFormat );

		TS3_ATTR_NO_DISCARD EIndexDataFormat indexFormat() const noexcept
		{
			return _indexFormat;
		}

	private:
		EIndexDataFormat _indexFormat;
	};

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_VERTEX_STREAM_DESCRIPTORS_H__
