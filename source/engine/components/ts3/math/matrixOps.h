
#ifndef __TS3_MATH_MATRIX_OPS_H__
#define __TS3_MATH_MATRIX_OPS_H__

#include "matrix.h"
#include "vectorOps.h"

namespace ts3::math
{

	template <typename TVal, size_t tC>
	inline void add( const Matrix<TVal, 2, tC> & pFirst, const Matrix<TVal, 2, tC> & pSecond, Matrix<TVal, 2, tC> & pResult )
	{
		pResult.row0 = pFirst.row0 + pSecond.row0;
		pResult.row1 = pFirst.row1 + pSecond.row1;
	}

	template <typename TVal, size_t tC>
	inline void add( const Matrix<TVal, 3, tC> & pFirst, const Matrix<TVal, 3, tC> & pSecond, Matrix<TVal, 3, tC> & pResult )
	{
		pResult.row0 = pFirst.row0 + pSecond.row0;
		pResult.row1 = pFirst.row1 + pSecond.row1;
		pResult.row2 = pFirst.row2 + pSecond.row2;
	}

	template <typename TVal, size_t tC>
	inline void add( const Matrix<TVal, 4, tC> & pFirst, const Matrix<TVal, 4, tC> & pSecond, Matrix<TVal, 4, tC> & pResult )
	{
		pResult.row0 = pFirst.row0 + pSecond.row0;
		pResult.row1 = pFirst.row1 + pSecond.row1;
		pResult.row2 = pFirst.row2 + pSecond.row2;
		pResult.row3 = pFirst.row3 + pSecond.row3;
	}

	template <typename TMatrix>
	inline TMatrix add( const TMatrix & pFirst, const TMatrix & pSecond )
	{
		TMatrix result;
		add( pFirst, pSecond, result );
		return result;
	}

	template <typename TVal, size_t tC>
	inline void sub( const Matrix<TVal, 2, tC> & pFirst, const Matrix<TVal, 2, tC> & pSecond, Matrix<TVal, 2, tC> & pResult )
	{
		pResult.row0 = pFirst.row0 - pSecond.row0;
		pResult.row1 = pFirst.row1 - pSecond.row1;
	}

	template <typename TVal, size_t tC>
	inline void sub( const Matrix<TVal, 3, tC> & pFirst, const Matrix<TVal, 3, tC> & pSecond, Matrix<TVal, 3, tC> & pResult )
	{
		pResult.row0 = pFirst.row0 - pSecond.row0;
		pResult.row1 = pFirst.row1 - pSecond.row1;
		pResult.row2 = pFirst.row2 - pSecond.row2;
	}

	template <typename TVal, size_t tC>
	inline void sub( const Matrix<TVal, 4, tC> & pFirst, const Matrix<TVal, 4, tC> & pSecond, Matrix<TVal, 4, tC> & pResult )
	{
		pResult.row0 = pFirst.row0 - pSecond.row0;
		pResult.row1 = pFirst.row1 - pSecond.row1;
		pResult.row2 = pFirst.row2 - pSecond.row2;
		pResult.row3 = pFirst.row3 - pSecond.row3;
	}

	template <typename TMatrix>
	inline TMatrix sub( const TMatrix & pFirst, const TMatrix & pSecond )
	{
		TMatrix result;
		sub( pFirst, pSecond, result );
		return result;
	}

	template <typename TVal, size_t tR, size_t tN, size_t tC>
	inline void mul( const Matrix<TVal, tR, tN> & pFirst, const Matrix<TVal, tN, tC> & pSecond, Matrix<TVal, tR, tC> & pResult )
	{
		for( size_t fRowIndex = 0; fRowIndex < tR; ++fRowIndex )
		{
			for( size_t sColumnIndex = 0; sColumnIndex < tC; ++sColumnIndex )
			{
				TVal currentElemValue = static_cast<TVal>( 0 );
				for( size_t elemIndex = 0; elemIndex < tN; ++elemIndex )
				{
					currentElemValue += ( pFirst[fRowIndex][elemIndex] * pSecond[elemIndex][sColumnIndex] );
				}
				pResult[fRowIndex][sColumnIndex] = currentElemValue;
			}
		}
	}

