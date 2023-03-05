
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
		IAVertexStreamBaseDescriptor( const GPUBufferReference & pBufferRef );

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
	};

	class TS3_GPUAPI_API IAVertexBufferDescriptor : public IAVertexStreamBaseDescriptor
	{
	public:
		IAVertexBufferDescriptor() = default;
		~IAVertexBufferDescriptor() = default;

		IAVertexBufferDescriptor( const IAVertexBufferDescriptor & ) = default;
		IAVertexBufferDescriptor & operator=( const IAVertexBufferDescriptor & ) = default;

		IAVertexBufferDescriptor( const GPUBufferReference & pVertexBufferRef, gpu_memory_size_t pVertexStride );

	private:
		gpu_memory_size_t _vertexStride;
	};

	class TS3_GPUAPI_API IAIndexBufferDescriptor : public IAVertexStreamBaseDescriptor
	{
	public:
		IAIndexBufferDescriptor() = default;
		~IAIndexBufferDescriptor() = default;

		IAIndexBufferDescriptor( const IAIndexBufferDescriptor & ) = default;
		IAIndexBufferDescriptor & operator=( const IAIndexBufferDescriptor & ) = default;

		IAIndexBufferDescriptor( const GPUBufferReference & pBufferRef, EIndexDataFormat pIndexFormat );

	private:
		EIndexDataFormat _indexFormat;
	};

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_VERTEX_STREAM_DESCRIPTORS_H__
