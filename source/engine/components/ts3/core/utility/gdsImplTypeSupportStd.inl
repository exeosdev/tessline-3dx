
#if !defined( __TS3_CORE_GDS_INLINE_DEF_INCLUDE_SECTION__ )
#  error "GDS implementation .inl files cannot be included directly!"
#endif

namespace ts3
{


	/***********************************************************************************************************/
	/******************************************** std::basic_string ********************************************/
	/***********************************************************************************************************/

	template <typename _Ch, typename _Tr, typename _Alloc>
	inline gds_size_t GdsCore::evalByteSize( const std::basic_string<_Ch, _Tr, _Alloc> & pString )
	{
		gds_size_t byteSize = 0;

		// The first member is the length of the specified string.
		byteSize += evalByteSize( gds::emptySizeType() );

		// Then we have the characters (potentially cast).
		byteSize += pString.length() * evalByteSize( _Ch{} );

		return byteSize;
	}
	
	template <typename _Ch, typename _Tr, typename _Alloc>
	inline gds_size_t GdsCore::serialize( byte * pOutputBuffer, const std::basic_string<_Ch, _Tr, _Alloc> & pString )
	{
		const auto strLength = pString.length();

		gds_size_t byteSize = serialize( pOutputBuffer, gds::asSizeType( strLength ) );

		for( size_t charIndex = 0; charIndex < strLength; ++charIndex )
		{
			byteSize += serialize( pOutputBuffer + byteSize, pString[charIndex] );
		}

		return byteSize;
	}
	
	template <typename _Ch, typename _Tr, typename _Alloc>
	inline gds_size_t GdsCore::deserialize( const byte * pInputData, std::basic_string<_Ch, _Tr, _Alloc> & pString )
	{
		size_t strLength = 0;
		gds_size_t byteSize = deserialize( pInputData, gds::asSizeType( strLength ) );

		pString.resize( strLength );

		for( size_t charIndex = 0; charIndex < strLength; ++charIndex )
		{
			byteSize += deserialize( pInputData + byteSize, pString[charIndex] );
		}

		return byteSize;
	}

	/***********************************************************************************************************/
	/************************************************ std::pair ************************************************/
	/***********************************************************************************************************/
		
	template <typename _Ty1, typename _Ty2>
	inline gds_size_t GdsCore::evalByteSize( const std::pair<_Ty1, _Ty2> & pPair )
	{
		gds_size_t byteSize = 0;
		byteSize += evalByteSize( pPair.first );
		byteSize += evalByteSize( pPair.second );
		return byteSize;
	}

	template <typename _Ty1, typename _Ty2>
	inline gds_size_t GdsCore::serialize( byte * pOutputBuffer, const std::pair<_Ty1, _Ty2> & pPair )
	{
		gds_size_t byteSize = 0;
		byteSize += serialize( pOutputBuffer, pPair.first );
		byteSize += serialize( pOutputBuffer, pPair.second );
		return byteSize;
	}

	template <typename _Ty1, typename _Ty2>
	inline gds_size_t GdsCore::deserialize( const byte * pInputData, std::pair<_Ty1, _Ty2> & pPair )
	{
		gds_size_t byteSize = 0;
		byteSize += deserialize( pInputData, pPair.first );
		byteSize += deserialize( pInputData, pPair.second );
		return byteSize;
	}

	/***********************************************************************************************************/
	/************************************************ std::array ***********************************************/
	/***********************************************************************************************************/

	template <typename _T, size_t _N>
	inline gds_size_t GdsCore::evalByteSize( const std::array<_T, _N> & pArray )
	{
		return evalContainerByteSize( pArray );
	}

	template <typename _T, size_t _N>
	inline gds_size_t GdsCore::serialize( byte * pOutputBuffer, const std::array<_T, _N> & pArray )
	{
		return serializeContainer( pOutputBuffer, pArray );
	}

	template <typename _T, size_t _N>
	inline gds_size_t GdsCore::deserialize( const byte * pInputData, std::array<_T, _N> & pArray )
	{
		// std::array doesn't have any push/insert methods, so that's the one case where index becomes useful.
		// We simply assign the incoming elements under their indexes passed by the helper method. Simple.
		return deserializeContainer( pInputData, pArray, []( auto & A, auto I, auto && E ) -> void { A[I] = std::move( E ); } );
	}

