
#ifndef __TS3_STDEXT_STRING_EXT_H__
#define __TS3_STDEXT_STRING_EXT_H__

#include "arrayView.h"
#include <string>

namespace ts3
{

    enum class IntegerBase : uint32
    {
        Auto = 0,
        Decimal = 10,
        Octal = 8,
        Hexadecimal = 16
    };

	/// @brief Creates ArrayView that wraps an std::basic_string object
	/// @tparam _Char Type of the string's internal characters.
	/// @param pString A string object to be mapped to a view.
	/// @return ArrayView referencing specified string object.
	template <typename _Char>
	inline ArrayView<_Char> bindArrayView( std::basic_string<_Char> & pString )
	{
		return ArrayView<_Char>( &( pString[0] ), pString.length() );
	}

	/// @brief Creates ArrayView that wraps an std::basic_string object
	/// @tparam _Char Type of the string's internal characters.
	/// @param pString A string object to be mapped to a view.
	/// @return ArrayView referencing specified string object.
	template <typename _Char>
	inline ArrayView<const _Char> bindArrayView( const std::basic_string<_Char> & pString )
	{
		return ArrayView<const _Char>( &( pString[0] ), pString.length() );
	}


	/// @brief
	template <typename _Char>
	struct ToStringProxy;

	template <>
	struct ToStringProxy<char>
    {
	    template <class _Tp>
        std::basic_string<char> toString( const _Tp & pValue )
        {
            return std::to_string( pValue );
        }
    };

	template <>
	struct ToStringProxy<wchar_t>
    {
	    template <class _Tp>
        std::basic_string<wchar_t> toString( const _Tp & pValue )
        {
            return std::to_wstring( pValue );
        }
    };


	/// @brief
	template <typename _Ret>
	struct FromStringProxy;

	template <>
	struct FromStringProxy<int32_t>
    {
	    static std::pair<int32_t, char *> fromString( const char * pInputStr, IntegerBase pBase = IntegerBase::Decimal )
	    {
	        char * endPtr = 0;
	        int32_t value = std::strtol( pInputStr, &endPtr, static_cast< int >( pBase ) );
	        return std::pair<int32_t, char *>( value, endPtr );
	    }

	    static std::pair<int32_t, wchar_t *> fromString( const wchar_t * pInputStr, IntegerBase pBase = IntegerBase::Decimal )
	    {
	        wchar_t * endPtr = 0;
	        int32_t value = std::wcstol( pInputStr, &endPtr, static_cast< int >( pBase ) );
	        return std::pair<int32_t, wchar_t *>( value, endPtr );
	    }

	    template <typename _Char>
	    static std::pair<int32_t, size_t> fromString( const std::basic_string<_Char> & pInputStr, IntegerBase pBase = IntegerBase::Decimal )
	    {
	        size_t processedNum = 0;
	        int32_t value = std::stol( pInputStr, &processedNum, static_cast< int >( pBase ) );
	        return std::pair<int32_t, size_t>( value, processedNum );
	    }
    };

	template <>
	struct FromStringProxy<uint32_t>
    {
	    static std::pair<uint32_t, char *> fromString( const char * pInputStr, IntegerBase pBase = IntegerBase::Decimal )
	    {
	        char * endPtr = 0;
	        uint32_t value = std::strtoul( pInputStr, &endPtr, static_cast< int >( pBase ) );
	        return std::pair<uint32_t, char *>( value, endPtr );
	    }

	    static std::pair<uint32_t, wchar_t *> fromString( const wchar_t * pInputStr, IntegerBase pBase = IntegerBase::Decimal )
	    {
	        wchar_t * endPtr = 0;
	        uint32_t value = std::wcstoul( pInputStr, &endPtr, static_cast< int >( pBase ) );
	        return std::pair<uint32_t, wchar_t *>( value, endPtr );
	    }

