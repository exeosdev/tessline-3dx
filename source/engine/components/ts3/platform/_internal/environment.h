
#ifndef  __TS3_PLATFORM_CONFIG_ENVIRONMENT_H__
#define  __TS3_PLATFORM_CONFIG_ENVIRONMENT_H__

#define TS3_PCL_CONFIG_BASE_DISABLE_EIS 0

#if !defined( TRUE )
#  define TRUE 1
#endif

#if !defined( FALSE  )
#  define FALSE 0
#endif

#if !defined( DONT_CARE  )
#  define DONT_CARE -1
#endif

#define TS3_PCL_CPP_VERSION_11 201103L
#define TS3_PCL_CPP_VERSION_14 201402L
#define TS3_PCL_CPP_VERSION_17 201703L
#define TS3_PCL_CPP_VERSION_2A 202000L
#define TS3_PCL_CPP_VERSION    __cplusplus

#define TS3_PCL_TARGET_ARCHITECTURE_X86    0x0001
#define TS3_PCL_TARGET_ARCHITECTURE_X86_64 0x0002
#define TS3_PCL_TARGET_ARCHITECTURE_ARM    0x0004
#define TS3_PCL_TARGET_ARCHITECTURE_ARM64  0x0004

#define TS3_PCL_TARGET_FLAG_OS_MSE         0x0001
#define TS3_PCL_TARGET_FLAG_OS_ANDROID     0x0002
#define TS3_PCL_TARGET_FLAG_OS_POSIX       0x0004
#define TS3_PCL_TARGET_FLAG_OS_APPLE       0x0008
#define TS3_PCL_TARGET_FLAG_OS_DESKTOP     0x0010

#define TS3_PCL_TARGET_FLAG_SYSAPI_NATIVE  0x0001
#define TS3_PCL_TARGET_FLAG_SYSAPI_EXT     0x0002

#define TS3_PCL_TARGET_PLATFORM_WINDOWS ( 0x0100L | TS3_PCL_TARGET_FLAG_OS_MSE )
#define TS3_PCL_TARGET_PLATFORM_LINUX   ( 0x0200L | TS3_PCL_TARGET_FLAG_OS_POSIX )
#define TS3_PCL_TARGET_PLATFORM_DARWIN  ( 0x0400L | TS3_PCL_TARGET_FLAG_OS_POSIX | TS3_PCL_TARGET_FLAG_OS_APPLE )
#define TS3_PCL_TARGET_PLATFORM_ANDROID ( 0x0800L | TS3_PCL_TARGET_FLAG_OS_POSIX | TS3_PCL_TARGET_FLAG_OS_ANDROID )

#define TS3_PCL_TARGET_OS_WINDESKTOP    ( 0x010000L | TS3_PCL_TARGET_PLATFORM_WINDOWS | TS3_PCL_TARGET_FLAG_OS_DESKTOP )
#define TS3_PCL_TARGET_OS_WINRT         ( 0x020000L | TS3_PCL_TARGET_PLATFORM_WINDOWS )
#define TS3_PCL_TARGET_OS_WINUWP        ( 0x040000L | TS3_PCL_TARGET_PLATFORM_WINDOWS )
#define TS3_PCL_TARGET_OS_WINPHONE8     ( 0x080000L | TS3_PCL_TARGET_PLATFORM_WINDOWS )
#define TS3_PCL_TARGET_OS_LINUX         ( 0x100000L | TS3_PCL_TARGET_PLATFORM_LINUX | TS3_PCL_TARGET_FLAG_OS_DESKTOP )
#define TS3_PCL_TARGET_OS_ANDROID       ( 0x200000L | TS3_PCL_TARGET_PLATFORM_ANDROID )
#define TS3_PCL_TARGET_OS_MACOS         ( 0x400000L | TS3_PCL_TARGET_PLATFORM_DARWIN | TS3_PCL_TARGET_FLAG_OS_DESKTOP )
#define TS3_PCL_TARGET_OS_IOS           ( 0x800000L | TS3_PCL_TARGET_PLATFORM_DARWIN )

#define TS3_PCL_TARGET_SYSAPI_WIN32     ( 0x0100 | TS3_PCL_TARGET_FLAG_SYSAPI_NATIVE )
#define TS3_PCL_TARGET_SYSAPI_WINRT     ( 0x0200 | TS3_PCL_TARGET_FLAG_SYSAPI_NATIVE )
#define TS3_PCL_TARGET_SYSAPI_WINUWP    ( 0x0400 | TS3_PCL_TARGET_FLAG_SYSAPI_NATIVE )
#define TS3_PCL_TARGET_SYSAPI_WINPHONE  ( 0x0800 | TS3_PCL_TARGET_FLAG_SYSAPI_NATIVE )
#define TS3_PCL_TARGET_SYSAPI_X11       ( 0x1000 | TS3_PCL_TARGET_FLAG_SYSAPI_NATIVE )
#define TS3_PCL_TARGET_SYSAPI_ANDROID   ( 0x2000 | TS3_PCL_TARGET_FLAG_SYSAPI_NATIVE )
#define TS3_PCL_TARGET_SYSAPI_OSX       ( 0x4000 | TS3_PCL_TARGET_FLAG_SYSAPI_NATIVE )
#define TS3_PCL_TARGET_SYSAPI_IOS       ( 0x8000 | TS3_PCL_TARGET_FLAG_SYSAPI_NATIVE )
#define TS3_PCL_TARGET_SYSAPI_SDL       ( 0x100000 | TS3_PCL_TARGET_FLAG_SYSAPI_EXT )


