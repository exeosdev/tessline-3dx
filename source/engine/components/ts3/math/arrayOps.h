
#ifndef __TS3_MATH_ARRAY_OPS_H__
#define __TS3_MATH_ARRAY_OPS_H__

#include "utility.h"

namespace ts3::math
{

	namespace arrayOps
	{

		template <typename TVal>
		inline void set( Array2<TVal> & pDest, TVal pX, TVal pY )
		{
			pDest[0] = pX;
			pDest[1] = pY;
		}

		template <typename TVal>
		inline void set( Array2<TVal> & pDest, TVal pValue )
		{
			pDest[0] = pValue;
			pDest[1] = pValue;
		}

		template <typename TVal>
		inline void set( Array2<TVal> & pDest, const TVal * pArray )
		{
			pDest[0] = pArray[0];
			pDest[1] = pArray[1];
		}

		template <typename TVal>
		inline void add( const Array2<TVal> & pFirst, const Array2<TVal> & pSecond, Array2<TVal> & pResult )
		{
			pResult[0] = pFirst[0] + pSecond[0];
			pResult[1] = pFirst[1] + pSecond[1];
		}

		template <typename TVal>
		inline void sub( const Array2<TVal> & pFirst, const Array2<TVal> & pSecond, Array2<TVal> & pResult )
		{
			pResult[0] = pFirst[0] - pSecond[0];
			pResult[1] = pFirst[1] - pSecond[1];
		}

		template <typename TVal>
		inline void mul( const Array2<TVal> & pFirst, const Array2<TVal> & pSecond, Array2<TVal> & pResult )
		{
			pResult[0] = pFirst[0] * pSecond[0];
			pResult[1] = pFirst[1] * pSecond[1];
		}

		template <typename TVal, typename TScalar, enable_if_scalar_t<TScalar> = true>
		inline void mul( const Array2<TVal> & pData, const TScalar pScalar, Array2<TVal> & pResult )
		{
			pResult[0] = pData[0] * pScalar;
			pResult[1] = pData[1] * pScalar;
		}

		template <typename TVal>
		inline void div( const Array2<TVal> & pFirst, const Array2<TVal> & pSecond, Array2<TVal> & pResult )
		{
			pResult[0] = pFirst[0] / pSecond[0];
			pResult[1] = pFirst[1] / pSecond[1];
		}

		template <typename TVal, typename TScalar, enable_if_scalar_t<TScalar> = true>
		inline void div( const Array2<TVal> & pData, const TScalar pScalar, Array2<TVal> & pResult )
		{
			pResult[0] = pData[0] / pScalar;
			pResult[1] = pData[1] / pScalar;
		}

		template <typename TVal, typename TScalar, enable_if_scalar_t<TScalar> = true>
		inline void div( const TScalar pScalar, const Array2<TVal> & pData, Array2<TVal> & pResult )
		{
			pResult[0] = pScalar / pData[0];
			pResult[1] = pScalar / pData[1];
		}

		template <typename TVal>
		inline void neg( const Array2<TVal> & pData, Array2<TVal> & pResult )
		{
			pResult[0] = -pData[0];
			pResult[1] = -pData[1];
		}

		template <typename TVal>
		inline TVal dot( const Array2<TVal> & pFirst, const Array2<TVal> & pSecond )
		{
			return pFirst[0] * pSecond[0] + pFirst[1] * pSecond[1];
		}

		template <typename TVal>
		inline TVal length( const Array2<TVal> & pData )
		{
			return static_cast<TVal>( sqrt( dot( pData, pData ) ) );
		}

		template <typename TVal>
		inline void normalize( const Array2<TVal> & pData, Array2<TVal> & pResult )
		{
			const auto dotInvSqrt = invSqrt( dot( pData, pData ) );
			pResult[0] = pData[0] * dotInvSqrt;
			pResult[1] = pData[1] * dotInvSqrt;
		}

		template <typename TVal>
		inline void normalizeInplace( Array2<TVal> & pData )
		{
			normalize( pData, pData );
		}

		template <typename TVal>
		inline bool cmpEqual( const Array2<TVal> & pFirst, const Array2<TVal> & pSecond )
		{
			return ( pFirst[0] == pSecond[0] ) && ( pFirst[1] == pSecond[1] );
		}

