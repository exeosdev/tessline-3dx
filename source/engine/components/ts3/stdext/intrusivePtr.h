
#ifndef __TS3_STDEXT_INTRUSIVE_PTR_H__
#define __TS3_STDEXT_INTRUSIVE_PTR_H__

#include "refCounter.h"
#include <cassert>
#include <functional>

namespace ts3
{

	template <typename TpType>
	struct ArrayDelete
	{
		void operator()( TpType * pInputPtr ) const
		{
			delete[] pInputPtr;
		}
	};

	template <typename TpType>
	struct DefaultDelete
	{
		void operator()( TpType * pInputPtr ) const
		{
			delete pInputPtr;
		}
	};

	template <typename TpType>
	struct EmptyDelete
	{
		void operator()( TpType * ) const
		{}
	};


	template <typename TpCounter = RefCounter>
	class RefCountedBase
	{
		friend struct RefCountedProxy;

	public:
		using MyType = RefCountedBase<TpCounter>;
		using CounterType = TpCounter;

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
		template <typename TpCounter>
		static ref_counter_value_t addRef( const RefCountedBase<TpCounter> * pRefCountedBase )
		{
			return pRefCountedBase->addRef();
		}

		template <typename TpCounter>
		static ref_counter_value_t releaseRef( const RefCountedBase<TpCounter> * pRefCountedBase )
		{
			return pRefCountedBase->releaseRef();
		}
	};


	template <typename TpType>
	inline ref_counter_value_t addRefDefault( const TpType * pObjectPtr )
	{
		return RefCountedProxy::addRef( pObjectPtr );
	}

	template <typename TpType>
	inline ref_counter_value_t releaseRefDefault( const TpType * pObjectPtr )
	{
		return RefCountedProxy::releaseRef( pObjectPtr );
	}


	template <typename TpType, typename TpDeleter = DefaultDelete<TpType>>
	class IntrusivePtr
	{
		template <typename, typename>
		friend class IntrusivePtr;

	public:
		using MyType = IntrusivePtr<TpType, TpDeleter>;

	public:
		IntrusivePtr()
			: _ptr( nullptr )
		{}

		IntrusivePtr( IntrusivePtr && pSource )
			: _ptr( nullptr )
		{
			swap( pSource );
		}

		IntrusivePtr( const IntrusivePtr & pSource )
			: _ptr( nullptr )
		{
			_setNoRelease( pSource._ptr );
		}

		template <typename TpOther>
		IntrusivePtr( IntrusivePtr<TpOther> && pOther )
			: _ptr( static_cast<TpType *>( pOther._ptr ) )
		{
			pOther._ptr = nullptr;
		}

		template <typename TpOther>
		IntrusivePtr( const IntrusivePtr<TpOther> & pOther )
			: _ptr( nullptr )
		{
			_setNoRelease( pOther._ptr );
		}

		IntrusivePtr( std::nullptr_t )
			: _ptr( nullptr )
		{}

		explicit IntrusivePtr( TpType * pObject )
			: _ptr( pObject )
		{}

		template <typename TpOther>
		explicit IntrusivePtr( TpOther * pObject )
			: _ptr( static_cast<TpType *>( pObject ) )
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

		template <typename TpOther>
		IntrusivePtr & operator=( IntrusivePtr<TpOther> && pRhs )
		{
			IntrusivePtr( std::move( pRhs ) ).swap( *this );
			return *this;
		}

		template <typename TpOther>
		IntrusivePtr & operator=( const IntrusivePtr<TpOther> & pRhs )
		{
			IntrusivePtr( pRhs ).swap( *this );
			return *this;
		}

		IntrusivePtr & operator=( std::nullptr_t )
		{
			_release();
			return *this;
		}

		IntrusivePtr & operator=( TpType * pRhs )
		{
			IntrusivePtr( pRhs ).swap( *this );
			return *this;
		}

		template <typename TpOther>
		IntrusivePtr & operator=( TpOther * pRhs )
		{
			IntrusivePtr( pRhs ).swap( *this );
			return *this;
		}

		explicit operator bool() const
		{
			return _ptr != nullptr;
		}

		explicit operator TpType * ( ) const
		{
			return _ptr;
		}

		TpType & operator*() const
		{
			assert( _ptr != nullptr );
			return *( _ptr );
		}

		TpType * operator->() const
		{
			assert( _ptr != nullptr );
			return _ptr;
		}

