
#ifndef __TS3_MATH_MATRIX_UTILS_H__
#define __TS3_MATH_MATRIX_UTILS_H__

#include "matrixOps.h"

namespace ts3::math
{

	template <typename Tp>
	inline Matrix2x2<Tp> identity2()
	{
		constexpr Tp cxOne = static_cast<Tp>( 1 );
		constexpr Tp cxZero = static_cast<Tp>( 0 );

		return Matrix2x2<Tp> {
			cxOne,  cxZero,
			cxZero, cxOne
		};
	}

	template <typename Tp>
	inline Matrix3x3<Tp> identity3()
	{
		constexpr Tp cxOne = static_cast<Tp>( 1 );
		constexpr Tp cxZero = static_cast<Tp>( 0 );

		return Matrix3x3<Tp> {
			cxOne,  cxZero, cxZero,
			cxZero, cxOne,  cxZero,
			cxZero, cxZero, cxOne
		};
	}

	template <typename Tp>
	inline Matrix4x4<Tp> identity4()
	{
		constexpr Tp cxOne = static_cast<Tp>( 1 );
		constexpr Tp cxZero = static_cast<Tp>( 0 );

		return Matrix4x4<Tp> {
			cxOne,  cxZero, cxZero, cxZero,
			cxZero, cxOne,  cxZero, cxZero,
			cxZero, cxZero, cxOne,  cxZero,
			cxZero, cxZero, cxZero, cxOne
		};
	}

	template <typename Tp>
	inline void transposeInplace( Matrix2x2<Tp> & pMatrix )
	{
		std::swap( pMatrix[1][0], pMatrix[0][1] );
	}

	template <typename Tp>
	inline Matrix2x2<Tp> transpose( const Matrix2x2<Tp> & pMatrix )
	{
		return Matrix2x2<Tp> {
			pMatrix[0][0], pMatrix[1][0],
			pMatrix[0][1], pMatrix[1][1]
		};
	}

	template <typename Tp>
	inline void transposeInplace( Matrix3x3<Tp> & pMatrix )
	{
		std::swap( pMatrix[1][0], pMatrix[0][1]);
		std::swap( pMatrix[2][0], pMatrix[0][2]);
		std::swap( pMatrix[2][1], pMatrix[1][2]);
	}

	template <typename Tp>
	inline Matrix3x3<Tp> transpose( const Matrix3x3<Tp> & pMatrix )
	{
		return Matrix3x3<Tp> {
			pMatrix[0][0], pMatrix[1][0], pMatrix[2][0],
			pMatrix[0][1], pMatrix[1][1], pMatrix[2][1],
			pMatrix[0][2], pMatrix[1][2], pMatrix[2][2]
		};
	}

	template <typename Tp>
	inline void transposeInplace( Matrix4x4<Tp> & pMatrix )
	{
		std::swap( pMatrix[1][0], pMatrix[0][1]);
		std::swap( pMatrix[2][0], pMatrix[0][2]);
		std::swap( pMatrix[2][1], pMatrix[1][2]);
		std::swap( pMatrix[3][0], pMatrix[0][3]);
		std::swap( pMatrix[3][1], pMatrix[1][3]);
		std::swap( pMatrix[3][2], pMatrix[2][3]);
	}

	template <typename Tp>
	inline Matrix4x4<Tp> transpose( const Matrix4x4<Tp> & pMatrix )
	{
		return Matrix4x4<Tp> {
			pMatrix[0][0], pMatrix[1][0], pMatrix[2][0], pMatrix[3][0],
			pMatrix[0][1], pMatrix[1][1], pMatrix[2][1], pMatrix[3][1],
			pMatrix[0][2], pMatrix[1][2], pMatrix[2][2], pMatrix[3][2],
			pMatrix[0][3], pMatrix[1][3], pMatrix[2][3], pMatrix[3][3]
		};
	}