#define TS3_PCL_COMPILER_MSVC       0x010000
#define TS3_PCL_COMPILER_MSVC_2012  0x0100B0
#define TS3_PCL_COMPILER_MSVC_2013  0x0100C0
#define TS3_PCL_COMPILER_MSVC_2015  0x0100E0
#define TS3_PCL_COMPILER_MSVC_2017  0x010100
#define TS3_PCL_COMPILER_MSVC_2019  0x010200
#define TS3_PCL_COMPILER_MSVC_UV    0x010001

#define TS3_PCL_COMPILER_ICC       0x020000
#define TS3_PCL_COMPILER_ICC_12    0x0200C0
#define TS3_PCL_COMPILER_ICC_12_1  0x0200C1
#define TS3_PCL_COMPILER_ICC_13    0x0200D0
#define TS3_PCL_COMPILER_ICC_14    0x0200E0
#define TS3_PCL_COMPILER_ICC_15    0x0200F0
#define TS3_PCL_COMPILER_ICC_16    0x020F00
#define TS3_PCL_COMPILER_ICC_17    0x020F10
#define TS3_PCL_COMPILER_ICC_18    0x020F20
#define TS3_PCL_COMPILER_ICC_19    0x020F30
#define TS3_PCL_COMPILER_ICC_UV    0x020001

#define TS3_PCL_COMPILER_GCC      0x040000
#define TS3_PCL_COMPILER_GCC_4_7  0x040047
#define TS3_PCL_COMPILER_GCC_4_8  0x040048
#define TS3_PCL_COMPILER_GCC_4_9  0x040049
#define TS3_PCL_COMPILER_GCC_5    0x04005F
#define TS3_PCL_COMPILER_GCC_6    0x04006F
#define TS3_PCL_COMPILER_GCC_7    0x04007F
#define TS3_PCL_COMPILER_GCC_8    0x04008F
#define TS3_PCL_COMPILER_GCC_9    0x04009F
#define TS3_PCL_COMPILER_GCC_UV   0x040001

#define TS3_PCL_COMPILER_MINGW64      0x080000
#define TS3_PCL_COMPILER_MINGW64_4_0  0x080040
#define TS3_PCL_COMPILER_MINGW64_4_1  0x080041
#define TS3_PCL_COMPILER_MINGW64_4_2  0x080042
#define TS3_PCL_COMPILER_MINGW64_4_X  0x08004F
#define TS3_PCL_COMPILER_MINGW64_5    0x080050
#define TS3_PCL_COMPILER_MINGW64_6    0x080060
#define TS3_PCL_COMPILER_MINGW64_7    0x080070
#define TS3_PCL_COMPILER_MINGW64_8    0x080080
#define TS3_PCL_COMPILER_MINGW64_9    0x080090
#define TS3_PCL_COMPILER_MINGW64_UV   0x080001

#define TS3_PCL_COMPILER_CLANG      0x100000
#define TS3_PCL_COMPILER_CLANG_4_0  0x100040
#define TS3_PCL_COMPILER_CLANG_4_1  0x100041
#define TS3_PCL_COMPILER_CLANG_4_2  0x100042
#define TS3_PCL_COMPILER_CLANG_4_X  0x10004F
#define TS3_PCL_COMPILER_CLANG_5    0x100050
#define TS3_PCL_COMPILER_CLANG_6    0x100060
#define TS3_PCL_COMPILER_CLANG_7    0x100070
#define TS3_PCL_COMPILER_CLANG_8    0x100080
#define TS3_PCL_COMPILER_CLANG_9    0x100090
#define TS3_PCL_COMPILER_CLANG_UV   0x100001

#define TS3_PCL_ENDIANNESS_BE       0x4001
#define TS3_PCL_ENDIANNESS_LE       0x4002
#define TS3_PCL_ENDIANNESS_UNKNOWN  0x4000

#define TS3_PCL_UNICODE_CHARACTER_BYTE  0x4101
#define TS3_PCL_UNICODE_CHARACTER_WIDE  0x4102

#define TS3_PCL_ENCODING_ASCII  0x4201
#define TS3_PCL_ENCODING_UTF16  0x4203
#define TS3_PCL_ENCODING_UTF32  0x4204
#define TS3_PCL_ENCODING_UTF8   0x4205

#define TS3_PCL_EIS_FEATURE_NONE   0x0000
#define TS3_PCL_EIS_FEATURE_SSE    0x0001
#define TS3_PCL_EIS_FEATURE_SSE2   0x0002
#define TS3_PCL_EIS_FEATURE_SSE3   0x0004
#define TS3_PCL_EIS_FEATURE_SSE3X  0x0008
#define TS3_PCL_EIS_FEATURE_SSE4A  0x0010
#define TS3_PCL_EIS_FEATURE_SSE41  0x0020
#define TS3_PCL_EIS_FEATURE_SSE42  0x0040
#define TS3_PCL_EIS_FEATURE_AVX    0x0100
#define TS3_PCL_EIS_FEATURE_AVX2   0x0200
#define TS3_PCL_EIS_FEATURE_NEON   0x1000

