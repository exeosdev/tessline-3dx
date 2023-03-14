
#ifndef __TS3_STDEXT_INTRUSIVE_PTR_H__
#define __TS3_STDEXT_INTRUSIVE_PTR_H__

#include "refCounter.h"
#include <cassert>
#include <functional>

namespace ts3
{

	template <typename TCounter = RefCounter>
	class RefCountedBase
	{
		friend struct RefCountedProxy;

	public:
		using MyType = RefCountedBase<TCounter>;
		using CounterType = TCounter;

	public:
		RefCountedBase( const RefCountedBase & ) = delete;
		RefCountedBase & operator=( const RefCountedBase & ) = delete;

		RefCountedBase() = default;

	protected:
		ref_counter_value_t addRef() const
		{
			return _counter.increment();
		}

		ref_counter_value_t releaseRef() const
		{
			return _counter.decrement();
		}

		ref_counter_value_t getRefsNum() const
		{
			return _counter.getValue();
		}

	private:
		mutable CounterType _counter;
	};


	struct RefCountedProxy
	{
		template <typename TCounter>
		static ref_counter_value_t addRef( const RefCountedBase<TCounter> * pRefCountedBase )
		{
			return pRefCountedBase->addRef();
		}

		template <typename TCounter>
		static ref_counter_value_t releaseRef( const RefCountedBase<TCounter> * pRefCountedBase )
		{
			return pRefCountedBase->releaseRef();
		}
	};


	template <typename TType>
	inline ref_counter_value_t addRefDefault( const TType * pObjectPtr )
	{
		return RefCountedProxy::addRef( pObjectPtr );
	}

	template <typename TType>
	inline ref_counter_value_t releaseRefDefault( const TType * pObjectPtr )
	{
		return RefCountedProxy::releaseRef( pObjectPtr );
	}


	template <typename TType, typename TDeleter = DefaultDelete<TType>>
	class IntrusivePtr
	{
		template <typename, typename>
		friend class IntrusivePtr;

	public:
		using MyType = IntrusivePtr<TType, TDeleter>;

	public:
		IntrusivePtr()
			: _ptr( nullptr )
		{}

		IntrusivePtr( IntrusivePtr && pSrcObject )
			: _ptr( nullptr )
		{
			swap( pSrcObject );
		}

		IntrusivePtr( const IntrusivePtr & pSrcObject )
			: _ptr( nullptr )
		{
			_setNoRelease( pSrcObject._ptr );
		}

		template <typename TOther>
		IntrusivePtr( IntrusivePtr<TOther> && pOther )
			: _ptr( static_cast<TType *>( pOther._ptr ) )
		{
			pOther._ptr = nullptr;
		}

		template <typename TOther>
		IntrusivePtr( const IntrusivePtr<TOther> & pOther )
			: _ptr( nullptr )
		{
			_setNoRelease( pOther._ptr );
		}

		IntrusivePtr( std::nullptr_t )
			: _ptr( nullptr )
		{}

		explicit IntrusivePtr( TType * pObject )
			: _ptr( pObject )
		{}

		template <typename TOther>
		explicit IntrusivePtr( TOther * pObject )
			: _ptr( static_cast<TType *>( pObject ) )
		{}

		~IntrusivePtr()
		{
			_release();
		}

		IntrusivePtr & operator=( IntrusivePtr && pRhs )
		{
			if( this != &pRhs )
			{
				IntrusivePtr( std::move( pRhs ) ).swap( *this );
			}

			return *this;
		}

		IntrusivePtr & operator=( const IntrusivePtr & pRhs )
		{
			if( this != &pRhs )
			{
				IntrusivePtr( pRhs ).swap( *this );
			}

			return *this;
		}

		template <typename TOther>
		IntrusivePtr & operator=( IntrusivePtr<TOther> && pRhs )
		{
			IntrusivePtr( std::move( pRhs ) ).swap( *this );
			return *this;
		}

		template <typename TOther>
		IntrusivePtr & operator=( const IntrusivePtr<TOther> & pRhs )
		{
			IntrusivePtr( pRhs ).swap( *this );
			return *this;
		}

		IntrusivePtr & operator=( std::nullptr_t )
		{
			_release();
			return *this;
		}

		IntrusivePtr & operator=( TType * pRhs )
		{
			IntrusivePtr( pRhs ).swap( *this );
			return *this;
		}

		template <typename TOther>
		IntrusivePtr & operator=( TOther * pRhs )
		{
			IntrusivePtr( pRhs ).swap( *this );
			return *this;
		}

		explicit operator bool() const
		{
			return _ptr != nullptr;
		}

		explicit operator TType * ( ) const
		{
			return _ptr;
		}

		TType & operator*() const
		{
			assert( _ptr != nullptr );
			return *( _ptr );
		}

