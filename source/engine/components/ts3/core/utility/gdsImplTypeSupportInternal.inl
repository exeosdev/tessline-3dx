
#if !defined( __TS3_CORE_GDS_INLINE_DEF_INCLUDE_SECTION__ )
#  error "GDS implementation .inl files cannot be included directly!"
#endif

namespace ts3
{

	/***********************************************************************************************************/
	/************************************************* ArrayView ***********************************************/
	/***********************************************************************************************************/

	template <typename Tp>
	inline gds_size_t GdsCore::evalByteSize( const ArrayView<Tp> & pArrayView )
	{
		gds_size_t byteSize = evalByteSize( gds::emptySizeType() );
		for( const auto & entry : pArrayView )
		{
			byteSize += evalByteSize( entry );
		}
		return byteSize;
	}

	template <typename Tp>
	inline gds_size_t GdsCore::serialize( byte * pOutputBuffer, const ArrayView<Tp> & pArrayView )
	{
		gds_size_t byteSize = serialize( pOutputBuffer, gds::asSizeType( pArrayView.size() ) );
		for( const auto & value : pArrayView )
		{
			byteSize += serialize(  pOutputBuffer + byteSize, value );
		}
		return byteSize;
	}

	template <typename Tp>
	inline gds_size_t GdsCore::deserialize( const byte * pInputData, ArrayView<Tp> & pArrayView )
	{
		size_t arraySize = 0;
		gds_size_t byteSize = deserialize( pInputData, gds::asSizeType( arraySize ) );
		for( auto & value : pArrayView )
		{
			byteSize += deserialize( pInputData + byteSize, value );
		}
		return byteSize;
	}

	/***********************************************************************************************************/
	/*********************************************** AtomicBitmask *********************************************/
	/***********************************************************************************************************/

	template <typename TpIntegral>
	inline gds_size_t GdsCore::evalByteSize( const AtomicBitmask<TpIntegral> & pBitmask )
	{
		return evalByteSize( static_cast<TpIntegral>( pBitmask ) );
	}

	template <typename TpIntegral>
	inline gds_size_t GdsCore::serialize( byte * pOutputBuffer, const AtomicBitmask<TpIntegral> & pBitmask )
	{
		return serialize( pOutputBuffer, static_cast<TpIntegral>( pBitmask ) );
	}

	template <typename TpIntegral>
	inline gds_size_t GdsCore::deserialize( const byte * pInputData, AtomicBitmask<TpIntegral> & pBitmask )
	{
		return deserialize( pInputData, static_cast<TpIntegral>( pBitmask ) );
	}

	/***********************************************************************************************************/
	/************************************************** Bitmask ************************************************/
	/***********************************************************************************************************/

	template <typename TpIntegral>
	inline gds_size_t GdsCore::evalByteSize( const Bitmask<TpIntegral> & pBitmask )
	{
		return evalByteSize( static_cast<TpIntegral>( pBitmask ) );
	}

	template <typename TpIntegral>
	inline gds_size_t GdsCore::serialize( byte * pOutputBuffer, const Bitmask<TpIntegral> & pBitmask )
	{
		return serialize( pOutputBuffer, static_cast<TpIntegral>( pBitmask ) );
	}

	template <typename TpIntegral>
	inline gds_size_t GdsCore::deserialize( const byte * pInputData, Bitmask<TpIntegral> & pBitmask )
	{
		return deserialize( pInputData, static_cast<TpIntegral>( pBitmask ) );
	}

	/***********************************************************************************************************/
	/************************************************ SortedArray **********************************************/
	/***********************************************************************************************************/

	template <typename Tp, typename TpCompare, typename TpAlloc>
	inline gds_size_t GdsCore::evalByteSize( const SortedArray<Tp, TpCompare, TpAlloc> & pSortedArray )
	{
		return evalContainerByteSize( pSortedArray );
	}

	template <typename Tp, typename TpCompare, typename TpAlloc>
	inline gds_size_t GdsCore::serialize( byte * pOutputBuffer, const SortedArray<Tp, TpCompare, TpAlloc> & pSortedArray )
	{
		return serializeContainer( pOutputBuffer, pSortedArray );
	}

	template <typename Tp, typename TpCompare, typename TpAlloc>
	inline gds_size_t GdsCore::deserialize( const byte * pInputData, const SortedArray<Tp, TpCompare, TpAlloc> & pSortedArray )
	{
		return deserializeContainer( pInputData, pSortedArray, []( auto & SA, auto, auto && E ) -> void { SA.insert( std::move( E ) ); } );
	}

}
