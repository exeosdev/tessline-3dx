
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
		: beginPtr( nullptr )
		, endPtr( nullptr )
		, size( 0 )
		{}

		ArrayView( Tp * pMemory, size_t pSize )
		: beginPtr( pMemory )
		, endPtr( pMemory + pSize )
		, size( pSize )
		{}

		template <typename TpOther>
		ArrayView( const ArrayView<TpOther> & pOther )
		: beginPtr( pOther.beginPtr )
		, endPtr( pOther.endPtr )
		, size( pOther.size )
		{}

		template <size_t tSize>
		explicit ArrayView( Tp( &pArray )[tSize] )
		: ArrayView( &( pArray[0] ), tSize )
		{}

		ArrayView<ByteType> asByteView() const
		{
		    return ArrayView<ByteType>{ reinterpret_cast<ByteType *>( beginPtr ), size * sizeof( Tp ) };
		}

		void swap( ArrayView & pOther )
		{
			std::swap( beginPtr, pOther.begin );
			std::swap( endPtr, pOther.end );
			std::swap( size, pOther.size );
		}

	public:
		// Pointer to the beginning of the data.
		Tp * beginPtr;
		// Pointer past end of the data.
		Tp * endPtr;
		// Size of the data, in number of elements of the underlying type (Tp).
		size_t size;
	};

	template <typename Tp>
	inline void swap( ArrayView<Tp> & pFirst, ArrayView<Tp> & pSecond )
	{
		pFirst.swap( pSecond );
	}

	template <typename Tp>
	inline Tp * begin( const ArrayView<Tp> & pArrayView )
	{
		return pArrayView.beginPtr;
	}

	template <typename Tp>
	inline Tp * end( const ArrayView<Tp> & pArrayView )
	{
		return pArrayView.endPtr;
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

}

#endif // __TS3_STDEXT_ARRAY_VIEW_H__
