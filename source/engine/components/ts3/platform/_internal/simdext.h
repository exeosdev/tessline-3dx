
#ifndef __TS3_PLATFORM_CONFIG_SIMDEXT_H__
#define __TS3_PLATFORM_CONFIG_SIMDEXT_H__

#if( TS3_PCL_FEATURE_EIS )

#define _MM_SHUFFLE_R( fp0, fp1, fp2, fp3 ) \
	_MM_SHUFFLE( fp3, fp2, fp1, fp0 )

#if( ( TS3_PCL_COMPILER & TS3_PCL_COMPILER_CLANG ) && ( TS3_PCL_COMPILER <= TS3_PCL_COMPILER_CLANG_7 ) )
#  define __TS3_PRAGMA_DIAGNOSTIC_POP_CLANG 1
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wpsabi"
#endif

#if( TS3_PCL_COMPILER & TS3_PCL_COMPILER_GCC )
#  define __TS3_PRAGMA_DIAGNOSTIC_POP_GCC 1
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wpsabi"
#endif

#if( TS3_PCL_EIS_SUPPORT_LEVEL & TS3_PCL_EIS_FEATURE_NEON )
// A replacement of Intel's _mm_pause() for NEON. Pauses the processor.
// Note, that we deliberately chose 'isb' over 'yield' instruction, as the latter
// is usually a no-op on most ARM core. The former is a good approximation, though.
inline void _mm_pause()
{
    __asm__ __volatile__( "isb\n" );
}

// A replacement of Intel's _mm_sfence() for NEON. Ensures, that all stores executed before this fence
// are globally visible after this fence is executed. Prevents from compiler-level re-ordering and emits
// an appropriate CPU barrier if necessary.
inline void _mm_sfence()
{
    __sync_synchronize();
}
#endif

#if( TS3_PCL_EIS_SUPPORT_LEVEL & TS3_PCL_EIS_FEATURE_SSE )
// Computes dot product of two 4-component vectors of single-precision floating point numbers.
inline __m128 _mm128f_dp_ps0( const __m128 & m1, const __m128 & m2 )
{
#if( ( TS3_PCL_EIS_SUPPORT_LEVEL & TS3_PCL_EIS_FEATURE_SSE41 ) && TS3_PCL_EIS_USE_DP_INTRINSICS )
	// Use the bultin intrinsic. This, however, is known to have worse performance on AMD hardware
	// (Ryzen included) and older/low-end Intels than doing it manually via below implementation.
	//
	// Compute using intrinsic, specify distribution mask only for the first component of the result vector.
	const __m128 dotpVector = _mm_dp_ps( m1, m2, 0x01 );
	// Get the result from the vector.
	return _mm_cvtss_f32( dotpVector );
#else
	// Multiply inputs components wise.
	const __m128 mulVector = _mm_mul_ps( m1, m2 );
	// > mulVector: [i0, i1, i2, i3] = [X1*X2, Y1*Y2, Z1*Z2, W1*W2]
	// Shuffle the mulVector values.
	const __m128 shuffleVector = _mm_shuffle_ps( mulVector, mulVector, _MM_SHUFFLE( 2, 3, 0, 1 ) );
	// > shuffled: [i1, i0, i3, i2]
	// Add shuffled and original mulVector.
	const __m128 sumVector = _mm_add_ps( mulVector, shuffleVector );
	// > sumVector: [i0+i1, i1+i0, i2+i3, i3+i2]
	// Move high part of the result to lower parts of a new vector.
	const __m128 sumHiShuffle = _mm_movehl_ps( shuffleVector, sumVector );
	// > sumHiShuffle: [i2+i3, i3+i2, ...]
	// Now simply add these two to get a final result.
	const __m128 resultVector = _mm_add_ss( sumHiShuffle, sumVector );
	// > resultVector: [i0+i1+i2+i3, ...]
	return resultVector;
#endif
}
#endif

#if( TS3_PCL_EIS_SUPPORT_LEVEL & TS3_PCL_EIS_FEATURE_SSE )
// Computes dot product of two 4-component vectors of single-precision floating point numbers.
inline __m128 _mm128f_dp_ps4( const __m128 & m1, const __m128 & m2 )
{
#if( ( TS3_PCL_EIS_SUPPORT_LEVEL & TS3_PCL_EIS_FEATURE_SSE41 ) && TS3_PCL_EIS_USE_DP_INTRINSICS )
	// Compute using intrinsic, specify distribution mask for all components of the result vector.
	return _mm_dp_ps( m1, m2, 0x0F );
#else
	const __m128 mulVector = _mm_mul_ps( m1, m2 );
	// > mulVector: [i0, i1, i2, i3] = [X1*X2, Y1*Y2, Z1*Z2, W1*W2]
	const __m128 shuffleVector = _mm_shuffle_ps( mulVector, mulVector, _MM_SHUFFLE( 2, 3, 0, 1 ) );
	// > shuffled: [i1, i0, i3, i2]
	const __m128 sumVector = _mm_add_ps( mulVector, shuffleVector );
	// > sumVector: [i0+i1, i1+i0, i2+i3, i3+i2]
	const __m128 sumHiShuffle = _mm_movehl_ps( shuffleVector, sumVector );
	// > sumHiShuffle: [i2+i3, i3+i2, ...]
	const __m128 dotpVector = _mm_add_ps( sumHiShuffle, sumVector );
	// > resultVector: [i0+i1+i2+i3, i0+i1+i2+i3, ...]
	// Return a vector filled with the value stored in lower two components of resultVector.
	return _mm_movelh_ps( dotpVector, dotpVector );
#endif
}
#endif

