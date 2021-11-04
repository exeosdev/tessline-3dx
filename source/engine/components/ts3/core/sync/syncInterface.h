
#pragma once

#ifndef __TS3_COMMON_SYNC_INTERFACE_H__
#define __TS3_COMMON_SYNC_INTERFACE_H__

#include "mutexLockTypes.h"

namespace ts3
{

    // Helper macro for acquiring a lock using a provider guard/holder type.
    #define __ts3AcquireUniqueLockHelper( pLockHolder, pLock ) \
        pLockHolder<typename std::remove_reference<decltype( pLock )>::type>{ pLock }

    /// @brief Acquires a unique access to a specified lock. Uses std::unique_lock<> for RAII.
    #define ts3SyncAcquireUnique( pLock ) \
        __ts3AcquireUniqueLockHelper( ::ts3::sync::AutoUniqueLock, getLock( pLock ) )

    /// @brief Acquires a shared access to a specified lock. Uses std::shared_lock<> for RAII.
    #define ts3SyncAcquireShared( pLock ) \
        __ts3AcquireUniqueLockHelper( ::ts3::sync::AutoSharedLock, getLock( pLock ) )

}

#endif // __TS3_COMMON_SYNC_INTERFACE_H__
