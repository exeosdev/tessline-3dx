
#ifndef __TS3_STDEXT_ARRAY_VIEW_H__
#define __TS3_STDEXT_ARRAY_VIEW_H__

#include "range.h"
#include "utilities.h"

namespace ts3
{

	template <bool tIsDataConst>
	struct ArrayViewByteType;

	template <>
	struct ArrayViewByteType<false>
	{
		using Type = byte;
	};

	template <>
	struct ArrayViewByteType<true>
	{
		using Type = const byte;
	};

	/// @brief Simple struct used to wrap and reference continuous blocks of memory.
	/// @tparam TVal Type of the data the view references.
	template <typename TVal>
	struct ArrayView
	{
		static_assert( !std::is_void<TVal>::value, "Cannot create an ArrayView for void data. Use ArrayView<byte> for that." );

	public:
		using ByteType = typename ArrayViewByteType<std::is_const<TVal>::value>::Type;
		
		ArrayView()
		: _dataPtr( nullptr )
		, _size( 0 )
		{}

		ArrayView( TVal * pMemory, size_t pSize )
		: _dataPtr( pMemory )
		, _size( pSize )
		{}

		template <typename TOther>
		ArrayView( const ArrayView<TOther> & pOther )
		: _dataPtr( pOther.data() )
		, _size( pOther.size() )
		{}

		template <size_t tSize>
		explicit ArrayView( TVal( &pArray )[tSize] )
		: ArrayView( &( pArray[0] ), tSize )
		{}

		explicit ArrayView( std::initializer_list<TVal> pInitList )
		: ArrayView( pInitList.data(), pInitList.size() )
		{}

		explicit operator bool() const
		{
			return !empty();
		}

		TS3_ATTR_NO_DISCARD TVal & operator[](size_t pIndex) const
		{
			ts3DebugAssert( pIndex < _size );
			return _dataPtr[pIndex];
		}

		TS3_ATTR_NO_DISCARD ArrayView<ByteType> asByteView() const
		{
			return ArrayView<ByteType>{ reinterpret_cast<ByteType *>( _dataPtr ), _size * sizeof( TVal ) };
		}

		TS3_ATTR_NO_DISCARD TVal * data() const
		{
			return _dataPtr;
		}

		TS3_ATTR_NO_DISCARD size_t size() const
		{
			return _size;
		}

		TS3_ATTR_NO_DISCARD bool empty() const
		{
			return _dataPtr && ( _size > 0 );
		}

		void swap( ArrayView & pOther )
		{
			std::swap( _dataPtr, pOther._dataPtr );
			std::swap( _size, pOther._size );
		}

	private:
		// Pointer to the beginning of the data.
		TVal * _dataPtr;
		// Size of the data, in number of elements of the underlying type (TVal).
		size_t _size;
	};

	template <typename TVal>
	inline void swap( ArrayView<TVal> & pFirst, ArrayView<TVal> & pSecond )
	{
		pFirst.swap( pSecond );
	}

	template <typename TVal>
	inline TVal * begin( const ArrayView<TVal> & pArrayView )
	{
		return pArrayView.data();
	}

	template <typename TVal>
	inline TVal * end( const ArrayView<TVal> & pArrayView )
	{
		return pArrayView.data() + pArrayView.size();
	}

	/// @brief Creates ArrayView that wraps specified memory.
	/// @tparam TVal Type of the data referenced by the view.
	/// @param pArrayMemory Pointer to the beginning of the array.
	/// @param pArraySize Size of the array, in number of elements of type TVal.
	/// @return ArrayView referencing specified data.
	template <typename TVal>
	inline ArrayView<TVal> bindArrayView( TVal * pMemory, size_t pSize )
	{
		return ArrayView<TVal>( pMemory, pSize );
	}

	template <typename TVal, typename TOffset, typename TSize>
	inline ArrayView<TVal> bindArrayView( TVal * pMemory, const Region<TSize, TOffset> & pRegion )
	{
		return ArrayView<TVal>( pMemory + pRegion.offset, pRegion.size );
	}

