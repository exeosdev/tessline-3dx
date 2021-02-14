
#ifndef __TS3_STDEXT_STRING_UTILS_H__
#define __TS3_STDEXT_STRING_UTILS_H__

#include "string_ex.h"
#include <cctype>
#include <cwctype>
#include <functional>

namespace ts3
{

	/// @brief
	template <typename _Char>
	struct CharConv;

	template <>
	struct CharConv<char>
	{
		static int toLower( char pChar )
		{
			return std::tolower( pChar );
		}

		static int toUpper( char pChar )
		{
			return std::toupper( pChar );
		}
	};

	template <>
	struct CharConv<wchar_t>
	{
		static wint_t toLower( wchar_t pChar )
		{
			return std::towlower( pChar );
		}

		static wint_t toUpper( wchar_t pChar )
		{
			return std::towupper( pChar );
		}
	};


	/// @brief
	template <typename _Char>
	struct StringConv;
	
	template <>
	struct StringConv<char>
	{
		template <class _Tp>
        std::basic_string<char> toString( const _Tp & pValue )
		{
			return std::to_string( pValue );
		}
	};
	
	template <>
	struct StringConv<wchar_t>
	{
		template <class _Tp>
        std::basic_string<wchar_t> toString( const _Tp & pValue )
		{
			return std::to_wstring( pValue );
		}
	};


	/// @brief
	template <typename _Ret>
	struct StringParse;
	
	template <>
	struct StringParse<int32_t>
	{
		static std::pair<int32_t, char *> fromString( const char * pInputStr, numeric_base pBase = numeric_base::decimal )
		{
			char * endPtr = 0;
			int32_t value = std::strtol( pInputStr, &endPtr, static_cast< int >( pBase ) );
			return std::pair<int32_t, char *>( value, endPtr );
		}
	
		static std::pair<int32_t, wchar_t *> fromString( const wchar_t * pInputStr, numeric_base pBase = numeric_base::decimal )
		{
			wchar_t * endPtr = 0;
			int32_t value = std::wcstol( pInputStr, &endPtr, static_cast< int >( pBase ) );
			return std::pair<int32_t, wchar_t *>( value, endPtr );
		}
	
		template <typename _Char>
		static std::pair<int32_t, size_t> fromString( const std::basic_string<_Char> & pInputStr, numeric_base pBase = numeric_base::decimal )
		{
			size_t processedNum = 0;
			int32_t value = std::stol( pInputStr, &processedNum, static_cast< int >( pBase ) );
			return std::pair<int32_t, size_t>( value, processedNum );
		}
	};
	
	template <>
	struct StringParse<uint32_t>
	{
		static std::pair<uint32_t, char *> fromString( const char * pInputStr, numeric_base pBase = numeric_base::decimal )
		{
			char * endPtr = 0;
			uint32_t value = std::strtoul( pInputStr, &endPtr, static_cast< int >( pBase ) );
			return std::pair<uint32_t, char *>( value, endPtr );
		}
	
		static std::pair<uint32_t, wchar_t *> fromString( const wchar_t * pInputStr, numeric_base pBase = numeric_base::decimal )
		{
			wchar_t * endPtr = 0;
			uint32_t value = std::wcstoul( pInputStr, &endPtr, static_cast< int >( pBase ) );
			return std::pair<uint32_t, wchar_t *>( value, endPtr );
		}
	
		template <typename _Char>
		static std::pair<uint32_t, size_t> fromString( const std::basic_string<_Char> & pInputStr, numeric_base pBase = numeric_base::decimal )
		{
			size_t processedNum = 0;
			uint32_t value = std::stoul( pInputStr, &processedNum, static_cast< int >( pBase ) );
			return std::pair<uint32_t, size_t>( value, processedNum );
		}
	};
	
	template <>
	struct StringParse<int64_t>
	{
		static std::pair<int64_t, char *> fromString( const char * pInputStr, numeric_base pBase = numeric_base::decimal )
		{
			char * endPtr = 0;
			int64_t value = std::strtoll( pInputStr, &endPtr, static_cast< int >( pBase ) );
			return std::pair<int64_t, char *>( value, endPtr );
		}
	
		static std::pair<int64_t, wchar_t *> fromString( const wchar_t * pInputStr, numeric_base pBase = numeric_base::decimal )
		{
			wchar_t * endPtr = 0;
			int64_t value = std::wcstoll( pInputStr, &endPtr, static_cast< int >( pBase ) );
			return std::pair<int64_t, wchar_t *>( value, endPtr );
		}
	