	template <typename Tp>
	inline Matrix4x4<Tp> lookAtLH( const Vec3<Tp> & pEyePos, const Vec3<Tp> & pLookAt, const Vec3<Tp> & pUpVec )
	{
		const auto zAxis = normalize( pLookAt - pEyePos );
		const auto xAxis = normalize( cross( pUpVec, zAxis ) );
		const auto yAxis = cross( zAxis, xAxis );

		const Tp xAxisEyeDot = dot( xAxis, pEyePos );
		const Tp yAxisEyeDot = dot( yAxis, pEyePos );
		const Tp zAxisEyeDot = dot( zAxis, pEyePos );

		return Matrix4x4<Tp> {
			xAxis.x, xAxis.y, xAxis.z, -xAxisEyeDot,
			yAxis.x, yAxis.y, yAxis.z, -yAxisEyeDot,
			zAxis.x, zAxis.y, zAxis.z, -zAxisEyeDot,
			0,       0,       0,        1
		};
	}

	template <typename Tp>
	inline Matrix4x4<Tp> lookAtRH( const Vec3<Tp> & pEyePos, const Vec3<Tp> & pLookAt, const Vec3<Tp> & pUpVec )
	{
		const auto zAxis = normalize( pLookAt - pEyePos );
		const auto xAxis = normalize( cross( zAxis, pUpVec ) );
		const auto yAxis = cross( xAxis, zAxis );

		const Tp xAxisEyeDot = dot( xAxis, pEyePos );
		const Tp yAxisEyeDot = dot( yAxis, pEyePos );
		const Tp zAxisEyeDot = dot( zAxis, pEyePos );

		return Matrix4x4<Tp> {
			 xAxis.x,  xAxis.y,  xAxis.z, -xAxisEyeDot,
			 yAxis.x,  yAxis.y,  yAxis.z, -yAxisEyeDot,
			-zAxis.x, -zAxis.y, -zAxis.z,  zAxisEyeDot,
			0,         0,        0,        1
		};
	}

	template <typename Tp>
	inline Matrix4x4<Tp> orthoCenterLH( Tp pWidth, Tp pHeight, Tp pZNear, Tp pZFar )
	{
		const Tp mElem00 = static_cast<Tp>( 2 ) / pWidth;
		const Tp mElem11 = static_cast<Tp>( 2 ) / pHeight;
		const Tp mElem22 = static_cast<Tp>( 1 ) / ( pZFar - pZNear );
		const Tp mElem23 = pZNear / ( pZNear - pZFar );

		return Matrix4x4<Tp> {
			mElem00, 0,       0,       0,
			0,       mElem11, 0,       0,
			0,       0,       mElem22, mElem23,
			0,       0,       0,       1
		};
	}

	template <typename Tp>
	inline Matrix4x4<Tp> orthoOffCenterLH( Tp pLeft, Tp pRight, Tp pBottom, Tp pTop, Tp pZNear, Tp pZFar )
	{
		const Tp mElem00 = static_cast<Tp>( 2 ) / ( pRight - pLeft );
		const Tp mElem11 = static_cast<Tp>( 2 ) / ( pTop - pBottom );
		const Tp mElem22 = static_cast<Tp>( 1 ) / ( pZFar - pZNear );
		const Tp mElem03 = ( pLeft + pRight ) / ( pLeft - pRight );
		const Tp mElem13 = ( pBottom + pTop ) / ( pBottom - pTop );
		const Tp mElem23 = pZNear / ( pZNear - pZFar );

		return Matrix4x4<Tp> {
				mElem00, 0,       0,       mElem03,
				0,       mElem11, 0,       mElem13,
				0,       0,       mElem22, mElem23,
				0,       0,       0,       1
		};
	}

	template <typename Tp>
	inline Matrix4x4<Tp> orthoCenterRH( Tp pWidth, Tp pHeight, Tp pZNear, Tp pZFar )
	{
		const Tp mElem00 = static_cast<Tp>( 2 ) / pWidth;
		const Tp mElem11 = static_cast<Tp>( 2 ) / pHeight;
		const Tp mElem22 = static_cast<Tp>( 1 ) / ( pZNear - pZFar );
		const Tp mElem23 = pZNear / ( pZNear - pZFar );

		return Matrix4x4<Tp> {
			mElem00, 0,       0,       0,
			0,       mElem11, 0,       0,
			0,       0,       mElem22, mElem23,
			0,       0,       0,       1
		};
	}

