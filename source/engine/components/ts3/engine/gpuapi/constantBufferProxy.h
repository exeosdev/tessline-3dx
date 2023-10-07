
#pragma once

#ifndef __TS3_ENGINE_CONSTANT_BUFFER_PROXY_H__
#define __TS3_ENGINE_CONSTANT_BUFFER_PROXY_H__

#include "../prerequisites.h"
#include <ts3/gpuapi/resources/gpuBufferCommon.h>

namespace ts3
{

	struct ConstantBufferDataRef
	{
		uint32 bufferSize = 0;
		void * dataPtr = nullptr;
		uint32 dataSize = 0;
		uint32 modified = 0;
	};

	class ConstantBufferProxy
	{
	public:
		gpuapi::GPUDevice & mGPUDevice;

	public:
		ConstantBufferProxy( gpuapi::GPUBufferHandle pBuffer );
		~ConstantBufferProxy();

		gpuapi::GPUBufferHandle buffer() const noexcept;

		template <typename TData>
		void setData( const TData & pData )
		{
			ts3DebugAssert( sizeof( TData ) == _dataRef.dataSize );
			memCopyUnchecked( _dataRef.dataPtr, _dataRef.dataSize, &pData, sizeof( TData ) );
		}

		template <typename TData, typename TMember >
		void setSubData( TMember TData::* pMember, const TMember & pData )
		{
			const auto offset = memberOffset( pMember );
			const auto size = sizeof( TMember );
			ts3DebugAssert( ( offset < _dataRef.dataSize ) && ( size <= _dataRef.dataSize - offset ) );
			memCopyUnchecked( _dataRef.dataPtr + offset, size, &pData, size );
		}

		template <typename TData>
		void initialize();

		void flushUpdates();

	private:
		gpuapi::GPUBufferHandle _buffer;
		ConstantBufferDataRef _dataRef;
		DynamicMemoryBuffer _localCache;
	};

} // namespace ts3

#endif // __TS3_ENGINE_CONSTANT_BUFFER_PROXY_H__