		template <typename _Char>
		static std::pair<int64_t, size_t> fromString( const std::basic_string<_Char> & pInputStr, numeric_base pBase = numeric_base::decimal )
		{
			size_t processedNum = 0;
			int64_t value = std::stoll( pInputStr, &processedNum, static_cast< int >( pBase ) );
			return std::pair<int64_t, size_t>( value, processedNum );
		}
	};
	
	template <>
	struct StringParse<uint64_t>
	{
		static std::pair<uint64_t, char *> fromString( const char * pInputStr, numeric_base pBase = numeric_base::decimal )
		{
			char * endPtr = 0;
			uint64_t value = std::strtoull( pInputStr, &endPtr, static_cast< int >( pBase ) );
			return std::pair<uint64_t, char *>( value, endPtr );
		}
	
		static std::pair<uint64_t, wchar_t *> fromString( const wchar_t * pInputStr, numeric_base pBase = numeric_base::decimal )
		{
			wchar_t * endPtr = 0;
			uint64_t value = std::wcstoull( pInputStr, &endPtr, static_cast< int >( pBase ) );
			return std::pair<uint64_t, wchar_t *>( value, endPtr );
		}
	
		template <typename _Char>
		static std::pair<uint64_t, size_t> fromString( const std::basic_string<_Char> & pInputStr, numeric_base pBase = numeric_base::decimal )
		{
			size_t processedNum = 0;
			uint64_t value = std::stoull( pInputStr, &processedNum, static_cast< int >( pBase ) );
			return std::pair<uint64_t, size_t>( value, processedNum );
		}
	};
	
	template <>
	struct StringParse<float>
	{
		static std::pair<float, char *> fromString( const char * pInputStr )
		{
			char * endPtr = 0;
			float value = std::strtof( pInputStr, &endPtr );
			return std::pair<float, char *>( value, endPtr );
		}
	
		static std::pair<float, wchar_t *> fromString( const wchar_t * pInputStr )
		{
			wchar_t * endPtr = 0;
			float value = std::wcstof( pInputStr, &endPtr );
			return std::pair<float, wchar_t *>( value, endPtr );
		}
	
		template <typename _Char>
		static std::pair<float, size_t> fromString( const std::basic_string<_Char> & pInputStr )
		{
			size_t processedNum = 0;
			float value = std::stof( pInputStr, &processedNum );
			return std::pair<float, size_t>( value, processedNum );
		}
	};
	
	template <>
	struct StringParse<double>
	{
		static std::pair<double, char *> fromString( const char * pInputStr )
		{
			char * endPtr = 0;
			double value = std::strtod( pInputStr, &endPtr );
			return std::pair<double, char *>( value, endPtr );
		}
	
		static std::pair<double, wchar_t *> fromString( const wchar_t * pInputStr )
		{
			wchar_t * endPtr = 0;
			double value = std::wcstod( pInputStr, &endPtr );
			return std::pair<double, wchar_t *>( value, endPtr );
		}
	
		template <typename _Char>
		static std::pair<double, size_t> fromString( const std::basic_string<_Char> & pInputStr )
		{
			size_t processedNum = 0;
			double value = std::stod( pInputStr, &processedNum );
			return std::pair<double, size_t>( value, processedNum );
		}
	};
	
	template <>
	struct StringParse<long double>
	{
		static std::pair<long double, char *> fromString( const char * pInputStr )
		{
			char * endPtr = 0;
			long double value = std::strtold( pInputStr, &endPtr );
			return std::pair<long double, char *>( value, endPtr );
		}
	
		static std::pair<long double, wchar_t *> fromString( const wchar_t * pInputStr )
		{
			wchar_t * endPtr = 0;
			long double value = std::wcstold( pInputStr, &endPtr );
			return std::pair<long double, wchar_t *>( value, endPtr );
		}
	
		template <typename _Char>
		static std::pair<long double, size_t> fromString( const std::basic_string<_Char> & pInputStr )
		{
			size_t processedNum = 0;
			long double value = std::stold( pInputStr, &processedNum );
			return std::pair<long double, size_t>( value, processedNum );
		}
	};


	/// @brief
	template <typename _Char, typename _Tp>
	std::basic_string<_Char> toString( const _Tp & pValue )
	{
		return string_conv<_Char>::to_string( pValue );
	}

	/// @brief
	template <typename _Tp, typename _Char>
	std::pair<_Tp, _Char *> fromString( const _Char * pInputStr )
	{
		return string_parse<_Tp>::from_string( pInputStr );
	}

	/// @brief
	template <typename _Tp, typename _Char>
	std::pair<_Tp, size_t> fromString( const std::basic_string<_Char> & pInputStr )
	{
		return string_parse<_Tp>::from_string( pInputStr );
	}


