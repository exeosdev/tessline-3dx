
#ifndef __TS3_STDEXT_ARRAY_VIEW_H__
#define __TS3_STDEXT_ARRAY_VIEW_H__

#include "prerequisites.h"

namespace ts3
{

	template <bool tpIsDataConst>
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
	/// @tparam Tp Type of the data the view references.
	template <typename Tp>
	struct ArrayView
	{
		static_assert( !std::is_void<Tp>::value, "Cannot create an ArrayView for void data. Use ArrayView<byte> for that." );

	public:
		using ByteType = typename ArrayViewByteType<std::is_const<Tp>::value>::Type;
		
		ArrayView()
		: _beginPtr( nullptr )
		, _size( 0 )
		{}

		ArrayView( Tp * pMemory, size_t pSize )
		: _beginPtr( pMemory )
		, _size( pSize )
		{}

		template <typename TpOther>
		ArrayView( const ArrayView<TpOther> & pOther )
		: _beginPtr( pOther.data() )
		, _size( pOther.size() )
		{}

		template <size_t tSize>
		explicit ArrayView( Tp( &pArray )[tSize] )
		: ArrayView( &( pArray[0] ), tSize )
		{}

		explicit operator bool() const
		{
			return !empty();
		}

		TS3_FUNC_NO_DISCARD ArrayView<ByteType> asByteView() const
		{
			return ArrayView<ByteType>{ reinterpret_cast<ByteType *>( _beginPtr ), _size * sizeof( Tp ) };
		}

		TS3_FUNC_NO_DISCARD Tp * data() const
		{
			return _beginPtr;
		}

		TS3_FUNC_NO_DISCARD size_t size() const
		{
			return _size;
		}

		TS3_FUNC_NO_DISCARD bool empty() const
		{
			return _beginPtr && ( _size > 0 );
		}

		void swap( ArrayView & pOther )
		{
			std::swap( _beginPtr, pOther.begin );
			std::swap( _size, pOther._size );
		}

	private:
		// Pointer to the beginning of the data.
		Tp * _beginPtr;
		// Size of the data, in number of elements of the underlying type (Tp).
		size_t _size;
	};

	template <typename Tp>
	inline void swap( ArrayView<Tp> & pFirst, ArrayView<Tp> & pSecond )
	{
		pFirst.swap( pSecond );
	}

	template <typename Tp>
	inline Tp * begin( const ArrayView<Tp> & pArrayView )
	{
		return pArrayView.data();
	}

	template <typename Tp>
	inline Tp * end( const ArrayView<Tp> & pArrayView )
	{
		return pArrayView.data() + pArrayView.size();
	}

	/// @brief Creates ArrayView that wraps specified memory.
	/// @tparam Tp Type of the data referenced by the view.
	/// @param pArrayMemory Pointer to the beginning of the array.
	/// @param pArraySize Size of the array, in number of elements of type Tp.
	/// @return ArrayView referencing specified data.
	template <typename Tp>
	inline ArrayView<Tp> bindArrayView( Tp * pMemory, size_t pSize )
	{
		return ArrayView<Tp>( pMemory, pSize );
	}

	/// @brief Creates ArrayView that wraps specified compile time-sized array.
	/// @tparam Tp Type of the data referenced by the view.
	/// @tparam tSize Size of the array.
	/// @param pArray Array to create view for.
	/// @return ArrayView referencing specified data.
	template <typename Tp, size_t tSize>
	inline ArrayView<Tp> bindArrayView( Tp( &pArray )[tSize] )
	{
		return ArrayView<Tp>( pArray );
	}

	using ReadOnlyMemoryView = ArrayView<const byte>;
	using ReadWriteMemoryView = ArrayView<byte>;

	template <typename Tp, std::enable_if_t<std::is_const<Tp>::value, int> = 0>
	inline ReadOnlyMemoryView bindMemoryView( Tp * pMemory, size_t pSize )
	{
		return ReadOnlyMemoryView( pMemory, pSize );
	}

	template <typename Tp, std::enable_if_t<!std::is_const<Tp>::value, int> = 0>
	inline ReadWriteMemoryView bindMemoryView( Tp * pMemory, size_t pSize )
	{
		return ReadWriteMemoryView( pMemory, pSize );
	}

}

#endif // __TS3_STDEXT_ARRAY_VIEW_H__
