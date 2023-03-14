
#ifndef __TS3_MATH_MATRIX_UTILS_H__
#define __TS3_MATH_MATRIX_UTILS_H__

#include "matrixOps.h"

namespace ts3::math
{

	template <typename TVal>
	inline Matrix2x2<TVal> identity2()
	{
		constexpr TVal cxOne = static_cast<TVal>( 1 );
		constexpr TVal cxZero = static_cast<TVal>( 0 );

		return Matrix2x2<TVal> {
			cxOne,  cxZero,
			cxZero, cxOne
		};
	}

	template <typename TVal>
	inline Matrix3x3<TVal> identity3()
	{
		constexpr TVal cxOne = static_cast<TVal>( 1 );
		constexpr TVal cxZero = static_cast<TVal>( 0 );

		return Matrix3x3<TVal> {
			cxOne,  cxZero, cxZero,
			cxZero, cxOne,  cxZero,
			cxZero, cxZero, cxOne
		};
	}

	template <typename TVal>
	inline Matrix4x4<TVal> identity4()
	{
		constexpr TVal cxOne = static_cast<TVal>( 1 );
		constexpr TVal cxZero = static_cast<TVal>( 0 );

		return Matrix4x4<TVal> {
			cxOne,  cxZero, cxZero, cxZero,
			cxZero, cxOne,  cxZero, cxZero,
			cxZero, cxZero, cxOne,  cxZero,
			cxZero, cxZero, cxZero, cxOne
		};
	}

	template <typename TVal>
	inline void transposeInplace( Matrix2x2<TVal> & pMatrix )
	{
		std::swap( pMatrix[1][0], pMatrix[0][1] );
	}

	template <typename TVal>
	inline Matrix2x2<TVal> transpose( const Matrix2x2<TVal> & pMatrix )
	{
		return Matrix2x2<TVal> {
			pMatrix[0][0], pMatrix[1][0],
			pMatrix[0][1], pMatrix[1][1]
		};
	}

	template <typename TVal>
	inline void transposeInplace( Matrix3x3<TVal> & pMatrix )
	{
		std::swap( pMatrix[1][0], pMatrix[0][1]);
		std::swap( pMatrix[2][0], pMatrix[0][2]);
		std::swap( pMatrix[2][1], pMatrix[1][2]);
	}

	template <typename TVal>
	inline Matrix3x3<TVal> transpose( const Matrix3x3<TVal> & pMatrix )
	{
		return Matrix3x3<TVal> {
			pMatrix[0][0], pMatrix[1][0], pMatrix[2][0],
			pMatrix[0][1], pMatrix[1][1], pMatrix[2][1],
			pMatrix[0][2], pMatrix[1][2], pMatrix[2][2]
		};
	}

	template <typename TVal>
	inline void transposeInplace( Matrix4x4<TVal> & pMatrix )
	{
		std::swap( pMatrix[1][0], pMatrix[0][1]);
		std::swap( pMatrix[2][0], pMatrix[0][2]);
		std::swap( pMatrix[2][1], pMatrix[1][2]);
		std::swap( pMatrix[3][0], pMatrix[0][3]);
		std::swap( pMatrix[3][1], pMatrix[1][3]);
		std::swap( pMatrix[3][2], pMatrix[2][3]);
	}

	template <typename TVal>
	inline Matrix4x4<TVal> transpose( const Matrix4x4<TVal> & pMatrix )
	{
		return Matrix4x4<TVal> {
			pMatrix[0][0], pMatrix[1][0], pMatrix[2][0], pMatrix[3][0],
			pMatrix[0][1], pMatrix[1][1], pMatrix[2][1], pMatrix[3][1],
			pMatrix[0][2], pMatrix[1][2], pMatrix[2][2], pMatrix[3][2],
			pMatrix[0][3], pMatrix[1][3], pMatrix[2][3], pMatrix[3][3]
		};
	}

