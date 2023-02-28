
#ifndef __TS3_STDEXT_VARIANT_H__
#define __TS3_STDEXT_VARIANT_H__

#include "staticAlgo.h"
#include <typeinfo>

namespace ts3
{

	/// @brief Type for storing the Variant type index.
	using variant_index_t = size_t;

	struct VariantTypeConvTag
	{};

	/// @brief Represents invalid Variant type index. Usually indicates, that incompatible type was used as a target.
	constexpr variant_index_t CX_INVALID_VARIANT_INDEX = 0;

	/// @brief Shared instance of `VariantTypeConvTag`. Use it to force desired overload.
	inline constexpr VariantTypeConvTag CX_VARIANT_TYPE_CONV{};

	template <typename TVal>
	struct VariantTypeIndex
	{
		static const variant_index_t value;
	};

	template <typename TVal>
	inline const variant_index_t VariantTypeIndex<TVal>::value = typeid( TVal ).hash_code();

	template <typename... TLst>
	struct VariantStorage
	{
		static constexpr size_t size = StaticMaxSizeofT<TLst...>::value;
		static constexpr size_t alignment = StaticMaxAlignofT<TLst...>::value;

		using Type = typename std::aligned_storage<size, alignment>::type;
	};


	template <typename TVal, typename... TRest>
	struct VariantTypeSelector
	{
	};


	template <bool tExactType, typename... TLst>
	struct VariantSetProxy;

	template <typename... TLst>
	struct VariantSetProxy<true, TLst...>
	{
		template <typename TArg>
		static variant_index_t construct( void * pStorage, TArg pArg )
		{
			using UnrefArg = std::remove_cv_t<TArg>;
			new ( reinterpret_cast<UnrefArg *>( pStorage ) ) UnrefArg( std::forward<TArg>( pArg ) );
			return VariantTypeIndex<UnrefArg>::value;
		}
	};

	template <typename TVal, typename... TLst>
	struct VariantSetProxy<false, TVal, TLst...>
	{
		template <typename... TArgs, typename std::enable_if<std::is_constructible<TVal, TArgs...>::value>>
		static variant_index_t construct( void * pStorage, TArgs && ...pArgs )
		{
			new ( reinterpret_cast<TVal *>( pStorage ) ) TVal( std::forward<TArgs>( pArgs )... );
			return VariantTypeIndex<TVal>::value;
		}

		template <typename... TArgs, typename std::enable_if<!std::is_constructible<TVal, TArgs...>::value>>
		static variant_index_t construct( void * pStorage, TArgs && ...pArgs )
		{
			return VariantSetProxy<false, TLst...>::construct( pStorage, std::forward<TArgs>( pArgs )... );
		}
	};

	template <>
	struct VariantSetProxy<false>
	{
		template <typename TInput>
		static variant_index_t construct( void * pStorage, TInput pInput )
		{
			return CX_INVALID_VARIANT_INDEX;
		}
	};


	// Proxy with the actual implementation of the Variant methods. Implementation is done using the following concept:
	//
	// Variant is a template defined for finite list of types. By using VariantTypeIndex (above) we assign a unique
	// index to every type used with Variant. When an operation is executed for
	// a given type T (e.g. set<int>( 5 ) ), we do as follows:
	// - pass the unique typeindex to the proper method (here: typeindex of <int>)
	// - check if index matches the first type on the Variant's type list
	// - if yes, execute the function assuming int storage and return
	// - if not, drop the first type and call the function recursively
	// Additional empty specialization of VariantProxy is provided with the fallback implementation of each method. When
	// it gets executed, it means, that requested type is not valid for the Variant object (not present on the type list).
	//
	// Performance notice: by using constexpr modifier, the if actually becomes a compile-time check. Since it's a tail
	// recursion, each function should always end up being just a single call (false ifs should be completely removed by
	// every decent compiler).
	template <typename... TLst>
	struct VariantProxy;

	template <typename TVal, typename... TRest>
	struct VariantProxy<TVal, TRest...>
	{
//		template <typename TArg, std::enable_if_t<IsTypeOnTypeList<TArg, TVal, TRest...>::value, int>>
//		static variant_index_t construct( void * pStorage, TArg pArg )
//		{
//			using Type = std::remove_cv_t<std::remove_reference_t<TArg>>;
//			new ( reinterpret_cast<Type *>( pStorage ) ) TArg( std::forward<TArg>( pArg ) );
//			return VariantTypeIndex<Type>::value;
//		}
//
//		template <typename... TArgs, std::enable_if<std::is_constructible<TVal, TArgs...>::value, int>>
//		static variant_index_t construct( void * pStorage, TArgs && ...pArgs )
//		{
//			new ( reinterpret_cast<TVal *>( pStorage ) ) TVal( std::forward<TArgs>( pArgs )... );
//			return VariantTypeIndex<TVal>::value;
//		}
//
//		template <typename... TArgs, std::enable_if<!std::is_constructible<TVal, TArgs...>::value, int>>
//		static variant_index_t construct( void * pStorage, TArgs && ...pArgs )
//		{
//			return VariantProxy<TRest...>::construct( pStorage, std::forward<TArgs>( pArgs )... );
//		}