	/// @brief Creates ArrayView that wraps specified compile time-sized array.
	/// @tparam TVal Type of the data referenced by the view.
	/// @tparam tSize Size of the array.
	/// @param pArray Array to create view for.
	/// @return ArrayView referencing specified data.
	template <typename TVal, size_t tSize>
	inline ArrayView<TVal> bindArrayView( TVal( &pArray )[tSize] )
	{
		return ArrayView<TVal>( pArray );
	}

	template <typename TVal, size_t tSize, typename TOffset, typename TSize>
	inline ArrayView<TVal> bindArrayView( TVal( &pArray )[tSize], const Region<TSize, TOffset> & pRegion )
	{
		const auto validRegion = getValidRegion( pRegion, tSize );
		return ArrayView<TVal>( &pArray[0] + validRegion.offset, validRegion.size );
	}

	/// @brief Creates ArrayView that wraps specified compile time-sized array.
	/// @tparam TVal Type of the data referenced by the view.
	/// @tparam tSize Size of the array.
	/// @param pArray Array to create view for.
	/// @return ArrayView referencing specified data.
	template <typename TVal, size_t tSize>
	inline ArrayView<TVal> bindArrayView( std::array<TVal, tSize> & pArray )
	{
		return ArrayView<TVal>( pArray.data(), pArray.size() );
	}

	template <typename TVal, size_t tSize, typename TOffset, typename TSize>
	inline ArrayView<TVal> bindArrayView( std::array<TVal, tSize> & pArray, const Region<TSize, TOffset> & pRegion )
	{
		const auto validRegion = getValidRegion( pRegion, tSize );
		return ArrayView<TVal>( pArray.data() + validRegion.offset, validRegion.size );
	}

	/// @brief Creates ArrayView that wraps specified compile time-sized array.
	/// @tparam TVal Type of the data referenced by the view.
	/// @tparam tSize Size of the array.
	/// @param pArray Array to create view for.
	/// @return ArrayView referencing specified data.
	template <typename TVal, size_t tSize>
	inline ArrayView<const TVal> bindArrayView( const std::array<TVal, tSize> & pArray )
	{
		return ArrayView<const TVal>( pArray.data(), pArray.size() );
	}

	template <typename TVal, size_t tSize, typename TOffset, typename TSize>
	inline ArrayView<const TVal> bindArrayView( const std::array<TVal, tSize> & pArray, const Region<TSize, TOffset> & pRegion )
	{
		const auto validRegion = getValidRegion( pRegion, tSize );
		return ArrayView<const TVal>( pArray.data() + validRegion.offset, validRegion.size );
	}

	using ReadOnlyMemoryView = ArrayView<const byte>;
	using ReadWriteMemoryView = ArrayView<byte>;

	template <typename TVal, std::enable_if_t<std::is_const<TVal>::value && !std::is_void<TVal>::value, int> = 0>
	inline ReadOnlyMemoryView bindMemoryView( TVal * pMemory, size_t pSize )
	{
		return ReadOnlyMemoryView( reinterpret_cast<const byte *>( pMemory ), pSize * sizeof( TVal ) );
	}

	template <typename TVal, std::enable_if_t<!std::is_const<TVal>::value && !std::is_void<TVal>::value, int> = 0>
	inline ReadWriteMemoryView bindMemoryView( TVal * pMemory, size_t pSize )
	{
		return ReadWriteMemoryView( reinterpret_cast<byte *>( pMemory ), pSize * sizeof( TVal ) );
	}

	inline ReadOnlyMemoryView bindMemoryView( const void * pMemory, size_t pByteLength )
	{
		return ReadOnlyMemoryView( reinterpret_cast<const byte *>( pMemory ), pByteLength );
	}

	inline ReadWriteMemoryView bindMemoryView( void * pMemory, size_t pByteLength )
	{
		return ReadWriteMemoryView( reinterpret_cast<byte *>( pMemory ), pByteLength );
	}

}

#endif // __TS3_STDEXT_ARRAY_VIEW_H__
