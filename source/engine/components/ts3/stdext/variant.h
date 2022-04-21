
#ifndef __TS3_STDEXT_VARIANT_H__
#define __TS3_STDEXT_VARIANT_H__

#include "prerequisites.h"
#include <typeinfo>

namespace stdx
{

	/// @brief Type for storing the Variant type index.
	using variant_index_t = size_t;

	struct VariantTypeConvTag
	{};

	namespace def
	{

		/// @brief Represents invalid Variant type index. Usually indicates, that incompatible type was used as a target.
		constexpr variant_index_t CX_INVALID_VARIANT_INDEX = 0;

		/// @brief Shared instance of `VariantTypeConvTag`. Use it to force desired overload.
		inline constexpr VariantTypeConvTag CX_VARIANT_TYPE_CONV{};

	}

	template <typename _Type>
	struct VariantTypeIndex
	{
		static const variant_index_t value;
	};

	template <typename _Type>
	inline const variant_index_t VariantTypeIndex<_Type>::value = typeid( _Type ).hash_code();

	template <typename... _Types>
	struct VariantStorage
	{
		static constexpr size_t size = StaticMaxSizeofTp<_Types...>::value;
		static constexpr size_t alignment = StaticMaxAlignofTp<_Types...>::value;

		using Type = typename std::aligned_storage<storageSize, storageAlignment>::type;
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
	template <typename... _Types>
	struct VariantProxy;

	template <typename _Type, typename... _Rest>
	struct VariantProxy<_Type, _Rest...>
	{
		using RawType = typename std::remove_reference<typename std::remove_cv<_Type>::type>::type;

		template <typename... _Args>
		static variant_index_t construct( const variant_index_t pTypeIndex, void * pStorage, _Args && ...pArgs )
		{
			if( pTypeIndex == VariantTypeIndex<RawType>::value )
			{
				new ( reinterpret_cast< RawType * >( pStorage ) ) _Type( std::forward<_Args>( pArgs )... );
				return VariantTypeIndex<RawType>::value;
			}
			else
			{
				VariantProxy<_Rest...>::construct( pTypeIndex, pStorage, std::forward<_Args>( pArgs )... );
			}
		}

		template <typename _Value>
		static variant_index_t constructConv( void * pStorage, _Value pValue )
		{
			if( std::is_convertible<_Value, RawType>::value )
			{
				new ( reinterpret_cast< RawType * >( pStorage ) ) RawType( std::forward<_Value>( pValue ) );
				return VariantTypeIndex<RawType>::value;
			}
			else
			{
				VariantProxy<_Rest...>::constructConv( std::forward<_Value>( pValue ) );
			}
		}

		static variant_index_t constructDefault( void * pStorage )
		{
			if( std::isDefault_constructible<RawType>::value )
			{
				new ( reinterpret_cast< RawType * >( pStorage ) ) RawType();
				return VariantTypeIndex<RawType>::value;
			}
			else
			{
				VariantProxy<_Rest...>::constructDefault( pStorage );
			}
		}

		static variant_index_t copy( const variant_index_t pTypeIndex, void * pStorage, void * pSource )
		{
			if( pTypeIndex == VariantTypeIndex<RawType>::value )
			{
				new ( reinterpret_cast< RawType * >( pStorage ) ) RawType( *( reinterpret_cast< RawType * >( pSource ) ) );
				return VariantTypeIndex<RawType>::value;
			}
			else
			{
				VariantProxy<_Rest...>::copy( pTypeIndex, pSource, pStorage );
			}
		}

		static variant_index_t move( const variant_index_t pTypeIndex, void * pStorage, void * pSource )
		{
			if( pTypeIndex == VariantTypeIndex<RawType>::value )
			{
				new ( reinterpret_cast< RawType * >( pStorage ) ) RawType( std::move( *( reinterpret_cast< RawType * >( pSource ) ) ) );
				return VariantTypeIndex<RawType>::value;
			}
			else
			{
				VariantProxy<_Rest...>::move( pTypeIndex, pSource, pStorage );
			}
		}

		static variant_index_t destroy( const variant_index_t pTypeIndex, void * pStorage )
		{
			if( pTypeIndex == VariantTypeIndex<RawType>::value )
			{
				reinterpret_cast< RawType * >( pStorage )->~_Type();
				return VariantTypeIndex<RawType>::value;
			}
			else
			{
				VariantProxy<_Rest...>::destroy( pTypeIndex, pStorage );
			}
		}

		static bool validateType( const variant_index_t pTypeIndex )
		{
			if( pTypeIndex == VariantTypeIndex<RawType>::value )
			{
				return true;
			}
			else
			{
				return VariantProxy<_Rest...>::validateType( pTypeIndex );
			}
		}

		template <typename _Storage>
		static void swap( const variant_index_t pFirstIndex, void * pFirstStorage, const variant_index_t pSecondIndex, void * pSecondStorage )
		{
			_Storage temporaryStorage;

			move( pFirstIndex, &temporaryStorage, pFirstStorage );
			move( pSecondIndex, pFirstStorage, pSecondStorage );
			move( pFirstIndex, pSecondStorage, &temporaryStorage );
		}
	};

