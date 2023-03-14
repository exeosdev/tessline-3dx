
#ifndef __TS3_MATH_VECTOR_OPS_H__
#define __TS3_MATH_VECTOR_OPS_H__

#include "arrayOps.h"
#include "vector.h"

namespace ts3::math
{

	template <typename TVal, size_t tSize>
	inline void add( const Vector<TVal, tSize> & pFirst, const Vector<TVal, tSize> & pSecond, Vector<TVal, tSize> & pResult )
	{
		arrayOps::add( pFirst.values, pSecond.values, pResult.values );
	}

	template <typename TVal, size_t tSize>
	inline Vector<TVal, tSize> add( const Vector<TVal, tSize> & pFirst, const Vector<TVal, tSize> & pSecond )
	{
		Vector<TVal, tSize> result;
		arrayOps::add( pFirst.values, pSecond.values, result.values );
		return result;
	}

	template <typename TVal, size_t tSize>
	inline void sub( const Vector<TVal, tSize> & pFirst, const Vector<TVal, tSize> & pSecond, Vector<TVal, tSize> & pResult )
	{
		arrayOps::add( pFirst.values, pSecond.values, pResult.values );
	}

	template <typename TVal, size_t tSize>
	inline Vector<TVal, tSize> sub( const Vector<TVal, tSize> & pFirst, const Vector<TVal, tSize> & pSecond )
	{
		Vector<TVal, tSize> result;
		arrayOps::sub( pFirst.values, pSecond.values, result.values );
		return result;
	}

	template <typename TVal, size_t tSize>
	inline void mul( const Vector<TVal, tSize> & pFirst, const Vector<TVal, tSize> & pSecond, Vector<TVal, tSize> & pResult )
	{
		arrayOps::mul( pFirst.values, pSecond.values, pResult.values );
	}

	template <typename TVal, size_t tSize, typename TScalar, enable_if_scalar_t<TScalar> = true>
	inline void mul( const Vector<TVal, tSize> & pVector, const TScalar pScalar, Vector<TVal, tSize> & pResult )
	{
		arrayOps::mul( pVector.values, pScalar, pResult.values );
	}

	template <typename TVal, size_t tSize>
	inline Vector<TVal, tSize> mul( const Vector<TVal, tSize> & pFirst, const Vector<TVal, tSize> & pSecond )
	{
		Vector<TVal, tSize> result;
		arrayOps::mul( pFirst.values, pSecond.values, result.values );
		return result;
	}

	template <typename TVal, size_t tSize, typename TScalar, enable_if_scalar_t<TScalar> = true>
	inline Vector<TVal, tSize> mul( const Vector<TVal, tSize> & pVector, const TScalar pScalar )
	{
		Vector<TVal, tSize> result;
		arrayOps::mul( pVector.values, pScalar, result.values );
		return result;
	}

	template <typename TVal, size_t tSize>
	inline void div( const Vector<TVal, tSize> & pFirst, const Vector<TVal, tSize> & pSecond, Vector<TVal, tSize> & pResult )
	{
		arrayOps::div( pFirst.values, pSecond.values, pResult.values );
	}

	template <typename TVal, size_t tSize, typename TScalar, enable_if_scalar_t<TScalar> = true>
	inline void div( const Vector<TVal, tSize> & pVector, const TScalar pScalar, Vector<TVal, tSize> & pResult )
	{
		arrayOps::div( pVector.values, pScalar, pResult.values );
	}

	template <typename TVal, size_t tSize, typename TScalar, enable_if_scalar_t<TScalar> = true>
	inline void div( const TScalar pScalar, const Vector<TVal, tSize> & pVector, Vector<TVal, tSize> & pResult )
	{
		arrayOps::div( pScalar, pVector.values, pResult.values );
	}

	template <typename TVal, size_t tSize>
	inline Vector<TVal, tSize> div( const Vector<TVal, tSize> & pFirst, const Vector<TVal, tSize> & pSecond )
	{
		Vector<TVal, tSize> result;
		arrayOps::div( pFirst.values, pSecond.values, result.values );
		return result;
	}

