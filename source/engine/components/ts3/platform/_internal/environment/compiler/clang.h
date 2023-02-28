
#ifndef  __TS3_PLATFORM_CONFIG_INTERNAL_COMPILER_CLANG_H__
#define  __TS3_PLATFORM_CONFIG_INTERNAL_COMPILER_CLANG_H__

#include <signal.h>
#include <unistd.h>

#define __cdecl
#define w64
#define gnoexcept noexcept

#define TS3_PCL_FILE  __FILE__
#define TS3_PCL_FUNC  __FUNCTION__
#define TS3_PCL_LINE  __LINE__

#define TS3_PCL_ATTR_ALIGN( n )               alignas( n )
#define TS3_PCL_ATTR_DEPRECATED( msg, repl )  __attribute__(( deprecated( msg,repl ) ))
#define TS3_PCL_ATTR_NO_RETURN                __attribute__(( noreturn ))
#define TS3_PCL_ATTR_THREAD_LOCAL             thread_local

#if( defined( __has_attribute ) && __has_attribute( always_inline ) )
#  define TS3_PCL_ATTR_ALWAYS_INLINE __attribute__(( always_inline )) inline
#else
#  define TS3_PCL_ATTR_ALWAYS_INLINE inline
#endif

#if defined( __has_declspec_attribute )
#  if __has_declspec_attribute( dllexport ) && __has_declspec_attribute( dllimport )
#    define TS3_PCL_ATTR_DLL_EXPORT __declspec( dllexport )
#    define TS3_PCL_ATTR_DLL_IMPORT __declspec( dllimport )
#  elif __has_declspec_attribute( __dllexport__ ) && __has_declspec_attribute( __dllimport__ )
#    define TS3_PCL_ATTR_DLL_EXPORT __declspec( __dllexport__ )
#    define TS3_PCL_ATTR_DLL_IMPORT __declspec( __dllimport__ )
#  else
#    define TS3_PCL_ATTR_DLL_EXPORT
#    define TS3_PCL_ATTR_DLL_IMPORT
#  endif
#else
#  define TS3_PCL_ATTR_DLL_EXPORT
#  define TS3_PCL_ATTR_DLL_IMPORT
#endif

#define TS3_PCL_DEBUG_BREAK()         raise( SIGINT )
#define TS3_PCL_RUNTIME_BREAK()       raise( SIGINT )
#define TS3_PCL_DEBUG_OUTPUT( text )  printf( "%s\n", text )
#define TS3_PCL_SLEEP( miliseconds )  usleep( miliseconds * 1000 )

#define TS3_PCL_BYTESWAP16  __builtin_bswap16
#define TS3_PCL_BYTESWAP32  __builtin_bswap32
#define TS3_PCL_BYTESWAP64  __builtin_bswap64

#define  TS3_PCL_POPCOUNT16	__builtin_popcount
#define  TS3_PCL_POPCOUNT32	__builtin_popcountl
#define  TS3_PCL_POPCOUNT64	__builtin_popcountll

#define TS3_PCL_ROTL16( x, n ) ( (x << n) | (x >> (16-n)) )
#define TS3_PCL_ROTL32( x, n ) ( (x << n) | (x >> (32-n)) )
#define TS3_PCL_ROTL64( x, n ) ( (x << n) | (x >> (64-n)) )
#define TS3_PCL_ROTR16( x, n ) ( (x >> n) | (x << (16-n)) )
#define TS3_PCL_ROTR32( x, n ) ( (x >> n) | (x << (32-n)) )
#define TS3_PCL_ROTR64( x, n ) ( (x >> n) | (x << (64-n)) )

#if( TS3_PCL_CPP_VERSION >= TS3_PCL_CPP_VERSION_14 )
#  pragma clang diagnostic ignored "-Wc++17-extensions"
#endif

#pragma clang diagnostic ignored "-W#pragma-messages"
#pragma clang diagnostic ignored "-Wbraced-scalar-init"
#pragma clang diagnostic ignored "-Wdynamic-class-memaccess"
#pragma clang diagnostic ignored "-Wmissing-braces"
#pragma clang diagnostic ignored "-Wunused-private-field"
#pragma clang diagnostic ignored "-Wunused-value"
#pragma clang diagnostic ignored "-Wnull-character"

#endif /*  __TS3_PLATFORM_CONFIG_INTERNAL_COMPILER_CLANG_H__ */
