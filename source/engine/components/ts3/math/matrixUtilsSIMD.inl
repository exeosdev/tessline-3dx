
#if !defined( __TS3_MATH_MATRIX_UTILS_H__ )
#  error ""
#endif

#if !defined( TS3_MATH_SIMD_ENABLE_TRANSPOSE_WITH_UNPACK )
#  define TS3_MATH_SIMD_ENABLE_TRANSPOSE_WITH_UNPACK 0
#endif

namespace ts3::math
{

#if ( TS3_MATH_SIMD_USE_VX128F )

	TS3_PCL_ATTR_ALWAYS_INLINE void _mm_transpose_mat4_128f( __m128 pM0, __m128 pM1, __m128 pM2, __m128 pM3, __m128 pResult[4] )
	{
		// M0: [ M00, M01, M02, M03 ]
		// M1: [ M10, M11, M12, M13 ]
		// M2: [ M20, M21, M22, M23 ]
		// M3: [ M30, M31, M32, M33 ]

	#if ( !TS3_MATH_SIMD_ENABLE_TRANSPOSE_WITH_UNPACK )
		const __m128 tmp0 = _mm_shuffle_ps( pM0, pM1, _MM_SHUFFLE_R( 0, 1, 0, 1 ) ); // [ M00, M01, M10, M11 ]
		const __m128 tmp1 = _mm_shuffle_ps( pM0, pM1, _MM_SHUFFLE_R( 2, 3, 2, 3 ) ); // [ M02, M03, M12, M13 ]
		const __m128 tmp2 = _mm_shuffle_ps( pM2, pM3, _MM_SHUFFLE_R( 0, 1, 0, 1 ) ); // [ M20, M21, M30, M31 ]
		const __m128 tmp3 = _mm_shuffle_ps( pM2, pM3, _MM_SHUFFLE_R( 2, 3, 2, 3 ) ); // [ M22, M23, M32, M33 ]

		pResult[0] = _mm_shuffle_ps( tmp0, tmp2, _MM_SHUFFLE_R( 0, 2, 0, 2 ) );  // [ M00, M10, M20, M30 ]
		pResult[1] = _mm_shuffle_ps( tmp0, tmp2, _MM_SHUFFLE_R( 1, 3, 1, 3 ) );  // [ M01, M11, M21, M31 ]
		pResult[2] = _mm_shuffle_ps( tmp1, tmp3, _MM_SHUFFLE_R( 0, 2, 0, 2 ) );  // [ M02, M12, M22, M32 ]
		pResult[3] = _mm_shuffle_ps( tmp1, tmp3, _MM_SHUFFLE_R( 1, 3, 1, 3 ) );  // [ M03, M13, M23, M33 ]
	#else
		const __m128 tmp0 = _mm_unpacklo_ps( pM0, pM1 );
		const __m128 tmp1 = _mm_unpackhi_ps( pM0, pM1 );
		const __m128 tmp2 = _mm_unpacklo_ps( pM2, pM3 );
		const __m128 tmp3 = _mm_unpackhi_ps( pM2, pM3 );

		pResult[0] = _mm_movelh_ps( tmp0, tmp2 );
		pResult[1] = _mm_movehl_ps( tmp2, tmp0 );
		pResult[2] = _mm_movelh_ps( tmp1, tmp3 );
		pResult[3] = _mm_movehl_ps( tmp3, tmp1 );
	#endif
	}

	TS3_PCL_ATTR_ALWAYS_INLINE Matrix4x4<float> transpose( const Matrix4x4<float> & pMatrix )
	{
		Matrix4x4<float> result;
		_mm_transpose_mat4_128f( pMatrix.mm0, pMatrix.mm1, pMatrix.mm2, pMatrix.mm3, result.simdPtr() );
		return result;
	}

	TS3_PCL_ATTR_ALWAYS_INLINE void transposeInplace( Matrix4x4<float> & pMatrix )
	{
		_mm_transpose_mat4_128f( pMatrix.mm0, pMatrix.mm1, pMatrix.mm2, pMatrix.mm3, pMatrix.simdPtr() );
	}

#endif // TS3_MATH_SIMD_USE_VX128F

}
