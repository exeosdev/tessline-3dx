
#ifndef __TS3_MATH_VECTOR_OPS_H__
#define __TS3_MATH_VECTOR_OPS_H__

#include "arrayOps.h"
#include "vector.h"

namespace ts3::math
{

	template <typename Tp, size_t tSize>
	inline void add( const Vector<Tp, tSize> & pFirst, const Vector<Tp, tSize> & pSecond, Vector<Tp, tSize> & pResult )
	{
		arrayOps::add( pFirst.valueArray, pSecond.valueArray, pResult.valueArray );
	}

	template <typename Tp, size_t tSize>
	inline Vector<Tp, tSize> add( const Vector<Tp, tSize> & pFirst, const Vector<Tp, tSize> & pSecond )
	{
		Vector<Tp, tSize> result;
		arrayOps::add( pFirst.valueArray, pSecond.valueArray, result.valueArray );
		return result;
	}

	template <typename Tp, size_t tSize>
	inline void sub( const Vector<Tp, tSize> & pFirst, const Vector<Tp, tSize> & pSecond, Vector<Tp, tSize> & pResult )
	{
		arrayOps::add( pFirst.valueArray, pSecond.valueArray, pResult.valueArray );
	}

	template <typename Tp, size_t tSize>
	inline Vector<Tp, tSize> sub( const Vector<Tp, tSize> & pFirst, const Vector<Tp, tSize> & pSecond )
	{
		Vector<Tp, tSize> result;
		arrayOps::sub( pFirst.valueArray, pSecond.valueArray, result.valueArray );
		return result;
	}

	template <typename Tp, size_t tSize>
	inline void mul( const Vector<Tp, tSize> & pFirst, const Vector<Tp, tSize> & pSecond, Vector<Tp, tSize> & pResult )
	{
		arrayOps::mul( pFirst.valueArray, pSecond.valueArray, pResult.valueArray );
	}

	template <typename Tp, size_t tSize, typename TpScalar, enable_if_scalar_t<TpScalar> = true>
	inline void mul( const Vector<Tp, tSize> & pVector, const TpScalar pScalar, Vector<Tp, tSize> & pResult )
	{
		arrayOps::mul( pVector.valueArray, pScalar, pResult.valueArray );
	}

	template <typename Tp, size_t tSize>
	inline Vector<Tp, tSize> mul( const Vector<Tp, tSize> & pFirst, const Vector<Tp, tSize> & pSecond )
	{
		Vector<Tp, tSize> result;
		arrayOps::mul( pFirst.valueArray, pSecond.valueArray, result.valueArray );
		return result;
	}

	template <typename Tp, size_t tSize, typename TpScalar, enable_if_scalar_t<TpScalar> = true>
	inline Vector<Tp, tSize> mul( const Vector<Tp, tSize> & pVector, const TpScalar pScalar )
	{
		Vector<Tp, tSize> result;
		arrayOps::mul( pVector.valueArray, pScalar, result.valueArray );
		return result;
	}

	template <typename Tp, size_t tSize>
	inline void div( const Vector<Tp, tSize> & pFirst, const Vector<Tp, tSize> & pSecond, Vector<Tp, tSize> & pResult )
	{
		arrayOps::div( pFirst.valueArray, pSecond.valueArray, pResult.valueArray );
	}

	template <typename Tp, size_t tSize, typename TpScalar, enable_if_scalar_t<TpScalar> = true>
	inline void div( const Vector<Tp, tSize> & pVector, const TpScalar pScalar, Vector<Tp, tSize> & pResult )
	{
		arrayOps::div( pVector.valueArray, pScalar, pResult.valueArray );
	}

	template <typename Tp, size_t tSize, typename TpScalar, enable_if_scalar_t<TpScalar> = true>
	inline void div( const TpScalar pScalar, const Vector<Tp, tSize> & pVector, Vector<Tp, tSize> & pResult )
	{
		arrayOps::div( pScalar, pVector.valueArray, pResult.valueArray );
	}

	template <typename Tp, size_t tSize>
	inline Vector<Tp, tSize> div( const Vector<Tp, tSize> & pFirst, const Vector<Tp, tSize> & pSecond )
	{
		Vector<Tp, tSize> result;
		arrayOps::div( pFirst.valueArray, pSecond.valueArray, result.valueArray );
		return result;
	}

