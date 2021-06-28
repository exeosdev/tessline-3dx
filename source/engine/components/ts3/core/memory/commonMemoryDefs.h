
#pragma once

#ifndef __TS3_COMMON_MEMORY_DEFS_H__
#define __TS3_COMMON_MEMORY_DEFS_H__

#include "../prerequisites.h"
#include <ts3/stdext/range.h>

namespace ts3
{

    using memory_align_t = uint32;
    using memory_diff_t = native_int;
    using memory_size_t = native_uint;

    using MemoryRegion = Region<memory_size_t>;
    using MemoryRange = MemoryRegion::Range;

    constexpr memory_align_t cxMemoryDefaultAlignment = 0;

    constexpr memory_size_t cxMemoryOffsetInvalid = ts3::Limits<memory_size_t>::maxValue;

    constexpr memory_size_t cxMemorySizeMax = ts3::Limits<memory_size_t>::maxValue;

}

#endif // __TS3_COMMON_MEMORY_DEFS_H__