		TpType * get() const
		{
			return _ptr;
		}

		void reset( TpType * pPointer = nullptr )
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

		void _setNoRelease( TpType * pPointer )
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

		template <typename TpOther>
		void _setNoRelease( TpOther * pPointer )
		{
			_setNoRelease( static_cast<TpType *>( pPointer ) );
		}

		void _set( TpType * pPointer )
		{
			if( pPointer != _ptr )
			{
				TpType * prevPtr = _ptr;
				_setNoRelease( pPointer );
				if( prevPtr != nullptr )
				{
					_releasePtr( prevPtr );
				}
			}
		}

		template <typename TpOther>
		void _set( TpOther * pPointer )
		{
			_set( static_cast<TpType *>( pPointer ) );
		}

		static void _releasePtr( TpType * pPointer )
		{
			ref_counter_value_t remainingRefs = releaseRefDefault( pPointer );
			if( remainingRefs == 0 )
			{
				TpDeleter()( pPointer );
			}
		}

	private:
		TpType * _ptr;
	};

	template <typename TpType, typename TpDeleter, typename TpOther, typename TpOtherdel>
	inline bool operator==( const IntrusivePtr<TpType, TpDeleter> & pLhs, const IntrusivePtr<TpOther, TpOtherdel> & pRhs )
	{
		return pLhs.get() == pRhs.get();
	}

	template <typename TpType, typename TpDeleter>
	inline bool operator==( const IntrusivePtr<TpType, TpDeleter> & pLhs, std::nullptr_t )
	{
		return pLhs.empty();
	}

	template <typename TpType, typename TpDeleter>
	inline bool operator==( std::nullptr_t, const IntrusivePtr<TpType, TpDeleter> & pRhs )
	{
		return pRhs.empty();
	}

	template <typename TpType, typename TpDeleter, typename TpOther, typename TpOtherdel>
	inline bool operator!=( const IntrusivePtr<TpType, TpDeleter> & pLhs, const IntrusivePtr<TpOther, TpOtherdel> & pRhs )
	{
		return pLhs.get() != pRhs.get();
	}

	template <typename TpType, typename TpDeleter>
	inline bool operator!=( const IntrusivePtr<TpType, TpDeleter> & pLhs, std::nullptr_t )
	{
		return !pLhs.empty();
	}

	template <typename TpType, typename TpDeleter>
	inline bool operator!=( std::nullptr_t, const IntrusivePtr<TpType, TpDeleter> & pRhs )
	{
		return !pRhs.empty();
	}

	template <typename TpType, typename TpDeleter, typename TpOther, typename TpOtherdel>
	inline bool operator>( const IntrusivePtr<TpType, TpDeleter> & pLhs, const IntrusivePtr<TpOther, TpOtherdel> & pRhs )
	{
		return pLhs.get() > pRhs.get();
	}

	template <typename TpType, typename TpDeleter, typename TpOther, typename TpOtherdel>
	inline bool operator>=( const IntrusivePtr<TpType, TpDeleter> & pLhs, const IntrusivePtr<TpOther, TpOtherdel> & pRhs )
	{
		return pLhs.get() >= pRhs.get();
	}

	template <typename TpType, typename TpDeleter, typename TpOther, typename TpOtherdel>
	inline bool operator<( const IntrusivePtr<TpType, TpDeleter> & pLhs, const IntrusivePtr<TpOther, TpOtherdel> & pRhs )
	{
		return pLhs.get() < pRhs.get();
	}

	template <typename TpType, typename TpDeleter, typename TpOther, typename TpOtherdel>
	inline bool operator<=( const IntrusivePtr<TpType, TpDeleter> & pLhs, const IntrusivePtr<TpOther, TpOtherdel> & pRhs )
	{
		return pLhs.get() <= pRhs.get();
	}


	template <typename TpType, typename TpDeleter>
	void swap( IntrusivePtr<TpType, TpDeleter> & pLeft, IntrusivePtr<TpType, TpDeleter> & pRight )
	{
		pLeft.swap( pRight );
	}


	template <typename TpType, class ..._Args>
	inline IntrusivePtr<TpType> makeIntrusive( _Args && ...pArgs )
	{
		return IntrusivePtr<TpType>( new TpType( std::forward<_Args>( pArgs )... ) );
	}

}

#endif /* __TS3_STDEXT_INTRUSIVE_PTR_H__ */
