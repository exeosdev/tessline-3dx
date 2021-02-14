
#if !defined( __TS3_MATH_ARRAY_OPS_H__ )
#  error ""
#endif

#if( TS3_PCL_COMPILER & TS3_PCL_COMPILER_CLANG )
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wignored-attributes"
#endif

#if( TS3_PCL_COMPILER & TS3_PCL_COMPILER_GCC )
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wignored-attributes"
#endif

namespace ts3::math
{

	namespace arrayOps
	{
	#if ( TS3_MATH_SIMD_USE_VX128F )

		inline void set( __m128 & pTarget, float pX, float pY, float pZ, float pW )
		{
			pTarget = _mm_set_ps( pW, pZ, pY, pX );
		}

		inline void set( __m128 & pTarget, float pValue )
		{
			pTarget = _mm_set1_ps( pValue );
		}

		inline void set( __m128 & pTarget, const float * pArray )
		{
			pTarget = _mm_loadu_ps( pArray );
		}

		inline void add( const __m128 & pLData, const __m128 & pRData, __m128 & pResult )
		{
			pResult = _mm_add_ps( pLData, pRData );
		}

		inline void sub( const __m128 & pLData, const __m128 & pRData, __m128 & pResult )
		{
			pResult = _mm_sub_ps( pLData, pRData );
		}

		inline void mul( const __m128 & pLData, const __m128 & pRData, __m128 & pResult )
		{
			pResult = _mm_mul_ps( pLData, pRData );
		}

		template <typename TpScalar>
		inline void mul( const __m128 & pData, const TpScalar pScalar, __m128 & pResult )
		{
			pResult = _mm_mul_ps( pData, _mm_set1_ps( static_cast<float>( pScalar ) ) );
		}

		template <typename TpScalar>
		inline void mul( const TpScalar pScalar, const __m128 & pData, __m128 & pResult )
		{
			pResult = _mm_mul_ps( _mm_set1_ps( static_cast<float>( pScalar ) ), pData );
		}

		inline void div( const __m128 & pLData, const __m128 & pRData, __m128 & pResult )
		{
			pResult = _mm_div_ps( pLData, pRData );
		}

		template <typename TpScalar>
		inline void div( const __m128 & pData, const TpScalar pScalar, __m128 & pResult )
		{
			pResult = _mm_div_ps( pData, _mm_set1_ps( static_cast<float>( pScalar ) ) );
		}

		template <typename TpScalar>
		inline void div( const TpScalar pScalar, const __m128 & pData, __m128 & pResult )
		{
			pResult = _mm_div_ps( _mm_set1_ps( static_cast<float>( pScalar ) ), pData );
		}

		inline void neg( const __m128 & pData, __m128 & pResult )
		{
			pResult = _mm_mul_ps( pData, _mm_set1_ps( -1.0f ) );
		}

		inline float dot( const __m128 & pLData, const __m128 & pRData )
		{
			const __m128 dotpVector0 = _mm128f_dp_ps0( pLData, pRData );
			return _mm_cvtss_f32( dotpVector0 );
		}

		inline double length( const __m128 & pData )
		{
			const __m128 dotpVector0 = _mm128f_dp_ps0( pData, pData );
			const __m128 sqrtVector0 = _mm_sqrt_ss( dotpVector0 );
			return _mm_cvtss_f32( sqrtVector0 );
		}

	#endif // TS3_MATH_SIMD_USE_VX128F
	}

}

#if( TS3_PCL_COMPILER & TS3_PCL_COMPILER_GCC )
#  pragma GCC diagnostic pop
#endif

#if( TS3_PCL_COMPILER & TS3_PCL_COMPILER_CLANG )
#  pragma clang diagnostic pop
#endif
