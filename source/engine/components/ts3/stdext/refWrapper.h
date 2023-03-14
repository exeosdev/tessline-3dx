
#ifndef __TS3_STDEXT_REF_WRAPPER_H__
#define __TS3_STDEXT_REF_WRAPPER_H__

#include "utilities.h"

namespace ts3
{

	template <typename TValue>
	struct RefWrapper
	{
	public:
		constexpr RefWrapper() noexcept
		: _valuePtr( nullptr )
		{}

		RefWrapper( const RefWrapper & pSource ) noexcept
		: _valuePtr( pSource._valuePtr )
		{}

		RefWrapper & operator=( const RefWrapper & pRhs ) noexcept
		{
			_valuePtr = pRhs._valuePtr;
			return *this;
		}

		RefWrapper & operator=( TValue & pRhs ) noexcept
		{
			_valuePtr = &pRhs;
			return *this;
		}

		operator TValue &() const noexcept
		{
			return *_valuePtr;
		}

		TValue * operator->() const noexcept
		{
			return _valuePtr;
		}

		constexpr explicit operator bool() const noexcept
		{
			return _valuePtr != nullptr;
		}

		TS3_ATTR_NO_DISCARD TValue & get() const noexcept
		{
			return *_valuePtr;
		}

		void swap( RefWrapper<TValue> & pOther ) noexcept
		{
			std::swap( _valuePtr, pOther._valuePtr );
		}

	private:
		TValue * _valuePtr;
	};

	template <typename TValue>
	inline constexpr RefWrapper<TValue> makeRef( TValue & pRef ) noexcept
	{
		return RefWrapper<TValue>( pRef );
	}

}

#endif // __TS3_STDEXT_REF_WRAPPER_H__
