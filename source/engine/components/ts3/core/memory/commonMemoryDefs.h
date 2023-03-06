
#pragma once

#ifndef __TS3_COMMON_MEMORY_DEFS_H__
#define __TS3_COMMON_MEMORY_DEFS_H__

#include "../prerequisites.h"
#include <ts3/stdext/memory.h>
#include <ts3/stdext/range.h>

namespace ts3
{

    using memory_align_t = uint32;
    using memory_diff_t = native_int;
    using memory_size_t = native_uint;

    using MemoryRegion = Region<memory_size_t>;
    using MemoryRange = MemoryRegion::Range;

	namespace cxdefs
	{

		/// @brief Represents an invalid memory offset, expressed as the maximum value of the memory_size_t type.
		constexpr memory_size_t MEMORY_OFFSET_INVALID = Limits<memory_size_t>::maxValue;

		/// @brief Represents maximum possible size of single block/region of memory.
		constexpr memory_size_t MEMORY_SIZE_MAX = Limits<memory_size_t>::maxValue;

	}

	struct AllocNewSizeExplicitTag
	{};

	inline constexpr AllocNewSizeExplicitTag cvAllocNewSizeExplicit{};

}

TS3_CORE_API void * operator new( size_t pSize, const ts3::AllocNewSizeExplicitTag &, size_t pExtraSize );

#endif // __TS3_COMMON_MEMORY_DEFS_H__
