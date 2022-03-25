
#if !defined( __TS3_CORE_GDS_INLINE_DEF_INCLUDE_SECTION__ )
#  error "GDS implementation .inl files cannot be included directly!"
#endif

namespace ts3
{

	template <typename TpBuffer, typename TpValue>
	inline gds_size_t GdsCore::serializeExternal( const TpValue & pValue, const DataWriteCallback & pWriteCallback, TpBuffer & pGdsCache )
	{
		// Serialize the object into the cache. We can use our neat serializeAuto()
		// to properly handle different type of buffers here!
		const auto writeSize = serializeAutoWithMetaData( pGdsCache, pValue );

		if( writeSize > 0 )
		{
			if( !pWriteCallback )
			{
				// This cannot be an empty function.
				ts3ThrowDesc( E_EXC_DEBUG_PLACEHOLDER, "Empty write callback." );
			}

			// Write the data using the callback.
			pWriteCallback( pGdsCache.data(), writeSize );
		}

		return writeSize;
	}

	template <typename TpValue>
	inline gds_size_t GdsCore::serializeExternal( const TpValue & pValue, const DataWriteCallback & pWriteCallback )
	{
		DynamicByteArray localGdsCache{};
		return serializeExternal( pValue, pWriteCallback, localGdsCache );
	}
	
	template <typename TpValue>
	inline gds_size_t GdsCore::deserializeExternal( TpValue & pValue, const DataReadCallback & pReadCallback, const ReadWriteMemoryView & pGdsCache )
	{
		return _deserializeExternalFixed( pValue, pReadCallback, pGdsCache );
	}

	template <typename TpValue, size_t tpSize>
	inline gds_size_t GdsCore::deserializeExternal( TpValue & pValue, const DataReadCallback & pReadCallback, std::array<byte, tpSize> & pGdsCache )
	{
		return _deserializeExternalFixed( pValue, pReadCallback, pGdsCache );
	}

	template <typename TpValue>
	inline gds_size_t GdsCore::deserializeExternal( TpValue & pValue, const DataReadCallback & pReadCallback, DynamicByteArray & pGdsCache )
	{
		return _deserializeExternalResizable( pValue, pReadCallback, pGdsCache );
	}

	template <typename TpValue>
	inline gds_size_t GdsCore::deserializeExternal( TpValue & pValue, const DataReadCallback & pReadCallback, DynamicMemoryBuffer & pGdsCache )
	{
		return _deserializeExternalResizable( pValue, pReadCallback, pGdsCache );
	}

	template <typename TpValue>
	inline gds_size_t GdsCore::deserializeExternal( TpValue & pValue, const DataReadCallback & pReadCallback, std::vector<byte> & pGdsCache )
	{
		return _deserializeExternalResizable( pValue, pReadCallback, pGdsCache );
	}

	template <typename TpValue>
	inline gds_size_t GdsCore::deserializeExternal( TpValue & pValue, const DataReadCallback & pReadCallback )
	{
		DynamicByteArray localGdsCache{};
		return deserializeExternal( pValue, pReadCallback, localGdsCache );
	}

	template <typename TpValue, typename TpFixedBuffer>
	inline gds_size_t GdsCore::_deserializeExternalFixed( TpValue & pValue, const DataReadCallback & pReadCallback, TpFixedBuffer & pGdsCache )
	{
		if( !_readDataExternalFixed( pReadCallback, pGdsCache ) )
		{
			return 0;
		}
		return deserializeWithMetaData( pGdsCache.data(), pValue );
	}

	template <typename TpValue, typename TpResizableBuffer>
	inline gds_size_t GdsCore::_deserializeExternalResizable( TpValue & pValue, const DataReadCallback & pReadCallback, TpResizableBuffer & pGdsCache )
	{
		// Read data from the stream using the MetaData info. If this succeeds, pGdsCache will have
		// a byte representation of a serialized object (including its MetaData block, of course).
		if( !_readDataExternalResizable( pReadCallback, pGdsCache ) )
		{
			return 0;
		}

		// Deserialize the object into the specified value. This will once again perform reading of the
		// MetaData block, but this is super cheap and we will get some additional validation as a bonus.
		return deserializeWithMetaData( pGdsCache.data(), pValue );
	}

	template <typename TpFixedBuffer>
	inline bool GdsCore::_readDataExternalFixed( const DataReadCallback & pReadCallback, TpFixedBuffer & pReadBuffer )
	{
		// Get the size of the metadata. This is the amount of data we need to read from the stream first.
		const auto metaDataSize = gds::getInstanceMetaDataSize();

		// This is a fixed buffer and cannot be resized. If there is not enough space, we need to abort.
		if( metaDataSize > pReadBuffer.size() )
		{
			ts3ThrowDesc( E_EXC_DEBUG_PLACEHOLDER, "Fixed buffer is to small for the output data." );
		}

		const auto metaDataReadSize = pReadCallback( pReadBuffer.data(), metaDataSize );
		if( metaDataReadSize != metaDataSize )
		{
			return false;
		}

		const auto metaData = gds::readInstanceMetaData( pReadBuffer.data() );
		if( !metaData )
		{
			return false;
		}

		// Again - we cannot resize the buffer. If the additional data does not fit, report an error and quit.
		if( metaData.outputBlockSize > pReadBuffer.size() )
		{
			ts3ThrowDesc( E_EXC_DEBUG_PLACEHOLDER, "Fixed buffer is to small for the output data." );
		}

		const auto objectDataReadSize = pReadCallback( pReadBuffer.data() + metaDataSize, metaData.objectDataSize );
		if( objectDataReadSize != metaData.objectDataSize )
		{
			return false;
		}

		return true;
	}

	template <typename TpResizableBuffer>
	inline bool GdsCore::_readDataExternalResizable( const DataReadCallback & pReadCallback, TpResizableBuffer & pReadBuffer )
	{
		// Get the size of the metadata. This is the amount of data we need to read from the stream first.
		const auto metaDataSize = gds::getInstanceMetaDataSize();

		// Resize the buffer accordingly.
		pReadBuffer.resize( metaDataSize );

		// Read the data from the stream. If there is not enough data, we cannot proceed.
		const auto metaDataReadSize = pReadCallback( pReadBuffer.data(), metaDataSize );
		if( metaDataReadSize != metaDataSize )
		{
			return false;
		}

		// Read the MetaData object itself. It is boolean-convertible, so we can validate it.
		const auto metaData = gds::readInstanceMetaData( pReadBuffer.data() );
		if( !metaData )
		{
			return false;
		}

		// MetaData contains information about the size. ::outputBlockSize is the size of the whole data block
		// (i.e. MetaData + ObjectData). Resize the buffer so we can read the remaining object data now.
		pReadBuffer.resize( trunc_numeric_cast<size_t>( metaData.outputBlockSize ) );

		// Read the object data. Save it after the MetaData (hence the offset), so we have a full representation.
		const auto objectDataReadSize = pReadCallback( pReadBuffer.data() + metaDataSize, metaData.objectDataSize );
		if( objectDataReadSize != metaData.objectDataSize )
		{
			return false;
		}

		return true;
	}

}
