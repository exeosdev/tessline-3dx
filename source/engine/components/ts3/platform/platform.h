
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

#define TS3_FUNC_NO_DISCARD \
    TS3_PCL_ATTR_NO_DISCARD

#define TS3_FUNC_PURE_VIRTUAL( pFunc ) \
    virtual pFunc = 0

#define TS3_FUNC_PURE_VIRTUAL_NO_DISCARD( pFunc ) \
    TS3_PCL_ATTR_NO_DISCARD virtual pFunc = 0

namespace ts3
{

    enum class EByteOrder : uint32
    {
        BigEndian = TS3_PCL_ENDIANNESS_BE,
        LittleEndian = TS3_PCL_ENDIANNESS_LE,
        Native = TS3_PCL_ENDIANNESS_NATIVE,
        NonNative = TS3_PCL_ENDIANNESS_NON_NATIVE,
        Default = static_cast<uint32>( BigEndian )
    };

}

#endif /* __TS3_PLATFORM_CONFIG_PLATFORM_H__ */
