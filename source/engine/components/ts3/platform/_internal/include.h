
#ifndef __TS3_PLATFORM_CONFIG_INCLUDE_H__
#define __TS3_PLATFORM_CONFIG_INCLUDE_H__

// C Library
#include <cstdlib>
#include <cstddef>
#include <cstdint>

// POSIX
#include <sys/types.h>
#include <sys/stat.h>

//
#include <algorithm>
#include <array>
#include <utility>
#include <vector>

#if( TS3_PCL_EIS_SUPPORT_LEVEL & TS3_PCL_EIS_FEATURE_NEON )
#  include <arm_neon.h>
#elif( TS3_PCL_EIS_SUPPORT_LEVEL & TS3_PCL_EIS_FEATURE_AVX )
#  include <immintrin.h>
#elif( TS3_PCL_EIS_SUPPORT_LEVEL & TS3_PCL_EIS_FEATURE_SSE42 )
#  include <smmintrin.h>
#elif( TS3_PCL_EIS_SUPPORT_LEVEL & TS3_PCL_EIS_FEATURE_SSE41 )
#  include <nmmintrin.h>
#elif( TS3_PCL_EIS_SUPPORT_LEVEL & TS3_PCL_EIS_FEATURE_SSE4A )
#  include <ammintrin.h>
#elif( TS3_PCL_EIS_SUPPORT_LEVEL & TS3_PCL_EIS_FEATURE_SSE3 )
#  include <pmmintrin.h>
#elif( TS3_PCL_EIS_SUPPORT_LEVEL & TS3_PCL_EIS_FEATURE_SSE2 )
#  include <emmintrin.h>
#elif( TS3_PCL_EIS_SUPPORT_LEVEL & TS3_PCL_EIS_FEATURE_SSE )
#  include <xmmintrin.h>
#endif

#endif /* __TS3_PLATFORM_CONFIG_INCLUDE_H__ */
