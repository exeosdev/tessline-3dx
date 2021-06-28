
#pragma once

namespace ts3
{

	/// @brief
	using result_code_value_t = enum_default_value_t;

	/// @brief
	enum class ResultType : uint8
	{
		Success,
		Warning,
		Error
	};

#if ( 1 )

	/// @brief
	struct Result
	{
	public:
		// Value of the result code. It should always be a valid code, created with ecDeclareResultCode() function.
		result_code_value_t code = 0U;

		// A string identifying the result code. Usually - name of the code used across sources.
		const char * codeStr;

		// Additional info string, optional.
		const char * infoStr;

	public:
		Result( const Result & ) = default;
		Result & operator=( const Result & ) = default;

		/// @brief Trivial default constructor.
		Result() = default;

		/// @brief Explicit constructor which initialises result object with a specified result code.
		/// @param code Result code.
		explicit Result( result_code_value_t pCode )
		: code( pCode )
		, codeStr( cxEmptyCstr )
		, infoStr( cxEmptyCstr )
		{}

		/// @brief Initializes result object with specified content.
		/// @param code Result code.
		/// @param codeStr Code string (text representation of the code, for example).
		/// @param infoStr Additional info string.
		Result( result_code_value_t pCode, const char * pCodeStr, const char * pInfoStr = cxEmptyCstr )
		: code( pCode )
		, codeStr( ( pCodeStr != nullptr ) ? pCodeStr : cxEmptyCstr )
		, infoStr( ( pInfoStr != nullptr ) ? pInfoStr : cxEmptyCstr )
		{}

		/// @brief Swaps two results.
		/// @param other Result object to swap with.
		void swap( Result & pOther )
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

	/// @brief Creates result from code.
	#define ts3MakeResult( pCode ) ::ts3::Result( pCode, #pCode )

	/// @brief Creates result from code and additional info string.
	#define ts3MakeResultEx( pCode, pInfo ) ::ts3::Result( pCode, #pCode, pInfo )

	/// @brief
	#define ts3ResultGetCode( pResult ) ( pResult.code )

	inline constexpr bool operator==( const Result & pLhs, const Result & pRhs )
	{
		return pLhs.code == pRhs.code;
	}

	template <typename TpEnum>
	inline constexpr bool operator==( const Result & pLhs, TpEnum pRhs )
	{
		return pLhs.code == pRhs;
	}

	template <typename TpEnum>
	inline constexpr bool operator==( TpEnum pLhs, const Result & pRhs )
	{
		return pLhs == pRhs.code;
	}

	inline constexpr bool operator!=( const Result & pLhs, const Result & pRhs )
	{
		return pLhs.code != pRhs.code;
	}

	template <typename TpEnum>
	inline constexpr bool operator!=( const Result & pLhs, TpEnum pRhs )
	{
		return pLhs.code != pRhs;
	}

	template <typename TpEnum>
	inline constexpr bool operator!=( TpEnum pLhs, const Result & pRhs )
	{
		return pLhs != pRhs.code;
	}

#else

	/// @brief Result code.
	using Result = result_code_value_t;

	/// @brief Creates result from code.
	#define ts3MakeResult( pCode ) ( pCode )

	/// @brief Creates result from code and additional info string.
	#define ts3MakeResultEx( pCode, pInfo ) ( pCode )

	/// @brief
	#define ts3ResultGetCode( pResult ) ( pResult )

#endif

	constexpr result_code_value_t cxResultCodeControlKey   = 0x18000000;
	constexpr result_code_value_t cxResultCodeTypeMask     = 0x00FF0000;
	constexpr result_code_value_t cxResultCodeCategoryMask = 0x0000FF00;
	constexpr result_code_value_t cxResultCodeIIDMask      = 0x000000FF;

	///
	inline constexpr result_code_value_t ecDeclareResultCode( ResultType pType, uint8 pCategory, uint8 pInternalID )
	{
		return ( cxResultCodeControlKey | ( ( result_code_value_t )( pType ) << 16 ) | ( ( result_code_value_t )( pCategory ) << 8 ) | pInternalID );
	}

	///
	inline constexpr ResultType ecGetResultCodeType( result_code_value_t pResultCode )
	{
		return ( ResultType )( ( pResultCode & cxResultCodeTypeMask ) >> 16 );
	}

	///
	inline constexpr uint8 ecGetResultCodeCategory( result_code_value_t pResultCode )
	{
		return ( uint8 )( ( pResultCode & cxResultCodeCategoryMask ) >> 8 );
	}

	///
	inline constexpr bool ecValidateResultCode( result_code_value_t pResultCode )
	{
		return ( pResultCode & cxResultCodeControlKey ) == cxResultCodeControlKey;
	}

	/// @brief
	#define ts3ResultIsError( pResult ) ( ecGetResultCodeType( ts3ResultGetCode( pResult ) ) == ResultType::Error )

}