	template <typename TVal, size_t tSize, typename TScalar, enable_if_scalar_t<TScalar> = true>
	inline Vector<TVal, tSize> div( const Vector<TVal, tSize> & pVector, const TScalar pScalar )
	{
		Vector<TVal, tSize> result;
		arrayOps::div( pVector.values, pScalar, result.values );
		return result;
	}

	template <typename TVal, size_t tSize, typename TScalar, enable_if_scalar_t<TScalar> = true>
	inline Vector<TVal, tSize> div( const TScalar pScalar, const Vector<TVal, tSize> & pVector )
	{
		Vector<TVal, tSize> result;
		arrayOps::div( pScalar, pVector.values, result.values );
		return result;
	}

	template <typename TVal, size_t tSize>
	inline void neg( const Vector<TVal, tSize> & pVector, Vector<TVal, tSize> & pResult )
	{
		arrayOps::neg( pVector.values, pResult.values );
	}

	template <typename TVal, size_t tSize>
	inline Vector<TVal, tSize> neg( const Vector<TVal, tSize> & pVector )
	{
		Vector<TVal, tSize> result;
		arrayOps::neg( pVector.values, result.values );
		return result;
	}

	template <typename TVal>
	inline void cross( const Vector3<TVal> & pFirst, const Vector3<TVal> & pSecond, Vector3<TVal> & pResult )
	{
		arrayOps::cross( pFirst.values, pSecond.values, pResult.values );
	}

	template <typename TVal>
	inline Vector3<TVal> cross( const Vector3<TVal> & pFirst, const Vector3<TVal> & pSecond )
	{
		Vector3<TVal> result;
		arrayOps::cross( pFirst.values, pSecond.values, result.values );
		return result;
	}

	template <typename TVal, size_t tSize>
	inline TVal dot( const Vector<TVal, tSize> & pFirst, const Vector<TVal, tSize> & pSecond )
	{
		return arrayOps::dot( pFirst.values, pSecond.values );
	}

	template <typename TVal, size_t tSize>
	inline void normalize( const Vector<TVal, tSize> & pVector, Vector<TVal, tSize> & pResult )
	{
		arrayOps::normalize( pVector.values, pResult.values );
	}

	template <typename TVal, size_t tSize>
	inline Vector<TVal, tSize> normalize( const Vector<TVal, tSize> & pVector )
	{
		Vector<TVal, tSize> result;
		arrayOps::normalize( pVector.values, result.values );
		return result;
	}

	template <typename TVal, size_t tSize>
	inline void normalizeInplace( Vector<TVal, tSize> & pVector )
	{
		arrayOps::normalizeInplace( pVector.values );
	}

	template <typename TVal, size_t tSize>
	inline TVal length( const Vector<TVal, tSize> & pVector )
	{
		return static_cast<TVal>( arrayOps::length( pVector.values ) );
	}

	template <typename TResult, typename TVal, size_t tSize>
	inline TResult lengthAs( const Vector<TVal, tSize> & pVector )
	{
		return static_cast<TResult>( length( pVector ) );
	}

	template <typename TVal, size_t tSize>
	inline bool cmpEqual( const Vector<TVal, tSize> & pFirst, const Vector<TVal, tSize> & pSecond )
	{
		return arrayOps::cmpEqual( pFirst.values, pSecond.values );
	}

	template <typename TVal, size_t tSize>
	inline bool cmpNotEqual( const Vector<TVal, tSize> & pFirst, const Vector<TVal, tSize> & pSecond )
	{
		return arrayOps::cmpNotEqual( pFirst.values, pSecond.values );
	}

	template <typename TVal, size_t tSize>
	inline bool cmpLess( const Vector<TVal, tSize> & pFirst, const Vector<TVal, tSize> & pSecond )
	{
		return arrayOps::cmpLess( pFirst.values, pSecond.values );
	}


	template <typename TVal, size_t tSize>
	inline bool operator==( const Vector<TVal, tSize> & pLhs, const Vector<TVal, tSize> & pRhs ) noexcept
	{
		return cmpEqual( pLhs, pRhs );
	}