#define TS3_PCL_EIS_LEVEL_SSE   ( TS3_PCL_EIS_FEATURE_SSE )
#define TS3_PCL_EIS_LEVEL_SSE2  ( TS3_PCL_EIS_FEATURE_SSE2  | TS3_PCL_EIS_LEVEL_SSE )
#define TS3_PCL_EIS_LEVEL_SSE3  ( TS3_PCL_EIS_FEATURE_SSE3  | TS3_PCL_EIS_LEVEL_SSE2 )
#define TS3_PCL_EIS_LEVEL_SSE3X ( TS3_PCL_EIS_FEATURE_SSE3X | TS3_PCL_EIS_LEVEL_SSE3 )
#define TS3_PCL_EIS_LEVEL_SSE4A ( TS3_PCL_EIS_FEATURE_SSE4A | TS3_PCL_EIS_LEVEL_SSE3X )
#define TS3_PCL_EIS_LEVEL_SSE41 ( TS3_PCL_EIS_FEATURE_SSE41 | TS3_PCL_EIS_LEVEL_SSE4A )
#define TS3_PCL_EIS_LEVEL_SSE42 ( TS3_PCL_EIS_FEATURE_SSE42 | TS3_PCL_EIS_LEVEL_SSE41 )
#define TS3_PCL_EIS_LEVEL_AVX   ( TS3_PCL_EIS_FEATURE_AVX   | TS3_PCL_EIS_LEVEL_SSE42 )
#define TS3_PCL_EIS_LEVEL_AVX2  ( TS3_PCL_EIS_FEATURE_AVX2  | TS3_PCL_EIS_LEVEL_AVX )
#define TS3_PCL_EIS_LEVEL_NEON  ( TS3_PCL_EIS_FEATURE_NEON )

#define TS3_PCL_BUILD_TYPE_DEBUG    0x0001
#define TS3_PCL_BUILD_TYPE_RELEASE  0x0002


#if defined( TS3_PCL_CONFIG_BASE_DISABLE_EIS ) && TS3_PCL_CONFIG_BASE_DISABLE_EIS
#  define TS3_PCL_FEATURE_EIS 0
#else
#  define TS3_PCL_FEATURE_EIS 1
#endif

#if defined( TS3_PCL_CONFIG_BASE_DISABLE_INTRINSICS ) && TS3_PCL_CONFIG_BASE_DISABLE_INTRINSICS
#  define TS3_PCL_FEATURE_INTRINSICS 0
#else
#  define TS3_PCL_FEATURE_INTRINSICS 1
#endif


#if !defined( TS3_PCL_BUILD_TYPE )
#  if( defined( DEBUG ) || defined( _DEBUG ) || defined( __DEBUG__ ) || defined( _DEBUGBUILD ) )
#    define TS3_PCL_BUILD_TYPE TS3_PCL_BUILD_TYPE_DEBUG
#  else
#    define TS3_PCL_BUILD_TYPE TS3_PCL_BUILD_TYPE_RELEASE
#  endif
#endif

#if( TS3_PCL_BUILD_TYPE == TS3_PCL_BUILD_TYPE_DEBUG )
#  define TS3_PCL_BUILD_TYPE_STR "Debug"
#elif( TS3_PCL_BUILD_TYPE == TS3_PCL_BUILD_TYPE_RELEASEDEBUG )
#  define TS3_PCL_BUILD_TYPE_STR "Release"
#else
#  define TS3_PCL_BUILD_TYPE_STR "Unknown"
#endif


#if defined ( __ANDROID__ )
#
#  define TS3_PCL_TARGET_OS             TS3_PCL_TARGET_OS_ANDROID
#  define TS3_PCL_TARGET_SYSAPI_NATIVE  TS3_PCL_TARGET_SYSAPI_ANDROID
#
#elif defined( linux ) || defined( __linux__ ) || defined( __CYGWIN__ )
#
#  define TS3_PCL_TARGET_OS             TS3_PCL_TARGET_OS_LINUX
#  define TS3_PCL_TARGET_SYSAPI_NATIVE  TS3_PCL_TARGET_SYSAPI_X11
#
#elif defined( __APPLE__ ) || defined( __apple )
#
#  include <TargetConditionals.h>
#  if( TARGET_OS_IPHONE )
#    define TS3_PCL_TARGET_OS             TS3_PCL_TARGET_OS_IOS
#    define TS3_PCL_TARGET_SYSAPI_NATIVE  TS3_PCL_TARGET_SYSAPI_IOS
#  elif defined( __MACH__ ) || TARGET_OS_MAC
#    define TS3_PCL_TARGET_OS             TS3_PCL_TARGET_OS_MACOS
#    define TS3_PCL_TARGET_SYSAPI_NATIVE  TS3_PCL_TARGET_SYSAPI_OSX
#  endif
#
#elif defined( WINAPI_FAMILY_PARTITION ) || defined( _WIN32 ) || defined( _WINDOWS )
#
#  if defined( WINAPI_FAMILY_PARTITION )
#    if WINAPI_FAMILY_PARTITION( WINAPI_PARTITION_DESKTOP )
#      define TS3_PCL_TARGET_OS             TS3_PCL_TARGET_OS_WINDESKTOP
#      define TS3_PCL_TARGET_SYSAPI_NATIVE  TS3_PCL_TARGET_SYSAPI_WIN32
#    elif WINAPI_FAMILY_PARTITION( WINAPI_PARTITION_PHONE_APP )
#      define TS3_PCL_TARGET_OS             TS3_PCL_TARGET_OS_WINPHONE8
#      define TS3_PCL_TARGET_SYSAPI_NATIVE  TS3_PCL_TARGET_SYSAPI_WINPHONE
#    elif WINAPI_FAMILY_PARTITION( WINAPI_PARTITION_PC_APP )
#      define TS3_PCL_TARGET_OS             TS3_PCL_TARGET_OS_WINUWP
#      define TS3_PCL_TARGET_SYSAPI_NATIVE  TS3_PCL_TARGET_SYSAPI_WINUWP
#    endif
#  endif
#
#  if !defined( TS3_PCL_TARGET_OS )
#    if defined( _EPCONFIG_BUILD_TARGET_UWP )
#      define TS3_PCL_TARGET_OS             TS3_PCL_TARGET_OS_WINUWP
#      define TS3_PCL_TARGET_SYSAPI_NATIVE  TS3_PCL_TARGET_SYSAPI_WINUWP
#    else
#      define TS3_PCL_TARGET_OS             TS3_PCL_TARGET_OS_WINDESKTOP
#      define TS3_PCL_TARGET_SYSAPI_NATIVE  TS3_PCL_TARGET_SYSAPI_WIN32
#    endif
#  endif
#
#  define NOCOMM 1
#  define NOCRYPT 1
#  define NOGDICAPMASKS 1
#  define NOCTLMGR 1
#  define NOMCX 1
#  define NOMEMMGR 1
#  define NOMINMAX 1
#  define NOOPENFILE 1
#  define NOSCROLL 1
#  define NOSOUND 1
#  define NOTEXTMETRIC 1
#  define WIN32_LEAN_AND_MEAN 1
#
#endif


