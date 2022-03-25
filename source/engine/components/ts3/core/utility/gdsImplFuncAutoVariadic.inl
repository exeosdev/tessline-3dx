
#if !defined( __TS3_CORE_GDS_INLINE_DEF_INCLUDE_SECTION__ )
#  error "GDS implementation .inl files cannot be included directly!"
#endif

namespace ts3
{

	/***********************************************************************************************************/
	/********************************************** Core API - Auto ********************************************/
	/***********************************************************************************************************/

	template <typename TpFixedBuffer, typename TpValue>
	inline gds_size_t GdsCore::_serializeAutoFixed( TpFixedBuffer & pOutputBuffer, const TpValue & pValue )
	{
		// Get the size of the serialized object (i.e. the minimum required size of the target buffer).
		const auto totalByteSize = evalByteSize( pValue );

		if( totalByteSize > 0 )
		{
			// The buffer is a fixed one - check if its size is large enough to store the whole data.
			if( totalByteSize > pOutputBuffer.size() )
			{
				// Buffer is too small, throw an error, nothing can be done here.
				ts3ThrowDesc( E_EXC_DEBUG_PLACEHOLDER, "Fixed buffer is to small for the output data." );
			}

			// Buffer is large enough, so we can use it as the target for the output.
			// Serialize the data into the buffer. This is the call to the actual, value-specific serialize().
			serialize( pOutputBuffer.data(), pValue );
		}

		return totalByteSize;
	}

	template <typename TpFixedBuffer, typename TpValue>
	inline gds_size_t GdsCore::_serializeAutoWithMetaDataFixed( TpFixedBuffer & pOutputBuffer, const TpValue & pValue )
	{
		// Same thing as above, but we also include the MetaData block alongside the object itself.

		const auto totalByteSize = evalByteSizeWithMetaData( pValue );

		if( totalByteSize > 0 )
		{
			if( totalByteSize > pOutputBuffer.size() )
			{
				ts3ThrowDesc( E_EXC_DEBUG_PLACEHOLDER, "Fixed buffer is to small for the output data." );
			}
			serializeWithMetaData( pOutputBuffer.data(), pValue );
		}

		return totalByteSize;
	}

	template <typename TpResizableBuffer, typename TpValue>
	inline gds_size_t GdsCore::_serializeAutoResizable( TpResizableBuffer & pOutputBuffer, const TpValue & pValue )
	{
		// Get the size of the serialized object (i.e. the minimum required size of the target buffer).
		const auto totalByteSize = evalByteSize( pValue );

		if( totalByteSize > 0 )
		{
			// Resize the output buffer.
			pOutputBuffer.resize( trunc_numeric_cast<size_t>( totalByteSize ) );

			// Serialize the data into the buffer. This is the call to the actual, value-specific serialize().
			serialize( pOutputBuffer.data(), pValue );
		}

		return totalByteSize;
	}

	template <typename TpResizableBuffer, typename TpValue>
	inline gds_size_t GdsCore::_serializeAutoWithMetaDataResizable( TpResizableBuffer & pOutputBuffer, const TpValue & pValue )
	{
		// Same thing as above, but we also include the MetaData block alongside the object itself.

		const auto totalByteSize = evalByteSizeWithMetaData( pValue );

		if( totalByteSize > 0 )
		{
			pOutputBuffer.resize( trunc_numeric_cast<size_t>( totalByteSize ) );
			serializeWithMetaData( pOutputBuffer.data(), pValue );
		}

		return totalByteSize;
	}

	template <typename TpValue>
	inline gds_size_t GdsCore::serializeAuto( const ReadWriteMemoryView & pOutputBuffer, const TpValue & pValue )
	{
		return _serializeAutoFixed( pOutputBuffer, pValue );
	}

	template <typename TpValue, size_t tpSize>
	inline gds_size_t GdsCore::serializeAuto( std::array<byte, tpSize> & pOutputBuffer, const TpValue & pValue )
	{
		return _serializeAutoFixed( pOutputBuffer, pValue );
	}

	template <typename TpValue>
	inline gds_size_t GdsCore::serializeAutoWithMetaData( const ReadWriteMemoryView & pOutputBuffer, const TpValue & pValue )
	{
		return _serializeAutoWithMetaDataFixed( pOutputBuffer, pValue );
	}