		TType * operator->() const
		{
			assert( _ptr != nullptr );
			return _ptr;
		}

		TType * get() const
		{
			return _ptr;
		}

		void reset( TType * pPointer = nullptr )
		{
			_set( pPointer );
		}

		void swap( IntrusivePtr & pOther )
		{
			std::swap( _ptr, pOther._ptr );
		}

		bool empty() const
		{
			return _ptr != nullptr;
		}

	private:
		void _release()
		{
			if( _ptr != nullptr )
			{
				_releasePtr( _ptr );
				_ptr = nullptr;
			}
		}

		void _setNoRelease( TType * pPointer )
		{
			if( ( pPointer != nullptr ) && ( addRefDefault( pPointer ) > 0 ) )
			{
				_ptr = pPointer;
			}
			else
			{
				_ptr = nullptr;
			}
		}

		template <typename TOther>
		void _setNoRelease( TOther * pPointer )
		{
			_setNoRelease( static_cast<TType *>( pPointer ) );
		}

		void _set( TType * pPointer )
		{
			if( pPointer != _ptr )
			{
				TType * prevPtr = _ptr;
				_setNoRelease( pPointer );
				if( prevPtr != nullptr )
				{
					_releasePtr( prevPtr );
				}
			}
		}

		template <typename TOther>
		void _set( TOther * pPointer )
		{
			_set( static_cast<TType *>( pPointer ) );
		}

		static void _releasePtr( TType * pPointer )
		{
			ref_counter_value_t remainingRefs = releaseRefDefault( pPointer );
			if( remainingRefs == 0 )
			{
				TDeleter()( pPointer );
			}
		}

	private:
		TType * _ptr;
	};

	template <typename TType, typename TDeleter, typename TOther, typename TOtherdel>
	inline bool operator==( const IntrusivePtr<TType, TDeleter> & pLhs, const IntrusivePtr<TOther, TOtherdel> & pRhs )
	{
		return pLhs.get() == pRhs.get();
	}

	template <typename TType, typename TDeleter>
	inline bool operator==( const IntrusivePtr<TType, TDeleter> & pLhs, std::nullptr_t )
	{
		return pLhs.empty();
	}

	template <typename TType, typename TDeleter>
	inline bool operator==( std::nullptr_t, const IntrusivePtr<TType, TDeleter> & pRhs )
	{
		return pRhs.empty();
	}

	template <typename TType, typename TDeleter, typename TOther, typename TOtherdel>
	inline bool operator!=( const IntrusivePtr<TType, TDeleter> & pLhs, const IntrusivePtr<TOther, TOtherdel> & pRhs )
	{
		return pLhs.get() != pRhs.get();
	}

	template <typename TType, typename TDeleter>
	inline bool operator!=( const IntrusivePtr<TType, TDeleter> & pLhs, std::nullptr_t )
	{
		return !pLhs.empty();
	}

	template <typename TType, typename TDeleter>
	inline bool operator!=( std::nullptr_t, const IntrusivePtr<TType, TDeleter> & pRhs )
	{
		return !pRhs.empty();
	}

	template <typename TType, typename TDeleter, typename TOther, typename TOtherdel>
	inline bool operator>( const IntrusivePtr<TType, TDeleter> & pLhs, const IntrusivePtr<TOther, TOtherdel> & pRhs )
	{
		return pLhs.get() > pRhs.get();
	}

	template <typename TType, typename TDeleter, typename TOther, typename TOtherdel>
	inline bool operator>=( const IntrusivePtr<TType, TDeleter> & pLhs, const IntrusivePtr<TOther, TOtherdel> & pRhs )
	{
		return pLhs.get() >= pRhs.get();
	}

	template <typename TType, typename TDeleter, typename TOther, typename TOtherdel>
	inline bool operator<( const IntrusivePtr<TType, TDeleter> & pLhs, const IntrusivePtr<TOther, TOtherdel> & pRhs )
	{
		return pLhs.get() < pRhs.get();
	}

	template <typename TType, typename TDeleter, typename TOther, typename TOtherdel>
	inline bool operator<=( const IntrusivePtr<TType, TDeleter> & pLhs, const IntrusivePtr<TOther, TOtherdel> & pRhs )
	{
		return pLhs.get() <= pRhs.get();
	}


	template <typename TType, typename TDeleter>
	void swap( IntrusivePtr<TType, TDeleter> & pLeft, IntrusivePtr<TType, TDeleter> & pRight )
	{
		pLeft.swap( pRight );
	}


	template <typename TType, class ..._Args>
	inline IntrusivePtr<TType> makeIntrusive( _Args && ...pArgs )
	{
		return IntrusivePtr<TType>( new TType( std::forward<_Args>( pArgs )... ) );
	}

}

#endif /* __TS3_STDEXT_INTRUSIVE_PTR_H__ */
