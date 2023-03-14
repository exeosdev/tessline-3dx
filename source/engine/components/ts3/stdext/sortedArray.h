
#ifndef __TS3_STDEXT_SORTED_ARRAY__H__
#define __TS3_STDEXT_SORTED_ARRAY__H__

#include "staticAlgo.h"
#include "staticLimits.h"
#include <stdexcept>

namespace ts3
{

	template < typename TValue,
	           typename TCompare = CmpLessT<TValue>,
	           typename TAlloc = std::allocator<TValue> >
	class SortedArray
	{
	public:
		using SelfType = SortedArray<TValue, TCompare, TAlloc>;

		using ValueType = TValue;
		using CompareType = TCompare;

		using UnderlyingContainerType = std::vector<TValue, TAlloc>;

		using Iterator = typename UnderlyingContainerType::iterator;
		using ConstIterator = typename UnderlyingContainerType::const_iterator;
		using ReverseIterator = typename UnderlyingContainerType::reverse_iterator;
		using ConstReverseIterator = typename UnderlyingContainerType::const_reverse_iterator;

	public:
		SortedArray( SortedArray && ) = default;
		SortedArray& operator=( SortedArray && ) = default;

		SortedArray( const SortedArray & ) = default;
		SortedArray& operator=( const SortedArray & ) = default;

		SortedArray()
		{}

		explicit SortedArray( const TAlloc & pAllocator )
		: _underlyingContainer( pAllocator )
		{}

		explicit SortedArray( size_t pCapacity, const TAlloc & pAllocator = TAlloc() )
		: _underlyingContainer( pCapacity, pAllocator )
		{}

		TValue & operator[]( size_t pIndex )
		{
			return _underlyingContainer[pIndex];
		}

		const TValue & operator[]( size_t pIndex ) const
		{
			return _underlyingContainer[pIndex];
		}

		TValue & at( size_t pIndex )
		{
			return _underlyingContainer.at( pIndex );
		}

		const TValue & at( size_t pIndex ) const
		{
			return _underlyingContainer.at( pIndex );
		}

		Iterator begin()
		{
			return _underlyingContainer.begin();
		}

		ConstIterator begin() const
		{
			return _underlyingContainer.begin();
		}

		Iterator end()
		{
			return _underlyingContainer.end();
		}

		ConstIterator end() const
		{
			return _underlyingContainer.end();
		}

		TValue & front()
		{
			return _underlyingContainer.front();
		}

		const TValue & front() const
		{
			return _underlyingContainer.front();
		}

		TValue & back()
		{
			return _underlyingContainer.back();
		}

		const TValue & back() const
		{
			return _underlyingContainer.back();
		}

		Iterator insert( TValue && pValue )
		{
			auto insertPosition = _getInsertPosition( pValue, CompareType{} );
			auto insertIterator = _underlyingContainer.begin() + insertPosition;
			return _underlyingContainer.insert( insertIterator, std::move( pValue ) );
		}

		Iterator insert( const TValue & pValue )
		{
			auto insertPosition = _getInsertPosition( pValue, CompareType{} );
			auto insertIterator = _underlyingContainer.begin() + insertPosition;
			return _underlyingContainer.insert( insertIterator, pValue );
		}

		Iterator find( const TValue & pValue )
		{
			auto elementPos = _findLower( pValue, CompareType{} );
			if( ( elementPos != cxdefs::INVALID_POSITION ) && ( _underlyingContainer[elementPos] == pValue ) )
			{
				return _underlyingContainer.begin() + elementPos;
			}
			return _underlyingContainer.end();
		}

		ConstIterator find( const TValue & pValue ) const
		{
			auto elementPos = _findLower( pValue, CompareType{} );
			if( ( elementPos != cxdefs::INVALID_POSITION ) && ( _underlyingContainer[elementPos] == pValue ) )
			{
				return _underlyingContainer.begin() + elementPos;
			}
			return _underlyingContainer.end();
		}

		template < typename TRef, typename TRefEqual = CmpEqualT<TValue> >
		Iterator find( const TRef & pValue, TRefEqual pRefEqual = TRefEqual{} )
		{
			auto elementPos = _findLower( pValue, CompareType{} );
			if( ( elementPos != cxdefs::INVALID_POSITION ) && pRefEqual( _underlyingContainer[elementPos], pValue ) )
			{
				return _underlyingContainer.begin() + elementPos;
			}
			return _underlyingContainer.end();
		}

		template < typename TRef, typename TRefEqual = CmpEqualT<TValue> >
		ConstIterator find( const TRef & pValue, TRefEqual pRefEqual = TRefEqual{} ) const
		{
			auto elementPos = _findLower( pValue, CompareType{} );
			if( ( elementPos != cxdefs::INVALID_POSITION ) && pRefEqual( _underlyingContainer[elementPos], pValue ) )
			{
				return _underlyingContainer.begin() + elementPos;
			}
			return _underlyingContainer.end();
		}

		template < typename TRef, typename TRefCompare = CmpLessT<TValue>, typename TRefEqual = CmpEqualT<TValue> >
		Iterator find( const TRef & pValue, TRefCompare pRefCompare, TRefEqual pRefEqual )
		{
			auto elementPos = _findLower( pValue, pRefCompare );
			if( ( elementPos != cxdefs::INVALID_POSITION ) && pRefEqual( _underlyingContainer[elementPos], pValue ) )
			{
				return _underlyingContainer.begin() + elementPos;
			}
			return _underlyingContainer.end();
		}

