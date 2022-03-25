
#if !defined( __TS3_CORE_GDS_INLINE_DEF_INCLUDE_SECTION__ )
#  error "GDS implementation .inl files cannot be included directly!"
#endif

namespace ts3
{

	/***********************************************************************************************************/
	/**************************************** Core API - evalByteSize() ****************************************/
	/***********************************************************************************************************/

	template <typename Tp, std::enable_if_t<IsGdsSerializable<Tp>::sValue, int>>
	inline gds_size_t GdsCore::evalByteSize( const Tp & pValue )
	{
		return pValue.evalByteSize();
	}

	template <typename Tp>
	inline gds_size_t GdsCore::evalByteSize( const Tp & pValue )
	{
		return gds::evalByteSize( pValue );
	}

	template <typename TpRef, typename TpInternal>
	inline gds_size_t GdsCore::evalByteSize( const gds::ValueRef<TpRef, TpInternal> & pValueRef )
	{
		return evalByteSize( pValueRef.get() );
	}

	template <typename Tp, typename TpInternal, std::enable_if_t<std::is_void<TpInternal>::value, int>>
	inline gds_size_t GdsCore::evalByteSize( const Tp & pValue, const gds::TypeCastTag<TpInternal> & )
	{
		return evalByteSize( pValue );
	}

	template <typename Tp, typename TpInternal, std::enable_if_t<!std::is_void<TpInternal>::value, int>>
	inline gds_size_t GdsCore::evalByteSize( const Tp & pValue, const gds::TypeCastTag<TpInternal> & )
	{
		return evalByteSize( gds::ValueRef<const Tp, TpInternal>{ pValue } );
	}

	template <typename Tp, typename TpInternal, std::enable_if_t<!std::is_void<TpInternal>::value, int>>
	inline gds_size_t GdsCore::evalByteSize( const gds::TypeCastInfo<Tp, TpInternal> & pCastInfo )
	{
		return evalByteSize( pCastInfo.refWrapper.get(), pCastInfo.castTag );
	}

	/***********************************************************************************************************/
	/***************************************** Core API - serialize() ******************************************/
	/***********************************************************************************************************/

	template <typename Tp, std::enable_if_t<IsGdsSerializable<Tp>::sValue, int>>
	inline gds_size_t GdsCore::serialize( byte * pOutputBuffer, const Tp & pValue )
	{
		return pValue.serialize( pOutputBuffer );
	}

	template <typename Tp>
	inline gds_size_t GdsCore::serialize( byte * pOutputBuffer, const Tp & pValue )
	{
		return gds::serialize( pOutputBuffer, pValue );
	}

	template <typename TpRef, typename TpInternal>
	inline gds_size_t GdsCore::serialize( byte * pOutputBuffer, const gds::ValueRef<TpRef, TpInternal> & pValueRef )
	{
		return serialize( pOutputBuffer, pValueRef.get() );
	}

	template <typename Tp, typename TpInternal, std::enable_if_t<std::is_void<TpInternal>::value, int>>
	inline gds_size_t GdsCore::serialize( byte * pOutputBuffer, const Tp & pValue, const gds::TypeCastTag<TpInternal> & )
	{
		return serialize( pOutputBuffer, pValue );
	}

	template <typename Tp, typename TpInternal, std::enable_if_t<!std::is_void<TpInternal>::value, int>>
	inline gds_size_t GdsCore::serialize( byte * pOutputBuffer, const Tp & pValue, const gds::TypeCastTag<TpInternal> & )
	{
		return serialize( pOutputBuffer, gds::ValueRef<Tp, TpInternal>{ pValue } );
	}

	template <typename Tp, typename TpInternal, std::enable_if_t<!std::is_void<TpInternal>::value, int>>
	inline gds_size_t GdsCore::serialize( byte * pOutputBuffer, const gds::TypeCastInfo<Tp, TpInternal> & pCastInfo )
	{
		return serialize( pOutputBuffer, pCastInfo.refWrapper.get(), pCastInfo.castTag );
	}

	/***********************************************************************************************************/
	/**************************************** Core API - deserialize() *****************************************/
	/***********************************************************************************************************/

	template <typename Tp, std::enable_if_t<IsGdsSerializable<Tp>::sValue, int>>
	inline gds_size_t GdsCore::deserialize( const byte * pInputData, Tp & pValue )
	{
		return pValue.deserialize( pInputData );
	}