	namespace strutils
	{

		template <typename _Char>
		inline void make_lower_transform( std::basic_string<_Char> & pInput )
		{
			std::transform( pInput.begin(), pInput.end(), pInput.begin(), CharConv<_Char>::toLower );
		}

		template <typename _Char>
		inline void make_upper_transform( std::basic_string<_Char> & pInput )
		{
			std::transform( pInput.begin(), pInput.end(), pInput.begin(), CharConv<_Char>::toUpper );
		}

		template <typename _Char>
		inline std::basic_string<_Char> make_lower( const std::basic_string<_Char> & pInput )
		{
			std::basic_string<_Char> result = pInput;
			make_lower_transform( result );
			return result;
		}

		template <typename _Char>
		inline std::basic_string<_Char> make_upper( const std::basic_string<_Char> & pInput )
		{
			std::basic_string<_Char> result = pInput;
			make_upper_transform( result );
			return result;
		}

		template <typename _Char, class _Apred>
		inline size_t split_string( const _Char * pInputStr, size_t pInputStrLength, _Char pSeparator, _Apred pAppendPredicate )
		{
			size_t result = 0;

			for ( const _Char * str_end = pInputStr + pInputStrLength; pInputStr < str_end; )
			{
				const _Char * token_ptr = std::char_traits<_Char>::find( pInputStr, pInputStrLength, pSeparator );
				if ( token_ptr == nullptr )
				{
					pAppendPredicate( pInputStr, str_end - pInputStr );
					break;
				}

				size_t substr_len = token_ptr - pInputStr;

				pAppendPredicate( pInputStr, substr_len );
				++result;

				pInputStr = token_ptr + 1;
				pInputStrLength -= ( substr_len + 1 );
			}

			return result;
		}

		template <typename _Char, class _Apred>
		inline size_t split_string( const _Char * pInputStr, _Char pSeparator, _Apred pAppendPredicate )
		{
			size_t input_str_length = std::char_traits<_Char>::length( pInputStr );
			return split_string( pInputStr, input_str_length, pSeparator, pAppendPredicate );
		}

		template <typename _Char, class _Apred>
		inline size_t split_string( const std::basic_string<_Char> & pInputStr, _Char pSeparator, _Apred pAppendPredicate )
		{
			return split_string( pInputStr.data(), pInputStr.length(), pSeparator, pAppendPredicate );
		}

		template <typename _Res, typename _Char, class _Apred>
		inline _Res split_string_ex( const _Char * pInputStr, size_t pInputStrLength, _Char pSeparator, _Apred pAppendPredicate )
		{
			_Res result{};
			auto append_predicate_res = std::bind( pAppendPredicate, std::ref( result ), std::placeholders::_1, std::placeholders::_2 );
			split_string( pInputStr, pInputStrLength, pSeparator, append_predicate_res );
			return result;
		}

		template <typename _Res, typename _Char, class _Apred>
		inline _Res split_string_ex( const _Char * pInputStr, _Char pSeparator, _Apred pAppendPredicate )
		{
			size_t input_str_length = std::char_traits<_Char>::length( pInputStr );
			return split_string_ex<_Res>( pInputStr, input_str_length, pSeparator, pAppendPredicate );
		}

		template <typename _Res, typename _Char, class _Apred>
		inline _Res split_string_ex( const std::basic_string<_Char> & pInputStr, _Char pSeparator, _Apred pAppendPredicate )
		{
			return split_string_ex<_Res>( pInputStr.data(), pInputStr.length(), pSeparator, pAppendPredicate );
		}

		template <typename _Char>
		inline std::basic_string<_Char> extract_short_file_path( const std::basic_string<_Char> & pFilename, _Char pPathSeparator )
		{
			std::basic_string<_Char> short_filename = pFilename;
			size_t filename_separator = short_filename.find_last_of( pPathSeparator, 0 );

			if ( filename_separator != std::basic_string<_Char>::npos )
			{
				size_t last_dir_separator = short_filename.find_last_of( pPathSeparator, filename_separator );
				if ( last_dir_separator != std::basic_string<_Char>::npos )
				{
					short_filename.erase( 0, last_dir_separator + 1 );
				}
			}

			return short_filename;
		}

		template <typename _Char>
		inline std::basic_string<_Char> extract_short_file_path( const _Char * pFilename, _Char pPathSeparator )
		{
			return extract_short_file_path( std::basic_string<_Char>{pFilename}, pPathSeparator );
		}

	}

}

#endif /* __TS3_STDEXT_STRING_UTILS_H__ */
