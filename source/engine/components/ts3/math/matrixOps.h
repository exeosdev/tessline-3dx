
#ifndef __TS3_MATH_MATRIX_OPS_H__
#define __TS3_MATH_MATRIX_OPS_H__

#include "matrix.h"
#include "vectorOps.h"

namespace ts3::math
{

	template <typename Tp, size_t tC>
	inline void add( const Matrix<Tp, 2, tC> & pFirst, const Matrix<Tp, 2, tC> & pSecond, Matrix<Tp, 2, tC> & pResult )
	{
		pResult.row0 = pFirst.row0 + pSecond.row0;
		pResult.row1 = pFirst.row1 + pSecond.row1;
	}

	template <typename Tp, size_t tC>
	inline void add( const Matrix<Tp, 3, tC> & pFirst, const Matrix<Tp, 3, tC> & pSecond, Matrix<Tp, 3, tC> & pResult )
	{
		pResult.row0 = pFirst.row0 + pSecond.row0;
		pResult.row1 = pFirst.row1 + pSecond.row1;
		pResult.row2 = pFirst.row2 + pSecond.row2;
	}

	template <typename Tp, size_t tC>
	inline void add( const Matrix<Tp, 4, tC> & pFirst, const Matrix<Tp, 4, tC> & pSecond, Matrix<Tp, 4, tC> & pResult )
	{
		pResult.row0 = pFirst.row0 + pSecond.row0;
		pResult.row1 = pFirst.row1 + pSecond.row1;
		pResult.row2 = pFirst.row2 + pSecond.row2;
		pResult.row3 = pFirst.row3 + pSecond.row3;
	}

	template <typename TpMatrix>
	inline TpMatrix add( const TpMatrix & pFirst, const TpMatrix & pSecond )
	{
		TpMatrix result;
		add( pFirst, pSecond, result );
		return result;
	}

	template <typename Tp, size_t tC>
	inline void sub( const Matrix<Tp, 2, tC> & pFirst, const Matrix<Tp, 2, tC> & pSecond, Matrix<Tp, 2, tC> & pResult )
	{
		pResult.row0 = pFirst.row0 - pSecond.row0;
		pResult.row1 = pFirst.row1 - pSecond.row1;
	}

	template <typename Tp, size_t tC>
	inline void sub( const Matrix<Tp, 3, tC> & pFirst, const Matrix<Tp, 3, tC> & pSecond, Matrix<Tp, 3, tC> & pResult )
	{
		pResult.row0 = pFirst.row0 - pSecond.row0;
		pResult.row1 = pFirst.row1 - pSecond.row1;
		pResult.row2 = pFirst.row2 - pSecond.row2;
	}

	template <typename Tp, size_t tC>
	inline void sub( const Matrix<Tp, 4, tC> & pFirst, const Matrix<Tp, 4, tC> & pSecond, Matrix<Tp, 4, tC> & pResult )
	{
		pResult.row0 = pFirst.row0 - pSecond.row0;
		pResult.row1 = pFirst.row1 - pSecond.row1;
		pResult.row2 = pFirst.row2 - pSecond.row2;
		pResult.row3 = pFirst.row3 - pSecond.row3;
	}

	template <typename TpMatrix>
	inline TpMatrix sub( const TpMatrix & pFirst, const TpMatrix & pSecond )
	{
		TpMatrix result;
		sub( pFirst, pSecond, result );
		return result;
	}

	template <typename Tp, size_t tR, size_t tN, size_t tC>
	inline void mul( const Matrix<Tp, tR, tN> & pFirst, const Matrix<Tp, tN, tC> & pSecond, Matrix<Tp, tR, tC> & pResult )
	{
		for( size_t fRowIndex = 0; fRowIndex < tR; ++fRowIndex )
		{
			for( size_t sColumnIndex = 0; sColumnIndex < tC; ++sColumnIndex )
			{
				Tp currentElemValue = static_cast<Tp>( 0 );
				for( size_t elemIndex = 0; elemIndex < tN; ++elemIndex )
				{
					currentElemValue += ( pFirst[fRowIndex][elemIndex] * pSecond[elemIndex][sColumnIndex] );
				}
				pResult[fRowIndex][sColumnIndex] = currentElemValue;
			}
		}
	}

