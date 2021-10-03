
#pragma once

namespace ts3
{

	/// @brief
	using result_code_value_t = uint32;

	/// @brief
	template <typename TpValue, typename TpErrorPredicate>
	struct ResultProxy
	{
	public:
		using ValueType = TpValue;
		using ErrorPredicateType = TpErrorPredicate;

	public:
		// Value of the result code.
		TpValue code = 0u;

		// A string identifying the result code. Usually - name of the code used across sources.
		const char * codeStr;

		// Additional info string, optional.
		const char * infoStr;

	public:
		ResultProxy( const ResultProxy & ) = default;
		ResultProxy & operator=( const ResultProxy & ) = default;

		/// @brief Trivial default constructor.
		ResultProxy() = default;

		/// @brief Explicit constructor which initialises result object with a specified result code.
		/// @param code Result code.
		template <typename TpOtherValue>
		explicit ResultProxy( TpOtherValue pCode )
		: code( static_cast<TpValue>( pCode ) )
		, codeStr( CX_STR_CHAR_EMPTY )
		, infoStr( CX_STR_CHAR_EMPTY )
		{}

		/// @brief Initializes result object with specified content.
		/// @param code Result code.
		/// @param codeStr Code string (text representation of the code, for example).
		/// @param infoStr Additional info string.
		template <typename TpOtherValue>
		ResultProxy( TpOtherValue pCode, const char * pCodeStr, const char * pInfoStr = nullptr )
		: code( static_cast<TpValue>( pCode ) )
		, codeStr( pCodeStr ? pCodeStr : CX_STR_CHAR_EMPTY )
		, infoStr( pInfoStr ? pCodeStr : CX_STR_CHAR_EMPTY )
		{}

		constexpr explicit operator bool() const
		{
			return !TpErrorPredicate()( code );
		}

		/// @brief Swaps two results.
		/// @param other Result object to swap with.
		void swap( ResultProxy & pOther )
		{
			std::swap( code, pOther.code );
			std::swap( codeStr, pOther.codeStr );
			std::swap( infoStr, pOther.infoStr );
		}

		/// @brief Returns whether the result object is empty (i.e. has no explicit code set or code is zero).
		/// @returns True if result object is empty or false otherwise.
		TS3_PCL_ATTR_NO_DISCARD bool empty() const
		{
			return code == 0;
		}
	};

	/// @brief
	enum class EResultCodeType : uint8
	{
		Success,
		Warning,
		Error
	};

	constexpr result_code_value_t CX_RESULT_CODE_CONTROL_KEY   = 0x18000000;
	constexpr result_code_value_t CX_RESULT_CODE_TYPE_MASK     = 0x00FF0000;
	constexpr result_code_value_t CX_RESULT_CODE_CATEGORY_MASK = 0x0000FF00;
	constexpr result_code_value_t CX_RESULT_CODE_IID_MASK      = 0x000000FF;

	///
	inline constexpr result_code_value_t ecDeclareResultCode( EResultCodeType pType, uint8 pCategory, uint8 pIID )
	{
		return ( CX_RESULT_CODE_CONTROL_KEY | ( ( result_code_value_t )( pType ) << 16 ) | ( ( result_code_value_t )( pCategory ) << 8 ) | pIID );
	}

	///
	inline constexpr EResultCodeType ecGetResultCodeType( result_code_value_t pResultCode )
	{
		return ( EResultCodeType )( ( pResultCode & CX_RESULT_CODE_TYPE_MASK ) >> 16 );
	}

	///
	inline constexpr uint8 ecGetResultCodeCategory( result_code_value_t pResultCode )
	{
		return ( uint8 )( ( pResultCode & CX_RESULT_CODE_CATEGORY_MASK ) >> 8 );
	}

	///
	inline constexpr bool ecValidateResultCode( result_code_value_t pResultCode )
	{
		return ( pResultCode & CX_RESULT_CODE_CONTROL_KEY ) == CX_RESULT_CODE_CONTROL_KEY;
	}

	enum : uint8
	{
		E_RESULT_CATEGORY_GENERIC = 0,
	};
	
	enum : result_code_value_t
	{
		E_RESULT_CODE_GENERIC_SUCCESS = ecDeclareResultCode( EResultCodeType::Success, E_RESULT_CATEGORY_GENERIC, 0 )
	};

	struct ResultCodeErrorPredicate
	{
		constexpr bool operator()( result_code_value_t pResultCode ) const
		{
			return ecGetResultCodeType( pResultCode ) == EResultCodeType::Error;
		}
	};

#if ( TRUE )

	using ResultCode = ResultProxy<result_code_value_t, ResultCodeErrorPredicate>;

	/// @brief Creates result from code.
	#define ts3MakeResult( pCode ) ::ts3::ResultCode( pCode, #pCode )

	/// @brief Creates result from code and additional info string.
	#define ts3MakeResultEx( pCode, pInfo ) ::ts3::ResultCode( pCode, #pCode, pInfo )

	/// @brief
	#define ts3ResultGetCode( pResult ) ( pResult.code )

	/// @brief
	#define ts3ResultGetCodeStr( pResult ) ( pResult.codeStr )

	/// @brief
	#define ts3ResultGetInfoStr( pResult ) ( pResult.infoStr )

#else

	/// @brief Result code.
	using ResultCode = result_code_value_t;

	/// @brief Creates result from code.
	#define ts3MakeResult( pCode ) ( pCode )