	template <>
	struct VariantProxy<>
	{
		template <typename... _Args>
		static variant_index_t construct( variant_index_t, void *, _Args && ... )
		{
			return def::CX_INVALID_VARIANT_INDEX;
		}

		template <typename _Value>
		static variant_index_t constructConv( variant_index_t, void *, _Value && )
		{
			return def::CX_INVALID_VARIANT_INDEX;
		}

		static variant_index_t constructDefault( void * )
		{
			return def::CX_INVALID_VARIANT_INDEX;
		}

		static variant_index_t copy( variant_index_t, void *, void * )
		{
			return def::CX_INVALID_VARIANT_INDEX;
		}

		static variant_index_t move( variant_index_t, void *, void * )
		{
			return def::CX_INVALID_VARIANT_INDEX;
		}

		static variant_index_t destroy( variant_index_t, void * )
		{
			return def::CX_INVALID_VARIANT_INDEX;
		}

		static bool validateType( variant_index_t )
		{
			return false;
		}
	};


	template <typename... _Types>
	class Variant
	{
	public:
		static constexpr size_t storageSize = VariantStorage<_Types...>::size;
		static constexpr size_t storageAlignment = VariantStorage<_Types...>::alignment;

		using MyType = Variant<_Types...>;
		using StorageType = typename VariantStorage<_Types...>::Type;

	public:
		Variant()
		{
			_typeIndex = VariantProxy<_Types...>::constructDefault( &( _storage ) );
		}

		Variant( Variant && pSource )
		{
			_typeIndex = VariantProxy<_Types...>::move( pSource._typeIndex, &( _storage ), &( pSource._pStorage ) );
		}

		Variant( const Variant & pSource )
		{
			_typeIndex = VariantProxy<_Types...>::copy( pSource._typeIndex, &( _storage ), &( pSource._pStorage ) );
		}

		template <typename _Value>
		Variant( _Value && pValue )
		{
			_typeIndex = VariantProxy<_Types...>::construct( VariantTypeIndex<_Value>::value, &( _storage ), std::move( pValue ) );
		}

		template <typename _Value>
		Variant( const _Value & pValue )
		{
			_typeIndex = VariantProxy<_Types...>::construct( VariantTypeIndex<_Value>::value, &( _storage ), pValue );
		}

		template <typename _Value>
		Variant( _Value && pValue, const VariantTypeConvTag & )
		{
			_typeIndex = VariantProxy<_Types...>::constructConv( &( _storage ), std::move( pValue ) );
		}

		template <typename _Value>
		Variant( const _Value & pValue, const VariantTypeConvTag & )
		{
			_typeIndex = VariantProxy<_Types...>::constructConv( &( _storage ), pValue );
		}

		~Variant()
		{
			_release();
		}

		Variant & operator=( Variant && pRhs )
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

		template <typename _Target>
		Variant & operator=( _Target && pRhs )
		{
			_reinit<_Target>( std::move( pRhs ) );
			return *this;
		}

		template <typename _Target>
		Variant & operator=( const _Target & pRhs )
		{
			_reinit<_Target>( pRhs );
			return *this;
		}

		template <typename _Target>
		void set( _Target && pValue )
		{
			_reinit<_Target>( std::move( pValue ) );
		}

		template <typename _Target>
		void set( const _Target & pValue )
		{
			_reinit<_Target>( pValue );
		}

		template <typename _Target>
		void setConv( _Target && pValue )
		{
			_reinitConv<_Target>( std::move( pValue ) );
		}

		template <typename _Target>
		void setConv( const _Target & pValue )
		{
			_reinitConv<_Target>( pValue );
		}

		template <typename _Target, typename... _Args>
		void emplace( _Args &&... pArgs )
		{
			_reinit<_Target>( std::forward<_Args>( pArgs )... );
		}

		void swap( Variant & pOther )
		{
			VariantProxy<_Types...>::swap( _typeIndex, &( _storage ), pOther._typeIndex, &( pOther._storage ) );
			std::swap( _typeIndex, pOther._typeIndex );
		}

		template <typename _Target>
		bool check_type() const
		{
			return _typeIndex == VariantTypeIndex<_Target>::value;
		}

		template <typename _Target>
		_Target & get() const
		{
			_validate<_Target>();
			return *( reinterpret_cast< _Target * >( &( _storage ) ) );
		}

	private:
		template <typename _Target, typename... _Args>
		void _reinit( _Args && ...pArgs )
		{
			VariantProxy<_Types...>::destroy( _typeIndex, &( _storage ) );
			_typeIndex = VariantProxy<_Types...>::construct( VariantTypeIndex<_Target>::value, &( _storage ), std::forward<_Args>( pArgs )... );
		}

		template <typename _Value>
		void _reinitConv( _Value pValue )
		{
			VariantProxy<_Types...>::destroy( _typeIndex, &( _storage ) );
			_typeIndex = VariantProxy<_Types...>::constructConv( &( _storage ), std::forward<_Value>( pValue ) );
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
			VariantProxy<_Types...>::destroy( _typeIndex, &( _storage ) );
			_typeIndex = def::CX_INVALID_VARIANT_INDEX;
		}

	private:
		StorageType _storage;
		variant_index_t _typeIndex;
	};

}

#endif /* __TS3_STDEXT_VARIANT_H__ */
