
#ifndef  __TS3_PLATFORM_CONFIG_INTERNAL_COMPILER_MSVC_H__
#define  __TS3_PLATFORM_CONFIG_INTERNAL_COMPILER_MSVC_H__

#include <sysinfoapi.h>
#include <debugapi.h>
#include <intrin.h>

#define w64
#define gnoexcept

#define  TS3_PCL_FILE  __FILE__
#define  TS3_PCL_FUNC  __FUNCSIG__
#define  TS3_PCL_LINE  __LINE__

#define  TS3_PCL_TODO( description ) __pragma( message( description ) )

#define  TS3_PCL_ATTR_ALIGN( n )               __declspec( align( n ) )
#define  TS3_PCL_ATTR_DEPRECATED( msg, repl )  __declspec( deprecated )
#define  TS3_PCL_ATTR_DLL_EXPORT               __declspec( dllexport )
#define  TS3_PCL_ATTR_DLL_IMPORT               __declspec( dllimport )
#define  TS3_PCL_ATTR_NO_RETURN                __declspec( noreturn )
#define  TS3_PCL_ATTR_ALWAYS_INLINE            __forceinline

#if(  TS3_PCL_COMPILER <  TS3_PCL_COMPILER_MSVC_2015 )
#  define  TS3_PCL_ATTR_THREAD_LOCAL __declspec( thread )
#else
#  define  TS3_PCL_ATTR_THREAD_LOCAL thread_local
#endif

#define  TS3_PCL_DEBUG_BREAK()         __debugbreak()
#define  TS3_PCL_RUNTIME_BREAK()       __debugbreak()
#define  TS3_PCL_DEBUG_OUTPUT( text )  OutputDebugStringA( text )
#define  TS3_PCL_SLEEP( miliseconds )  Sleep( miliseconds )

#define  TS3_PCL_BYTESWAP16	_byteswap_ushort
#define  TS3_PCL_BYTESWAP32	_byteswap_ulong
#define  TS3_PCL_BYTESWAP64	_byteswap_uint64

#define  TS3_PCL_ROTL16 _rotl16
#define  TS3_PCL_ROTL32 _rotl
#define  TS3_PCL_ROTL64 _rotl64

#define  TS3_PCL_ROTR16 _rotr16
#define  TS3_PCL_ROTR32 _rotr
#define  TS3_PCL_ROTR64 _rotr64

#define TS3_PCL_COPY_MEMORY( destPtr, srcPtr, size ) CopyMemory( (destPtr), (srcPtr), (size) )
#define TS3_PCL_FILL_MEMORY( memPtr, size, value )   FillMemory( (memPtr), (size), (value) )
#define TS3_PCL_MOVE_MEMORY( destPtr, srcPtr, size ) MoveMemory( (destPtr), (srcPtr), (size) )
#define TS3_PCL_ZERO_MEMORY( memPtr, size )          ZeroMemory( (memPtr), (size) )

#if(  TS3_PCL_FEATURE_INTRINSICS )
#  pragma intrinsic( _rotl16 )
#  pragma intrinsic( _rotl )
#  pragma intrinsic( _rotl64 )
#  pragma intrinsic( _rotr16 )
#  pragma intrinsic( _rotr )
#  pragma intrinsic( _rotr64 )
#  pragma intrinsic( _byteswap_ushort )
#  pragma intrinsic( _byteswap_ulong )
#  pragma intrinsic( _byteswap_uint64 )
#endif

#if(  TS3_PCL_FEATURE_INTRINSICS )
#  pragma intrinsic( memcmp )
#  pragma intrinsic( memcpy )
#  pragma intrinsic( memset )
#  pragma intrinsic( strcmp )
#  pragma intrinsic( strcpy )
#  pragma intrinsic( strlen )
#  pragma intrinsic( wcscmp )
#  pragma intrinsic( wcscpy )
#  pragma intrinsic( wcslen )
#endif

#pragma warning( disable : 5040 )  /* dynamic exception specifications are valid only in C++14 and earlier; treating as noexcept(false) */

#pragma warning( disable : 4005 )  /* 'MACRONAME': macro redefinition */
#pragma warning( disable : 4065 )  /* switch statement contains 'default' but no 'case' labels */
#pragma warning( disable : 4100 )  /* 'VARNAME': unreferenced formal parameter */
#pragma warning( disable : 4127 )  /* conditional expression is constant */
#pragma warning( disable : 4189 )  /* 'VARNAME': local variable is initialized but not referenced */
#pragma warning( disable : 4201 )  /* nonstandard extension used: nameless struct/union */
#pragma warning( disable : 4251 )  /* class 'A' needs to have dll-interface to be used by clients of class 'B' */
#pragma warning( disable : 4275 )  /* non dll-interface class 'A' used as base for dll-interface class 'B' */
#pragma warning( disable : 4333 )  /* '>>': right shift by too large amount, data loss */
#pragma warning( disable : 4312 )  /* 'reinterpret_cast': conversion from 'A' to 'B' of greater size */
#pragma warning( disable : 4512 )  /* 'CLASSNAME': assignment operator could not be generated */
#pragma warning( disable : 4624 )  /* 'CLASSNAME': destructor was implicitly defined as deleted */
#pragma warning( disable : 4702 )  /* unreachable code */
#pragma warning( disable : 4752 )  /* found Intel(R) Advanced Vector ext; consider using /arch:AVX */
#pragma warning( disable : 4996 )  /* this function or variable may be unsafe. Consider using 'funcname'_s instead */

#pragma warning( error: 4242 ) /* conversion from 'X' to 'Y', possible loss of data */
#pragma warning( error: 4267 ) /* conversion from 'size_t' to 'T', possible loss of data */

#endif /*  __TS3_PLATFORM_CONFIG_INTERNAL_COMPILER_MSVC_H__ */
