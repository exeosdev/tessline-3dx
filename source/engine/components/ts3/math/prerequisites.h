
#ifndef __TS3_MATH_PREREQUISITES_H__
#define __TS3_MATH_PREREQUISITES_H__

#include <ts3/stdext/prerequisites.h>
#include <cmath>

#if( TS3_PCL_FEATURE_EIS )
#  define TS3_MATH_SIMD_ENABLE 1
#  if( TS3_PCL_EIS_SUPPORT_LEVEL & TS3_PCL_EIS_FEATURE_SSE ) // SSE -> float[4] (__m128)
#    define TS3_MATH_SIMD_USE_VX128F 1
#  else
#    define TS3_MATH_SIMD_USE_VX128F 0
#  endif
#  if( TS3_PCL_EIS_SUPPORT_LEVEL & TS3_PCL_EIS_FEATURE_SSE2 ) // SSE2 -> int32[4] (__m128i)
#    define TS3_MATH_SIMD_USE_VX128I 1
#  else
#    define TS3_MATH_SIMD_USE_VX128I 0
#  endif
#  if( TS3_PCL_EIS_SUPPORT_LEVEL & TS3_PCL_EIS_FEATURE_AVX ) // AVX -> double[4] (__m256d)
#    define TS3_MATH_SIMD_USE_VX256D 1
#  else
#    define TS3_MATH_SIMD_USE_VX256D 0
#  endif
#  if( TS3_PCL_EIS_SUPPORT_LEVEL & TS3_PCL_EIS_FEATURE_AVX2 ) // AVX2 -> int64[4] (__m256i)
#    define TS3_MATH_SIMD_USE_VX256I 1
#  else
#    define TS3_MATH_SIMD_USE_VX256I 0
#  endif
#endif

namespace ts3::math
{

	template <typename TpValue, size_t tpSize>
	using Array = TpValue[tpSize];

	template <typename TpValue>
	using Array2 = Array<TpValue, 2>;

	template <typename TpValue>
	using Array3 = Array<TpValue, 3>;

	template <typename TpValue>
	using Array4 = Array<TpValue, 4>;

	template <typename TpScalar>
	using enable_if_scalar_t = std::enable_if_t<std::is_arithmetic<TpScalar>::value, bool>;

	namespace constants
	{

		inline constexpr float cxFloatPi           = 3.1415926536f;
		inline constexpr float cxFloatPiDiv2       = 1.5707963268f;
		inline constexpr float cxFloatPiDiv4       = 0.7853981634f;
		inline constexpr float cxFloatE            = 2.7182818285f;
		inline constexpr float cxFloatLog2E        = 1.4426950409f;
		inline constexpr float cxFloatLog10E       = 0.4342944819f;
		inline constexpr float cxFloatLn2          = 0.6931471806f;
		inline constexpr float cxFloatLn10         = 2.3025850930f;
		inline constexpr float cxFloatOneDivPi     = 0.3183098862f;
		inline constexpr float cxFloatTwoDivPi     = 0.6366197724f;
		inline constexpr float cxFloatSqrt2        = 1.4142135624f;
		inline constexpr float cxFloatSqrtPi       = 1.7724538509f;
		inline constexpr float cxFloatRad1Degree   = 0.0174532925f;   // 1 degree in radians
		inline constexpr float cxFloatRad15Degree  = 0.2617993878f;   // 15 degrees in radians
		inline constexpr float cxFloatRad30Degree  = 0.5235987756f;   // 30 degrees in radians
		inline constexpr float cxFloatRad45Degree  = 0.7853981634f;   // 45 degrees in radians
		inline constexpr float cxFloatRad60Degree  = 1.0471975512f;   // 60 degrees in radians
		inline constexpr float cxFloatRad90Degree  = 1.5707963268f;   // 90 degrees in radians
		inline constexpr float cxFloatRad180Degree = 3.1415926536f;   // 180 degrees in radians
		inline constexpr float cxFloatRad360Degree = 6.2831853072f;   // 360 degrees in radians
		inline constexpr float cxFloatDeg1Radian   = 57.2957795131f;  // 1 radian in degrees
		inline constexpr float cxFloatDeg2Radian   = 114.5915590262f; // 2 radians in degrees
		inline constexpr float cxFloatDeg3Radian   = 171.8873385392f; // 3 radians in degrees

		inline constexpr double cxDoublePi           = 3.14159265358979323846;
		inline constexpr double cxDoublePiDiv2       = 1.57079632679489661923;
		inline constexpr double cxDoublePiDiv4       = 0.78539816339744830961;
		inline constexpr double cxDoubleE            = 2.71828182845904523536;
		inline constexpr double cxDoubleLog2E        = 1.44269504088896340736;
		inline constexpr double cxDoubleLog10E       = 0.43429448190325182765;
		inline constexpr double cxDoubleLn2          = 0.69314718055994530941;
		inline constexpr double cxDoubleLn10         = 2.30258509299404568402;
		inline constexpr double cxDoubleOneDivPi     = 0.31830988618379067153;
		inline constexpr double cxDoubleTwoDivPi     = 0.63661977236758134307;
		inline constexpr double cxDoubleSqrt2        = 1.41421356237309504880;
		inline constexpr double cxDoubleSqrtPi       = 1.77245385090551602729;
		inline constexpr double cxDoubleRad1Degree   = 0.01745329251994329576;   // 1 degree in radians
		inline constexpr double cxDoubleRad15Degree  = 0.26179938779914943653;   // 15 degrees in radians
		inline constexpr double cxDoubleRad30Degree  = 0.52359877559829887307;   // 30 degrees in radians
		inline constexpr double cxDoubleRad45Degree  = 0.78539816339744830961;   // 45 degrees in radians
		inline constexpr double cxDoubleRad60Degree  = 1.04719755119659774615;   // 60 degrees in radians
		inline constexpr double cxDoubleRad90Degree  = 1.57079632679489661923;   // 90 degrees in radians
		inline constexpr double cxDoubleRad180Degree = 3.14159265358979323846;   // 180 degrees in radians
		inline constexpr double cxDoubleRad360Degree = 6.28318530717958647692;   // 360 degrees in radians
		inline constexpr double cxDoubleDeg1Radian   = 57.29577951308232087679;  // 1 radian in degrees
		inline constexpr double cxDoubleDeg2Radian   = 114.59155902616464175359; // 2 radians in degrees
		inline constexpr double cxDoubleDeg3Radian   = 171.88733853924696263039; // 3 radians in degrees

	}

}

#endif // __TS3_MATH_PREREQUISITES_H__
