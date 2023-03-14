
#pragma once

#ifndef __TS3_COMMON_SYNC_DEFS_H__
#define __TS3_COMMON_SYNC_DEFS_H__

#include "../prerequisites.h"
#include <ts3/platform/syncCommon.h>

namespace ts3
{

    namespace sync
    {

        template <typename TMutex>
        struct AutoUniqueLockTypeProxy;

        template <typename TMutex>
        struct AutoSharedLockTypeProxy;

    }

    /// @brief Returns a lock from a specified lockable object. Used by Sync macros.
    /// @tparam TLock Type of the lockable object, deduced automatically from the parameter.
    /// @param  pLock  A lockable object. By default it is returned directly.
    /// @details
    /// This is a simple helper function that enables our sync macros to have a single, lock-type-agnostic
    /// implementation. In order to make custom lockable types working,
    template <typename TLock>
    inline constexpr TLock & getLock( TLock & pLock )
    {
        return pLock;
    }

}

#endif // __TS3_COMMON_SYNC_DEFS_H__
