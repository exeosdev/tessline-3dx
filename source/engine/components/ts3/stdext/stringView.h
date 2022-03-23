
#ifndef __TS3_STDEXT_STRING_VIEW_H__
#define __TS3_STDEXT_STRING_VIEW_H__

#include "stringExt.h"
#include "staticAlgo.h"
#include "typeTraits.h"

namespace ts3
{

	template <typename TpChar = char>
	struct StringView
	{
		static_assert( IsTypeOnTypeList<TpChar, char, wchar_t, char16_t, char32_t>::value, "Invalid char type for StringView" );

	public:
		using CharType = typename std::remove_cv<TpChar>::type;

		StringView()
		: _basePtr( nullptr )
		, _length( 0 )
		{}

		StringView( const CharType * pStr )
		: _basePtr( pStr )
		, _length( std::char_traits<CharType>::length( pStr ) )
		{}

		StringView( const CharType * pBegin, const CharType * pEnd )
		: _basePtr( pBegin )
		, _length( pEnd - pBegin )
		{}

		StringView( const CharType * pStr, size_t pLength )
		: _basePtr( pStr )
		, _length( pLength )
		{}

		StringView( const std::basic_string<TpChar> & pString )
		: _basePtr( pString.c_str() )
		, _length( pString.length() )
		{}

		template <typename TpOther>
		StringView( const StringView<TpOther> & pOther )
		: _basePtr( pOther.data() )
		, _length( pOther.length() )
		{}

		template <size_t tSize>
		explicit StringView( CharType( &pArray )[tSize] )
		: StringView( &( pArray[0] ), tSize )
		{}

		template <size_t tSize>
		explicit StringView( const CharType( &pArray )[tSize] )
		: StringView( &( pArray[0] ), tSize )
		{}

		explicit operator bool() const
		{
			return !empty();
		}

		TS3_FUNC_NO_DISCARD CharType * str() const
		{
			return _basePtr;
		}

		TS3_FUNC_NO_DISCARD size_t length() const
		{
			return _length;
		}

		TS3_FUNC_NO_DISCARD size_t size() const
		{
			return _length;
		}

		TS3_FUNC_NO_DISCARD size_t byteSize() const
		{
			return _length * sizeof( TpChar );
		}

		TS3_FUNC_NO_DISCARD bool empty() const
		{
			return _basePtr && ( _length > 0 );
		}

		void swap( StringView & pOther )
		{
			std::swap( _basePtr, pOther._basePtr );
			std::swap( _length, pOther._length );
		}

	private:
		const CharType * _basePtr;
		size_t _length;
	};

	template <typename TpChar>
	inline void swap( StringView<TpChar> & pFirst, StringView<TpChar> & pSecond )
	{
		pFirst.swap( pSecond );
	}

	template <typename TpChar>
	inline TpChar * begin( const StringView<TpChar> & pStringView )
	{
		return pStringView.str();
	}

	template <typename TpChar>
	inline TpChar * end( const StringView<TpChar> & pStringView )
	{
		return pStringView.str() + pStringView.length();
	}

	template <typename TpChar>
	inline bool operator==( const StringView<TpChar> & pLhs, const std::basic_string<TpChar> & pRhs )
	{
		return std::char_traits<TpChar>::compare( pLhs.str(), pRhs.c_str(), getMinOf( pLhs.length(), pRhs.length() ) ) == 0;
	}

	template <typename TpChar>
	inline bool operator==( const std::basic_string<TpChar> & pLhs, const StringView<TpChar> & pRhs )
	{
		return std::char_traits<TpChar>::compare( pLhs.c_str(), pRhs.str(), getMinOf( pLhs.length(), pRhs.length() ) ) == 0;
	}

	template <typename TpChar>
	inline bool operator!=( const StringView<TpChar> & pLhs, const std::basic_string<TpChar> & pRhs )
	{
		return std::char_traits<TpChar>::compare( pLhs.str(), pRhs.c_str(), getMinOf( pLhs.length(), pRhs.length() ) ) != 0;
	}

	template <typename TpChar>
	inline bool operator!=( const std::basic_string<TpChar> & pLhs, const StringView<TpChar> & pRhs )
	{
		return std::char_traits<TpChar>::compare( pLhs.c_str(), pRhs.str(), getMinOf( pLhs.length(), pRhs.length() ) ) != 0;
	}

	template <typename TpChar>
	inline bool operator<( const StringView<TpChar> & pLhs, const std::basic_string<TpChar> & pRhs )
	{
		return std::char_traits<TpChar>::compare( pLhs.str(), pRhs.c_str(), getMinOf( pLhs.length(), pRhs.length() ) ) < 0;
	}

	template <typename TpChar>
	inline bool operator<( const std::basic_string<TpChar> & pLhs, const StringView<TpChar> & pRhs )
	{
		return std::char_traits<TpChar>::compare( pLhs.c_str(), pRhs.str(), getMinOf( pLhs.length(), pRhs.length() ) ) < 0;
	}

	template <typename TpChar>
	inline bool operator<=( const StringView<TpChar> & pLhs, const std::basic_string<TpChar> & pRhs )
	{
		return std::char_traits<TpChar>::compare( pLhs.str(), pRhs.c_str(), getMinOf( pLhs.length(), pRhs.length() ) ) <= 0;
	}

	template <typename TpChar>
	inline bool operator<=( const std::basic_string<TpChar> & pLhs, const StringView<TpChar> & pRhs )
	{
		return std::char_traits<TpChar>::compare( pLhs.c_str(), pRhs.str(), getMinOf( pLhs.length(), pRhs.length() ) ) <= 0;
	}

	template <typename TpChar>
	inline bool operator>( const StringView<TpChar> & pLhs, const std::basic_string<TpChar> & pRhs )
	{
		return std::char_traits<TpChar>::compare( pLhs.str(), pRhs.c_str(), getMinOf( pLhs.length(), pRhs.length() ) ) > 0;
	}

	template <typename TpChar>
	inline bool operator>( const std::basic_string<TpChar> & pLhs, const StringView<TpChar> & pRhs )
	{
		return std::char_traits<TpChar>::compare( pLhs.c_str(), pRhs.str(), getMinOf( pLhs.length(), pRhs.length() ) ) > 0;
	}

	template <typename TpChar>
	inline bool operator>=( const StringView<TpChar> & pLhs, const std::basic_string<TpChar> & pRhs )
	{
		return std::char_traits<TpChar>::compare( pLhs.str(), pRhs.c_str(), getMinOf( pLhs.length(), pRhs.length() ) ) >= 0;
	}

	template <typename TpChar>
	inline bool operator>=( const std::basic_string<TpChar> & pLhs, const StringView<TpChar> & pRhs )
	{
		return std::char_traits<TpChar>::compare( pLhs.c_str(), pRhs.str(), getMinOf( pLhs.length(), pRhs.length() ) ) >= 0;
	}

}

#endif /* __TS3_STDEXT_STRING_VIEW_H__ */