	template <typename Tp>
	inline Matrix4x4<Tp> orthoOffCenterRH( Tp pLeft, Tp pRight, Tp pBottom, Tp pTop, Tp pZNear, Tp pZFar )
	{
		const Tp mElem00 = static_cast<Tp>( 2 ) / ( pRight - pLeft );
		const Tp mElem11 = static_cast<Tp>( 2 ) / ( pTop - pBottom );
		const Tp mElem22 = static_cast<Tp>( 1 ) / ( pZNear - pZFar );
		const Tp mElem03 = ( pLeft + pRight ) / ( pLeft - pRight );
		const Tp mElem13 = ( pBottom + pTop ) / ( pBottom - pTop );
		const Tp mElem23 = pZNear / ( pZNear - pZFar );

		return Matrix4x4<Tp> {
				mElem00, 0,       0,       mElem03,
				0,       mElem11, 0,       mElem13,
				0,       0,       mElem22, mElem23,
				0,       0,       0,       1
		};
	}

	template <typename Tp>
	inline Matrix4x4<Tp> perspectiveAspectLH( Tp pFOV, Tp pAspectRatio, Tp pZNear, Tp pZFar )
	{
		const Tp halfTanFOV = std::tan( static_cast<Tp>( 0.5 ) * pFOV );

		const Tp mElem00 = static_cast<Tp>( 1 ) / ( pAspectRatio * halfTanFOV );
		const Tp mElem11 = static_cast<Tp>( 1 ) / halfTanFOV;
		const Tp mElem22 = ( pZFar + pZNear ) / ( pZFar - pZNear );
		const Tp mElem23 = ( 2 * pZFar * pZNear ) / ( pZNear - pZFar );

		return Matrix4x4<Tp> {
			mElem00, 0,       0,       0,
			0,       mElem11, 0,       0,
			0,       0,       mElem22, mElem23,
			0,       0,       1,       0
		};
	}

	template <typename Tp>
	inline Matrix4x4<Tp> perspectiveAspectRH( Tp pFOV, Tp pAspectRatio, Tp pZNear, Tp pZFar )
	{
		const Tp halfTanFOV = std::tan( static_cast<Tp>( 0.5 ) * pFOV );

		const Tp mElem00 = static_cast<Tp>( 1 ) / ( pAspectRatio * halfTanFOV );
		const Tp mElem11 = static_cast<Tp>( 1 ) / halfTanFOV;
		const Tp mElem22 = ( pZFar + pZNear ) / ( pZNear - pZFar );
		const Tp mElem23 = ( 2 * pZFar * pZNear ) / ( pZNear - pZFar );

		return Matrix4x4<Tp> {
			mElem00, 0,       0,       0,
			0,       mElem11, 0,       0,
			0,       0,       mElem22, mElem23,
			0,       0,       -1,      0
		};
	}

	template <typename Tp>
	inline Matrix4x4<Tp> perspectiveRectLH( Tp pFOV, Tp pWidth, Tp pHeight, Tp pZNear, Tp pZFar )
	{
		const Tp fovFactor = std::cos( static_cast<Tp>( 0.5 ) * pFOV ) / std::sin( static_cast<Tp>( 0.5 ) * pFOV );

		const Tp mElem00 = fovFactor * pHeight / pWidth;
		const Tp mElem11 = fovFactor;
		const Tp mElem22 = ( pZFar + pZNear ) / ( pZFar - pZNear );
		const Tp mElem23 = ( 2 * pZFar * pZNear ) / ( pZNear - pZFar );

		return Matrix4x4<Tp> {
			mElem00, 0,       0,       0,
			0,       mElem11, 0,       0,
			0,       0,       mElem22, mElem23,
			0,       0,       1,       0
		};
	}