	template <typename TVal>
	inline void mul( const Matrix2x2<TVal> & pFirst, const Matrix2x2<TVal> & pSecond, Matrix2x2<TVal> & pResult )
	{
		const auto firstE00MulSecondR0 = pFirst.row0[0] * pSecond.row0;
		const auto firstE01MulSecondR1 = pFirst.row0[1] * pSecond.row1;
		pResult.row0 = firstE00MulSecondR0 + firstE01MulSecondR1;

		const auto firstE10MulSecondR0 = pFirst.row1[0] * pSecond.row0;
		const auto firstE11MulSecondR1 = pFirst.row1[1] * pSecond.row1;
		pResult.row1 = firstE10MulSecondR0 + firstE11MulSecondR1;
	}

	template <typename TVal>
	inline void mul( const Matrix3x3<TVal> & pFirst, const Matrix3x3<TVal> & pSecond, Matrix3x3<TVal> & pResult )
	{
		const auto firstE00MulSecondR0 = pFirst.row0[0] * pSecond.row0;
		const auto firstE01MulSecondR1 = pFirst.row0[1] * pSecond.row1;
		const auto firstE02MulSecondR2 = pFirst.row0[2] * pSecond.row2;
		pResult.row0 = firstE00MulSecondR0 + firstE01MulSecondR1 + firstE02MulSecondR2;

		const auto firstE10MulSecondR0 = pFirst.row1[0] * pSecond.row0;
		const auto firstE11MulSecondR1 = pFirst.row1[1] * pSecond.row1;
		const auto firstE12MulSecondR2 = pFirst.row1[2] * pSecond.row2;
		pResult.row1 = firstE10MulSecondR0 + firstE11MulSecondR1 + firstE12MulSecondR2;

		const auto firstE20MulSecondR0 = pFirst.row2[0] * pSecond.row0;
		const auto firstE21MulSecondR1 = pFirst.row2[1] * pSecond.row1;
		const auto firstE22MulSecondR2 = pFirst.row2[2] * pSecond.row2;
		pResult.row2 = firstE20MulSecondR0 + firstE21MulSecondR1 + firstE22MulSecondR2;
	}

	template <typename TVal>
	inline void mul( const Matrix4x4<TVal> & pFirst, const Matrix4x4<TVal> & pSecond, Matrix4x4<TVal> & pResult )
	{
		const auto firstE00MulSecondR0 = pFirst.row0[0] * pSecond.row0;
		const auto firstE01MulSecondR1 = pFirst.row0[1] * pSecond.row1;
		const auto firstE02MulSecondR2 = pFirst.row0[2] * pSecond.row2;
		const auto firstE03MulSecondR3 = pFirst.row0[3] * pSecond.row3;
		pResult.row0 = firstE00MulSecondR0 + firstE01MulSecondR1 + firstE02MulSecondR2 + firstE03MulSecondR3;

		const auto firstE10MulSecondR0 = pFirst.row1[0] * pSecond.row0;
		const auto firstE11MulSecondR1 = pFirst.row1[1] * pSecond.row1;
		const auto firstE12MulSecondR2 = pFirst.row1[2] * pSecond.row2;
		const auto firstE13MulSecondR3 = pFirst.row1[3] * pSecond.row3;
		pResult.row1 = firstE10MulSecondR0 + firstE11MulSecondR1 + firstE12MulSecondR2 + firstE13MulSecondR3;

		const auto firstE20MulSecondR0 = pFirst.row2[0] * pSecond.row0;
		const auto firstE21MulSecondR1 = pFirst.row2[1] * pSecond.row1;
		const auto firstE22MulSecondR2 = pFirst.row2[2] * pSecond.row2;
		const auto firstE23MulSecondR3 = pFirst.row2[3] * pSecond.row3;
		pResult.row2 = firstE20MulSecondR0 + firstE21MulSecondR1 + firstE22MulSecondR2 + firstE23MulSecondR3;

		const auto firstE30MulSecondR0 = pFirst.row3[0] * pSecond.row0;
		const auto firstE31MulSecondR1 = pFirst.row3[1] * pSecond.row1;
		const auto firstE32MulSecondR2 = pFirst.row3[2] * pSecond.row2;
		const auto firstE33MulSecondR3 = pFirst.row3[3] * pSecond.row3;
		pResult.row3 = firstE30MulSecondR0 + firstE31MulSecondR1 + firstE32MulSecondR2 + firstE33MulSecondR3;
	}

