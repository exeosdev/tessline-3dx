
#ifndef __TS3_STDEXT_STRING_VIEW_H__
#define __TS3_STDEXT_STRING_VIEW_H__

#include "stringExt.h"
#include "staticAlgo.h"
#include "typeTraits.h"

namespace ts3
{

	template <typename TChar = char>
	struct StringView
	{
		static_assert( IsTypeOnTypeList<TChar, char, wchar_t, char16_t, char32_t>::value, "Invalid char type for StringView" );

	public:
		using CharType = typename std::remove_cv<TChar>::type;

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

		StringView( const std::basic_string<TChar> & pString )
		: _basePtr( pString.c_str() )
		, _length( pString.length() )
		{}

		template <typename TOther>
		StringView( const StringView<TOther> & pOther )
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

		TS3_ATTR_NO_DISCARD const CharType * str() const
		{
			return _basePtr;
		}

		TS3_ATTR_NO_DISCARD size_t length() const
		{
			return _length;
		}

		TS3_ATTR_NO_DISCARD size_t size() const
		{
			return _length;
		}

		TS3_ATTR_NO_DISCARD size_t byteSize() const
		{
			return _length * sizeof( TChar );
		}

		TS3_ATTR_NO_DISCARD bool empty() const
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

	template <typename TChar>
	inline void swap( StringView<TChar> & pFirst, StringView<TChar> & pSecond )
	{
		pFirst.swap( pSecond );
	}

	template <typename TChar>
	inline TChar * begin( const StringView<TChar> & pStringView )
	{
		return pStringView.str();
	}

	template <typename TChar>
	inline TChar * end( const StringView<TChar> & pStringView )
	{
		return pStringView.str() + pStringView.length();
	}

	template <typename TChar>
	inline bool operator==( const StringView<TChar> & pLhs, const StringView<TChar> & pRhs )
	{
		return std::char_traits<TChar>::compare( pLhs.str(), pRhs.str(), getMinOf( pLhs.length(), pRhs.length() ) ) == 0;
	}

	template <typename TChar>
	inline bool operator==( const StringView<TChar> & pLhs, const std::basic_string<TChar> & pRhs )
	{
		return std::char_traits<TChar>::compare( pLhs.str(), pRhs.c_str(), getMinOf( pLhs.length(), pRhs.length() ) ) == 0;
	}

	template <typename TChar>
	inline bool operator==( const std::basic_string<TChar> & pLhs, const StringView<TChar> & pRhs )
	{
		return std::char_traits<TChar>::compare( pLhs.c_str(), pRhs.str(), getMinOf( pLhs.length(), pRhs.length() ) ) == 0;
	}

	template <typename TChar>
	inline bool operator!=( const StringView<TChar> & pLhs, const StringView<TChar> & pRhs )
	{
		return std::char_traits<TChar>::compare( pLhs.str(), pRhs.str(), getMinOf( pLhs.length(), pRhs.length() ) ) != 0;
	}

	template <typename TChar>
	inline bool operator!=( const StringView<TChar> & pLhs, const std::basic_string<TChar> & pRhs )
	{
		return std::char_traits<TChar>::compare( pLhs.str(), pRhs.c_str(), getMinOf( pLhs.length(), pRhs.length() ) ) != 0;
	}

	template <typename TChar>
	inline bool operator!=( const std::basic_string<TChar> & pLhs, const StringView<TChar> & pRhs )
	{
		return std::char_traits<TChar>::compare( pLhs.c_str(), pRhs.str(), getMinOf( pLhs.length(), pRhs.length() ) ) != 0;
	}

	template <typename TChar>
	inline bool operator<( const StringView<TChar> & pLhs, const StringView<TChar> & pRhs )
	{
		return std::char_traits<TChar>::compare( pLhs.str(), pRhs.str(), getMinOf( pLhs.length(), pRhs.length() ) ) < 0;
	}

