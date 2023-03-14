
#ifndef __TS3_PLATFORM_CONFIG_PLATFORM_H__
#define __TS3_PLATFORM_CONFIG_PLATFORM_H__

#include "_internal/environment.h"
#include "_internal/include.h"
#include "_internal/types.h"
#include "_internal/simdext.h"

/// @def TS3_DEBUG
/// @brief Identifies a debug build. Enables additional logging, build-time info, extra API checks and more.
#if( TS3_PCL_BUILD_TYPE == TS3_PCL_BUILD_TYPE_DEBUG )
#  define TS3_DEBUG 1
#else
#  define TS3_DEBUG 0
#endif

/// @def TS3_RELEASE_OPT_MAX
/// @brief Identifies a production-oriented build, with all possible code-level optimisations enabled. Disabled by default.
#if !defined( TS3_RELEASE_OPT_MAX )
#  define TS3_RELEASE_OPT_MAX 0
#endif

/// @def TS3_ATTR_MAYBE_UNUSED
/// @brief Replaced with compiler-specific version (or the standard one - if supported) [[maybe_unused]] class attribute.
#define TS3_ATTR_MAYBE_UNUSED \
	TS3_PCL_ATTR_MAYBE_UNUSED

/// @def TS3_ATTR_NO_DISCARD
/// @brief Replaced with compiler-specific version (or the standard one - if supported) [[nodiscard]] function attribute.
#define TS3_ATTR_NO_DISCARD \
	TS3_PCL_ATTR_FUNC_NO_DISCARD

namespace ts3
{

	/// @brief Represents one of the possible byte orders in binary data representation.
	enum class EByteOrder : uint32
	{
		/// Big-Endian order: high-order byte comes first.
		BigEndian = TS3_PCL_ENDIANNESS_BE,

		/// Little-Endian order: low-order byte comes first.
		LittleEndian = TS3_PCL_ENDIANNESS_LE,

		/// The native byte order for the current target architecture (e.g. Big-Endian for ARM, Little-Endian for x86 family).
		Native = TS3_PCL_ENDIANNESS_NATIVE,

		/// A non-naive byte order for the current target architecture (e.g. Little-Endian for ARM, Big-Endian for x86 family).
		NonNative = TS3_PCL_ENDIANNESS_NON_NATIVE,

		/// A default byte order used by the framework. This is always Big-Endian as it is the default one for network.
		Default = static_cast<uint32>( BigEndian )
	};

}

#endif /* __TS3_PLATFORM_CONFIG_PLATFORM_H__ */