	template <typename TVal>
	inline void mul( const Matrix2x2<TVal> & pMatrix, const Vector2<TVal> & pVector, Vector2<TVal> & pResult )
	{
		pResult[0] = pMatrix.row0 * pVector;
		pResult[1] = pMatrix.row1 * pVector;
	}

	template <typename TVal>
	inline void mul( const Vector2<TVal> & pVector, const Matrix2x2<TVal> & pMatrix, Vector2<TVal> & pResult )
	{
		pResult[0] = dot( pVector, pMatrix.column( 0 ) );
		pResult[1] = dot( pVector, pMatrix.column( 1 ) );
	}

	template <typename TVal>
	inline void mul( const Matrix3x3<TVal> & pMatrix, const Vector3<TVal> & pVector, Vector3<TVal> & pResult )
	{
		pResult.x = pMatrix.row0 * pVector;
		pResult.y = pMatrix.row1 * pVector;
		pResult.z = pMatrix.row2 * pVector;
	}

	template <typename TVal>
	inline void mul( const Vector3<TVal> & pVector, const Matrix3x3<TVal> & pMatrix, Vector3<TVal> & pResult )
	{
		pResult[0] = dot( pVector, pMatrix.column( 0 ) );
		pResult[1] = dot( pVector, pMatrix.column( 1 ) );
		pResult[2] = dot( pVector, pMatrix.column( 2 ) );
	}

	template <typename TVal>
	inline void mul( const Matrix4x4<TVal> & pMatrix, const Vector4<TVal> & pVector, Vector4<TVal> & pResult )
	{
		pResult.x = dot( pMatrix.row0, pVector );
		pResult.y = dot( pMatrix.row1, pVector );
		pResult.z = dot( pMatrix.row2, pVector );
		pResult.w = dot( pMatrix.row3, pVector );
	}

	template <typename TVal>
	inline void mul( const Vector4<TVal> & pVector, const Matrix4x4<TVal> & pMatrix, Vector4<TVal> & pResult )
	{
		pResult[0] = dot( pVector, pMatrix.column( 0 ) );
		pResult[1] = dot( pVector, pMatrix.column( 1 ) );
		pResult[2] = dot( pVector, pMatrix.column( 2 ) );
		pResult[3] = dot( pVector, pMatrix.column( 3 ) );
	}

	template <typename TVal, size_t tN>
	inline Matrix<TVal, tN, tN> mul( const Matrix<TVal, tN, tN> & pFirst, const Matrix<TVal, tN, tN> & pSecond )
	{
		Matrix<TVal, tN, tN> result;
		mul( pFirst, pSecond, result );
		return result;
	}

	template <typename TVal, size_t tN>
	inline Vector<TVal, tN> mul( const Matrix<TVal, tN, tN> & pMatrix, const Vector<TVal, tN> & pVector )
	{
		Vector<TVal, tN> result;
		mul( pMatrix, pVector, result );
		return result;
	}

	template <typename TVal, size_t tN>
	inline Vector<TVal, tN> mul( const Vector<TVal, tN> & pVector, const Matrix<TVal, tN, tN> & pMatrix )
	{
		Vector<TVal, tN> result;
		mul( pVector, pMatrix, result );
		return result;
	}

}

#if( TS3_MATH_SIMD_ENABLE )
//#  include "matrixOpsSIMD.inl"
#endif

#endif //TESSLINE_3DX_MATRIXOPS_H
