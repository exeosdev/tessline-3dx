
#ifndef __TS3_STDEXT_MEMORY_BUFFER_H__
#define __TS3_STDEXT_MEMORY_BUFFER_H__

#include "arrayView.h"
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
		, _bufferLength( 0u )
		{}

		MemoryBuffer( MemoryBuffer && pSource ) noexcept
		: _bufferBasePtr( pSource._bufferBasePtr )
		, _bufferLength( pSource._bufferLength )
		{
			pSource._bufferBasePtr = nullptr;
			pSource._bufferLength = 0u;
		}

		MemoryBuffer( void * pMemory, size_t pMemorySize ) noexcept
		: _bufferBasePtr( reinterpret_cast<byte *>( pMemory ) )
		, _bufferLength( pMemorySize )
		{}

		TS3_PCL_ATTR_NO_DISCARD byte & operator[]( size_t pOffset ) noexcept
		{
			ts3DebugAssert( pOffset < _bufferLength );
			return _bufferBasePtr[pOffset];
		}

		TS3_PCL_ATTR_NO_DISCARD const byte & operator[]( size_t pOffset ) const noexcept
		{
			ts3DebugAssert( pOffset < _bufferLength );
			return _bufferBasePtr[pOffset];
		}

		TS3_PCL_ATTR_NO_DISCARD byte * data() noexcept
		{
			ts3DebugAssert( _bufferLength > 0 );
			return _bufferBasePtr;
		}

		TS3_PCL_ATTR_NO_DISCARD const byte * data() const noexcept
		{
			ts3DebugAssert( _bufferLength > 0 );
			return _bufferBasePtr;
		}

		TS3_PCL_ATTR_NO_DISCARD byte * data( size_t pOffset ) noexcept
		{
			ts3DebugAssert( ( _bufferLength > 0 ) && ( pOffset < _bufferLength ) );
			return _bufferBasePtr + pOffset;
		}

		TS3_PCL_ATTR_NO_DISCARD const byte * data( size_t pOffset ) const noexcept
		{
			ts3DebugAssert( ( _bufferLength > 0 ) && ( pOffset < _bufferLength ) );
			return _bufferBasePtr + pOffset;
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
		size_t _bufferLength;
	};

	inline ReadWriteMemoryView bindMemoryView( MemoryBuffer & pMemoryBuffer )
	{
	    return ReadWriteMemoryView( pMemoryBuffer.data(), pMemoryBuffer.size() );
	}

	inline ReadOnlyMemoryView bindMemoryView( const MemoryBuffer & pMemoryBuffer )
	{
	    return ReadOnlyMemoryView( pMemoryBuffer.data(), pMemoryBuffer.size() );
	}

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
			pNewSize = memGetAlignedValue( pNewSize, _allocationProxy.memoryAlignment );
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
				_bufferLength = pNewSize;
				return _bufferLength;
			}
		}

		void release()
		{
			if( _bufferBasePtr != nullptr )
			{
				_allocationProxy.apiFree( _bufferBasePtr );
				_bufferBasePtr = nullptr;
				_bufferLength = 0;
			}
		}

		void swap( DynamicMemoryBuffer & pOther )
		{
			std::swap( _bufferBasePtr, pOther._bufferBasePtr );
			std::swap( _bufferLength, pOther._bufferLength );
			ts3::swap( _allocationProxy, pOther._allocationProxy );
		}

	private:
		MemoryAllocationProxy _allocationProxy;
	};

	inline void swap( DynamicMemoryBuffer & pFirst, DynamicMemoryBuffer & pSecond )
	{
	    pFirst.swap( pSecond );
	}

	template <size_t tpSize>
	class FixedMemoryBuffer : public MemoryBuffer
	{
	public:
		FixedMemoryBuffer( const FixedMemoryBuffer & ) = delete;
		FixedMemoryBuffer & operator=( const FixedMemoryBuffer & ) = delete;

		FixedMemoryBuffer()
		: MemoryBuffer( &( _storage[0] ), tpSize )
		{}

	private:
		byte _storage[tpSize];
	};

}

#endif /* __TS3_STDEXT_MEMORY_BUFFER_H__ */