#if( TS3_PCL_EIS_SUPPORT_LEVEL & TS3_PCL_EIS_FEATURE_SSE )
// Computes dot product of two 4-component vectors of single-precision floating point numbers.
inline float _mm128f_dp_psf( const __m128 & m1, const __m128 & m2 )
{
	const __m128 dotpVector0 = _mm128f_dp_ps0( m1, m2 );
	// Extract and return the first component.
	return _mm_cvtss_f32( dotpVector0 );
}
#endif

#if( TS3_PCL_EIS_SUPPORT_LEVEL & TS3_PCL_EIS_FEATURE_AVX )
// Computes dot product of two 4-component vectors of double-precision floating point numbers.
inline double _mm256d_dp_pd0( const __m256d & m1, const __m256d & m2 )
{
#if( ( TS3_PCL_EIS_SUPPORT_LEVEL & TS3_PCL_EIS_FEATURE_AVX2 ) && TS3_PCL_EIS_USE_DP_INTRINSICS )
	// Compute using intrinsic, specify distribution mask only for the first component of the result vector.
	// const __m256 dotpVector = _mm_dp_pd( m1, m2, 0x01 );
	// Get the result from the vector.
	// return _mm256_cvtsd_f64( dotpVector );
#else
	// Multiply inputs components wise.
	const __m256d mulVector = _mm256_mul_pd( m1, m2 );
	// > mulVector: [i0, i1, i2, i3] = [X1*X2, Y1*Y2, Z1*Z2, W1*W2]
	// Shuffle the mulVector values.
	const __m256d shuffleVector = _mm256_shuffle_pd( mulVector, mulVector, _MM_SHUFFLE( 2, 3, 0, 1 ) );
	// > shuffled: [i1, i0, i3, i2]
	// Add shuffled and original mulVector.
	const __m256d sumVector = _mm256_add_pd( mulVector, shuffleVector );
	// > sumVector: [i0+i1, i1+i0, i2+i3, i3+i2]
	// Shuffle the sumVector values.
	const __m256d shuffleSumVector = _mm256_shuffle_pd( sumVector, sumVector, _MM_SHUFFLE( 0, 1, 2, 3 ) );
	// > shuffleSumVector: [i2+i3, i3+i2, i0+i1, i1+i0]
	// Add partial sums.
	const __m256d dotpVector = ( sumVector, shuffleSumVector );
	// Return the first component.
	return _mm256_cvtsd_f64( dotpVector );
#endif
}
#endif

#if( TS3_PCL_EIS_SUPPORT_LEVEL & TS3_PCL_EIS_FEATURE_AVX )
// Computes dot product of two 4-component vectors of double-precision floating point numbers.
inline __m256d _mm256d_dp_pd4( const __m256d & m1, const __m256d & m2 )
{
#if( ( TS3_PCL_EIS_SUPPORT_LEVEL & TS3_PCL_EIS_FEATURE_AVX2 ) && TS3_PCL_EIS_USE_DP_INTRINSICS )
	// Compute using intrinsic, specify distribution mask for all components of the result vector.
	// return _mm_dp_pd( m1, m2, 0x0F );
#else
	// Multiply inputs components wise.
	const __m256d mulVector = _mm256_mul_pd( m1, m2 );
	// > mulVector: [i0, i1, i2, i3] = [X1*X2, Y1*Y2, Z1*Z2, W1*W2]
	// Shuffle the mulVector values.
	const __m256d shuffleVector = _mm256_shuffle_pd( mulVector, mulVector, _MM_SHUFFLE( 2, 3, 0, 1 ) );
	// > shuffled: [i1, i0, i3, i2]
	// Add shuffled and original mulVector.
	const __m256d sumVector = _mm256_add_pd( mulVector, shuffleVector );
	// > sumVector: [i0+i1, i1+i0, i2+i3, i3+i2]
	// Shuffle the sumVector values.
	const __m256d shuffleSumVector = _mm256_shuffle_pd( sumVector, sumVector, _MM_SHUFFLE( 0, 1, 2, 3 ) );
	// > shuffleSumVector: [i2+i3, i3+i2, i0+i1, i1+i0]
	// Add partial sums.
	const __m256d dotpVector = ( sumVector, shuffleSumVector );
	// Return the result.
	return dotpVector;
#endif
}
#endif

#if( __TS3_PRAGMA_DIAGNOSTIC_POP_GCC )
#  pragma GCC diagnostic pop
#endif

#if( __TS3_PRAGMA_DIAGNOSTIC_POP_CLANG )
#  pragma clang diagnostic pop
#endif

#endif /* TS3_PCL_FEATURE_EIS */

#endif /* __TS3_PLATFORM_CONFIG_SIMDEXT_H__ */
