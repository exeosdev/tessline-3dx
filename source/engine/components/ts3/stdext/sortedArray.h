
#ifndef __TS3_STDEXT_SORTED_ARRAY__H__
#define __TS3_STDEXT_SORTED_ARRAY__H__

#include "staticLimits.h"
#include <stdexcept>

namespace ts3
{

	constexpr size_t cxInvalidPosition = static_cast<size_t>( -1 );

	template <typename Tp1, typename Tp2>
	struct DefaultCmpEqual
	{
		constexpr bool operator()( const Tp1 & pLhs, const Tp2 & pRhs ) const
		{
			return pLhs == pRhs;
		}
	};

	template < typename TpValue,
	           typename TpCompare = std::less<TpValue>,
	           typename TpAlloc = std::allocator<TpValue> >
    class SortedArray
    {
    public:
    	using SelfType = SortedArray<TpValue, TpCompare, TpAlloc>;

	    using ValueType = TpValue;
	    using CompareType = TpCompare;

	    using UnderlyingContainerType = std::vector<TpValue, TpAlloc>;

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

    	explicit SortedArray( const TpAlloc & pAllocator )
    	: _underlyingContainer( pAllocator )
    	{}

    	explicit SortedArray( size_t pCapacity, const TpAlloc & pAllocator = TpAlloc() )
    	: _underlyingContainer( pCapacity, pAllocator )
    	{}

	    TpValue & operator[]( size_t pIndex )
	    {
		    return _underlyingContainer[pIndex];
	    }

	    const TpValue & operator[]( size_t pIndex ) const
	    {
		    return _underlyingContainer[pIndex];
	    }

	    TpValue & at( size_t pIndex )
	    {
		    return _underlyingContainer.at( pIndex );
	    }

	    const TpValue & at( size_t pIndex ) const
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

	    TpValue & front()
	    {
		    return _underlyingContainer.front();
	    }

	    const TpValue & front() const
	    {
		    return _underlyingContainer.front();
	    }

	    TpValue & back()
	    {
		    return _underlyingContainer.back();
	    }

	    const TpValue & back() const
	    {
		    return _underlyingContainer.back();
	    }

	    Iterator insert( TpValue && pValue )
	    {
    		auto insertPosition = _getInsertPosition( pValue, CompareType() );
		    auto insertIterator = _underlyingContainer.begin() + insertPosition;
		    return _underlyingContainer.insert( insertIterator, std::move( pValue ) );
	    }

	    Iterator insert( const TpValue & pValue )
	    {
		    auto insertPosition = _getInsertPosition( pValue, CompareType() );
		    auto insertIterator = _underlyingContainer.begin() + insertPosition;
		    return _underlyingContainer.insert( insertIterator, pValue );
	    }

	    Iterator find( const TpValue & pValue )
	    {
    		auto elementPos = _findLower( pValue, CompareType() );
    		if( ( elementPos != cxInvalidPosition ) && ( _underlyingContainer[elementPos] == pValue ) )
		    {
    			return _underlyingContainer.begin() + elementPos;
		    }
    		return _underlyingContainer.end();
	    }

	    ConstIterator find( const TpValue & pValue ) const
	    {
		    auto elementPos = _findLower( pValue, CompareType() );
		    if( ( elementPos != cxInvalidPosition ) && ( _underlyingContainer[elementPos] == pValue ) )
		    {
			    return _underlyingContainer.begin() + elementPos;
		    }
		    return _underlyingContainer.end();
	    }

	    template < typename TpRef, typename TpEqCmp = DefaultCmpEqual<TpValue, TpRef> >
	    Iterator find( const TpRef & pValue, const TpEqCmp & pEqCmp = TpEqCmp() )
	    {
		    auto elementPos = _findLower( pValue, CompareType() );
		    if( ( elementPos != cxInvalidPosition ) && pEqCmp( _underlyingContainer[elementPos], pValue ) )
		    {
			    return _underlyingContainer.begin() + elementPos;
		    }
		    return _underlyingContainer.end();
	    }

