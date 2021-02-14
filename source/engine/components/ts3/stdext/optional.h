
#ifndef __TS3_STDEXT_OPTIONAL_H__
#define __TS3_STDEXT_OPTIONAL_H__

#include "static_algo.h"

namespace ts3
{

	template <typename _Type>
	struct OptionalStorage
	{
		static constexpr size_t size = sizeof( _Type );
		static constexpr size_t alignment = alignof( _Type );

		using Type = typename std::aligned_storage<size, alignment>::type;
	};

	template <typename _Type>
	class Optional
	{
	public:
		static constexpr size_t storageSize = OptionalStorage<_Type>::size;
		static constexpr size_t storageAlignment = OptionalStorage<_Type>::alignment;

		using ValueType = _Type;
		using MyType = Optional<_Type>;
		using StorageType = typename OptionalStorage<_Type>::Type;

	public:
		Optional()
			: _valuePtr( nullptr )
		{}

		Optional( _Type && pInput )
		{
			_init( std::move( pInput ) );
		}

		Optional( const _Type & pInput )
		{
			_init( pInput );
		}

		~Optional()
		{
			_release();
		}

		Optional & operator=( Optional && pRhs )
		{
			Optional( std::move( pRhs ) ).swap( *this );
			return *this;
		}

		Optional & operator=( const Optional & pRhs )
		{
			Optional( pRhs ).swap( *this );
			return *this;
		}

		Optional & operator=( _Type && pRhs )
		{
			_reinit( std::move( pRhs ) );
			return *this;
		}

		Optional & operator=( const _Type & pRhs )
		{
			_reinit( pRhs );
			return *this;
		}

		operator bool() const
		{
			return _check();
		}

		_Type& operator*() const
		{
			_validate();
			return *(_valuePtr);
		}

		_Type* operator->() const
		{
			_validate();
			return _valuePtr;
		}

		void set( _Type && value )
		{
			_reinit( std::move( value ) );
		}

		void set( const _Type & value )
		{
			_reinit( value );
		}

		template <typename... Args>
		void emplace( Args && ...pArgs )
		{
			_reinit( std::forward<Args>( pArgs )... );
		}

		void reset()
		{
			_release();
		}

		bool empty() const
		{
			return !_check();
		}

		const _Type & value() const
		{
			_validate();
			return *(_valuePtr);
		}

		const _Type & valueOrDefault( const _Type & pDefault ) const
		{
			return _check() ? *(_valuePtr) : pDefault;
		}

		void swap( MyType & other )
		{
			std::swap( *(_valuePtr), *( other._valuePtr ) );
		}

	private:
		template <typename... Args>
		void _init( Args && ...pArgs )
		{
			_valuePtr = reinterpret_cast<_Type*>( &_storage );
			new (_valuePtr) _Type( std::forward<Args>( pArgs )... );
		}

		template <typename... Args>
		void _reinit( Args && ...pArgs )
		{
			if (_valuePtr != nullptr)
			{
				_valuePtr->~_Type();
			}
			else
			{
				_valuePtr = reinterpret_cast<_Type*>( &_storage );
			}

			new (_valuePtr) _Type( std::forward<Args>( pArgs )... );
		}

		bool _check() const
		{
			return _valuePtr != nullptr;
		}

		void _validate() const
		{
			if (_valuePtr == nullptr)
			{
				throw 0;
			}
		}

		void _release()
		{
			if (_valuePtr != nullptr)
			{
				_valuePtr->~_Type();
				_valuePtr = nullptr;
			}
		}

	private:
		StorageType _storage;
		_Type * _valuePtr;
	};

}

#endif /* __TS3_STDEXT_OPTIONAL_H__ */