	template <typename Tp>
	inline Matrix4x4<Tp> perspectiveRectRH( Tp pFOV, Tp pWidth, Tp pHeight, Tp pZNear, Tp pZFar )
	{
		const Tp fovFactor = std::cos( static_cast<Tp>( 0.5 ) * pFOV ) / std::sin( static_cast<Tp>( 0.5 ) * pFOV );

		const Tp mElem00 = fovFactor * pHeight / pWidth;
		const Tp mElem11 = fovFactor;
		const Tp mElem22 = ( pZFar + pZNear ) / ( pZFar - pZNear );
		const Tp mElem23 = ( 2 * pZFar * pZNear ) / ( pZNear - pZFar );

		return Matrix4x4<Tp> {
			mElem00, 0,        0,       0,
			0,       mElem11,  0,       0,
			0,       0,        mElem22, mElem23,
			0,       0,       -1,       0
		};
	}

	template <typename Tp>
	inline Matrix4x4<Tp> rotationAxisX( Tp pAngle )
	{
		const Tp angleSin = std::sin( pAngle );
		const Tp angleCos = std::cos( pAngle );

		return Matrix4x4<Tp> {
			1, 0,         0,        0,
			0, angleCos, -angleSin, 0,
			0, angleSin,  angleCos, 0,
			0, 0,         0,        1
		};
	}

	template <typename Tp>
	inline Matrix4x4<Tp> rotationAxisY( Tp pAngle )
	{
		const Tp angleSin = std::sin( pAngle );
		const Tp angleCos = std::cos( pAngle );

		return Matrix4x4<Tp> {
			 angleCos, 0, angleSin, 0,
			 0,        1, 0,        0,
			-angleSin, 0, angleCos, 0,
			 0,        0, 0,        1
		};
	}

	template <typename Tp>
	inline Matrix4x4<Tp> rotationAxisZ( Tp pAngle )
	{
		const Tp angleSin = std::sin( pAngle );
		const Tp angleCos = std::cos( pAngle );

		return Matrix4x4<Tp> {
			angleCos, -angleSin, 0, 0,
			angleSin,  angleCos, 0, 0,
			0,         0,        1, 0,
			0,         0,        0, 1
		};
	}

	template <typename Tp>
	inline Matrix4x4<Tp> rotationCombined( const Vector3<Tp> & pAxis, Tp pAngle )
	{
		const Tp angleSin = std::sin( pAngle );
		const Tp angleCos = std::cos( pAngle );
		const auto normalizedAxis = normalize( pAxis );
		const auto icnAxis = normalizedAxis * ( static_cast<Tp>( 1 ) - angleCos );

		Matrix3x3<Tp> rMat;
		rMat[0][0] = pAxis.x * icnAxis.x + angleCos;
		rMat[0][1] = pAxis.x * icnAxis.y - pAxis.z * angleSin;
		rMat[0][2] = pAxis.x * icnAxis.z + pAxis.y * angleSin;

		rMat[1][0] = pAxis.y * icnAxis.x + pAxis.z * angleSin;
		rMat[1][1] = pAxis.y * icnAxis.y + angleCos;
		rMat[1][2] = pAxis.y * icnAxis.z - pAxis.x * angleSin;

		rMat[2][0] = pAxis.z * icnAxis.x - pAxis.y * angleSin;
		rMat[2][1] = pAxis.z * icnAxis.y + pAxis.y * angleSin;
		rMat[2][2] = pAxis.z * icnAxis.z + angleCos;

		return Matrix4x4<Tp> {
			rMat.rv0.x, rMat.rv0.y, rMat.rv0.z, 0,
			rMat.rv1.x, rMat.rv1.y, rMat.rv1.z, 0,
			rMat.rv2.x, rMat.rv2.y, rMat.rv2.z, 0,
			0,          0,          0,          1
		};
	}

	template <typename Tp>
	inline Matrix4x4<Tp> scaling( Tp pScaleX, Tp pScaleY, Tp pScaleZ )
	{
		return Matrix4x4<Tp> {
			pScaleX, 0,       0,       0,
			0,       pScaleY, 0,       0,
			0,       0,       pScaleZ, 0,
			0,       0,       0,       1
		};
	}

	template <typename Tp>
	inline Matrix4x4<Tp> translation( Tp pMoveX, Tp pMoveY, Tp pMoveZ )
	{
		return Matrix4x4<Tp> {
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