	template <typename Tp>
	inline gds_size_t GdsCore::deserialize( const byte * pInputData, Tp & pValue )
	{
		return gds::deserialize( pInputData, pValue );
	}

	template <typename TpRef, typename TpInternal>
	inline gds_size_t GdsCore::deserialize( const byte * pInputData, const gds::ValueRef<TpRef, TpInternal> & pValueRef )
	{
		return deserialize( pInputData, pValueRef.get() );
	}

	template <typename Tp, typename TpInternal, std::enable_if_t<std::is_void<TpInternal>::value, int>>
	inline gds_size_t GdsCore::deserialize( const byte * pInputData, Tp & pRef, const gds::TypeCastTag<TpInternal> & )
	{
		return deserialize( pInputData, pRef );
	}

	template <typename Tp, typename TpInternal, std::enable_if_t<!std::is_void<TpInternal>::value, int>>
	inline gds_size_t GdsCore::deserialize( const byte * pInputData, Tp & pRef, const gds::TypeCastTag<TpInternal> & )
	{
		return deserialize( pInputData, gds::ValueRef<Tp, TpInternal>{ pRef } );
	}

	template <typename Tp, typename TpInternal, std::enable_if_t<!std::is_void<TpInternal>::value, int>>
	inline gds_size_t GdsCore::deserialize( const byte * pInputData, const gds::TypeCastInfo<Tp, TpInternal> & pCastInfo )
	{
		return deserialize( pInputData, pCastInfo.refWrapper.get(), pCastInfo.castTag );
	}

	/***********************************************************************************************************/
	/******************************************** Core API - MetaData ******************************************/
	/***********************************************************************************************************/

	template <typename Tp>
	inline gds_size_t GdsCore::evalByteSizeWithMetaData( const Tp & pValue )
	{
		const auto metaDataSize = gds::getInstanceMetaDataSize();
		const auto valueByteSize = evalByteSize( pValue );

		// Technically, user-defined evalByteSize() can be defined to return 0.
		// We always skip empty entries, so in case of metadata let's do the same.
		// If the object itself has no data, stick to that and report 0 size.
		return ( valueByteSize > 0 ) ? ( metaDataSize + valueByteSize ) : 0u;
	}

	template <typename Tp>
	inline gds_size_t GdsCore::serializeWithMetaData( byte * pOutputBuffer, const Tp & pValue )
	{
		// Size of the metadata, in bytes.
		const auto metaDataSize = gds::getInstanceMetaDataSize();

		// The actual serialization. Skip 'metaDataSize' bytes needed for metadata
		// and write after that. serialize() returns the number of bytes written.
		const auto valueByteSize = serialize( pOutputBuffer + metaDataSize, pValue );

		// MetaData for this instance.
		gds::InstanceMetaData metaData;
		metaData.controlKey = gds::CX_GDS_VALUE_META_DATA_CONTROL_KEY;
		metaData.objectDataSize = valueByteSize; // Size of the serialized data of the OBJECT only.
		metaData.outputBlockSize = metaDataSize + valueByteSize; // Size of the whole data block (including MetaData).

		// Now, serialize the metadata itself.
		gds::serialize( pOutputBuffer, metaData );

		return metaData.outputBlockSize;
	}

	template <typename Tp>
	inline gds_size_t GdsCore::deserializeWithMetaData( const byte * pInputData, Tp & pValue )
	{
		// The metadata object to which we will deserialize.
		gds::InstanceMetaData metaData;

		// Deserialize the metadata first. It is always at the beginning of the data block.
		const auto metaDataSize = gds::deserialize( pInputData, metaData );

		// deserialize() returns the number of bytes read from the buffer.
		// This should match the byte size of the InstanceMetaData struct.
		ts3DebugAssert( metaDataSize == gds::getInstanceMetaDataSize() );

		const auto valueByteSize = deserialize( pInputData + metaDataSize, pValue );

		return metaDataSize + valueByteSize;
	}

	/***********************************************************************************************************/
	/******************************************* Container/Map Helpers *****************************************/
	/***********************************************************************************************************/

	template <template <typename...> typename TpC, typename... TpCArgs>
	inline gds_size_t GdsCore::evalContainerByteSize( const TpC<TpCArgs...> & pContainer )
	{
		// Compute the size for the specified container.
		// The space required is the serialized size + binary representation of all elements in the container.
		gds_size_t byteSize = 0;

		// The first member is the number of elements in a container. We are not interested in its value, though.
		byteSize += evalByteSize( gds::emptySizeType() );

		// Enumerate over the container's elements.
		for( const auto & entry : pContainer )
		{
			// Evaluate the size of each element. Propagate cast tag to support container-wise value casting.
			byteSize += evalByteSize( entry );
		}

		return byteSize;
	}