	/***********************************************************************************************************/
	/*********************************************** std::vector ***********************************************/
	/***********************************************************************************************************/

	template <typename _T, typename _Alloc>
	inline gds_size_t GdsCore::evalByteSize( const std::vector<_T, _Alloc> & pVector )
	{
		return evalContainerByteSize( pVector );
	}

	template <typename _T, typename _Alloc>
	inline gds_size_t GdsCore::serialize( byte * pOutputBuffer, const std::vector<_T, _Alloc> & pVector )
	{
		return serializeContainer( pOutputBuffer, pVector );
	}

	template <typename _T, typename _Alloc>
	inline gds_size_t GdsCore::deserialize( const byte * pInputData, std::vector<_T, _Alloc> & pVector )
	{
		return deserializeContainer( pInputData, pVector, []( auto & V, auto, auto && E ) -> void { V.push_back( std::move( E ) ); } );
	}

	/***********************************************************************************************************/
	/************************************************ std::list ************************************************/
	/***********************************************************************************************************/

	template <typename _T, typename _Alloc>
	inline gds_size_t GdsCore::evalByteSize( const std::list<_T, _Alloc> & pList )
	{
		return evalContainerByteSize( pList );
	}

	template <typename _T, typename _Alloc>
	inline gds_size_t GdsCore::serialize( byte * pOutputBuffer, const std::list<_T, _Alloc> & pList )
	{
		return serializeContainer( pOutputBuffer, pList );
	}

	template <typename _T, typename _Alloc>
	inline gds_size_t GdsCore::deserialize( const byte * pInputData, std::list<_T, _Alloc> & pList )
	{
		return deserializeContainer( pInputData, pList, []( auto & L, auto, auto && E ) -> void { L.push_back( std::move( E ) ); } );
	}

	/***********************************************************************************************************/
	/************************************************ std::deque ***********************************************/
	/***********************************************************************************************************/

	template <typename _T, typename _Alloc>
	inline gds_size_t GdsCore::evalByteSize( const std::deque<_T, _Alloc> & pDeque )
	{
		return evalContainerByteSize( pDeque );
	}

	template <typename _T, typename _Alloc>
	inline gds_size_t GdsCore::serialize( byte * pOutputBuffer, const std::deque<_T, _Alloc> & pDeque )
	{
		return serializeContainer( pOutputBuffer, pDeque );
	}

	template <typename _T, typename _Alloc>
	inline gds_size_t GdsCore::deserialize( const byte * pInputData, std::deque<_T, _Alloc> & pDeque )
	{
		return deserializeContainer( pInputData, pDeque, []( auto & D, auto, auto && E ) -> void { D.push_back( std::move( E ) ); } );
	}

	/***********************************************************************************************************/
	/************************************************ std::queue ***********************************************/
	/***********************************************************************************************************/

	template <typename _T, typename _Cnt>
	inline gds_size_t GdsCore::evalByteSize( const std::queue<_T, _Cnt> & pQueue )
	{
		return evalContainerByteSize( pQueue );
	}

	template <typename _T, typename _Cnt>
	inline gds_size_t GdsCore::serialize( byte * pOutputBuffer, const std::queue<_T, _Cnt> & pQueue )
	{
		return serializeContainer( pOutputBuffer, pQueue );
	}

	template <typename _T, typename _Cnt>
	inline gds_size_t GdsCore::deserialize( const byte * pInputData, std::queue<_T, _Cnt> & pQueue )
	{
		return deserializeContainer( pInputData, pQueue, []( auto & Q, auto, auto && E ) -> void { Q.push( std::move( E ) ); } );
	}

	/***********************************************************************************************************/
	/************************************************ std::stack ***********************************************/
	/***********************************************************************************************************/

	template <typename _T, typename _Cnt>
	inline gds_size_t GdsCore::evalByteSize( const std::stack<_T, _Cnt> & pStack )
	{
		return evalContainerByteSize( pStack );
	}

	template <typename _T, typename _Cnt>
	inline gds_size_t GdsCore::serialize( byte * pOutputBuffer, const std::stack<_T, _Cnt> & pStack )
	{
		return serializeContainer( pOutputBuffer, pStack );
	}