		template <typename... TArgs>
		static variant_index_t construct( void * pStorage, TArgs && ...pArgs )
		{
			return VariantSetProxy<false, TVal, TRest...>::template construct<TArgs...>( pStorage, std::forward<TArgs>( pArgs )... );
		}

		template <typename... TArgs>
		static variant_index_t construct( const variant_index_t pTypeIndex, void * pStorage, TArgs && ...pArgs )
		{
			if( pTypeIndex == VariantTypeIndex<TVal>::value )
			{
				new ( reinterpret_cast< TVal * >( pStorage ) ) TVal( std::forward<TArgs>( pArgs )... );
				return VariantTypeIndex<TVal>::value;
			}
			else
			{
				return VariantProxy<TRest...>::construct( pTypeIndex, pStorage, std::forward<TArgs>( pArgs )... );
			}
		}

		template <typename TValue>
		static variant_index_t constructConv( void * pStorage, TValue pValue )
		{
			if( std::is_convertible<TValue, TVal>::value )
			{
				new ( reinterpret_cast< TVal * >( pStorage ) ) TVal( std::forward<TValue>( pValue ) );
				return VariantTypeIndex<TVal>::value;
			}
			else
			{
				return VariantProxy<TRest...>::constructConv( std::forward<TValue>( pValue ) );
			}
		}

		static variant_index_t constructDefault( void * pStorage )
		{
			if( std::is_default_constructible<TVal>::value )
			{
				new ( reinterpret_cast< TVal * >( pStorage ) ) TVal();
				return VariantTypeIndex<TVal>::value;
			}
			else
			{
				return VariantProxy<TRest...>::constructDefault( pStorage );
			}
		}

		static variant_index_t copy( const variant_index_t pTypeIndex, void * pStorage, void * pSource )
		{
			if( pTypeIndex == VariantTypeIndex<TVal>::value )
			{
				new ( reinterpret_cast<TVal *>( pStorage ) ) TVal( *( reinterpret_cast< TVal * >( pSource ) ) );
				return VariantTypeIndex<TVal>::value;
			}
			else
			{
				return VariantProxy<TRest...>::copy( pTypeIndex, pSource, pStorage );
			}
		}

		static variant_index_t move( const variant_index_t pTypeIndex, void * pStorage, void * pSource )
		{
			if( pTypeIndex == VariantTypeIndex<TVal>::value )
			{
				new ( reinterpret_cast< TVal * >( pStorage ) ) TVal( std::move( *( reinterpret_cast< TVal * >( pSource ) ) ) );
				return VariantTypeIndex<TVal>::value;
			}
			else
			{
				return VariantProxy<TRest...>::move( pTypeIndex, pSource, pStorage );
			}
		}

		static variant_index_t destroy( const variant_index_t pTypeIndex, void * pStorage )
		{
			if( pTypeIndex == VariantTypeIndex<TVal>::value )
			{
				reinterpret_cast< TVal * >( pStorage )->~TVal();
				return VariantTypeIndex<TVal>::value;
			}
			else
			{
				return VariantProxy<TRest...>::destroy( pTypeIndex, pStorage );
			}
		}

		static bool validateType( const variant_index_t pTypeIndex )
		{
			if( pTypeIndex == VariantTypeIndex<TVal>::value )
			{
				return true;
			}
			else
			{
				return VariantProxy<TRest...>::validateType( pTypeIndex );
			}
		}

		template <typename TStorage>
		static void swap( const variant_index_t pFirstIndex, void * pFirstStorage, const variant_index_t pSecondIndex, void * pSecondStorage )
		{
			TStorage temporaryStorage;

			move( pFirstIndex, &temporaryStorage, pFirstStorage );
			move( pSecondIndex, pFirstStorage, pSecondStorage );
			move( pFirstIndex, pSecondStorage, &temporaryStorage );
		}
	};

	template <>
	struct VariantProxy<>
	{
		template <typename... TArgs>
		static variant_index_t construct( void *, TArgs && ... )
		{
			return CX_INVALID_VARIANT_INDEX;
		}

		template <typename TValue>
		static variant_index_t constructConv( variant_index_t, void *, TValue && )
		{
			return CX_INVALID_VARIANT_INDEX;
		}

		static variant_index_t constructDefault( void * )
		{
			return CX_INVALID_VARIANT_INDEX;
		}

		static variant_index_t copy( variant_index_t, void *, void * )
		{
			return CX_INVALID_VARIANT_INDEX;
		}

		static variant_index_t move( variant_index_t, void *, void * )
		{
			return CX_INVALID_VARIANT_INDEX;
		}

		static variant_index_t destroy( variant_index_t, void * )
		{
			return CX_INVALID_VARIANT_INDEX;
		}

