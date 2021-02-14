
#ifndef __TS3_MATH_ARRAY_OPS_H__
#define __TS3_MATH_ARRAY_OPS_H__

#include "utility.h"

namespace ts3::math
{

	namespace arrayOps
	{

		template <typename Tp>
		inline void set( Array2<Tp> & pDest, Tp pX, Tp pY )
		{
			pDest[0] = pX;
			pDest[1] = pY;
		}

		template <typename Tp>
		inline void set( Array2<Tp> & pDest, Tp pValue )
		{
			pDest[0] = pValue;
			pDest[1] = pValue;
		}

		template <typename Tp>
		inline void set( Array2<Tp> & pDest, const Tp * pArray )
		{
			pDest[0] = pArray[0];
			pDest[1] = pArray[1];
		}

		template <typename Tp>
		inline void add( const Array2<Tp> & pFirst, const Array2<Tp> & pSecond, Array2<Tp> & pResult )
		{
			pResult[0] = pFirst[0] + pSecond[0];
			pResult[1] = pFirst[1] + pSecond[1];
		}

		template <typename Tp>
		inline void sub( const Array2<Tp> & pFirst, const Array2<Tp> & pSecond, Array2<Tp> & pResult )
		{
			pResult[0] = pFirst[0] - pSecond[0];
			pResult[1] = pFirst[1] - pSecond[1];
		}

		template <typename Tp>
		inline void mul( const Array2<Tp> & pFirst, const Array2<Tp> & pSecond, Array2<Tp> & pResult )
		{
			pResult[0] = pFirst[0] * pSecond[0];
			pResult[1] = pFirst[1] * pSecond[1];
		}

		template <typename Tp, typename TpScalar, enable_if_scalar_t<TpScalar> = true>
		inline void mul( const Array2<Tp> & pData, const TpScalar pScalar, Array2<Tp> & pResult )
		{
			pResult[0] = pData[0] * pScalar;
			pResult[1] = pData[1] * pScalar;
		}

		template <typename Tp>
		inline void div( const Array2<Tp> & pFirst, const Array2<Tp> & pSecond, Array2<Tp> & pResult )
		{
			pResult[0] = pFirst[0] / pSecond[0];
			pResult[1] = pFirst[1] / pSecond[1];
		}

		template <typename Tp, typename TpScalar, enable_if_scalar_t<TpScalar> = true>
		inline void div( const Array2<Tp> & pData, const TpScalar pScalar, Array2<Tp> & pResult )
		{
			pResult[0] = pData[0] / pScalar;
			pResult[1] = pData[1] / pScalar;
		}

		template <typename Tp, typename TpScalar, enable_if_scalar_t<TpScalar> = true>
		inline void div( const TpScalar pScalar, const Array2<Tp> & pData, Array2<Tp> & pResult )
		{
			pResult[0] = pScalar / pData[0];
			pResult[1] = pScalar / pData[1];
		}

		template <typename Tp>
		inline void neg( const Array2<Tp> & pData, Array2<Tp> & pResult )
		{
			pResult[0] = -pData[0];
			pResult[1] = -pData[1];
		}

		template <typename Tp>
		inline Tp dot( const Array2<Tp> & pFirst, const Array2<Tp> & pSecond )
		{
			return pFirst[0] * pSecond[0] + pFirst[1] * pSecond[1];
		}

		template <typename Tp>
		inline Tp length( const Array2<Tp> & pData )
		{
			return static_cast<Tp>( sqrt( dot( pData, pData ) ) );
		}

		template <typename Tp>
		inline void normalize( const Array2<Tp> & pData, Array2<Tp> & pResult )
		{
			const auto dotInvSqrt = invSqrt( dot( pData, pData ) );
			pResult[0] = pData[0] * dotInvSqrt;
			pResult[1] = pData[1] * dotInvSqrt;
		}

		template <typename Tp>
		inline void normalizeInplace( Array2<Tp> & pData )
		{
			normalize( pData, pData );
		}

		template <typename Tp>
		inline bool cmpEqual( const Array2<Tp> & pFirst, const Array2<Tp> & pSecond )
		{
			return ( pFirst[0] == pSecond[0] ) && ( pFirst[1] == pSecond[1] );
		}

