
#ifndef  __TS3_PLATFORM_CONFIG_INTERNAL_COMPILER_GCC_H__
#define  __TS3_PLATFORM_CONFIG_INTERNAL_COMPILER_GCC_H__

#include <signal.h>
#include <unistd.h>

#define w64
#define gnoexcept noexcept

#define TS3_PCL_FILE  __FILE__
#define TS3_PCL_FUNC  __FUNCTION__
#define TS3_PCL_LINE  __LINE__

#define TS3_PCL_TODO( description )

#define TS3_PCL_ATTR_ALIGN( n )               alignas(n)
#define TS3_PCL_ATTR_DEPRECATED( msg, repl )  __attribute__(( deprecated( msg, repl ) ))
#define TS3_PCL_ATTR_DLL_EXPORT               __attribute__(( dllexport ))
#define TS3_PCL_ATTR_DLL_IMPORT               __attribute__(( dllimport ))
#define TS3_PCL_ATTR_NO_RETURN                __attribute__(( noreturn ))
#define TS3_PCL_ATTR_THREAD_LOCAL             __thread
#define TS3_PCL_ATTR_ALWAYS_INLINE            __attribute__(( always_inline )) inline

#define TS3_PCL_DEBUG_BREAK()         raise( SIGINT )
#define TS3_PCL_RUNTIME_BREAK()       raise( SIGINT )
#define TS3_PCL_DEBUG_OUTPUT( text )  printf( "%s\n", text )
#define TS3_PCL_SLEEP( miliseconds )  usleep( miliseconds * 1000 )

#define TS3_PCL_BYTESWAP16	__builtin_bswap16
#define TS3_PCL_BYTESWAP32	__builtin_bswap32
#define TS3_PCL_BYTESWAP64	__builtin_bswap64

#define TS3_PCL_ROTL16( x, n ) ( (x << n) | (x >> (16-n)) )
#define TS3_PCL_ROTL32( x, n ) ( (x << n) | (x >> (32-n)) )
#define TS3_PCL_ROTL64( x, n ) ( (x << n) | (x >> (64-n)) )
#define TS3_PCL_ROTR16( x, n ) ( (x >> n) | (x << (16-n)) )
#define TS3_PCL_ROTR32( x, n ) ( (x >> n) | (x << (32-n)) )
#define TS3_PCL_ROTR64( x, n ) ( (x >> n) | (x << (64-n)) )

#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#pragma GCC diagnostic ignored "-Wmissing-braces"
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#pragma GCC diagnostic ignored "-Wunused-value"
#pragma GCC diagnostic ignored "-Wunused-parameter"

#endif /*  __TS3_PLATFORM_CONFIG_INTERNAL_COMPILER_GCC_H__ */