#if !defined( TS3_PCL_TARGET_SYSAPI )
#    define TS3_PCL_TARGET_SYSAPI TS3_PCL_TARGET_SYSAPI_NATIVE
#endif


#if _ECL_TARGET_CPU_X86_64 || defined( _M_AMD64 ) || defined( _M_X64 ) || defined( __x86_64__ )
#
#  define TS3_PCL_TARGET_ARCHITECTURE  TS3_PCL_TARGET_ARCHITECTURE_X86_64
#
#elif _ECL_TARGET_CPU_X86 || defined( _M_IX86 ) || defined( _X86_ ) || defined( __i386__ ) || defined( __IA32__ )
#
#  define TS3_PCL_TARGET_ARCHITECTURE  TS3_PCL_TARGET_ARCHITECTURE_X86
#
#elif _ECL_TARGET_CPU_ARM64 || defined( __ARM_ARCH_ISA_A64 ) || defined( __aarch64__ )
#
#  define TS3_PCL_TARGET_ARCHITECTURE  TS3_PCL_TARGET_ARCHITECTURE_ARM64
#
#elif _ECL_TARGET_CPU_ARM || defined( _M_ARM ) || defined( _M_ARMT ) || defined( _ARM ) || defined( __arm__ )
#
#  define TS3_PCL_TARGET_ARCHITECTURE  TS3_PCL_TARGET_ARCHITECTURE_ARM
#
#endif