	    template <typename _Char>
	    static std::pair<uint32_t, size_t> fromString( const std::basic_string<_Char> & pInputStr, IntegerBase pBase = IntegerBase::Decimal )
	    {
	        size_t processedNum = 0;
	        uint32_t value = std::stoul( pInputStr, &processedNum, static_cast< int >( pBase ) );
	        return std::pair<uint32_t, size_t>( value, processedNum );
	    }
    };

	template <>
	struct FromStringProxy<int64_t>
    {
	    static std::pair<int64_t, char *> fromString( const char * pInputStr, IntegerBase pBase = IntegerBase::Decimal )
	    {
	        char * endPtr = 0;
	        int64_t value = std::strtoll( pInputStr, &endPtr, static_cast< int >( pBase ) );
	        return std::pair<int64_t, char *>( value, endPtr );
	    }

	    static std::pair<int64_t, wchar_t *> fromString( const wchar_t * pInputStr, IntegerBase pBase = IntegerBase::Decimal )
	    {
	        wchar_t * endPtr = 0;
	        int64_t value = std::wcstoll( pInputStr, &endPtr, static_cast< int >( pBase ) );
	        return std::pair<int64_t, wchar_t *>( value, endPtr );
	    }

	    template <typename _Char>
	    static std::pair<int64_t, size_t> fromString( const std::basic_string<_Char> & pInputStr, IntegerBase pBase = IntegerBase::Decimal )
	    {
	        size_t processedNum = 0;
	        int64_t value = std::stoll( pInputStr, &processedNum, static_cast< int >( pBase ) );
	        return std::pair<int64_t, size_t>( value, processedNum );
	    }
    };

	template <>
	struct FromStringProxy<uint64_t>
    {
	    static std::pair<uint64_t, char *> fromString( const char * pInputStr, IntegerBase pBase = IntegerBase::Decimal )
	    {
	        char * endPtr = 0;
	        uint64_t value = std::strtoull( pInputStr, &endPtr, static_cast< int >( pBase ) );
	        return std::pair<uint64_t, char *>( value, endPtr );
	    }

	    static std::pair<uint64_t, wchar_t *> fromString( const wchar_t * pInputStr, IntegerBase pBase = IntegerBase::Decimal )
	    {
	        wchar_t * endPtr = 0;
	        uint64_t value = std::wcstoull( pInputStr, &endPtr, static_cast< int >( pBase ) );
	        return std::pair<uint64_t, wchar_t *>( value, endPtr );
	    }

	    template <typename _Char>
	    static std::pair<uint64_t, size_t> fromString( const std::basic_string<_Char> & pInputStr, IntegerBase pBase = IntegerBase::Decimal )
	    {
	        size_t processedNum = 0;
	        uint64_t value = std::stoull( pInputStr, &processedNum, static_cast< int >( pBase ) );
	        return std::pair<uint64_t, size_t>( value, processedNum );
	    }
    };

	template <>
	struct FromStringProxy<float>
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
	struct FromStringProxy<double>
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
	struct FromStringProxy<long double>
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
	    return ToStringProxy<_Char>::toString( pValue );
	}

	/// @brief
	template <typename _Tp, typename _Char>
	std::pair<_Tp, _Char *> fromString( const _Char * pInputStr )
	{
	    return FromStringProxy<_Tp>::fromString( pInputStr );
	}

	/// @brief
	template <typename _Tp, typename _Char>
	std::pair<_Tp, _Char *> fromString( const _Char * pInputStr, IntegerBase pBase )
	{
	    return FromStringProxy<_Tp>::fromString( pInputStr, pBase );
	}

	/// @brief
	template <typename _Tp, typename _Char>
	std::pair<_Tp, size_t> fromString( const std::basic_string<_Char> & pInputStr )
	{
	    return FromStringProxy<_Tp>::fromString( pInputStr );
	}

	/// @brief
	template <typename _Tp, typename _Char>
	std::pair<_Tp, size_t> fromString( const std::basic_string<_Char> & pInputStr, IntegerBase pBase )
	{
	    return FromStringProxy<_Tp>::fromString( pInputStr, pBase );
	}

}

#endif /* __TS3_STDEXT_STRING_EXT_H__ */