		static bool validateType( variant_index_t )
		{
			return false;
		}
	};


	template <typename... TLst>
	class Variant
	{
		static_assert( std::is_void<typename FirstMatchingType<std::is_reference, TLst...>::Type>::value, "No references allowed" );
		
	public:
		static constexpr size_t storageSize = VariantStorage<TLst...>::size;
		static constexpr size_t storageAlignment = VariantStorage<TLst...>::alignment;

		using MyType = Variant<TLst...>;
		using StorageType = typename VariantStorage<TLst...>::Type;

	public:
		Variant()
		{
			_typeIndex = VariantProxy<TLst...>::constructDefault( &( _storage ) );
		}

		Variant( Variant && pSrcObject ) noexcept
		{
			_typeIndex = VariantProxy<TLst...>::move( _typeIndex, &( _storage ), &( pSrcObject._pStorage ) );
		}

		Variant( const Variant & pSrcObject )
		{
			_typeIndex = VariantProxy<TLst...>::copy( _typeIndex, &( _storage ), &( pSrcObject._pStorage ) );
		}

		template <typename TValue>//, typename std::enable_if_t<!std::is_same<TValue, MyType>::value, int>>
		Variant( TValue && pValue ) noexcept
		{
			_typeIndex = VariantProxy<TLst...>::construct( &( _storage ), std::move( pValue ) );
		}

		template <typename TValue>//, typename std::enable_if_t<!std::is_same<TValue, MyType>::value, int>>
		Variant( const TValue & pValue )
		{
			_typeIndex = VariantProxy<TLst...>::construct( &( _storage ), pValue );
		}

		~Variant()
		{
			_release();
		}

		Variant & operator=( Variant && pRhs ) noexcept
		{
			if( this != &pRhs )
			{
				Variant( std::move( pRhs ) ).swap( *this );
			}

			return *this;
		}

		Variant & operator=( const Variant & pRhs )
		{
			if( this != &pRhs )
			{
				Variant( pRhs ).swap( *this );
			}

			return *this;
		}

		template <typename TTarget>
		Variant & operator=( TTarget && pRhs )
		{
			_reInit<TTarget>( std::move( pRhs ) );
			return *this;
		}

		template <typename TTarget>
		Variant & operator=( const TTarget & pRhs )
		{
			_reInit<TTarget>( pRhs );
			return *this;
		}

		template <typename TTarget>
		void set( TTarget && pValue )
		{
			_reInit<TTarget>( std::move( pValue ) );
		}

		template <typename TTarget>
		void set( const TTarget & pValue )
		{
			_reInit<TTarget>( pValue );
		}

		template <typename TTarget>
		void setConv( TTarget && pValue )
		{
			_reInitConv<TTarget>( std::move( pValue ) );
		}

		template <typename TTarget>
		void setConv( const TTarget & pValue )
		{
			_reInitConv<TTarget>( pValue );
		}

		template <typename TTarget, typename... TArgs>
		void emplace( TArgs &&... pArgs )
		{
			_reInit<TTarget>( std::forward<TArgs>( pArgs )... );
		}

		void swap( Variant & pOther )
		{
			VariantProxy<TLst...>::template swap<StorageType>( _typeIndex, &( _storage ), pOther._typeIndex, &( pOther._storage ) );
			std::swap( _typeIndex, pOther._typeIndex );
		}

		template <typename TTarget>
		TS3_ATTR_NO_DISCARD bool checkType() const
		{
			return _typeIndex == VariantTypeIndex<TTarget>::value;
		}

		template <typename TTarget>
		TS3_ATTR_NO_DISCARD TTarget & get() const
		{
			_validate<TTarget>();
			return *( reinterpret_cast<TTarget *>( &( _storage ) ) );
		}

	private:
		template <typename TTarget, typename... TArgs>
		void _reInit( TArgs && ...pArgs )
		{
			VariantProxy<TLst...>::destroy( _typeIndex, &( _storage ) );
			_typeIndex = VariantProxy<TLst...>::construct( VariantTypeIndex<TTarget>::value, &( _storage ), std::forward<TArgs>( pArgs )... );
		}

		template <typename T>
		void _reInitConv( T pValue )
		{
			VariantProxy<TLst...>::destroy( _typeIndex, &( _storage ) );
			_typeIndex = VariantProxy<TLst...>::constructConv( &( _storage ), std::forward<T>( pValue ) );
		}

		template <typename T>
		void _validate() const
		{
			if( _typeIndex != VariantTypeIndex<T>::value )
			{
				throw 0;
			}
		}

		void _release()
		{
			VariantProxy<TLst...>::destroy( _typeIndex, &( _storage ) );
			_typeIndex = CX_INVALID_VARIANT_INDEX;
		}

	private:
		StorageType _storage;
		variant_index_t _typeIndex;
	};

}

#endif /* __TS3_STDEXT_VARIANT_H__ */