	/// @brief Creates result from code and additional info string.
	#define ts3MakeResultEx( pCode, pInfo ) ( pCode )

	/// @brief
	#define ts3ResultGetCode( pResult ) ( pResult )

	/// @brief
	#define ts3ResultGetCodeStr( pResult ) ts3MakeStr( pResult )

	/// @brief
	#define ts3ResultGetInfoStr( pResult ) CX_STR_CHAR_EMPTY

#endif

	template <typename TpValue, typename TpErrorPredicate>
	inline constexpr bool operator==( const ResultProxy<TpValue, TpErrorPredicate> & pLhs,
									  const ResultProxy<TpValue, TpErrorPredicate> & pRhs )
	{
		return pLhs.code == pRhs.code;
	}

	template <typename TpValue, typename TpErrorPredicate, typename TpIntegral>
	inline constexpr bool operator==( const ResultProxy<TpValue, TpErrorPredicate> & pLhs, TpIntegral pRhs )
	{
		return pLhs.code == pRhs;
	}

	template <typename TpValue, typename TpErrorPredicate, typename TpIntegral>
	inline constexpr bool operator==( TpIntegral pLhs, const ResultProxy<TpValue, TpErrorPredicate> & pRhs )
	{
		return pLhs == pRhs.code;
	}

	template <typename TpValue, typename TpErrorPredicate>
	inline constexpr bool operator!=( const ResultProxy<TpValue, TpErrorPredicate> & pLhs,
									  const ResultProxy<TpValue, TpErrorPredicate> & pRhs )
	{
		return pLhs.code != pRhs.code;
	}

	template <typename TpValue, typename TpErrorPredicate, typename TpIntegral>
	inline constexpr bool operator!=( const ResultProxy<TpValue, TpErrorPredicate> & pLhs, TpIntegral pRhs )
	{
		return pLhs.code != pRhs;
	}

	template <typename TpValue, typename TpErrorPredicate, typename TpIntegral>
	inline constexpr bool operator!=( TpIntegral pLhs, const ResultProxy<TpValue, TpErrorPredicate> & pRhs )
	{
		return pLhs != pRhs.code;
	}

	template <typename TpValue, typename TpErrorPredicate>
	inline constexpr bool operator<( const ResultProxy<TpValue, TpErrorPredicate> & pLhs,
									 const ResultProxy<TpValue, TpErrorPredicate> & pRhs )
	{
		return pLhs.code < pRhs.code;
	}

	template <typename TpValue, typename TpErrorPredicate, typename TpIntegral>
	inline constexpr bool operator<( const ResultProxy<TpValue, TpErrorPredicate> & pLhs, TpIntegral pRhs )
	{
		return pLhs.code < pRhs;
	}

	template <typename TpValue, typename TpErrorPredicate, typename TpIntegral>
	inline constexpr bool operator<( TpIntegral pLhs, const ResultProxy<TpValue, TpErrorPredicate> & pRhs )
	{
		return pLhs < pRhs.code;
	}

	template <typename TpValue, typename TpErrorPredicate>
	inline constexpr bool operator<=( const ResultProxy<TpValue, TpErrorPredicate> & pLhs,
									  const ResultProxy<TpValue, TpErrorPredicate> & pRhs )
	{
		return pLhs.code <= pRhs.code;
	}

	template <typename TpValue, typename TpErrorPredicate, typename TpIntegral>
	inline constexpr bool operator<=( const ResultProxy<TpValue, TpErrorPredicate> & pLhs, TpIntegral pRhs )
	{
		return pLhs.code <= pRhs;
	}

	template <typename TpValue, typename TpErrorPredicate, typename TpIntegral>
	inline constexpr bool operator<=( TpIntegral pLhs, const ResultProxy<TpValue, TpErrorPredicate> & pRhs )
	{
		return pLhs <= pRhs.code;
	}

	template <typename TpValue, typename TpErrorPredicate>
	inline constexpr bool operator>( const ResultProxy<TpValue, TpErrorPredicate> & pLhs,
									 const ResultProxy<TpValue, TpErrorPredicate> & pRhs )
	{
		return pLhs.code > pRhs.code;
	}

	template <typename TpValue, typename TpErrorPredicate, typename TpIntegral>
	inline constexpr bool operator>( const ResultProxy<TpValue, TpErrorPredicate> & pLhs, TpIntegral pRhs )
	{
		return pLhs.code > pRhs;
	}

	template <typename TpValue, typename TpErrorPredicate, typename TpIntegral>
	inline constexpr bool operator>( TpIntegral pLhs, const ResultProxy<TpValue, TpErrorPredicate> & pRhs )
	{
		return pLhs > pRhs.code;
	}

	template <typename TpValue, typename TpErrorPredicate>
	inline constexpr bool operator>=( const ResultProxy<TpValue, TpErrorPredicate> & pLhs,
									  const ResultProxy<TpValue, TpErrorPredicate> & pRhs )
	{
		return pLhs.code >= pRhs.code;
	}

	template <typename TpValue, typename TpErrorPredicate, typename TpIntegral>
	inline constexpr bool operator>=( const ResultProxy<TpValue, TpErrorPredicate> & pLhs, TpIntegral pRhs )
	{
		return pLhs.code >= pRhs;
	}

	template <typename TpValue, typename TpErrorPredicate, typename TpIntegral>
	inline constexpr bool operator>=( TpIntegral pLhs, const ResultProxy<TpValue, TpErrorPredicate> & pRhs )
	{
		return pLhs >= pRhs.code;
	}

}