		template <typename TVal>
		inline bool cmpNotEqual( const Array2<TVal> & pFirst, const Array2<TVal> & pSecond )
		{
			return ( pFirst[0] != pSecond[0] ) || ( pFirst[1] != pSecond[1] );
		}

		template <typename TVal>
		inline bool cmpLess( const Array2<TVal> & pFirst, const Array2<TVal> & pSecond )
		{
			return  ( pFirst[0] < pSecond[0] ) || ( ( pFirst[0] == pSecond[0] ) && ( pFirst[1] < pSecond[1] ) );
		}



		template <typename TVal>
		inline void set( Array3<TVal> & pDest, float pX, float pY, float pZ )
		{
			pDest[0] = pX;
			pDest[1] = pY;
			pDest[2] = pZ;
		}

		template <typename TVal>
		inline void set( Array3<TVal> & pDest, TVal pValue )
		{
			pDest[0] = pValue;
			pDest[1] = pValue;
			pDest[2] = pValue;
		}

		template <typename TVal>
		inline void set( Array3<TVal> & pDest, const float * pArray )
		{
			pDest[0] = pArray[0];
			pDest[1] = pArray[1];
			pDest[2] = pArray[2];
		}

		template <typename TVal>
		inline void add( const Array3<TVal> & pFirst, const Array3<TVal> & pSecond, Array3<TVal> & pResult )
		{
			pResult[0] = pFirst[0] + pSecond[0];
			pResult[1] = pFirst[1] + pSecond[1];
			pResult[2] = pFirst[2] + pSecond[2];
		}

		template <typename TVal>
		inline void sub( const Array3<TVal> & pFirst, const Array3<TVal> & pSecond, Array3<TVal> & pResult )
		{
			pResult[0] = pFirst[0] - pSecond[0];
			pResult[1] = pFirst[1] - pSecond[1];
			pResult[2] = pFirst[2] - pSecond[2];
		}

		template <typename TVal>
		inline void mul( const Array3<TVal> & pFirst, const Array3<TVal> & pSecond, Array3<TVal> & pResult )
		{
			pResult[0] = pFirst[0] * pSecond[0];
			pResult[1] = pFirst[1] * pSecond[1];
			pResult[2] = pFirst[2] * pSecond[2];
		}

		template <typename TVal, typename TScalar, enable_if_scalar_t<TScalar> = true>
		inline void mul( const Array3<TVal> & pData, const TScalar pScalar, Array3<TVal> & pResult )
		{
			pResult[0] = pData[0] * pScalar;
			pResult[1] = pData[1] * pScalar;
			pResult[2] = pData[2] * pScalar;
		}

		template <typename TVal>
		inline void div( const Array3<TVal> & pFirst, const Array3<TVal> & pSecond, Array3<TVal> & pResult )
		{
			pResult[0] = pFirst[0] / pSecond[0];
			pResult[1] = pFirst[1] / pSecond[1];
			pResult[2] = pFirst[2] / pSecond[2];
		}

		template <typename TVal, typename TScalar, enable_if_scalar_t<TScalar> = true>
		inline void div( const Array3<TVal> & pData, const TScalar pScalar, Array3<TVal> & pResult )
		{
			pResult[0] = pData[0] / pScalar;
			pResult[1] = pData[1] / pScalar;
			pResult[2] = pData[2] / pScalar;
		}

		template <typename TVal, typename TScalar, enable_if_scalar_t<TScalar> = true>
		inline void div( const TScalar pScalar, const Array3<TVal> & pData, Array3<TVal> & pResult )
		{
			pResult[0] = pScalar / pData[0];
			pResult[1] = pScalar / pData[1];
			pResult[2] = pScalar / pData[2];
		}

		template <typename TVal>
		inline void neg( const Array3<TVal> & pData, Array3<TVal> & pResult )
		{
			pResult[0] = ( -pData[0] );
			pResult[1] = ( -pData[1] );
			pResult[2] = ( -pData[2] );
		}

		template <typename TVal>
		inline void cross( const Array3<TVal> & pFirst, const Array3<TVal> & pSecond, Array3<TVal> & pResult )
		{
			pResult[0] = pFirst[1] * pSecond[2] - pFirst[2] * pSecond[1];
			pResult[1] = pFirst[2] * pSecond[0] - pFirst[0] * pSecond[2];
			pResult[2] = pFirst[0] * pSecond[1] - pFirst[1] * pSecond[0];
		}