		template <typename Tp>
		inline bool cmpNotEqual( const Array2<Tp> & pFirst, const Array2<Tp> & pSecond )
		{
			return ( pFirst[0] != pSecond[0] ) || ( pFirst[1] != pSecond[1] );
		}

		template <typename Tp>
		inline bool cmpLess( const Array2<Tp> & pFirst, const Array2<Tp> & pSecond )
		{
			return  ( pFirst[0] < pSecond[0] ) || ( ( pFirst[0] == pSecond[0] ) && ( pFirst[1] < pSecond[1] ) );
		}



		template <typename Tp>
		inline void set( Array3<Tp> & pDest, float pX, float pY, float pZ )
		{
			pDest[0] = pX;
			pDest[1] = pY;
			pDest[2] = pZ;
		}

		template <typename Tp>
		inline void set( Array3<Tp> & pDest, Tp pValue )
		{
			pDest[0] = pValue;
			pDest[1] = pValue;
			pDest[2] = pValue;
		}

		template <typename Tp>
		inline void set( Array3<Tp> & pDest, const float * pArray )
		{
			pDest[0] = pArray[0];
			pDest[1] = pArray[1];
			pDest[2] = pArray[2];
		}

		template <typename Tp>
		inline void add( const Array3<Tp> & pFirst, const Array3<Tp> & pSecond, Array3<Tp> & pResult )
		{
			pResult[0] = pFirst[0] + pSecond[0];
			pResult[1] = pFirst[1] + pSecond[1];
			pResult[2] = pFirst[2] + pSecond[2];
		}

		template <typename Tp>
		inline void sub( const Array3<Tp> & pFirst, const Array3<Tp> & pSecond, Array3<Tp> & pResult )
		{
			pResult[0] = pFirst[0] - pSecond[0];
			pResult[1] = pFirst[1] - pSecond[1];
			pResult[2] = pFirst[2] - pSecond[2];
		}

		template <typename Tp>
		inline void mul( const Array3<Tp> & pFirst, const Array3<Tp> & pSecond, Array3<Tp> & pResult )
		{
			pResult[0] = pFirst[0] * pSecond[0];
			pResult[1] = pFirst[1] * pSecond[1];
			pResult[2] = pFirst[2] * pSecond[2];
		}

		template <typename Tp, typename TpScalar, enable_if_scalar_t<TpScalar> = true>
		inline void mul( const Array3<Tp> & pData, const TpScalar pScalar, Array3<Tp> & pResult )
		{
			pResult[0] = pData[0] * pScalar;
			pResult[1] = pData[1] * pScalar;
			pResult[2] = pData[2] * pScalar;
		}

		template <typename Tp>
		inline void div( const Array3<Tp> & pFirst, const Array3<Tp> & pSecond, Array3<Tp> & pResult )
		{
			pResult[0] = pFirst[0] / pSecond[0];
			pResult[1] = pFirst[1] / pSecond[1];
			pResult[2] = pFirst[2] / pSecond[2];
		}

		template <typename Tp, typename TpScalar, enable_if_scalar_t<TpScalar> = true>
		inline void div( const Array3<Tp> & pData, const TpScalar pScalar, Array3<Tp> & pResult )
		{
			pResult[0] = pData[0] / pScalar;
			pResult[1] = pData[1] / pScalar;
			pResult[2] = pData[2] / pScalar;
		}

		template <typename Tp, typename TpScalar, enable_if_scalar_t<TpScalar> = true>
		inline void div( const TpScalar pScalar, const Array3<Tp> & pData, Array3<Tp> & pResult )
		{
			pResult[0] = pScalar / pData[0];
			pResult[1] = pScalar / pData[1];
			pResult[2] = pScalar / pData[2];
		}

		template <typename Tp>
		inline void neg( const Array3<Tp> & pData, Array3<Tp> & pResult )
		{
			pResult[0] = ( -pData[0] );
			pResult[1] = ( -pData[1] );
			pResult[2] = ( -pData[2] );
		}

		template <typename Tp>
		inline void cross( const Array3<Tp> & pFirst, const Array3<Tp> & pSecond, Array3<Tp> & pResult )
		{
			pResult[0] = pFirst[1] * pSecond[2] - pFirst[2] * pSecond[1];
			pResult[1] = pFirst[2] * pSecond[0] - pFirst[0] * pSecond[2];
			pResult[2] = pFirst[0] * pSecond[1] - pFirst[1] * pSecond[0];
		}

