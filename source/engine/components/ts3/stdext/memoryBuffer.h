
#ifndef __TS3_STDEXT_MEMORY_BUFFER_H__
#define __TS3_STDEXT_MEMORY_BUFFER_H__

#include "memory.h"

namespace ts3
{

	class MemoryBuffer
	{
	public:
		MemoryBuffer( const MemoryBuffer & ) = delete;
		MemoryBuffer & operator=( const MemoryBuffer & ) = delete;

		MemoryBuffer() noexcept
		: _bufferBasePtr( nullptr )
		, _bufferEndPtr( nullptr )
		, _bufferLength( 0u )
		{}

		MemoryBuffer( MemoryBuffer && pSource ) noexcept
		: _bufferBasePtr( pSource._bufferBasePtr )
		, _bufferEndPtr( pSource._bufferEndPtr )
		, _bufferLength( pSource._bufferLength )
		{
			pSource._bufferBasePtr = nullptr;
			pSource._bufferEndPtr = nullptr;
			pSource._bufferLength = 0u;
		}

		MemoryBuffer( void * pMemory, size_t pMemorySize ) noexcept
		: _bufferBasePtr( reinterpret_cast<byte *>( pMemory ) )
		, _bufferEndPtr( _bufferBasePtr + pMemorySize )
		, _bufferLength( pMemorySize )
		{}

		byte & operator[]( size_t pOffset ) noexcept
		{
			ts3DebugAssert( pOffset < _bufferLength );
			return _bufferBasePtr[pOffset];
		}

		const byte & operator[]( size_t pOffset ) const noexcept
		{
			ts3DebugAssert( pOffset < _bufferLength );
			return _bufferBasePtr[pOffset];
		}

		byte * dataPtr() noexcept
		{
			ts3DebugAssert( _bufferLength > 0 );
			return _bufferBasePtr;
		}

		const byte * dataPtr() const noexcept
		{
			ts3DebugAssert( _bufferLength > 0 );
			return _bufferBasePtr;
		}

		byte * dataPtr( size_t pOffset ) noexcept
		{
			ts3DebugAssert( ( _bufferLength > 0 ) && ( pOffset < _bufferLength ) );
			return _bufferBasePtr + pOffset;
		}

		const byte * dataPtr( size_t pOffset ) const noexcept
		{
			ts3DebugAssert( ( _bufferLength > 0 ) && ( pOffset < _bufferLength ) );
			return _bufferBasePtr + pOffset;
		}

		template <typename TpResult>
		TpResult * dataPtrAs() noexcept
		{
			auto * bufferBytePtr = dataPtr();
			return reinterpret_cast<TpResult *>( bufferBytePtr );
		}

		template <typename TpResult>
		const TpResult * dataPtrAs() const noexcept
		{
			auto * bufferBytePtr = dataPtr();
			return reinterpret_cast<TpResult *>( bufferBytePtr );
		}

		template <typename TpResult>
		TpResult * dataPtrAs( size_t pOffset ) noexcept
		{
			auto * bufferBytePtr = dataPtr();
			return reinterpret_cast<TpResult *>( bufferBytePtr );
		}

		template <typename TpResult>
		const TpResult * dataPtrAs( size_t pOffset ) const noexcept
		{
			auto * bufferBytePtr = dataPtr();
			return reinterpret_cast<TpResult *>( bufferBytePtr );
		}

		void fillBytes( byte pValue, size_t pFillCount = cxMaxSize, size_t pFillOffset = 0 )
		{
			if( pFillOffset >= _bufferLength )
			{
				return;
			}

			auto bufferCapacity = _bufferLength - pFillOffset;
			pFillCount = getMinOf( pFillCount, bufferCapacity );
			memFillChecked( _bufferBasePtr + pFillOffset, bufferCapacity, pValue, pFillCount );
		}

		void setData( const void * pData, size_t pDataSize, size_t pSetOffset = 0 )
		{
			if( ( pData == nullptr ) || ( pDataSize == 0 ) || ( pSetOffset >= _bufferLength ) )
			{
				return;
			}

			auto bufferCapacity = _bufferLength - pSetOffset;
			pSetOffset = getMinOf( pSetOffset, bufferCapacity );
			memCopyUnchecked( _bufferBasePtr + pSetOffset, bufferCapacity, pData, pDataSize );
		}