	    template < typename TpRef, typename TpEqCmp = DefaultCmpEqual<TpValue, TpRef> >
	    ConstIterator find( const TpRef & pValue, const TpEqCmp & pEqCmp = TpEqCmp() ) const
	    {
		    auto elementPos = _findLower( pValue, CompareType() );
		    if( ( elementPos != cxInvalidPosition ) && pEqCmp( _underlyingContainer[elementPos], pValue ) )
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

	    Iterator remove( const TpValue & pValue )
	    {
    		auto rangeBeginPos = _findLower( pValue, CompareType() );
    		if( rangeBeginPos != cxInvalidPosition )
		    {
			    auto rangeEndPos = _findUpper( pValue, CompareType() );
			    const auto eraseFrom = _underlyingContainer.begin() + rangeBeginPos;
			    const auto eraseTo = _underlyingContainer.begin() + rangeEndPos;
			    return _underlyingContainer.erase( eraseFrom, eraseTo );
		    }
    		return _underlyingContainer.end();
	    }

	    Iterator remove( const TpValue & pLeft, const TpValue & pRight )
	    {
		    auto rangeBeginPos = _findLower( pLeft, CompareType() );
		    if( rangeBeginPos != cxInvalidPosition )
		    {
			    auto rangeEndPos = _findUpper( pRight, CompareType() );
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

	    void insertData( const std::vector<TpValue> & pValueArray )
	    {
		    _underlyingContainer.insert( _underlyingContainer.end(), pValueArray.begin(), pValueArray.end() );

		    std::sort( _underlyingContainer.begin(),
		               _underlyingContainer.end(),
		               CompareType() );
	    }

	    void setData( std::vector<TpValue> pValueArray )
	    {
		    _underlyingContainer = std::move( pValueArray );

		    std::sort( _underlyingContainer.begin(),
		               _underlyingContainer.end(),
		               CompareType() );
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
    	template <typename TpRef, typename... TpPred>
    	size_t _getInsertPosition( const TpRef & pRefValue, TpPred... pPredicate ) const
	    {
		    auto rangeBegin = _underlyingContainer.begin();
		    auto rangeEnd = _underlyingContainer.end();
		    auto resultIter = std::lower_bound( rangeBegin, rangeEnd, pRefValue, std::forward<TpPred>( pPredicate )... );
		    return ( resultIter != rangeEnd ) ? ( resultIter - rangeBegin ) : ( rangeEnd - rangeBegin );
	    }

	    template <typename TpRef, typename... TpPred>
	    size_t _findLower( const TpRef & pRefValue, TpPred... pPredicate ) const
	    {
		    auto rangeBegin = _underlyingContainer.begin();
		    auto rangeEnd = _underlyingContainer.end();
		    auto resultIter = std::lower_bound( rangeBegin, rangeEnd, pRefValue, std::forward<TpPred>( pPredicate )... );
		    return ( resultIter != rangeEnd ) ? ( resultIter - rangeBegin ) : cxInvalidPosition;
	    }

	    template <typename TpRef, typename... TpPred>
	    size_t _findUpper( const TpRef & pRefValue, TpPred... pPredicate ) const
	    {
		    auto rangeBegin = _underlyingContainer.begin();
		    auto rangeEnd = _underlyingContainer.end();
		    auto resultIter = std::upper_bound( rangeBegin, rangeEnd, pRefValue, std::forward<TpPred>( pPredicate )... );
		    return ( resultIter != rangeEnd ) ? ( resultIter - rangeBegin ) : cxInvalidPosition;
	    }

    private:
	    UnderlyingContainerType _underlyingContainer;
    };

	template <typename TpValue, typename TpCompare, typename TpAlloc>
	inline typename SortedArray<TpValue, TpCompare, TpAlloc>::Iterator begin( SortedArray<TpValue, TpCompare, TpAlloc> & pContainer )
	{
		return pContainer.begin();
	}

	template <typename TpValue, typename TpCompare, typename TpAlloc>
	inline typename SortedArray<TpValue, TpCompare, TpAlloc>::ConstIterator begin( const SortedArray<TpValue, TpCompare, TpAlloc> & pContainer )
	{
		return pContainer.begin();
	}

	template <typename TpValue, typename TpCompare, typename TpAlloc>
	inline typename SortedArray<TpValue, TpCompare, TpAlloc>::Iterator end( SortedArray<TpValue, TpCompare, TpAlloc> & pContainer )
	{
		return pContainer.end();
	}

	template <typename TpValue, typename TpCompare, typename TpAlloc>
	inline typename SortedArray<TpValue, TpCompare, TpAlloc>::ConstIterator end( const SortedArray<TpValue, TpCompare, TpAlloc> & pContainer )
	{
		return pContainer.end();
	}

}

#endif // __TS3_STDEXT_SORTED_ARRAY__H__