		template <typename Tp>
		inline Tp dot( const Array3<Tp> & pFirst, const Array3<Tp> & pSecond )
		{
			return pFirst[0] * pSecond[0] + pFirst[1] * pSecond[1] + pFirst[2] * pSecond[2];
		}

		template <typename Tp>
		static double length( const Array3<Tp> & pData )
		{
			return sqrt( dot( pData, pData ) );
		}

		template <typename Tp>
		inline void normalize( const Array3<Tp> & pData, Array3<Tp> & pResult )
		{
			const auto dotInvSqrt = invSqrt( dot( pData, pData ) );
			pResult[0] = pData[0] * dotInvSqrt;
			pResult[1] = pData[1] * dotInvSqrt;
			pResult[2] = pData[2] * dotInvSqrt;
		}

		template <typename Tp>
		inline void normalizeInplace( Array3<Tp> & pData )
		{
			normalize( pData, pData );
		}

		template <typename Tp>
		inline bool cmpEqual( const Array3<Tp> & pFirst, const Array3<Tp> & pSecond )
		{
			return ( pFirst[0] == pSecond[0] ) && ( pFirst[1] == pSecond[1] ) && ( pFirst[2] == pSecond[2] );
		}

		template <typename Tp>
		inline bool cmpNotEqual( const Array3<Tp> & pFirst, const Array3<Tp> & pSecond )
		{
			return ( pFirst[0] != pSecond[0] ) || ( pFirst[1] != pSecond[1] ) || ( pFirst[2] != pSecond[2] );
		}

		template <typename Tp>
		inline bool cmpLess( const Array3<Tp> & pFirst, const Array3<Tp> & pSecond )
		{
			return ( pFirst[0] < pSecond[0] ) ||
			       ( ( pFirst[0] == pSecond[0] ) && ( pFirst[1] < pSecond[1] ) ) ||
			       ( ( pFirst[0] == pSecond[0] ) && ( pFirst[1] == pSecond[1] ) && ( pFirst[2] < pSecond[2] ) );
		}


		template <typename Tp>
		inline void set( Array4<Tp> & pDest, float pX, float pY, float pZ, float pW )
		{
			pDest[0] = pX;
			pDest[1] = pY;
			pDest[2] = pZ;
			pDest[3] = pW;
		}

		template <typename Tp>
		inline void set( Array4<Tp> & pDest, Tp pValue )
		{
			pDest[0] = pValue;
			pDest[1] = pValue;
			pDest[2] = pValue;
			pDest[3] = pValue;
		}

		template <typename Tp>
		inline void set( Array4<Tp> & pDest, const float * pArray )
		{
			pDest[0] = pArray[0];
			pDest[1] = pArray[1];
			pDest[2] = pArray[2];
			pDest[3] = pArray[3];
		}

		template <typename Tp>
		inline void add( const Array4<Tp> & pFirst, const Array4<Tp> & pSecond, Array4<Tp> & pResult )
		{
			pResult[0] = pFirst[0] + pSecond[0];
			pResult[1] = pFirst[1] + pSecond[1];
			pResult[2] = pFirst[2] + pSecond[2];
			pResult[3] = pFirst[3] + pSecond[3];
		}

		template <typename Tp>
		inline void sub( const Array4<Tp> & pFirst, const Array4<Tp> & pSecond, Array4<Tp> & pResult )
		{
			pResult[0] = pFirst[0] - pSecond[0];
			pResult[1] = pFirst[1] - pSecond[1];
			pResult[2] = pFirst[2] - pSecond[2];
			pResult[3] = pFirst[3] - pSecond[3];
		}

		template <typename Tp>
		inline void mul( const Array4<Tp> & pFirst, const Array4<Tp> & pSecond, Array4<Tp> & pResult )
		{
			pResult[0] = pFirst[0] * pSecond[0];
			pResult[1] = pFirst[1] * pSecond[1];
			pResult[2] = pFirst[2] * pSecond[2];
			pResult[3] = pFirst[3] * pSecond[3];
		}

		template <typename Tp, typename TpScalar, enable_if_scalar_t<TpScalar> = true>
		inline void mul( const Array4<Tp> & pData, const TpScalar pScalar, Array4<Tp> & pResult )
		{
			pResult[0] = pData[0] * pScalar;
			pResult[1] = pData[1] * pScalar;
			pResult[2] = pData[2] * pScalar;
			pResult[3] = pData[3] * pScalar;
		}