	template <typename TVal, size_t tSize>
	inline bool operator!=( const Vector<TVal, tSize> & pLhs, const Vector<TVal, tSize> & pRhs ) noexcept
	{
		return cmpNotEqual( pLhs, pRhs );
	}

	template <typename TVal, size_t tSize>
	inline Vector<TVal, tSize> operator+( const Vector<TVal, tSize> & pLhs, const Vector<TVal, tSize> & pRhs ) noexcept
	{
		return add( pLhs, pRhs );
	}

	template <typename TVal, size_t tSize>
	inline Vector<TVal, tSize> operator-( const Vector<TVal, tSize> & pLhs, const Vector<TVal, tSize> & pRhs ) noexcept
	{
		return sub( pLhs, pRhs );
	}

	template <typename TVal, size_t tSize>
	inline Vector<TVal, tSize> operator*( const Vector<TVal, tSize> & pLhs, const Vector<TVal, tSize> & pRhs ) noexcept
	{
		return mul( pLhs, pRhs );
	}

	template <typename TVal, size_t tSize, typename TScalar, enable_if_scalar_t<TScalar> = true>
	inline Vector<TVal, tSize> operator*( const Vector<TVal, tSize> & pLhs, const TScalar pRhs ) noexcept
	{
		return mul( pLhs, pRhs );
	}

	template <typename TVal, size_t tSize, typename TScalar, enable_if_scalar_t<TScalar> = true>
	inline Vector<TVal, tSize> operator*( const TScalar pLhs, const Vector<TVal, tSize> & pRhs ) noexcept
	{
		return mul( pRhs, pLhs );
	}

	template <typename TVal, size_t tSize>
	inline Vector<TVal, tSize> operator/( const Vector<TVal, tSize> & pLhs, const Vector<TVal, tSize> & pRhs ) noexcept
	{
		return div( pLhs, pRhs );
	}

	template <typename TVal, size_t tSize, typename TScalar, enable_if_scalar_t<TScalar> = true>
	inline Vector<TVal, tSize> operator/( const Vector<TVal, tSize> & pLhs, const TScalar pRhs ) noexcept
	{
		return div( pLhs, pRhs );
	}

	template <typename TVal, size_t tSize, typename TScalar, enable_if_scalar_t<TScalar> = true>
	inline Vector<TVal, tSize> operator/( const TScalar pLhs, const Vector<TVal, tSize> & pRhs ) noexcept
	{
		return div( pLhs, pRhs );
	}

	template <typename TVal, size_t tSize>
	inline Vector<TVal, tSize> operator-( const Vector<TVal, tSize> & pRhs ) noexcept
	{
		return neg( pRhs );
	}


	template <typename TVal, size_t tSize>
	inline Vector<TVal, tSize> & operator+=( Vector<TVal, tSize> & pLhs, const Vector<TVal, tSize> & pRhs ) noexcept
	{
		pLhs = add( pLhs, pRhs );
		return pLhs;
	}

	template <typename TVal, size_t tSize>
	inline Vector<TVal, tSize> & operator-=( Vector<TVal, tSize> & pLhs, const Vector<TVal, tSize> & pRhs ) noexcept
	{
		pLhs = sub( pLhs, pRhs );
		return pLhs;
	}

	template <typename TVal, size_t tSize, typename TScalar, enable_if_scalar_t<TScalar> = true>
	inline Vector<TVal, tSize> & operator*=( Vector<TVal, tSize> & pLhs, const TScalar pRhs ) noexcept
	{
		pLhs = mul( pLhs, pRhs );
		return pLhs;
	}

	template <typename TVal, size_t tSize, typename TScalar, enable_if_scalar_t<TScalar> = true>
	inline Vector<TVal, tSize> & operator/=( Vector<TVal, tSize> & pLhs, const TScalar pRhs ) noexcept
	{
		pLhs = div( pLhs, pRhs );
		return pLhs;
	}

}

#endif // __TS3_MATH_VECTOR_OPS_H__
