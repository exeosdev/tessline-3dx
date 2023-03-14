
#ifndef __TS3_PLATFORM_CONFIG_TYPES_H__
#define __TS3_PLATFORM_CONFIG_TYPES_H__

using byte = uint8_t;
using int8 = int8_t;
using int16 = int16_t;
using int32 = int32_t;
using int64 = int64_t;
using uint8 = uint8_t;
using uint16 = uint16_t;
using uint32 = uint32_t;
using uint64 = uint64_t;

#if !defined( w64 )
/// Supported by some compilers (e.g. __w64 on MSVC) - compiler will report any
/// warnings that would be reported if the code was compiled with a 64-bit compiler.
/// Useful for code compiled (for any reason...) for 32-bit platforms.
#  define w64
#endif

/// @def native_int
/// @brief Native signed integer, with size matching the machine word size (either 32 or 64 bits).

/// @def native_uint
/// @brief Native unsigned integer, with size matching the machine word size (either 32 or 64 bits).

#if( TS3_PCL_TARGET_64 )
using native_int = int64;
using native_uint = uint64;
#else
using native_int = w64 int32;
using native_uint = w64 uint32;
#endif

/// @brief Value type which is able to store the numerical value of a pointer (address) on the current platform.
using ptr_value_t = native_uint;

/// @brief Value type which is able to store the result of pointer difference calculation (i.e. relative pointer offset).
using ptr_diff_t = native_int;

/// @brief Default underlying type for enumerations, when no specific type is required.
using enum_default_value_t = uint32;

#endif /* __TS3_PLATFORM_CONFIG_TYPES_H__ */