		template < typename TRef, typename TRefCompare = CmpLessT<TValue>, typename TRefEqual = CmpEqualT<TValue> >
		ConstIterator find( const TRef & pValue, TRefCompare pRefCompare, TRefEqual pRefEqual ) const
		{
			auto elementPos = _findLower( pValue, pRefCompare );
			if( ( elementPos != cxdefs::INVALID_POSITION ) && pRefEqual( _underlyingContainer[elementPos], pValue ) )
			{
				return _underlyingContainer.begin() + elementPos;
			}
			return _underlyingContainer.end();
		}

		Iterator erase( Iterator pIter )
		{
			return _underlyingContainer.erase( pIter );
		}

		Iterator erase( Iterator pStart, Iterator pEnd )
		{
			return _underlyingContainer.erase( pStart, pEnd );
		}

		Iterator remove( const TValue & pValue )
		{
			auto rangeBeginPos = _findLower( pValue, CompareType{} );
			if( rangeBeginPos != cxdefs::INVALID_POSITION )
			{
				auto rangeEndPos = _findUpper( pValue, CompareType{} );
				const auto eraseFrom = _underlyingContainer.begin() + rangeBeginPos;
				const auto eraseTo = _underlyingContainer.begin() + rangeEndPos;
				return _underlyingContainer.erase( eraseFrom, eraseTo );
			}
			return _underlyingContainer.end();
		}

		Iterator remove( const TValue & pLeft, const TValue & pRight )
		{
			auto rangeBeginPos = _findLower( pLeft, CompareType{} );
			if( rangeBeginPos != cxdefs::INVALID_POSITION )
			{
				auto rangeEndPos = _findUpper( pRight, CompareType{} );
				if( rangeEndPos < rangeBeginPos )
				{
					throw std::out_of_range("Invalid range specification" );
				}
				const auto eraseFrom = _underlyingContainer.begin() + rangeBeginPos;
				const auto eraseTo = _underlyingContainer.begin() + rangeEndPos;
				return _underlyingContainer.erase( eraseFrom, eraseTo );
			}
			return _underlyingContainer.end();
		}

		void insertData( const std::vector<TValue> & pValueArray )
		{
			_underlyingContainer.insert( _underlyingContainer.end(), pValueArray.begin(), pValueArray.end() );

			std::sort( _underlyingContainer.begin(),
			           _underlyingContainer.end(),
			           CompareType{} );
		}

		void setData( std::vector<TValue> pValueArray )
		{
			_underlyingContainer = std::move( pValueArray );

			std::sort( _underlyingContainer.begin(),
			           _underlyingContainer.end(),
			           CompareType{} );
		}

		void clear()
		{
			_underlyingContainer.clear();
		}

		void reserve( size_t pCapacity )
		{
			_underlyingContainer.reserve( pCapacity );
		}

		size_t size() const
		{
			return _underlyingContainer.size();
		}

		bool empty() const
		{
			return _underlyingContainer.empty();
		}

		void swap( SelfType & pOther )
		{
			std::swap( _underlyingContainer, pOther._underlyingContainer );
		}

	private:
		template <typename TRef, typename... TPred>
		size_t _getInsertPosition( const TRef & pRefValue, TPred... pPredicate ) const
		{
			auto rangeBegin = _underlyingContainer.begin();
			auto rangeEnd = _underlyingContainer.end();
			auto resultIter = std::lower_bound( rangeBegin, rangeEnd, pRefValue, std::forward<TPred>( pPredicate )... );
			return ( resultIter != rangeEnd ) ? ( resultIter - rangeBegin ) : ( rangeEnd - rangeBegin );
		}

		template <typename TRef, typename... TPred>
		size_t _findLower( const TRef & pRefValue, TPred... pPredicate ) const
		{
			auto rangeBegin = _underlyingContainer.begin();
			auto rangeEnd = _underlyingContainer.end();
			auto resultIter = std::lower_bound( rangeBegin, rangeEnd, pRefValue, std::forward<TPred>( pPredicate )... );
			return ( resultIter != rangeEnd ) ? ( resultIter - rangeBegin ) : cxdefs::INVALID_POSITION;
		}

		template <typename TRef, typename... TPred>
		size_t _findUpper( const TRef & pRefValue, TPred... pPredicate ) const
		{
			auto rangeBegin = _underlyingContainer.begin();
			auto rangeEnd = _underlyingContainer.end();
			auto resultIter = std::upper_bound( rangeBegin, rangeEnd, pRefValue, std::forward<TPred>( pPredicate )... );
			return ( resultIter != rangeEnd ) ? ( resultIter - rangeBegin ) : cxdefs::INVALID_POSITION;
		}

	private:
		UnderlyingContainerType _underlyingContainer;
	};

	template <typename TValue, typename TCompare, typename TAlloc>
	inline typename SortedArray<TValue, TCompare, TAlloc>::Iterator begin( SortedArray<TValue, TCompare, TAlloc> & pContainer )
	{
		return pContainer.begin();
	}

	template <typename TValue, typename TCompare, typename TAlloc>
	inline typename SortedArray<TValue, TCompare, TAlloc>::ConstIterator begin( const SortedArray<TValue, TCompare, TAlloc> & pContainer )
	{
		return pContainer.begin();
	}

	template <typename TValue, typename TCompare, typename TAlloc>
	inline typename SortedArray<TValue, TCompare, TAlloc>::Iterator end( SortedArray<TValue, TCompare, TAlloc> & pContainer )
	{
		return pContainer.end();
	}

	template <typename TValue, typename TCompare, typename TAlloc>
	inline typename SortedArray<TValue, TCompare, TAlloc>::ConstIterator end( const SortedArray<TValue, TCompare, TAlloc> & pContainer )
	{
		return pContainer.end();
	}

}

#endif // __TS3_STDEXT_SORTED_ARRAY__H__