		void copy( void * pBuffer, size_t pBufferSize, size_t pCopySize, size_t pCopyOffset = 0 )
		{
			if( ( pBuffer == nullptr ) || ( pBufferSize == 0 ) || ( pCopyOffset >= _bufferLength ) )
			{
				return;
			}

			pCopySize = getMinOf( pCopySize, pBufferSize );
			pCopySize = getMinOf( pCopySize, _bufferLength - pCopyOffset );
			memCopyUnchecked( pBuffer, pBufferSize, _bufferBasePtr + pCopyOffset, pCopySize );
		}

		TS3_PCL_ATTR_NO_DISCARD bool empty() const noexcept
		{
			return ( _bufferBasePtr == nullptr ) && ( _bufferLength == 0 );
		}

		TS3_PCL_ATTR_NO_DISCARD size_t size() const noexcept
		{
			return _bufferLength;
		}

	protected:
		byte * _bufferBasePtr;
		byte * _bufferEndPtr;
		size_t _bufferLength;
	};

	class DynamicMemoryBuffer : public MemoryBuffer
	{
	public:
		DynamicMemoryBuffer( const DynamicMemoryBuffer & ) = delete;
		DynamicMemoryBuffer & operator=( const DynamicMemoryBuffer & ) = delete;

		DynamicMemoryBuffer()
		: MemoryBuffer()
		, _allocationProxy( cvDefaultMemoryAllocationProxy )
		{}

		DynamicMemoryBuffer( std::nullptr_t )
		: MemoryBuffer()
		, _allocationProxy( cvDefaultMemoryAllocationProxy )
		{}

		explicit DynamicMemoryBuffer( MemoryAllocationProxy pAllocationProxy )
		: _allocationProxy( pAllocationProxy ? std::move( pAllocationProxy ) : MemoryAllocationProxy( cvDefaultMemoryAllocationProxy ) )
		{}

		DynamicMemoryBuffer( DynamicMemoryBuffer && pSource ) noexcept
		: MemoryBuffer( std::move( pSource ) )
		, _allocationProxy( std::move( pSource._allocationProxy ) )
		{}

		DynamicMemoryBuffer & operator=( DynamicMemoryBuffer && pRhs ) noexcept
		{
			DynamicMemoryBuffer( std::move( pRhs ) ).swap( *this );
			return *this;
		}

		explicit operator bool() const
		{
			return _bufferBasePtr && ( _bufferLength > 0 );
		}

		size_t resize( size_t pNewSize )
		{
			pNewSize = getAlignedValue( pNewSize, _allocationProxy.memoryAlignment );
			if( pNewSize == _bufferLength )
			{
				return _bufferLength;
			}
			else if( pNewSize == 0 )
			{
				release();
				return 0;
			}
			else
			{
				auto * newMemoryPtr = _allocationProxy.apiRealloc( _bufferBasePtr, pNewSize );
				_bufferBasePtr = reinterpret_cast<byte *>( newMemoryPtr );
				_bufferEndPtr = _bufferBasePtr + pNewSize;
				_bufferLength = pNewSize;
				return _bufferLength;
			}
		}

		size_t expand( size_t pMinSize = 0, float pFactor = 1.6f )
		{
			if( ( pMinSize > 0 ) && ( _bufferLength >= pMinSize ) )
			{
				return _bufferLength;
			}

			auto newSize = getMaxOf( pMinSize, static_cast<size_t>( _bufferLength * pFactor ) );
			newSize = getAlignedValue( newSize, _allocationProxy.memoryAlignment );
			resize( newSize );

			return newSize;
		}

		void release()
		{
			if( _bufferBasePtr != nullptr )
			{
				_allocationProxy.apiFree( _bufferBasePtr );
				_bufferBasePtr = nullptr;
				_bufferEndPtr = nullptr;
				_bufferLength = 0;
			}
		}

		void swap( DynamicMemoryBuffer & pOther )
		{
			std::swap( _bufferBasePtr, pOther._bufferBasePtr );
			std::swap( _bufferEndPtr, pOther._bufferEndPtr );
			std::swap( _bufferLength, pOther._bufferLength );
			ts3::swap( _allocationProxy, pOther._allocationProxy );
		}

	private:
		MemoryAllocationProxy _allocationProxy;
	};

	template <size_t pSize>
	class FixedMemoryBuffer : public MemoryBuffer
	{
	public:
		FixedMemoryBuffer( const FixedMemoryBuffer & ) = delete;
		FixedMemoryBuffer & operator=( const FixedMemoryBuffer & ) = delete;

		FixedMemoryBuffer()
		: MemoryBuffer( &( _storage[0] ), pSize )
		{}

	private:
		byte _storage[pSize];
	};

}

#endif /* __TS3_STDEXT_MEMORY_BUFFER_H__ */