	template <typename TpValue, size_t tpSize>
	inline gds_size_t GdsCore::serializeAutoWithMetaData( std::array<byte, tpSize> & pOutputBuffer, const TpValue & pValue )
	{
		return _serializeAutoWithMetaDataFixed( pOutputBuffer, pValue );
	}

	template <typename TpValue>
	inline gds_size_t GdsCore::serializeAuto( DynamicByteArray & pOutputBuffer, const TpValue & pValue )
	{
		return _serializeAutoResizable( pOutputBuffer, pValue );
	}

	template <typename TpValue>
	inline gds_size_t GdsCore::serializeAuto( DynamicMemoryBuffer & pOutputBuffer, const TpValue & pValue )
	{
		return _serializeAutoResizable( pOutputBuffer, pValue );
	}

	template <typename TpValue>
	inline gds_size_t GdsCore::serializeAuto( std::vector<byte> & pOutputBuffer, const TpValue & pValue )
	{
		return _serializeAutoResizable( pOutputBuffer, pValue );
	}

	template <typename TpValue>
	inline gds_size_t GdsCore::serializeAutoWithMetaData( DynamicByteArray & pOutputBuffer, const TpValue & pValue )
	{
		return _serializeAutoWithMetaDataResizable( pOutputBuffer, pValue );
	}

	template <typename TpValue>
	inline gds_size_t GdsCore::serializeAutoWithMetaData( DynamicMemoryBuffer & pOutputBuffer, const TpValue & pValue )
	{
		return _serializeAutoWithMetaDataResizable( pOutputBuffer, pValue );
	}

	template <typename TpValue>
	inline gds_size_t GdsCore::serializeAutoWithMetaData( std::vector<byte> & pOutputBuffer, const TpValue & pValue )
	{
		return _serializeAutoWithMetaDataResizable( pOutputBuffer, pValue );
	}

	template <typename TpBuffer, typename TpValue>
	inline gds_size_t GdsCore::deserializeAuto( const TpBuffer & pInputData, TpValue & pValue )
	{
		return deserialize( pInputData.data(), pValue );
	}

	template <typename TpBuffer, typename TpValue>
	inline gds_size_t GdsCore::deserializeAutoWithMetaData( const TpBuffer & pInputData, TpValue & pValue )
	{
		return deserializeWithMetaData( pInputData.data(), pValue );
	}

	/***********************************************************************************************************/
	/******************************************** Core API - Variadic ******************************************/
	/***********************************************************************************************************/

	template <typename Tp>
	inline gds_size_t GdsCore::evalByteSizeAll( const Tp & pValue )
	{
		return evalByteSize( pValue );
	}

	template <typename Tp, typename... TpRest>
	inline gds_size_t GdsCore::evalByteSizeAll( const Tp & pValue, TpRest && ...pRest )
	{
		gds_size_t byteSize = evalByteSize( pValue );
		byteSize += evalByteSizeAll( std::forward<TpRest>( pRest )... );
		return byteSize;
	}

	template <typename Tp>
	inline gds_size_t GdsCore::serializeAll( byte * pOutputBuffer, const Tp & pValue )
	{
		return serialize( pOutputBuffer, pValue );
	}

	template <typename Tp, typename... TpRest>
	inline gds_size_t GdsCore::serializeAll( byte * pOutputBuffer, const Tp & pValue, TpRest && ...pRest )
	{
		gds_size_t byteSize = serialize( pOutputBuffer, pValue );
		byteSize += serializeAll( pOutputBuffer + byteSize, std::forward<TpRest>( pRest )... );
		return byteSize;
	}

	template <typename Tp>
	inline gds_size_t GdsCore::deserializeAll( const byte * pInputData, Tp & pValue )
	{
		return deserialize( pInputData, pValue );
	}

	template <typename Tp, typename... TpRest>
	inline gds_size_t GdsCore::deserializeAll( const byte * pInputData, Tp & pValue, TpRest && ...pRest )
	{
		gds_size_t byteSize = deserialize( pInputData, pValue );
		byteSize += deserializeAll( pInputData + byteSize, std::forward<TpRest>( pRest )... );
		return byteSize;
	}

}