	template <typename Tp, size_t tSize, typename TpScalar, enable_if_scalar_t<TpScalar> = true>
	inline Vector<Tp, tSize> div( const Vector<Tp, tSize> & pVector, const TpScalar pScalar )
	{
		Vector<Tp, tSize> result;
		arrayOps::div( pVector.valueArray, pScalar, result.valueArray );
		return result;
	}

	template <typename Tp, size_t tSize, typename TpScalar, enable_if_scalar_t<TpScalar> = true>
	inline Vector<Tp, tSize> div( const TpScalar pScalar, const Vector<Tp, tSize> & pVector )
	{
		Vector<Tp, tSize> result;
		arrayOps::div( pScalar, pVector.valueArray, result.valueArray );
		return result;
	}

	template <typename Tp, size_t tSize>
	inline void neg( const Vector<Tp, tSize> & pVector, Vector<Tp, tSize> & pResult )
	{
		arrayOps::neg( pVector.valueArray, pResult.valueArray );
	}

	template <typename Tp, size_t tSize>
	inline Vector<Tp, tSize> neg( const Vector<Tp, tSize> & pVector )
	{
		Vector<Tp, tSize> result;
		arrayOps::neg( pVector.valueArray, result.valueArray );
		return result;
	}

	template <typename Tp>
	inline void cross( const Vector3<Tp> & pFirst, const Vector3<Tp> & pSecond, Vector3<Tp> & pResult )
	{
		arrayOps::cross( pFirst.valueArray, pSecond.valueArray, pResult.valueArray );
	}

	template <typename Tp>
	inline Vector3<Tp> cross( const Vector3<Tp> & pFirst, const Vector3<Tp> & pSecond )
	{
		Vector3<Tp> result;
		arrayOps::cross( pFirst.valueArray, pSecond.valueArray, result.valueArray );
		return result;
	}

	template <typename Tp, size_t tSize>
	inline Tp dot( const Vector<Tp, tSize> & pFirst, const Vector<Tp, tSize> & pSecond )
	{
		return arrayOps::dot( pFirst.valueArray, pSecond.valueArray );
	}

	template <typename Tp, size_t tSize>
	inline void normalize( const Vector<Tp, tSize> & pVector, Vector<Tp, tSize> & pResult )
	{
		arrayOps::normalize( pVector.valueArray, pResult.valueArray );
	}

	template <typename Tp, size_t tSize>
	inline Vector<Tp, tSize> normalize( const Vector<Tp, tSize> & pVector )
	{
		Vector<Tp, tSize> result;
		arrayOps::normalize( pVector.valueArray, result.valueArray );
		return result;
	}

	template <typename Tp, size_t tSize>
	inline void normalizeInplace( Vector<Tp, tSize> & pVector )
	{
		arrayOps::normalizeInplace( pVector.valueArray );
	}

	template <typename Tp, size_t tSize>
	inline Tp length( const Vector<Tp, tSize> & pVector )
	{
		return arrayOps::length( pVector.valueArray );
	}

	template <typename TpResult, typename Tp, size_t tSize>
	inline TpResult lengthAs( const Vector<Tp, tSize> & pVector )
	{
		return static_cast<TpResult>( length( pVector ) );
	}

	template <typename Tp, size_t tSize>
	inline bool cmpEqual( const Vector<Tp, tSize> & pFirst, const Vector<Tp, tSize> & pSecond )
	{
		return arrayOps::cmpEqual( pFirst.valueArray, pSecond.valueArray );
	}

	template <typename Tp, size_t tSize>
	inline bool cmpNotEqual( const Vector<Tp, tSize> & pFirst, const Vector<Tp, tSize> & pSecond )
	{
		return arrayOps::cmpNotEqual( pFirst.valueArray, pSecond.valueArray );
	}

	template <typename Tp, size_t tSize>
	inline bool cmpLess( const Vector<Tp, tSize> & pFirst, const Vector<Tp, tSize> & pSecond )
	{
		return arrayOps::cmpLess( pFirst.valueArray, pSecond.valueArray );
	}