	template <typename _T, typename _Cnt>
	inline gds_size_t GdsCore::deserialize( const byte * pInputData, std::stack<_T, _Cnt> & pStack )
	{
		return deserializeContainer( pInputData, pStack, []( auto & S, auto, auto && E ) -> void { S.push( std::move( E ) ); } );
	}

	/***********************************************************************************************************/
	/************************************************* std::set ************************************************/
	/***********************************************************************************************************/

	template <typename _K, typename _Cmp, typename _Alloc>
	inline gds_size_t GdsCore::evalByteSize( const std::set<_K, _Cmp, _Alloc> & pSet )
	{
		return evalContainerByteSize( pSet );
	}

	template <typename _K, typename _Cmp, typename _Alloc>
	inline gds_size_t GdsCore::serialize( byte * pOutputBuffer, const std::set<_K, _Cmp, _Alloc> & pSet )
	{
		return serializeContainer( pOutputBuffer, pSet );
	}

	template <typename _K, typename _Cmp, typename _Alloc>
	inline gds_size_t GdsCore::deserialize( const byte * pInputData, std::set<_K, _Cmp, _Alloc> & pSet )
	{
		return deserializeContainer( pInputData, pSet, []( auto & S, auto, auto && E ) -> void { S.insert( std::move( E ) ); } );
	}

	/***********************************************************************************************************/
	/******************************************** std::unordered_set *******************************************/
	/***********************************************************************************************************/

	template <typename _K, typename _H, typename _Eq, typename _Alloc>
	inline gds_size_t GdsCore::evalByteSize( const std::unordered_set<_K, _H, _Eq, _Alloc> & pUOSet )
	{
		return evalContainerByteSize( pUOSet );
	}

	template <typename _K, typename _H, typename _Eq, typename _Alloc>
	inline gds_size_t GdsCore::serialize( byte * pOutputBuffer, const std::unordered_set<_K, _H, _Eq, _Alloc> & pUOSet )
	{
		return serializeContainer( pOutputBuffer, pUOSet );
	}

	template <typename _K, typename _H, typename _Eq, typename _Alloc>
	inline gds_size_t GdsCore::deserialize( const byte * pInputData, std::unordered_set<_K, _H, _Eq, _Alloc> & pUOSet )
	{
		return deserializeContainer( pInputData, pUOSet, []( auto & S, auto, auto && E ) -> void { S.insert( std::move( E ) ); } );
	}

	/***********************************************************************************************************/
	/************************************************* std::map ************************************************/
	/***********************************************************************************************************/

	template <typename _K, typename _V, typename _Cmp, typename _Alloc>
	inline gds_size_t GdsCore::evalByteSize( const std::map<_K, _V, _Cmp, _Alloc> & pMap )
	{
		return evalMapByteSize( pMap );
	}

	template <typename _K, typename _V, typename _Cmp, typename _Alloc>
	inline gds_size_t GdsCore::serialize( byte * pOutputBuffer, const std::map<_K, _V, _Cmp, _Alloc> & pMap )
	{
		return serializeMap( pOutputBuffer, pMap );
	}

	template <typename _K, typename _V, typename _Cmp, typename _Alloc>
	inline gds_size_t GdsCore::deserialize( const byte * pInputData, std::map<_K, _V, _Cmp, _Alloc> & pMap )
	{
		return deserializeMap( pInputData, pMap );
	}

	/***********************************************************************************************************/
	/******************************************** std::unordered_map *******************************************/
	/***********************************************************************************************************/

	template <typename _K, typename _V, typename _H, typename _Eq, typename _Alloc>
	inline gds_size_t GdsCore::evalByteSize( const std::unordered_map<_K, _V, _H, _Eq, _Alloc> & pUOMap )
	{
		return evalMapByteSize( pUOMap );
	}

	template <typename _K, typename _V, typename _H, typename _Eq, typename _Alloc>
	inline gds_size_t GdsCore::serialize( byte * pOutputBuffer, const std::unordered_map<_K, _V, _H, _Eq, _Alloc> & pUOMap )
	{
		return serializeMap( pOutputBuffer, pUOMap );
	}

	template <typename _K, typename _V, typename _H, typename _Eq, typename _Alloc>
	inline gds_size_t GdsCore::deserialize( const byte * pInputData, std::unordered_map<_K, _V, _H, _Eq, _Alloc> & pUOMap )
	{
		return deserializeMap( pInputData, pUOMap );
	}
		

}
