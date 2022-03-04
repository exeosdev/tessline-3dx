
#ifndef __TS3_STDEXT_BYTE_BUFFER_H__
#define __TS3_STDEXT_BYTE_BUFFER_H__

#include "memoryBuffer.h"

namespace ts3
{

	class ByteBuffer
	{
	public:
		ByteBuffer( const ByteBuffer & ) = delete;
		ByteBuffer & operator=( const ByteBuffer & ) = delete;

		ByteBuffer()
		: _usedSubRegionLength( 0u )
		{}

		explicit ByteBuffer( size_t pSize )
		: _usedSubRegionLength( 0u )
		{
			_usedSubRegionLength = _storageBuffer.resize( pSize );
		}

		ByteBuffer( ByteBuffer && pSource ) noexcept
		: _storageBuffer( std::move( pSource._storageBuffer ) )
		, _usedSubRegionLength( pSource._usedSubRegionLength )
		{}

		ByteBuffer & operator=( ByteBuffer && pRhs ) noexcept
		{
			ByteBuffer( std::move( pRhs ) ).swap( *this );
			return *this;
		}

		explicit operator bool() const
		{
			return _storageBuffer && ( _usedSubRegionLength > 0 );
		}

		byte & operator[]( size_t pOffset ) noexcept
		{
			ts3DebugAssert( pOffset < _usedSubRegionLength );
			return _storageBuffer[pOffset];
		}

		const byte & operator[]( size_t pOffset ) const noexcept
		{
			ts3DebugAssert( pOffset < _usedSubRegionLength );
			return _storageBuffer[pOffset];
		}

		byte * dataPtr() noexcept
		{
			return _storageBuffer.dataPtr();
		}

		const byte * dataPtr() const noexcept
		{
			return _storageBuffer.dataPtr();
		}

		byte * dataPtr( size_t pOffset ) noexcept
		{
			ts3DebugAssert( pOffset < _usedSubRegionLength );
			return _storageBuffer.dataPtr( pOffset );
		}

		const byte * dataPtr( size_t pOffset ) const noexcept
		{
			ts3DebugAssert( pOffset < _usedSubRegionLength );
			return _storageBuffer.dataPtr( pOffset );
		}

		void swap( ByteBuffer & pOther )
		{
			swap( _storageBuffer, pOther._storageBuffer );
			std::swap( _usedSubRegionLength, pOther._usedSubRegionLength );
		}

		void resize( size_t pNewSize )
		{
			if( pNewSize > _storageBuffer.size() )
			{
				_storageBuffer.resize( pNewSize );
			}
			_usedSubRegionLength = pNewSize;
		}

		TS3_PCL_ATTR_NO_DISCARD bool empty() const noexcept
		{
			return !_storageBuffer || ( _usedSubRegionLength == 0 );
		}

		TS3_PCL_ATTR_NO_DISCARD size_t size() const noexcept
		{
			return _usedSubRegionLength;
		}

		void release()
		{
			_storageBuffer.release();
			_usedSubRegionLength = 0;
		}

	private:
		DynamicMemoryBuffer _storageBuffer;
		size_t _usedSubRegionLength;
	};

}

#endif /* __TS3_STDEXT_BYTE_BUFFER_H__ */