	template <typename Tp, size_t tSize>
	inline bool operator==( const Vector<Tp, tSize> & pLhs, const Vector<Tp, tSize> & pRhs ) noexcept
	{
		return cmpEqual( pLhs, pRhs );
	}

	template <typename Tp, size_t tSize>
	inline bool operator!=( const Vector<Tp, tSize> & pLhs, const Vector<Tp, tSize> & pRhs ) noexcept
	{
		return cmpNotEqual( pLhs, pRhs );
	}

	template <typename Tp, size_t tSize>
	inline Vector<Tp, tSize> operator+( const Vector<Tp, tSize> & pLhs, const Vector<Tp, tSize> & pRhs ) noexcept
	{
		return add( pLhs, pRhs );
	}

	template <typename Tp, size_t tSize>
	inline Vector<Tp, tSize> operator-( const Vector<Tp, tSize> & pLhs, const Vector<Tp, tSize> & pRhs ) noexcept
	{
		return sub( pLhs, pRhs );
	}

	template <typename Tp, size_t tSize>
	inline Vector<Tp, tSize> operator*( const Vector<Tp, tSize> & pLhs, const Vector<Tp, tSize> & pRhs ) noexcept
	{
		return mul( pLhs, pRhs );
	}

	template <typename Tp, size_t tSize, typename TpScalar, enable_if_scalar_t<TpScalar> = true>
	inline Vector<Tp, tSize> operator*( const Vector<Tp, tSize> & pLhs, const TpScalar pRhs ) noexcept
	{
		return mul( pLhs, pRhs );
	}

	template <typename Tp, size_t tSize, typename TpScalar, enable_if_scalar_t<TpScalar> = true>
	inline Vector<Tp, tSize> operator*( const TpScalar pLhs, const Vector<Tp, tSize> & pRhs ) noexcept
	{
		return mul( pRhs, pLhs );
	}

	template <typename Tp, size_t tSize>
	inline Vector<Tp, tSize> operator/( const Vector<Tp, tSize> & pLhs, const Vector<Tp, tSize> & pRhs ) noexcept
	{
		return div( pLhs, pRhs );
	}

	template <typename Tp, size_t tSize, typename TpScalar, enable_if_scalar_t<TpScalar> = true>
	inline Vector<Tp, tSize> operator/( const Vector<Tp, tSize> & pLhs, const TpScalar pRhs ) noexcept
	{
		return div( pLhs, pRhs );
	}

	template <typename Tp, size_t tSize, typename TpScalar, enable_if_scalar_t<TpScalar> = true>
	inline Vector<Tp, tSize> operator/( const TpScalar pLhs, const Vector<Tp, tSize> & pRhs ) noexcept
	{
		return div( pLhs, pRhs );
	}

	template <typename Tp, size_t tSize>
	inline Vector<Tp, tSize> operator-( const Vector<Tp, tSize> & pRhs ) noexcept
	{
		return neg( pRhs );
	}


	template <typename Tp, size_t tSize>
	inline Vector<Tp, tSize> & operator+=( Vector<Tp, tSize> & pLhs, const Vector<Tp, tSize> & pRhs ) noexcept
	{
		pLhs = add( pLhs, pRhs );
		return pLhs;
	}

	template <typename Tp, size_t tSize>
	inline Vector<Tp, tSize> & operator-=( Vector<Tp, tSize> & pLhs, const Vector<Tp, tSize> & pRhs ) noexcept
	{
		pLhs = sub( pLhs, pRhs );
		return pLhs;
	}

	template <typename Tp, size_t tSize, typename TpScalar, enable_if_scalar_t<TpScalar> = true>
	inline Vector<Tp, tSize> & operator*=( Vector<Tp, tSize> & pLhs, const TpScalar pRhs ) noexcept
	{
		pLhs = mul( pLhs, pRhs );
		return pLhs;
	}

	template <typename Tp, size_t tSize, typename TpScalar, enable_if_scalar_t<TpScalar> = true>
	inline Vector<Tp, tSize> & operator/=( Vector<Tp, tSize> & pLhs, const TpScalar pRhs ) noexcept
	{
		pLhs = div( pLhs, pRhs );
		return pLhs;
	}

}

#endif // __TS3_MATH_VECTOR_OPS_H__