	template <typename TChar>
	inline bool operator<( const StringView<TChar> & pLhs, const std::basic_string<TChar> & pRhs )
	{
		return std::char_traits<TChar>::compare( pLhs.str(), pRhs.c_str(), getMinOf( pLhs.length(), pRhs.length() ) ) < 0;
	}

	template <typename TChar>
	inline bool operator<( const std::basic_string<TChar> & pLhs, const StringView<TChar> & pRhs )
	{
		return std::char_traits<TChar>::compare( pLhs.c_str(), pRhs.str(), getMinOf( pLhs.length(), pRhs.length() ) ) < 0;
	}

	template <typename TChar>
	inline bool operator<=( const StringView<TChar> & pLhs, const StringView<TChar> & pRhs )
	{
		return std::char_traits<TChar>::compare( pLhs.str(), pRhs.str(), getMinOf( pLhs.length(), pRhs.length() ) ) <= 0;
	}

	template <typename TChar>
	inline bool operator<=( const StringView<TChar> & pLhs, const std::basic_string<TChar> & pRhs )
	{
		return std::char_traits<TChar>::compare( pLhs.str(), pRhs.c_str(), getMinOf( pLhs.length(), pRhs.length() ) ) <= 0;
	}

	template <typename TChar>
	inline bool operator<=( const std::basic_string<TChar> & pLhs, const StringView<TChar> & pRhs )
	{
		return std::char_traits<TChar>::compare( pLhs.c_str(), pRhs.str(), getMinOf( pLhs.length(), pRhs.length() ) ) <= 0;
	}

	template <typename TChar>
	inline bool operator>( const StringView<TChar> & pLhs, const StringView<TChar> & pRhs )
	{
		return std::char_traits<TChar>::compare( pLhs.str(), pRhs.str(), getMinOf( pLhs.length(), pRhs.length() ) ) > 0;
	}

	template <typename TChar>
	inline bool operator>( const StringView<TChar> & pLhs, const std::basic_string<TChar> & pRhs )
	{
		return std::char_traits<TChar>::compare( pLhs.str(), pRhs.c_str(), getMinOf( pLhs.length(), pRhs.length() ) ) > 0;
	}

	template <typename TChar>
	inline bool operator>( const std::basic_string<TChar> & pLhs, const StringView<TChar> & pRhs )
	{
		return std::char_traits<TChar>::compare( pLhs.c_str(), pRhs.str(), getMinOf( pLhs.length(), pRhs.length() ) ) > 0;
	}

	template <typename TChar>
	inline bool operator>=( const StringView<TChar> & pLhs, const StringView<TChar> & pRhs )
	{
		return std::char_traits<TChar>::compare( pLhs.str(), pRhs.str(), getMinOf( pLhs.length(), pRhs.length() ) ) >= 0;
	}

	template <typename TChar>
	inline bool operator>=( const StringView<TChar> & pLhs, const std::basic_string<TChar> & pRhs )
	{
		return std::char_traits<TChar>::compare( pLhs.str(), pRhs.c_str(), getMinOf( pLhs.length(), pRhs.length() ) ) >= 0;
	}

	template <typename TChar>
	inline bool operator>=( const std::basic_string<TChar> & pLhs, const StringView<TChar> & pRhs )
	{
		return std::char_traits<TChar>::compare( pLhs.c_str(), pRhs.str(), getMinOf( pLhs.length(), pRhs.length() ) ) >= 0;
	}


	template <typename TChar>
	struct StringViewCmpEqual
	{
		using is_transparent = void;

		bool operator()( const StringView<TChar> & pLhs, const StringView<TChar> & pRhs ) const
		{
			return pLhs == pRhs;
		}

		bool operator()( const StringView<TChar> & pLhs, const std::basic_string<TChar> & pRhs ) const
		{
			return pLhs == pRhs;
		}

		bool operator()( const std::basic_string<TChar> & pLhs, const StringView<TChar> & pRhs ) const
		{
			return pLhs == pRhs;
		}