#if defined( __INTEL_COMPILER )
#
#  if( __INTEL_COMPILER >= 1900 )
#    define TS3_PCL_COMPILER TS3_PCL_COMPILER_ICC_19
#  elif( __INTEL_COMPILER >= 1800 )
#    define TS3_PCL_COMPILER TS3_PCL_COMPILER_ICC_18
#  elif( __INTEL_COMPILER >= 1700 )
#    define TS3_PCL_COMPILER TS3_PCL_COMPILER_ICC_17
#  elif( __INTEL_COMPILER >= 1600 )
#    define TS3_PCL_COMPILER TS3_PCL_COMPILER_ICC_16
#  elif( __INTEL_COMPILER >= 1500 )
#    define TS3_PCL_COMPILER TS3_PCL_COMPILER_ICC_15
#  elif( __INTEL_COMPILER >= 1400 )
#    define TS3_PCL_COMPILER TS3_PCL_COMPILER_ICC_14
#  elif( __INTEL_COMPILER >= 1300 )
#    define TS3_PCL_COMPILER TS3_PCL_COMPILER_ICC_13
#  elif( __INTEL_COMPILER == 1210 )
#    define TS3_PCL_COMPILER TS3_PCL_COMPILER_ICC_12_1
#  elif( __INTEL_COMPILER == 1200 )
#    define TS3_PCL_COMPILER TS3_PCL_COMPILER_ICC_12
#  else
#    define TS3_PCL_COMPILER TS3_PCL_COMPILER_ICC_UV
#  endif
#
#elif defined( __LLVM ) || defined( __CLANG__ ) || defined( __clang__ )
#
#  if( __clang_major__ == 4 ) && ( __clang_minor__ == 0 )
#    define TS3_PCL_COMPILER TS3_PCL_COMPILER_CLANG_4_0
#  elif( __clang_major__ == 4 ) && ( __clang_minor__ == 1 )
#    define TS3_PCL_COMPILER TS3_PCL_COMPILER_CLANG_4_1
#  elif( __clang_major__ == 4 ) && ( __clang_minor__ == 2 )
#    define TS3_PCL_COMPILER TS3_PCL_COMPILER_CLANG_4_2
#  elif( __clang_major__ == 4 ) && ( __clang_minor__ > 2 )
#    define TS3_PCL_COMPILER TS3_PCL_COMPILER_CLANG_4_X
#  elif( __clang_major__ == 5 )
#    define TS3_PCL_COMPILER TS3_PCL_COMPILER_CLANG_5
#    define TS3_PCL_FEATURE_ANONYMOUS_UNIONS 1
#  elif( __clang_major__ == 6 )
#    define TS3_PCL_COMPILER TS3_PCL_COMPILER_CLANG_6
#    define TS3_PCL_FEATURE_ANONYMOUS_UNIONS 1
#  elif( __clang_major__ == 7 )
#    define TS3_PCL_COMPILER TS3_PCL_COMPILER_CLANG_7
#    define TS3_PCL_FEATURE_ANONYMOUS_UNIONS 1
#  elif( __clang_major__ == 8 )
#    define TS3_PCL_COMPILER TS3_PCL_COMPILER_CLANG_8
#    define TS3_PCL_FEATURE_ANONYMOUS_UNIONS 1
#  elif( __clang_major__ == 9 )
#    define TS3_PCL_COMPILER TS3_PCL_COMPILER_CLANG_9
#    define TS3_PCL_FEATURE_ANONYMOUS_UNIONS 1
#  else
#    define TS3_PCL_COMPILER TS3_PCL_COMPILER_CLANG_UV
#    define TS3_PCL_FEATURE_ANONYMOUS_UNIONS 0
#  endif
#
#  define TS3_PCL_FEATURE_FILESYSTEM ( TS3_PCL_COMPILER > TS3_PCL_COMPILER_CLANG_6_X )
#  define TS3_PCL_FEATURE_FILESYSTEM_EXPERIMENTAL ( TS3_PCL_COMPILER >= TS3_PCL_COMPILER_CLANG_4_2 ) && ( TS3_PCL_COMPILER <= TS3_PCL_COMPILER_CLANG_6_X )
#
#elif defined( _MSC_VER )
#
#  if( ( _MSC_VER >= 1920 ) && ( _MSC_VER <= 1930 ) )
#    define TS3_PCL_COMPILER  TS3_PCL_COMPILER_MSVC_2019
#  elif( ( _MSC_VER >= 1911 ) && ( _MSC_VER <= 1919 ) )
#    define TS3_PCL_COMPILER  TS3_PCL_COMPILER_MSVC_2017
#  elif( _MSC_VER == 1900 )
#    define TS3_PCL_COMPILER  TS3_PCL_COMPILER_MSVC_2015
#  elif( _MSC_VER == 1800 )
#    define TS3_PCL_COMPILER  TS3_PCL_COMPILER_MSVC_2013
#  elif( _MSC_VER == 1700 )
#    define TS3_PCL_COMPILER  TS3_PCL_COMPILER_MSVC_2012
#  else
#    define TS3_PCL_COMPILER  TS3_PCL_COMPILER_MSVC_UV
#  endif
#
#  define TS3_PCL_FEATURE_FILESYSTEM ( TS3_PCL_COMPILER > TS3_PCL_COMPILER_MSVC_2017 )
#  define TS3_PCL_FEATURE_FILESYSTEM_EXPERIMENTAL ( TS3_PCL_COMPILER >= TS3_PCL_COMPILER_MSVC_2015 ) && ( TS3_PCL_COMPILER <= TS3_PCL_COMPILER_MSVC_2017 )
#
#  define _SCL_SECURE_NO_WARNINGS    1
#  define _CRT_SECURE_NO_WARNINGS    1
#
#elif defined( __MINGW64__ )
#
#  include <cstdlib>
#
#  define TS3_PCL_COMPILER TS3_PCL_COMPILER_MINGW64_UV
#
#elif defined( __GNUC__ )
#
#  include <cstdlib>
#
#  if( __GNUC__ == 4 )
#    if( __GNUC_MINOR__ == 6 )
#      define TS3_PCL_COMPILER TS3_PCL_COMPILER_GCC_4_6
#    elif( __GNUC_MINOR__ == 7 )
#      define TS3_PCL_COMPILER TS3_PCL_COMPILER_GCC_4_7
#    elif( __GNUC_MINOR__ == 8 )
#      define TS3_PCL_COMPILER TS3_PCL_COMPILER_GCC_4_8
#    elif( __GNUC_MINOR__ == 9 )
#      define TS3_PCL_COMPILER TS3_PCL_COMPILER_GCC_4_9
#    endif
#  elif( __GNUC__ == 5 )
#    define TS3_PCL_COMPILER TS3_PCL_COMPILER_GCC_5
#    define TS3_PCL_FEATURE_ANONYMOUS_UNIONS 1
#  elif( __GNUC__ == 6 )
#    define TS3_PCL_COMPILER TS3_PCL_COMPILER_GCC_6
#    define TS3_PCL_FEATURE_ANONYMOUS_UNIONS 1
#  elif( __GNUC__ == 7 )
#    define TS3_PCL_COMPILER TS3_PCL_COMPILER_GCC_7
#    define TS3_PCL_FEATURE_ANONYMOUS_UNIONS 1
#  elif( __GNUC__ == 8 )
#    define TS3_PCL_COMPILER TS3_PCL_COMPILER_GCC_8
#    define TS3_PCL_FEATURE_ANONYMOUS_UNIONS 1
#  elif( __GNUC__ == 9 )
#    define TS3_PCL_COMPILER TS3_PCL_COMPILER_GCC_9
#    define TS3_PCL_FEATURE_ANONYMOUS_UNIONS 1
#  elif( __GNUC__ == 9 )
#    define TS3_PCL_COMPILER TS3_PCL_COMPILER_GCC_UV
#  endif
#
#  define TS3_PCL_FEATURE_FILESYSTEM ( TS3_PCL_COMPILER >= TS3_PCL_COMPILER_GCC_8_X )
#  define TS3_PCL_FEATURE_FILESYSTEM_EXPERIMENTAL ( TS3_PCL_COMPILER < TS3_PCL_COMPILER_GCC_8_X ) && ( TS3_PCL_COMPILER >= TS3_PCL_COMPILER_GCC_5_X )
#
#endif

