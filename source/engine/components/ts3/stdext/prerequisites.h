
#ifndef __TS3_STDEXT_PREREQUISITES_H__
#define __TS3_STDEXT_PREREQUISITES_H__

#include <ts3/platform/debug.h>

#include <algorithm>
#include <type_traits>
#include <utility>

namespace ts3
{

    /// @brief Represents max value which can be stored in the size_t type.
    inline constexpr size_t CX_MAX_SIZE = static_cast<size_t>( -1 );

    /// @brief Represents and invalid position (e.g. in a container).
	inline constexpr size_t CX_INVALID_POSITION = static_cast<size_t>( -1 );

}

#endif // __TS3_STDEXT_PREREQUISITES_H__