	template <typename TVal>
	inline Matrix4x4<TVal> lookAtLH( const Vec3<TVal> & pEyePos, const Vec3<TVal> & pLookAt, const Vec3<TVal> & pUpVec )
	{
		const auto zAxis = normalize( pLookAt - pEyePos );
		const auto xAxis = normalize( cross( pUpVec, zAxis ) );
		const auto yAxis = cross( zAxis, xAxis );

		const TVal xAxisEyeDot = dot( xAxis, pEyePos );
		const TVal yAxisEyeDot = dot( yAxis, pEyePos );
		const TVal zAxisEyeDot = dot( zAxis, pEyePos );

		return Matrix4x4<TVal> {
			xAxis.x, xAxis.y, xAxis.z, -xAxisEyeDot,
			yAxis.x, yAxis.y, yAxis.z, -yAxisEyeDot,
			zAxis.x, zAxis.y, zAxis.z, -zAxisEyeDot,
			0,       0,       0,        1
		};
	}

	template <typename TVal>
	inline Matrix4x4<TVal> lookAtRH( const Vec3<TVal> & pEyePos, const Vec3<TVal> & pLookAt, const Vec3<TVal> & pUpVec )
	{
		const auto zAxis = normalize( pLookAt - pEyePos );
		const auto xAxis = normalize( cross( zAxis, pUpVec ) );
		const auto yAxis = cross( xAxis, zAxis );

		const TVal xAxisEyeDot = dot( xAxis, pEyePos );
		const TVal yAxisEyeDot = dot( yAxis, pEyePos );
		const TVal zAxisEyeDot = dot( zAxis, pEyePos );

		return Matrix4x4<TVal> {
			 xAxis.x,  xAxis.y,  xAxis.z, -xAxisEyeDot,
			 yAxis.x,  yAxis.y,  yAxis.z, -yAxisEyeDot,
			-zAxis.x, -zAxis.y, -zAxis.z,  zAxisEyeDot,
			0,         0,        0,        1
		};
	}

	template <typename TVal>
	inline Matrix4x4<TVal> orthoCenterLH( TVal pWidth, TVal pHeight, TVal pZNear, TVal pZFar )
	{
		const TVal mElem00 = static_cast<TVal>( 2 ) / pWidth;
		const TVal mElem11 = static_cast<TVal>( 2 ) / pHeight;
		const TVal mElem22 = static_cast<TVal>( 1 ) / ( pZFar - pZNear );
		const TVal mElem23 = pZNear / ( pZNear - pZFar );

		return Matrix4x4<TVal> {
			mElem00, 0,       0,       0,
			0,       mElem11, 0,       0,
			0,       0,       mElem22, mElem23,
			0,       0,       0,       1
		};
	}

	template <typename TVal>
	inline Matrix4x4<TVal> orthoOffCenterLH( TVal pLeft, TVal pRight, TVal pBottom, TVal pTop, TVal pZNear, TVal pZFar )
	{
		const TVal mElem00 = static_cast<TVal>( 2 ) / ( pRight - pLeft );
		const TVal mElem11 = static_cast<TVal>( 2 ) / ( pTop - pBottom );
		const TVal mElem22 = static_cast<TVal>( 1 ) / ( pZFar - pZNear );
		const TVal mElem03 = ( pLeft + pRight ) / ( pLeft - pRight );
		const TVal mElem13 = ( pBottom + pTop ) / ( pBottom - pTop );
		const TVal mElem23 = pZNear / ( pZNear - pZFar );

		return Matrix4x4<TVal> {
				mElem00, 0,       0,       mElem03,
				0,       mElem11, 0,       mElem13,
				0,       0,       mElem22, mElem23,
				0,       0,       0,       1
		};
	}

	template <typename TVal>
	inline Matrix4x4<TVal> orthoCenterRH( TVal pWidth, TVal pHeight, TVal pZNear, TVal pZFar )
	{
		const TVal mElem00 = static_cast<TVal>( 2 ) / pWidth;
		const TVal mElem11 = static_cast<TVal>( 2 ) / pHeight;
		const TVal mElem22 = static_cast<TVal>( 1 ) / ( pZNear - pZFar );
		const TVal mElem23 = pZNear / ( pZNear - pZFar );

		return Matrix4x4<TVal> {
			mElem00, 0,       0,       0,
			0,       mElem11, 0,       0,
			0,       0,       mElem22, mElem23,
			0,       0,       0,       1
		};
	}