		template <typename TVal>
		inline TVal dot( const Array3<TVal> & pFirst, const Array3<TVal> & pSecond )
		{
			return pFirst[0] * pSecond[0] + pFirst[1] * pSecond[1] + pFirst[2] * pSecond[2];
		}

		template <typename TVal>
		static double length( const Array3<TVal> & pData )
		{
			return sqrt( dot( pData, pData ) );
		}

		template <typename TVal>
		inline void normalize( const Array3<TVal> & pData, Array3<TVal> & pResult )
		{
			const auto dotInvSqrt = invSqrt( dot( pData, pData ) );
			pResult[0] = pData[0] * dotInvSqrt;
			pResult[1] = pData[1] * dotInvSqrt;
			pResult[2] = pData[2] * dotInvSqrt;
		}

		template <typename TVal>
		inline void normalizeInplace( Array3<TVal> & pData )
		{
			normalize( pData, pData );
		}

		template <typename TVal>
		inline bool cmpEqual( const Array3<TVal> & pFirst, const Array3<TVal> & pSecond )
		{
			return ( pFirst[0] == pSecond[0] ) && ( pFirst[1] == pSecond[1] ) && ( pFirst[2] == pSecond[2] );
		}

		template <typename TVal>
		inline bool cmpNotEqual( const Array3<TVal> & pFirst, const Array3<TVal> & pSecond )
		{
			return ( pFirst[0] != pSecond[0] ) || ( pFirst[1] != pSecond[1] ) || ( pFirst[2] != pSecond[2] );
		}

		template <typename TVal>
		inline bool cmpLess( const Array3<TVal> & pFirst, const Array3<TVal> & pSecond )
		{
			return ( pFirst[0] < pSecond[0] ) ||
			       ( ( pFirst[0] == pSecond[0] ) && ( pFirst[1] < pSecond[1] ) ) ||
			       ( ( pFirst[0] == pSecond[0] ) && ( pFirst[1] == pSecond[1] ) && ( pFirst[2] < pSecond[2] ) );
		}


		template <typename TVal>
		inline void set( Array4<TVal> & pDest, float pX, float pY, float pZ, float pW )
		{
			pDest[0] = pX;
			pDest[1] = pY;
			pDest[2] = pZ;
			pDest[3] = pW;
		}

		template <typename TVal>
		inline void set( Array4<TVal> & pDest, TVal pValue )
		{
			pDest[0] = pValue;
			pDest[1] = pValue;
			pDest[2] = pValue;
			pDest[3] = pValue;
		}

		template <typename TVal>
		inline void set( Array4<TVal> & pDest, const float * pArray )
		{
			pDest[0] = pArray[0];
			pDest[1] = pArray[1];
			pDest[2] = pArray[2];
			pDest[3] = pArray[3];
		}

		template <typename TVal>
		inline void add( const Array4<TVal> & pFirst, const Array4<TVal> & pSecond, Array4<TVal> & pResult )
		{
			pResult[0] = pFirst[0] + pSecond[0];
			pResult[1] = pFirst[1] + pSecond[1];
			pResult[2] = pFirst[2] + pSecond[2];
			pResult[3] = pFirst[3] + pSecond[3];
		}

		template <typename TVal>
		inline void sub( const Array4<TVal> & pFirst, const Array4<TVal> & pSecond, Array4<TVal> & pResult )
		{
			pResult[0] = pFirst[0] - pSecond[0];
			pResult[1] = pFirst[1] - pSecond[1];
			pResult[2] = pFirst[2] - pSecond[2];
			pResult[3] = pFirst[3] - pSecond[3];
		}

		template <typename TVal>
		inline void mul( const Array4<TVal> & pFirst, const Array4<TVal> & pSecond, Array4<TVal> & pResult )
		{
			pResult[0] = pFirst[0] * pSecond[0];
			pResult[1] = pFirst[1] * pSecond[1];
			pResult[2] = pFirst[2] * pSecond[2];
			pResult[3] = pFirst[3] * pSecond[3];
		}

		template <typename TVal, typename TScalar, enable_if_scalar_t<TScalar> = true>
		inline void mul( const Array4<TVal> & pData, const TScalar pScalar, Array4<TVal> & pResult )
		{
			pResult[0] = pData[0] * pScalar;
			pResult[1] = pData[1] * pScalar;
			pResult[2] = pData[2] * pScalar;
			pResult[3] = pData[3] * pScalar;
		}