#if( ( TS3_PCL_CPP_VERSION < TS3_PCL_CPP_VERSION_14 ) && ( TS3_PCL_COMPILER & TS3_PCL_COMPILER_MSVC ) )
#  if( TS3_PCL_COMPILER >= TS3_PCL_COMPILER_MSVC_2017 )
#    undef TS3_PCL_CPP_VERSION
#    define TS3_PCL_CPP_VERSION TS3_PCL_CPP_VERSION_17
#  elif( TS3_PCL_COMPILER >= TS3_PCL_COMPILER_MSVC_2013 )
#    undef TS3_PCL_CPP_VERSION
#    define TS3_PCL_CPP_VERSION TS3_PCL_CPP_VERSION_14
#  endif
#endif

#if( TS3_PCL_CPP_VERSION >= TS3_PCL_CPP_VERSION_17 )
#  define TS3_PCL_ATTR_NO_DISCARD [[nodiscard]]
#else
#  define TS3_PCL_ATTR_NO_DISCARD
#endif


#if !defined( TS3_PCL_FEATURE_ANONYMOUS_UNIONS )
#  if( TS3_PCL_COMPILER & ( TS3_PCL_COMPILER_MSVC | TS3_PCL_COMPILER_ICC ) )
#    if defined( _MSC_EXTENSIONS )
#      define TS3_PCL_FEATURE_ANONYMOUS_UNIONS 1
#    else
#      define TS3_PCL_FEATURE_ANONYMOUS_UNIONS 0
#    endif
#  else
#    define TS3_PCL_FEATURE_ANONYMOUS_UNIONS 0
#  endif
#endif


#if( ( TS3_PCL_TARGET_OS & TS3_PCL_TARGET_FLAG_OS_WINDOWS ) && (EPCONFIG_COMPILER & (EPCONFIG_COMPILER_ICC | TS3_PCL_COMPILER_MSVC)) )
#  define TS3_PCL_TARGET_PROPERTY_LIB_PRAGMA_SUPPORTED 1
#else
#  define TS3_PCL_TARGET_PROPERTY_LIB_PRAGMA_SUPPORTED 0
#endif


#if( TS3_PCL_COMPILER & ( TS3_PCL_COMPILER_LLVM | TS3_PCL_COMPILER_CLANG ) )
#  if defined( __AVX2__ )
#    define TS3_PCL_EIS_SUPPORT_COMPILER_LEVEL TS3_PCL_EIS_LEVEL_AVX2
#  elif defined( __AVX__ )
#    define TS3_PCL_EIS_SUPPORT_COMPILER_LEVEL TS3_PCL_EIS_LEVEL_AVX
#  elif defined( __SSE4_2__ )
#    define TS3_PCL_EIS_SUPPORT_COMPILER_LEVEL TS3_PCL_EIS_LEVEL_SSE42
#  elif defined( __SSE4_1__ )
#    define TS3_PCL_EIS_SUPPORT_COMPILER_LEVEL TS3_PCL_EIS_LEVEL_SSE41
#  elif defined( __SSE4__ )
#    define TS3_PCL_EIS_SUPPORT_COMPILER_LEVEL TS3_PCL_EIS_LEVEL_SSE4A
#  elif defined( __SSE3__ )
#    define TS3_PCL_EIS_SUPPORT_COMPILER_LEVEL TS3_PCL_EIS_LEVEL_SSE3
#  elif defined( __SSE2__ )
#    define TS3_PCL_EIS_SUPPORT_COMPILER_LEVEL TS3_PCL_EIS_LEVEL_SSE2
#  endif
#elif( ( TS3_PCL_COMPILER & TS3_PCL_COMPILER_ICC ) && ( TS3_PCL_TARGET_OS == TS3_PCL_TARGET_OS_LINUX ) )
#  if defined( __AVX2__ )
#    define TS3_PCL_EIS_SUPPORT_COMPILER_LEVEL TS3_PCL_EIS_LEVEL_AVX2
#  elif defined( __AVX__ )
#    define TS3_PCL_EIS_SUPPORT_COMPILER_LEVEL TS3_PCL_EIS_LEVEL_AVX
#  elif defined( __SSE3__ )
#    define TS3_PCL_EIS_SUPPORT_COMPILER_LEVEL TS3_PCL_EIS_LEVEL_SSE3
#  elif defined( __SSE2__ )
#    define TS3_PCL_EIS_SUPPORT_COMPILER_LEVEL TS3_PCL_EIS_LEVEL_SSE2
#  elif defined( __SSE__ )
#    define TS3_PCL_EIS_SUPPORT_COMPILER_LEVEL TS3_PCL_EIS_LEVEL_SSE2
#  endif
#elif( ( TS3_PCL_COMPILER & TS3_PCL_COMPILER_MSVC ) || ( ( TS3_PCL_COMPILER & TS3_PCL_COMPILER_ICC ) && ( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_WIN32 ) ) )
#  if defined( _M_ARM_FP )
#    define TS3_PCL_EIS_SUPPORT_COMPILER_LEVEL TS3_PCL_EIS_LEVEL_NEON
#  elif defined( __AVX2__ )
#    define TS3_PCL_EIS_SUPPORT_COMPILER_LEVEL TS3_PCL_EIS_LEVEL_AVX2
#  elif defined( __AVX__ )
#    define TS3_PCL_EIS_SUPPORT_COMPILER_LEVEL TS3_PCL_EIS_LEVEL_AVX
#  elif( _M_IX86_FP == 2 )
#    define TS3_PCL_EIS_SUPPORT_COMPILER_LEVEL TS3_PCL_EIS_LEVEL_SSE2
#  elif( _M_IX86_FP == 1 )
#    define TS3_PCL_EIS_SUPPORT_COMPILER_LEVEL TS3_PCL_EIS_LEVEL_SSE
#  endif
#// GCC, all pltforms, x86/x86_64
#elif( TS3_PCL_COMPILER & TS3_PCL_COMPILER_GCC ) && ( TS3_PCL_TARGET_ARCHITECTURE & ( TS3_PCL_TARGET_ARCHITECTURE_X86 | TS3_PCL_TARGET_ARCHITECTURE_X86_64 ) )
#  if defined( __AVX2__ )
#    define TS3_PCL_EIS_SUPPORT_COMPILER_LEVEL TS3_PCL_EIS_LEVEL_AVX2
#  elif defined( __AVX__ )
#    define TS3_PCL_EIS_SUPPORT_COMPILER_LEVEL TS3_PCL_EIS_LEVEL_AVX
#  elif defined( __SSE4_1__ )
#    define TS3_PCL_EIS_SUPPORT_COMPILER_LEVEL TS3_PCL_EIS_LEVEL_SSE41
#  elif defined( __SSE3__ )
#    define TS3_PCL_EIS_SUPPORT_COMPILER_LEVEL TS3_PCL_EIS_LEVEL_SSE3
#  elif defined( __SSE2__ )
#    define TS3_PCL_EIS_SUPPORT_COMPILER_LEVEL TS3_PCL_EIS_LEVEL_SSE2
#  elif defined( __SSE__ )
#    define TS3_PCL_EIS_SUPPORT_COMPILER_LEVEL TS3_PCL_EIS_LEVEL_SSE
#  endif
#endif