	template <typename TVal>
	inline Matrix4x4<TVal> orthoOffCenterRH( TVal pLeft, TVal pRight, TVal pBottom, TVal pTop, TVal pZNear, TVal pZFar )
	{
		const TVal mElem00 = static_cast<TVal>( 2 ) / ( pRight - pLeft );
		const TVal mElem11 = static_cast<TVal>( 2 ) / ( pTop - pBottom );
		const TVal mElem22 = static_cast<TVal>( 1 ) / ( pZNear - pZFar );
		const TVal mElem03 = ( pLeft + pRight ) / ( pLeft - pRight );
		const TVal mElem13 = ( pBottom + pTop ) / ( pBottom - pTop );
		const TVal mElem23 = pZNear / ( pZNear - pZFar );

		return Matrix4x4<TVal> {
				mElem00, 0,       0,       mElem03,
				0,       mElem11, 0,       mElem13,
				0,       0,       mElem22, mElem23,
				0,       0,       0,       1
		};
	}

	template <typename TVal>
	inline Matrix4x4<TVal> perspectiveAspectLH( TVal pFOV, TVal pAspectRatio, TVal pZNear, TVal pZFar )
	{
		const TVal halfTanFOV = std::tan( static_cast<TVal>( 0.5 ) * pFOV );

		const TVal mElem00 = static_cast<TVal>( 1 ) / ( pAspectRatio * halfTanFOV );
		const TVal mElem11 = static_cast<TVal>( 1 ) / halfTanFOV;
		const TVal mElem22 = ( pZFar + pZNear ) / ( pZFar - pZNear );
		const TVal mElem23 = ( 2 * pZFar * pZNear ) / ( pZNear - pZFar );

		return Matrix4x4<TVal> {
			mElem00, 0,       0,       0,
			0,       mElem11, 0,       0,
			0,       0,       mElem22, mElem23,
			0,       0,       1,       0
		};
	}

	template <typename TVal>
	inline Matrix4x4<TVal> perspectiveAspectRH( TVal pFOV, TVal pAspectRatio, TVal pZNear, TVal pZFar )
	{
		const TVal halfTanFOV = std::tan( static_cast<TVal>( 0.5 ) * pFOV );

		const TVal mElem00 = static_cast<TVal>( 1 ) / ( pAspectRatio * halfTanFOV );
		const TVal mElem11 = static_cast<TVal>( 1 ) / halfTanFOV;
		const TVal mElem22 = ( pZFar + pZNear ) / ( pZNear - pZFar );
		const TVal mElem23 = ( 2 * pZFar * pZNear ) / ( pZNear - pZFar );

		return Matrix4x4<TVal> {
			mElem00, 0,       0,       0,
			0,       mElem11, 0,       0,
			0,       0,       mElem22, mElem23,
			0,       0,       -1,      0
		};
	}

	template <typename TVal>
	inline Matrix4x4<TVal> perspectiveRectLH( TVal pFOV, TVal pWidth, TVal pHeight, TVal pZNear, TVal pZFar )
	{
		const TVal fovFactor = std::cos( static_cast<TVal>( 0.5 ) * pFOV ) / std::sin( static_cast<TVal>( 0.5 ) * pFOV );

		const TVal mElem00 = fovFactor * pHeight / pWidth;
		const TVal mElem11 = fovFactor;
		const TVal mElem22 = ( pZFar + pZNear ) / ( pZFar - pZNear );
		const TVal mElem23 = ( 2 * pZFar * pZNear ) / ( pZNear - pZFar );

		return Matrix4x4<TVal> {
			mElem00, 0,       0,       0,
			0,       mElem11, 0,       0,
			0,       0,       mElem22, mElem23,
			0,       0,       1,       0
		};
	}

	template <typename TVal>
	inline Matrix4x4<TVal> perspectiveRectRH( TVal pFOV, TVal pWidth, TVal pHeight, TVal pZNear, TVal pZFar )
	{
		const TVal fovFactor = std::cos( static_cast<TVal>( 0.5 ) * pFOV ) / std::sin( static_cast<TVal>( 0.5 ) * pFOV );

		const TVal mElem00 = fovFactor * pHeight / pWidth;
		const TVal mElem11 = fovFactor;
		const TVal mElem22 = ( pZFar + pZNear ) / ( pZFar - pZNear );
		const TVal mElem23 = ( 2 * pZFar * pZNear ) / ( pZNear - pZFar );

		return Matrix4x4<TVal> {
			mElem00, 0,        0,       0,
			0,       mElem11,  0,       0,
			0,       0,        mElem22, mElem23,
			0,       0,       -1,       0
		};
	}