	template <typename Tp>
	inline void mul( const Matrix2x2<Tp> & pFirst, const Matrix2x2<Tp> & pSecond, Matrix2x2<Tp> & pResult )
	{
		const auto firstE00MulSecondR0 = pFirst.row0[0] * pSecond.row0;
		const auto firstE01MulSecondR1 = pFirst.row0[1] * pSecond.row1;
		pResult.row0 = firstE00MulSecondR0 + firstE01MulSecondR1;

		const auto firstE10MulSecondR0 = pFirst.row1[0] * pSecond.row0;
		const auto firstE11MulSecondR1 = pFirst.row1[1] * pSecond.row1;
		pResult.row1 = firstE10MulSecondR0 + firstE11MulSecondR1;
	}

	template <typename Tp>
	inline void mul( const Matrix3x3<Tp> & pFirst, const Matrix3x3<Tp> & pSecond, Matrix3x3<Tp> & pResult )
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

	template <typename Tp>
	inline void mul( const Matrix4x4<Tp> & pFirst, const Matrix4x4<Tp> & pSecond, Matrix4x4<Tp> & pResult )
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

	template <typename Tp>
	inline void mul( const Matrix2x2<Tp> & pMatrix, const Vector2<Tp> & pVector, Vector2<Tp> & pResult )
	{
		pResult[0] = pMatrix.row0 * pVector;
		pResult[1] = pMatrix.row1 * pVector;
	}

	template <typename Tp>
	inline void mul( const Vector2<Tp> & pVector, const Matrix2x2<Tp> & pMatrix, Vector2<Tp> & pResult )
	{
		pResult[0] = dot( pVector, pMatrix.column( 0 ) );
		pResult[1] = dot( pVector, pMatrix.column( 1 ) );
	}

	template <typename Tp>
	inline void mul( const Matrix3x3<Tp> & pMatrix, const Vector3<Tp> & pVector, Vector3<Tp> & pResult )
	{
		pResult.x = pMatrix.row0 * pVector;
		pResult.y = pMatrix.row1 * pVector;
		pResult.z = pMatrix.row2 * pVector;
	}

	template <typename Tp>
	inline void mul( const Vector3<Tp> & pVector, const Matrix3x3<Tp> & pMatrix, Vector3<Tp> & pResult )
	{
		pResult[0] = dot( pVector, pMatrix.column( 0 ) );
		pResult[1] = dot( pVector, pMatrix.column( 1 ) );
		pResult[2] = dot( pVector, pMatrix.column( 2 ) );
	}

	template <typename Tp>
	inline void mul( const Matrix4x4<Tp> & pMatrix, const Vector4<Tp> & pVector, Vector4<Tp> & pResult )
	{
		pResult.x = dot( pMatrix.row0, pVector );
		pResult.y = dot( pMatrix.row1, pVector );
		pResult.z = dot( pMatrix.row2, pVector );
		pResult.w = dot( pMatrix.row3, pVector );
	}

	template <typename Tp>
	inline void mul( const Vector4<Tp> & pVector, const Matrix4x4<Tp> & pMatrix, Vector4<Tp> & pResult )
	{
		pResult[0] = dot( pVector, pMatrix.column( 0 ) );
		pResult[1] = dot( pVector, pMatrix.column( 1 ) );
		pResult[2] = dot( pVector, pMatrix.column( 2 ) );
		pResult[3] = dot( pVector, pMatrix.column( 3 ) );
	}

	template <typename Tp, size_t tN>
	inline Matrix<Tp, tN, tN> mul( const Matrix<Tp, tN, tN> & pFirst, const Matrix<Tp, tN, tN> & pSecond )
	{
		Matrix<Tp, tN, tN> result;
		mul( pFirst, pSecond, result );
		return result;
	}

	template <typename Tp, size_t tN>
	inline Vector<Tp, tN> mul( const Matrix<Tp, tN, tN> & pMatrix, const Vector<Tp, tN> & pVector )
	{
		Vector<Tp, tN> result;
		mul( pMatrix, pVector, result );
		return result;
	}

	template <typename Tp, size_t tN>
	inline Vector<Tp, tN> mul( const Vector<Tp, tN> & pVector, const Matrix<Tp, tN, tN> & pMatrix )
	{
		Vector<Tp, tN> result;
		mul( pVector, pMatrix, result );
		return result;
	}

}

#if( TS3_MATH_SIMD_ENABLE )
//#  include "matrixOpsSIMD.inl"
#endif

#endif //TESSLINE_3DX_MATRIXOPS_H