#if defined( __TS3_PCL_EIS_LEVEL_AVX2__ ) && __TS3_PCL_EIS_LEVEL_AVX2__
#  define TS3_PCL_EIS_SUPPORT_LEVEL_EBS TS3_PCL_EIS_LEVEL_AVX2
#elif defined( __TS3_PCL_EIS_LEVEL_AVX__ ) && __TS3_PCL_EIS_LEVEL_AVX__
#  define TS3_PCL_EIS_SUPPORT_LEVEL_EBS TS3_PCL_EIS_LEVEL_AVX
#elif defined( __TS3_PCL_EIS_LEVEL_SSE42__ ) && __TS3_PCL_EIS_LEVEL_SSE42__
#  define TS3_PCL_EIS_SUPPORT_LEVEL_EBS TS3_PCL_EIS_LEVEL_SSE42
#elif defined( __TS3_PCL_EIS_LEVEL_SSE41__ ) && __TS3_PCL_EIS_LEVEL_SSE41__
#  define TS3_PCL_EIS_SUPPORT_LEVEL_EBS TS3_PCL_EIS_LEVEL_SSE41
#elif defined( __TS3_PCL_EIS_LEVEL_SSE4A__ ) && __TS3_PCL_EIS_LEVEL_SSE4A__
#  define TS3_PCL_EIS_SUPPORT_LEVEL_EBS TS3_PCL_EIS_LEVEL_SSE4A
#elif defined( __TS3_PCL_EIS_LEVEL_SSE3X__ ) && __TS3_PCL_EIS_LEVEL_SSE3X__
#  define TS3_PCL_EIS_SUPPORT_LEVEL_EBS TS3_PCL_EIS_LEVEL_SSE3X
#elif defined( __TS3_PCL_EIS_LEVEL_SSE3__ ) && __TS3_PCL_EIS_LEVEL_SSE3__
#  define TS3_PCL_EIS_SUPPORT_LEVEL_EBS TS3_PCL_EIS_LEVEL_SSE3
#elif defined( __TS3_PCL_EIS_LEVEL_SSE2__ ) && __TS3_PCL_EIS_LEVEL_SSE2__
#  define TS3_PCL_EIS_SUPPORT_LEVEL_EBS TS3_PCL_EIS_LEVEL_SSE2
#elif defined( __TS3_PCL_EIS_LEVEL_SSE__ ) && __TS3_PCL_EIS_LEVEL_SSE__
#  define TS3_PCL_EIS_SUPPORT_LEVEL_EBS TS3_PCL_EIS_LEVEL_SSE
#elif defined( __TS3_PCL_EIS_LEVEL_NEONv2__ ) && __TS3_PCL_EIS_LEVEL_NEONv2__
#  define TS3_PCL_EIS_SUPPORT_LEVEL_EBS TS3_PCL_EIS_LEVEL_NEON
#elif defined( __TS3_PCL_EIS_LEVEL_NEONv1__ ) && __TS3_PCL_EIS_LEVEL_NEONv1__
#  define TS3_PCL_EIS_SUPPORT_LEVEL_EBS TS3_PCL_EIS_LEVEL_NEON
#endif


#if defined( __TS3_PCL_EIS_FORCE__ ) && __TS3_PCL_EIS_FORCE__ && defined( TS3_PCL_EIS_SUPPORT_LEVEL_EBS )
#  define TS3_PCL_EIS_SUPPORT_LEVEL TS3_PCL_EIS_SUPPORT_LEVEL_EBS
#elif TS3_PCL_FEATURE_EIS && defined( TS3_PCL_EIS_SUPPORT_COMPILER_LEVEL )
#  define TS3_PCL_EIS_SUPPORT_LEVEL TS3_PCL_EIS_SUPPORT_COMPILER_LEVEL
#else
#  define TS3_PCL_EIS_SUPPORT_LEVEL 0
#endif

#if defined( __MINGW64__ )
#  if defined( _WIN32_WINNT ) && ( _WIN32_WINNT < 0x0602 )
#    undef _WIN32_WINNT
#  endif
#  define _WIN32_WINNT 0x0602
#endif