	template <typename TVal>
	inline Matrix4x4<TVal> rotationAxisX( TVal pAngle )
	{
		const TVal angleSin = std::sin( pAngle );
		const TVal angleCos = std::cos( pAngle );

		return Matrix4x4<TVal> {
			1, 0,         0,        0,
			0, angleCos, -angleSin, 0,
			0, angleSin,  angleCos, 0,
			0, 0,         0,        1
		};
	}

	template <typename TVal>
	inline Matrix4x4<TVal> rotationAxisY( TVal pAngle )
	{
		const TVal angleSin = std::sin( pAngle );
		const TVal angleCos = std::cos( pAngle );

		return Matrix4x4<TVal> {
			 angleCos, 0, angleSin, 0,
			 0,        1, 0,        0,
			-angleSin, 0, angleCos, 0,
			 0,        0, 0,        1
		};
	}

	template <typename TVal>
	inline Matrix4x4<TVal> rotationAxisZ( TVal pAngle )
	{
		const TVal angleSin = std::sin( pAngle );
		const TVal angleCos = std::cos( pAngle );

		return Matrix4x4<TVal> {
			angleCos, -angleSin, 0, 0,
			angleSin,  angleCos, 0, 0,
			0,         0,        1, 0,
			0,         0,        0, 1
		};
	}

	template <typename TVal>
	inline Matrix4x4<TVal> rotationCombined( const Vector3<TVal> & pAxis, TVal pAngle )
	{
		const TVal angleSin = std::sin( pAngle );
		const TVal angleCos = std::cos( pAngle );
		const auto normalizedAxis = normalize( pAxis );
		const auto icnAxis = normalizedAxis * ( static_cast<TVal>( 1 ) - angleCos );

		Matrix3x3<TVal> rMat;
		rMat[0][0] = pAxis.x * icnAxis.x + angleCos;
		rMat[0][1] = pAxis.x * icnAxis.y - pAxis.z * angleSin;
		rMat[0][2] = pAxis.x * icnAxis.z + pAxis.y * angleSin;

		rMat[1][0] = pAxis.y * icnAxis.x + pAxis.z * angleSin;
		rMat[1][1] = pAxis.y * icnAxis.y + angleCos;
		rMat[1][2] = pAxis.y * icnAxis.z - pAxis.x * angleSin;

		rMat[2][0] = pAxis.z * icnAxis.x - pAxis.y * angleSin;
		rMat[2][1] = pAxis.z * icnAxis.y + pAxis.y * angleSin;
		rMat[2][2] = pAxis.z * icnAxis.z + angleCos;

		return Matrix4x4<TVal> {
			rMat.row0.x, rMat.row0.y, rMat.row0.z, 0,
			rMat.row1.x, rMat.row1.y, rMat.row1.z, 0,
			rMat.row2.x, rMat.row2.y, rMat.row2.z, 0,
			0,           0,           0,           1
		};
	}

	template <typename TVal>
	inline Matrix4x4<TVal> scaling( TVal pScaleX, TVal pScaleY, TVal pScaleZ )
	{
		return Matrix4x4<TVal> {
			pScaleX, 0,       0,       0,
			0,       pScaleY, 0,       0,
			0,       0,       pScaleZ, 0,
			0,       0,       0,       1
		};
	}

	template <typename TVal>
	inline Matrix4x4<TVal> translation( TVal pMoveX, TVal pMoveY, TVal pMoveZ )
	{
		return Matrix4x4<TVal> {
			1, 0, 0, pMoveX,
			0, 1, 0, pMoveY,
			0, 0, 1, pMoveZ,
			0, 0, 0, 1
		};
	}

}

#if( TS3_MATH_SIMD_ENABLE )
#  include "matrixUtilsSIMD.inl"
#endif

#endif // __TS3_MATH_MATRIX_UTILS_H__
