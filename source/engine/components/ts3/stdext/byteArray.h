
#ifndef __TS3_STDEXT_BYTE_ARRAY_H__
#define __TS3_STDEXT_BYTE_ARRAY_H__

#include "memoryBuffer.h"

namespace ts3
{

	class ByteArray
	{
	public:
		ByteArray( const ByteArray & ) = delete;
		ByteArray & operator=( const ByteArray & ) = delete;

		ByteArray() noexcept
		: _storageMemoryPtr( nullptr )
		, _dataSize( 0u )
		{}

		ByteArray( ByteArray && pSource ) noexcept
		: _storageMemoryPtr( pSource._storageMemoryPtr )
		, _dataSize( pSource._dataSize )
		{
		    pSource._storageMemoryPtr = nullptr;
		    pSource._dataSize = 0u;
		}

		ByteArray( void * pBufferBasePtr, size_t pBufferLength ) noexcept
		: _storageMemoryPtr( reinterpret_cast<byte *>( pBufferBasePtr ) )
		, _dataSize( pBufferLength )
		{}

		explicit operator bool() const
		{
		    return _storageMemoryPtr && ( _dataSize > 0 );
		}

		void fill( byte pValue, size_t pFillCount = CX_MAX_SIZE, size_t pFillOffset = 0 )
		{
		    if( pFillOffset >= _dataSize )
		    {
		        return;
		    }

		    auto bufferCapacity = _dataSize - pFillOffset;
		    pFillCount = getMinOf( pFillCount, bufferCapacity );
		    memFillChecked( _storageMemoryPtr + pFillOffset, bufferCapacity, pValue, pFillCount );
		}

		void setData( const void * pData, size_t pDataSize, size_t pSetOffset = 0 )
		{
		    if( !pData || ( pDataSize == 0 ) || ( pSetOffset >= _dataSize ) )
		    {
		        return;
		    }

		    auto bufferCapacity = _dataSize - pSetOffset;
		    pSetOffset = getMinOf( pSetOffset, bufferCapacity );
		    memCopyUnchecked( _storageMemoryPtr + pSetOffset, bufferCapacity, pData, pDataSize );
		}

		void copy( void * pBuffer, size_t pBufferSize, size_t pCopySize, size_t pCopyOffset = 0 )
		{
		    if( !pBuffer || ( pBufferSize == 0 ) || ( pCopyOffset >= _dataSize ) )
		    {
		        return;
		    }

		    pCopySize = getMinOf( pCopySize, pBufferSize );
		    pCopySize = getMinOf( pCopySize, _dataSize - pCopyOffset );

		    if( pCopySize == 0 )
		    {
		        return;
		    }

		    memCopyUnchecked( pBuffer, pBufferSize, _storageMemoryPtr + pCopyOffset, pCopySize );
		}

		TS3_PCL_ATTR_NO_DISCARD byte & operator[]( size_t pOffset ) noexcept
		{
		    ts3DebugAssert( pOffset < _dataSize );
		    return _storageMemoryPtr[pOffset];
		}

		TS3_PCL_ATTR_NO_DISCARD const byte & operator[]( size_t pOffset ) const noexcept
		{
		    ts3DebugAssert( pOffset < _dataSize );
		    return _storageMemoryPtr[pOffset];
		}

		TS3_PCL_ATTR_NO_DISCARD byte * data() noexcept
		{
		    return _storageMemoryPtr;
		}

		TS3_PCL_ATTR_NO_DISCARD const byte * data() const noexcept
		{
		    return _storageMemoryPtr;
		}

		TS3_PCL_ATTR_NO_DISCARD byte * dataPtrOffset( size_t pOffset ) noexcept
		{
		    ts3DebugAssert( pOffset < _dataSize );
		    return _storageMemoryPtr + pOffset;
		}

		TS3_PCL_ATTR_NO_DISCARD const byte * dataPtrOffset( size_t pOffset ) const noexcept
		{
		    ts3DebugAssert( pOffset < _dataSize );
		    return _storageMemoryPtr + pOffset;
		}

		template <typename TpResult>
		TS3_PCL_ATTR_NO_DISCARD TpResult * dataPtrAs() noexcept
		{
		    ts3DebugAssert( _dataSize % sizeof( TpResult ) == 0 );
		    return reinterpret_cast<TpResult *>( data() );
		}

		template <typename TpResult>
		TS3_PCL_ATTR_NO_DISCARD const TpResult * dataPtrAs() const noexcept
		{
		    ts3DebugAssert( _dataSize % sizeof( TpResult ) == 0 );
		    return reinterpret_cast<TpResult *>( data() );
		}