#if defined( __MINGW64__ ) && ( TS3_PCL_EIS_SUPPORT_LEVEL != TS3_PCL_EIS_FEATURE_NONE )
#  if defined( _WINDOWS_ ) || defined( _INC_WINDOWS )
#    error "Include mismatch! On MinGW-W64, <initrin.h> must be included before <Windows.h>!"
#  endif
#  include <intrin.h>
#endif

#if( TS3_PCL_EIS_SUPPORT_LEVEL & TS3_PCL_EIS_FEATURE_NEON )
#  define TS3_PCL_EIS_SUPPORT_HAS_MM_PAUSE 1
#endif

#if( TS3_PCL_EIS_SUPPORT_LEVEL & TS3_PCL_EIS_FEATURE_SSE )
#  define TS3_PCL_EIS_SUPPORT_HAS_MM_PAUSE 1
#endif

#if( TS3_PCL_TARGET_ARCHITECTURE == TS3_PCL_TARGET_ARCHITECTURE_X86 )
#  include "environment/arch/x86.h"
#elif( TS3_PCL_TARGET_ARCHITECTURE == TS3_PCL_TARGET_ARCHITECTURE_X86_64 )
#  include "environment/arch/x86_64.h"
#elif( TS3_PCL_TARGET_ARCHITECTURE == TS3_PCL_TARGET_ARCHITECTURE_ARM )
#  include "environment/arch/arm.h"
#elif( TS3_PCL_TARGET_ARCHITECTURE == TS3_PCL_TARGET_ARCHITECTURE_ARM64 )
#  include "environment/arch/arm64.h"
#else
#  error "Unsupported architecture!"
#endif

#if( TS3_PCL_TARGET_OS == TS3_PCL_TARGET_OS_WINDESKTOP )
#  include "environment/os/windesktop.h"
#elif( TS3_PCL_TARGET_OS == TS3_PCL_TARGET_OS_WINUWP )
#  include "environment/os/winuwp.h"
#elif( TS3_PCL_TARGET_OS == TS3_PCL_TARGET_OS_ANDROID )
#  include "environment/os/android.h"
#elif( TS3_PCL_TARGET_OS == TS3_PCL_TARGET_OS_LINUX )
#  include "environment/os/linux.h"
#elif( TS3_PCL_TARGET_OS == TS3_PCL_TARGET_OS_MACOS )
#  include "environment/os/osx.h"
#elif( TS3_PCL_TARGET_OS == TS3_PCL_TARGET_OS_IOS )
#  include "environment/os/ios.h"
#else
#  error "Unsupported os!"
#endif

#if( TS3_PCL_COMPILER & TS3_PCL_COMPILER_CLANG )
#  include "environment/compiler/clang.h"
#elif( TS3_PCL_COMPILER & TS3_PCL_COMPILER_GCC )
#  include "environment/compiler/gcc.h"
#elif( TS3_PCL_COMPILER & TS3_PCL_COMPILER_MINGW64 )
#  include "environment/compiler/mingw64.h"
#elif( TS3_PCL_COMPILER & TS3_PCL_COMPILER_ICC )
#  include "environment/compiler/icc.h"
#elif( TS3_PCL_COMPILER & TS3_PCL_COMPILER_MSVC )
#  include "environment/compiler/msvc.h"
#else
#  error "Unsupported compiler!"
#endif

#if !defined( TS3_UNUSED )
#  define TS3_UNUSED( x ) ( x );
#endif

#if !defined( TS3_PCL_COPY_MEMORY )
#  define TS3_PCL_COPY_MEMORY( destPtr, srcPtr, size ) memcpy( (destPtr), (srcPtr), (size) )
#endif

#if !defined( TS3_PCL_FILL_MEMORY )
#  define TS3_PCL_FILL_MEMORY( memPtr, size, value ) memset( (memPtr), (value), (size) )
#endif

#if !defined( TS3_PCL_MOVE_MEMORY )
#  define TS3_PCL_MOVE_MEMORY( destPtr, srcPtr, size ) memmove( (destPtr), (srcPtr), (size) )
#endif

#if !defined( TS3_PCL_ZERO_MEMORY )
#  define TS3_PCL_ZERO_MEMORY( memPtr, size ) memset( (memPtr), 0, (size) )
#endif


#if( TS3_PCL_ENDIANNESS_NATIVE == TS3_PCL_ENDIANNESS_LE )
#  define TS3_PCL_GET_U16_LE( pValue ) ( pValue )
#  define TS3_PCL_GET_U32_LE( pValue ) ( pValue )
#  define TS3_PCL_GET_U64_LE( pValue ) ( pValue )
#  define TS3_PCL_GET_U16_BE( pValue ) TS3_PCL_BYTESWAP16( pValue )
#  define TS3_PCL_GET_U32_BE( pValue ) TS3_PCL_BYTESWAP32( pValue )
#  define TS3_PCL_GET_U64_BE( pValue ) TS3_PCL_BYTESWAP64( pValue )
#else
#  define TS3_PCL_GET_U16_LE( pValue ) TS3_PCL_BYTESWAP16( pValue )
#  define TS3_PCL_GET_U32_LE( pValue ) TS3_PCL_BYTESWAP32( pValue )
#  define TS3_PCL_GET_U64_LE( pValue ) TS3_PCL_BYTESWAP64( pValue )
#  define TS3_PCL_GET_U16_BE( pValue ) ( pValue )
#  define TS3_PCL_GET_U32_BE( pValue ) ( pValue )
#  define TS3_PCL_GET_U64_BE( pValue ) ( pValue )
#endif

#endif /*  __TS3_PLATFORM_CONFIG_ENVIRONMENT_H__ */