		bool operator()( const std::basic_string<TChar> & pLhs, const std::basic_string<TChar> & pRhs ) const
		{
			return pLhs == pRhs;
		}
	};

	template <typename TChar>
	struct StringViewCmpNotEqual
	{
		using is_transparent = void;

		bool operator()( const StringView<TChar> & pLhs, const StringView<TChar> & pRhs ) const
		{
			return pLhs != pRhs;
		}

		bool operator()( const StringView<TChar> & pLhs, const std::basic_string<TChar> & pRhs ) const
		{
			return pLhs != pRhs;
		}

		bool operator()( const std::basic_string<TChar> & pLhs, const StringView<TChar> & pRhs ) const
		{
			return pLhs != pRhs;
		}

		bool operator()( const std::basic_string<TChar> & pLhs, const std::basic_string<TChar> & pRhs ) const
		{
			return pLhs != pRhs;
		}
	};

	template <typename TChar>
	struct StringViewCmpLess
	{
		using is_transparent = void;

		bool operator()( const StringView<TChar> & pLhs, const StringView<TChar> & pRhs ) const
		{
			return pLhs < pRhs;
		}

		bool operator()( const StringView<TChar> & pLhs, const std::basic_string<TChar> & pRhs ) const
		{
			return pLhs < pRhs;
		}

		bool operator()( const std::basic_string<TChar> & pLhs, const StringView<TChar> & pRhs ) const
		{
			return pLhs < pRhs;
		}

		bool operator()( const std::basic_string<TChar> & pLhs, const std::basic_string<TChar> & pRhs ) const
		{
			return pLhs < pRhs;
		}
	};

	template <typename TChar>
	struct StringViewCmpLessEqual
	{
		using is_transparent = void;

		bool operator()( const StringView<TChar> & pLhs, const StringView<TChar> & pRhs ) const
		{
			return pLhs <= pRhs;
		}

		bool operator()( const StringView<TChar> & pLhs, const std::basic_string<TChar> & pRhs ) const
		{
			return pLhs <= pRhs;
		}

		bool operator()( const std::basic_string<TChar> & pLhs, const StringView<TChar> & pRhs ) const
		{
			return pLhs <= pRhs;
		}

		bool operator()( const std::basic_string<TChar> & pLhs, const std::basic_string<TChar> & pRhs ) const
		{
			return pLhs <= pRhs;
		}
	};

	template <typename TChar>
	struct StringViewCmpGreater
	{
		using is_transparent = void;

		bool operator()( const StringView<TChar> & pLhs, const StringView<TChar> & pRhs ) const
		{
			return pLhs > pRhs;
		}

		bool operator()( const StringView<TChar> & pLhs, const std::basic_string<TChar> & pRhs ) const
		{
			return pLhs > pRhs;
		}

		bool operator()( const std::basic_string<TChar> & pLhs, const StringView<TChar> & pRhs ) const
		{
			return pLhs > pRhs;
		}

		bool operator()( const std::basic_string<TChar> & pLhs, const std::basic_string<TChar> & pRhs ) const
		{
			return pLhs > pRhs;
		}
	};

	template <typename TChar>
	struct StringViewCmpGreaterEqual
	{
		using is_transparent = void;

		bool operator()( const StringView<TChar> & pLhs, const StringView<TChar> & pRhs ) const
		{
			return pLhs >= pRhs;
		}

		bool operator()( const StringView<TChar> & pLhs, const std::basic_string<TChar> & pRhs ) const
		{
			return pLhs >= pRhs;
		}

		bool operator()( const std::basic_string<TChar> & pLhs, const StringView<TChar> & pRhs ) const
		{
			return pLhs >= pRhs;
		}

		bool operator()( const std::basic_string<TChar> & pLhs, const std::basic_string<TChar> & pRhs ) const
		{
			return pLhs >= pRhs;
		}
	};

}

#endif /* __TS3_STDEXT_STRING_VIEW_H__ */