		template <typename TpResult>
		TS3_PCL_ATTR_NO_DISCARD TpResult * dataPtrOffsetAs( size_t pOffset ) noexcept
		{
		    ts3DebugAssert( ( _dataSize - pOffset ) % sizeof( TpResult ) == 0 );
		    return reinterpret_cast<TpResult *>( dataPtrOffset( pOffset ) );
		}

		template <typename TpResult>
		TS3_PCL_ATTR_NO_DISCARD const TpResult * dataPtrOffsetAs( size_t pOffset ) const noexcept
		{
		    ts3DebugAssert( ( _dataSize - pOffset ) % sizeof( TpResult ) == 0 );
		    return reinterpret_cast<TpResult *>( dataPtrOffset( pOffset ) );
		}

		TS3_PCL_ATTR_NO_DISCARD bool empty() const noexcept
		{
		    return !_storageMemoryPtr || ( _dataSize == 0 );
		}

		TS3_PCL_ATTR_NO_DISCARD size_t size() const noexcept
		{
		    return _dataSize;
		}

	protected:
	    void updateStorage( void * pBufferBasePtr )
	    {
		    _storageMemoryPtr = reinterpret_cast<byte *>( pBufferBasePtr );
	    }

	protected:
	    byte * _storageMemoryPtr;
		size_t _dataSize;
	};

	inline ReadWriteMemoryView bindMemoryView( ByteArray & pByteArray )
    {
	    return ReadWriteMemoryView( pByteArray.data(), pByteArray.size() );
    }

    inline ReadOnlyMemoryView bindMemoryView( const ByteArray & pByteArray )
    {
	    return ReadOnlyMemoryView( pByteArray.data(), pByteArray.size() );
    }

	class DynamicByteArray : public ByteArray
    {
    public:
        DynamicByteArray() = default;

        DynamicByteArray( DynamicByteArray && pSource ) noexcept
        : ByteArray( std::move( pSource ) )
        , _internalBuffer( std::move( pSource._internalBuffer ) )
        {}

        DynamicByteArray( DynamicMemoryBuffer pInternalBuffer, size_t pSize = CX_MAX_SIZE )
        : _internalBuffer( std::move( pInternalBuffer ) )
        {
            _storageMemoryPtr = _internalBuffer.data();
            _dataSize = getMinOf( _internalBuffer.size(), pSize );
        }

        explicit DynamicByteArray( size_t pSize )
        {
            resize( pSize );
        }

        DynamicByteArray & operator=( DynamicByteArray && pRhs ) noexcept
        {
            DynamicByteArray( std::move( pRhs ) ).swap( *this );
            return *this;
        }

        DynamicByteArray clone() const
        {
            DynamicByteArray newArray;
            newArray.assign( *this );
            return newArray;
        }

        DynamicByteArray & assign( const DynamicByteArray & pOther )
	    {
	        resize( pOther.size() );
	        setData( pOther.data(), pOther.size(), 0 );
	        return *this;
	    }

	    void resize( size_t pNewSize )
	    {
	        if( pNewSize > _internalBuffer.size() )
	        {
	            _internalBuffer.resize( pNewSize );
	            updateStorage( _internalBuffer.data() );
	        }
	        _dataSize = pNewSize;
	    }

	    bool optimizeStorage( float pAllowedFactor = 0.5f )
	    {
	        const auto storageCapacity = _internalBuffer.size();
	        const auto overSpace = static_cast<float>( storageCapacity - _dataSize ) / _dataSize;

	        if( overSpace > pAllowedFactor )
	        {
	            _internalBuffer.resize( _dataSize );
	            updateStorage( _internalBuffer.data() );
	            return true;
	        }

	        return false;
	    }

	    void release()
	    {
	        _internalBuffer.release();
	        _dataSize = 0;
	    }

	    void swap( DynamicByteArray & pOther )
	    {
            ts3::swap( _internalBuffer, pOther._internalBuffer );
            std::swap( _storageMemoryPtr, pOther._storageMemoryPtr );
	        std::swap( _dataSize, pOther._dataSize );
	    }

    private:
        DynamicMemoryBuffer _internalBuffer;
    };

	inline void swap( DynamicByteArray & pFirst, DynamicByteArray & pSecond )
	{
	    pFirst.swap( pSecond );
	}

	template <size_t tpSize>
	class FixedByteArray : public ByteArray
    {
    public:
        FixedByteArray( const FixedByteArray & ) = delete;
        FixedByteArray & operator=( const FixedByteArray & ) = delete;

        FixedByteArray()
        {
            _storageMemoryPtr = _internalBuffer.data();
            _dataSize = _internalBuffer.size();
        }

    private:
        FixedMemoryBuffer<tpSize> _internalBuffer;
    };

}

#endif /* __TS3_STDEXT_BYTE_ARRAY_H__ */
