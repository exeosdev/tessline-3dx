
#ifndef __TS3_STDEXT_STRING_EXT_H__
#define __TS3_STDEXT_STRING_EXT_H__

#include "prerequisites.h"
#include <string>

namespace stdx
{

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


	template <typename _Char, typename _Tp>
	std::basic_string<_Char> to_string( const _Tp & pValue )
	{
		return string_conv<_Char>::to_string( pValue );
	}

	template <typename _Tp, typename _Char>
	std::pair<_Tp, _Char *> from_string( const _Char * pInputStr )
	{
		return string_parse<_Tp>::from_string( pInputStr );
	}

	template <typename _Tp, typename _Char>
	std::pair<_Tp, size_t> from_string( const std::basic_string<_Char> & pInputStr )
	{
		return string_parse<_Tp>::from_string( pInputStr );
	}

}

#endif /* __TS3_STDEXT_STRING_EXT_H__ */
