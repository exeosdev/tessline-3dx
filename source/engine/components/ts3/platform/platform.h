
#ifndef __TS3_PLATFORM_CONFIG_PLATFORM_H__
#define __TS3_PLATFORM_CONFIG_PLATFORM_H__

#include "_internal/environment.h"
#include "_internal/include.h"
#include "_internal/types.h"
#include "_internal/simdext.h"

#if( TS3_PCL_BUILD_TYPE == TS3_PCL_BUILD_TYPE_DEBUG )
#  define TS3_DEBUG 1
#else
#  define TS3_DEBUG 0
#endif

#endif /* __TS3_PLATFORM_CONFIG_PLATFORM_H__ */