		template <typename Tp>
		inline void div( const Array4<Tp> & pFirst, const Array4<Tp> & pSecond, Array4<Tp> & pResult )
		{
			pResult[0] = pFirst[0] / pSecond[0];
			pResult[1] = pFirst[1] / pSecond[1];
			pResult[2] = pFirst[2] / pSecond[2];
			pResult[3] = pFirst[3] / pSecond[3];
		}

		template <typename Tp, typename TpScalar, enable_if_scalar_t<TpScalar> = true>
		inline void div( const Array4<Tp> & pData, const TpScalar pScalar, Array4<Tp> & pResult )
		{
			pResult[0] = pData[0] / pScalar;
			pResult[1] = pData[1] / pScalar;
			pResult[2] = pData[2] / pScalar;
			pResult[3] = pData[3] / pScalar;
		}

		template <typename Tp, typename TpScalar, enable_if_scalar_t<TpScalar> = true>
		inline void div( const TpScalar pScalar, const Array4<Tp> & pData, Array4<Tp> & pResult )
		{
			pResult[0] = pScalar / pData[0];
			pResult[1] = pScalar / pData[1];
			pResult[2] = pScalar / pData[2];
			pResult[3] = pScalar / pData[3];
		}

		template <typename Tp>
		inline void neg( const Array4<Tp> & pData, Array4<Tp> & pResult )
		{
			pResult[0] = ( -pData[0] );
			pResult[1] = ( -pData[1] );
			pResult[2] = ( -pData[2] );
			pResult[3] = ( -pData[3] );
		}

		template <typename Tp>
		inline Tp dot( const Array4<Tp> & pFirst, const Array4<Tp> & pSecond )
		{
			return pFirst[0] * pSecond[0] + pFirst[1] * pSecond[1] + pFirst[2] * pSecond[2] + pFirst[3] * pSecond[3];
		}

		template <typename Tp>
		static double length( const Array4<Tp> & pData )
		{
			return sqrt( dot( pData, pData ) );
		}

		template <typename Tp>
		inline void normalize( const Array4<Tp> & pData, Array4<Tp> & pResult )
		{
			const auto dotInvSqrt = invSqrt( dot( pData, pData ) );
			pResult[0] = pData[0] * dotInvSqrt;
			pResult[1] = pData[1] * dotInvSqrt;
			pResult[2] = pData[2] * dotInvSqrt;
			pResult[3] = pData[3] * dotInvSqrt;
		}

		template <typename Tp>
		inline void normalizeInplace( Array4<Tp> & pData )
		{
			normalize( pData, pData );
		}

		template <typename Tp>
		inline bool cmpEqual( const Array4<Tp> & pFirst, const Array4<Tp> & pSecond )
		{
			return ( pFirst[0] == pSecond[0] ) && ( pFirst[1] == pSecond[1] ) && ( pFirst[2] == pSecond[2] ) && ( pFirst[3] == pSecond[3] );
		}

		template <typename Tp>
		inline bool cmpNotEqual( const Array4<Tp> & pFirst, const Array4<Tp> & pSecond )
		{
			return ( pFirst[0] != pSecond[0] ) || ( pFirst[1] != pSecond[1] ) || ( pFirst[2] != pSecond[2] ) || ( pFirst[3] != pSecond[3] );
		}

		template <typename Tp>
		inline bool cmpLess( const Array4<Tp> & pFirst, const Array4<Tp> & pSecond )
		{
			return ( pFirst[0] < pSecond[0] ) ||
			       ( ( pFirst[0] == pSecond[0] ) && ( pFirst[1] < pSecond[1] ) ) ||
			       ( ( pFirst[0] == pSecond[0] ) && ( pFirst[1] == pSecond[1] ) && ( pFirst[2] < pSecond[2] ) ) ||
			       ( ( pFirst[0] == pSecond[0] ) && ( pFirst[1] == pSecond[1] ) && ( pFirst[2] == pSecond[2] ) && ( pFirst[3] < pSecond[3] ) );
		}
	};

}

#if( TS3_MATH_SIMD_ENABLE )
#  include "arrayOpsSIMD.inl"
#endif

#endif // __TS3_MATH_ARRAY_OPS_H__