		template <typename TVal>
		inline void div( const Array4<TVal> & pFirst, const Array4<TVal> & pSecond, Array4<TVal> & pResult )
		{
			pResult[0] = pFirst[0] / pSecond[0];
			pResult[1] = pFirst[1] / pSecond[1];
			pResult[2] = pFirst[2] / pSecond[2];
			pResult[3] = pFirst[3] / pSecond[3];
		}

		template <typename TVal, typename TScalar, enable_if_scalar_t<TScalar> = true>
		inline void div( const Array4<TVal> & pData, const TScalar pScalar, Array4<TVal> & pResult )
		{
			pResult[0] = pData[0] / pScalar;
			pResult[1] = pData[1] / pScalar;
			pResult[2] = pData[2] / pScalar;
			pResult[3] = pData[3] / pScalar;
		}

		template <typename TVal, typename TScalar, enable_if_scalar_t<TScalar> = true>
		inline void div( const TScalar pScalar, const Array4<TVal> & pData, Array4<TVal> & pResult )
		{
			pResult[0] = pScalar / pData[0];
			pResult[1] = pScalar / pData[1];
			pResult[2] = pScalar / pData[2];
			pResult[3] = pScalar / pData[3];
		}

		template <typename TVal>
		inline void neg( const Array4<TVal> & pData, Array4<TVal> & pResult )
		{
			pResult[0] = ( -pData[0] );
			pResult[1] = ( -pData[1] );
			pResult[2] = ( -pData[2] );
			pResult[3] = ( -pData[3] );
		}

		template <typename TVal>
		inline TVal dot( const Array4<TVal> & pFirst, const Array4<TVal> & pSecond )
		{
			return pFirst[0] * pSecond[0] + pFirst[1] * pSecond[1] + pFirst[2] * pSecond[2] + pFirst[3] * pSecond[3];
		}

		template <typename TVal>
		static double length( const Array4<TVal> & pData )
		{
			return sqrt( dot( pData, pData ) );
		}

		template <typename TVal>
		inline void normalize( const Array4<TVal> & pData, Array4<TVal> & pResult )
		{
			const auto dotInvSqrt = invSqrt( dot( pData, pData ) );
			pResult[0] = pData[0] * dotInvSqrt;
			pResult[1] = pData[1] * dotInvSqrt;
			pResult[2] = pData[2] * dotInvSqrt;
			pResult[3] = pData[3] * dotInvSqrt;
		}

		template <typename TVal>
		inline void normalizeInplace( Array4<TVal> & pData )
		{
			normalize( pData, pData );
		}

		template <typename TVal>
		inline bool cmpEqual( const Array4<TVal> & pFirst, const Array4<TVal> & pSecond )
		{
			return ( pFirst[0] == pSecond[0] ) && ( pFirst[1] == pSecond[1] ) && ( pFirst[2] == pSecond[2] ) && ( pFirst[3] == pSecond[3] );
		}

		template <typename TVal>
		inline bool cmpNotEqual( const Array4<TVal> & pFirst, const Array4<TVal> & pSecond )
		{
			return ( pFirst[0] != pSecond[0] ) || ( pFirst[1] != pSecond[1] ) || ( pFirst[2] != pSecond[2] ) || ( pFirst[3] != pSecond[3] );
		}

		template <typename TVal>
		inline bool cmpLess( const Array4<TVal> & pFirst, const Array4<TVal> & pSecond )
		{
			return ( pFirst[0] < pSecond[0] ) ||
			       ( ( pFirst[0] == pSecond[0] ) && ( pFirst[1] < pSecond[1] ) ) ||
			       ( ( pFirst[0] == pSecond[0] ) && ( pFirst[1] == pSecond[1] ) && ( pFirst[2] < pSecond[2] ) ) ||
			       ( ( pFirst[0] == pSecond[0] ) && ( pFirst[1] == pSecond[1] ) && ( pFirst[2] == pSecond[2] ) && ( pFirst[3] < pSecond[3] ) );
		}
	}

}

#if( TS3_MATH_SIMD_ENABLE )
#  include "arrayOpsSIMD.inl"
#endif

#endif // __TS3_MATH_ARRAY_OPS_H__