	template <template <typename...> typename TpC, typename... TpCArgs>
	inline gds_size_t GdsCore::serializeContainer( byte * pOutputBuffer, const TpC<TpCArgs...> & pContainer )
	{
		// First, get the size of the container (number of elements).
		const size_t containerSize = pContainer.size();

		// size_t may have different sizes on different platforms. asSizeTypeValue() ensures this is properly
		// handled by the GDS, so we can deserialize the data on a different platform, and get expected results.
		gds_size_t byteSize = serialize( pOutputBuffer, gds::asSizeType( containerSize ) );

		// Enumerate over the container's elements.
		for( const auto & entry : pContainer )
		{
			// Serialize the element, add the size to the total byte size. Propagate cast tag.
			byteSize += serialize( pOutputBuffer + byteSize, entry );
		}

		return byteSize;
	}

	template <template <typename...> typename TpC, typename... TpCArgs, typename TpIns>
	inline gds_size_t GdsCore::deserializeContainer( const byte * pInputData, TpC<TpCArgs...> & pContainer, const TpIns & pInsertCallback )
	{
		// The variable to store the container's size.
		size_t containerSize = 0;

		// Deserialize the size. Since we used asSizeTypeValue() during serialization, we must now use asSizeTypeRef()
		// to properly retrieve the deserialized value. The output will be properly stored in the wrapped variable.
		gds_size_t byteSize = deserialize( pInputData, gds::asSizeType( containerSize ) );

		for( size_t entryIndex = 0; entryIndex < containerSize; ++entryIndex )
		{
			// Temporary for the deserialized state.
			typename TpC<TpCArgs...>::value_type tempEntry{};

			// Deserialize current element. Propagate cast tag to support container-wise value casting.
			byteSize += deserialize( pInputData + byteSize, tempEntry );

			// Call the insert callback to add it to the container. Index is needed e.g. for std::array.
			pInsertCallback( pContainer, entryIndex, std::move( tempEntry ) );
		}

		return byteSize;
	}

	template <template <typename...> typename TpC, typename... TpCArgs>
	inline gds_size_t GdsCore::evalMapByteSize( const TpC<TpCArgs...> & pMap )
	{
		gds_size_t byteSize = 0;

		// The first member is the number of elements in a container. We are not interested in its value, though.
		byteSize += evalByteSize( gds::emptySizeType() );

		// Enumerate over the container's elements.
		for( const auto & mapEntry : pMap )
		{
			// Evaluate the size of each element. Propagate cast tag to support container-wise value casting.
			byteSize += evalByteSize( mapEntry.first );
			byteSize += evalByteSize( mapEntry.second );
		}

		return byteSize;
	}

	template <template <typename...> typename TpM, typename TpK, typename TpV, typename... TpCArgs>
	inline gds_size_t GdsCore::serializeMap( byte * pOutputBuffer, const TpM<TpK, TpV, TpCArgs...> & pMap )
	{
		const size_t mapSize = pMap.size();

		gds_size_t byteSize = serialize( pOutputBuffer, gds::asSizeType( mapSize ) );

		for( const auto & mapEntry : pMap )
		{
			byteSize += serialize( pOutputBuffer + byteSize, mapEntry.first );
			byteSize += serialize( pOutputBuffer + byteSize, mapEntry.second );
		}

		return byteSize;
	}

	template <template <typename...> typename TpM, typename TpK, typename TpV, typename... TpCArgs>
	inline gds_size_t GdsCore::deserializeMap( const byte * pInputData, TpM<TpK, TpV, TpCArgs...> & pMap )
	{
		size_t mapSize = 0;

		gds_size_t byteSize = deserialize( pInputData, gds::asSizeType( mapSize ) );

		for( size_t entryIndex = 0; entryIndex < mapSize; ++entryIndex )
		{
			TpK tempKey{};
			byteSize += deserialize( pInputData + byteSize, tempKey );

			TpV tempValue{};
			byteSize += deserialize( pInputData + byteSize, tempValue );

			pMap.emplace( std::move( tempKey ), std::move( tempValue ) );
		}

		return byteSize;
	}

}
