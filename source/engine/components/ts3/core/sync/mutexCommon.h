
#pragma once

#ifndef __TS3_CORE_MUTEX_COMMON_H__
#define __TS3_CORE_MUTEX_COMMON_H__

#include "commonSyncDefs.h"

namespace std
{

    class mutex;
    class shared_mutex;

}

namespace ts3
{

    namespace sync
    {

        struct MutexInterfaceProxyDefault
        {
            template <typename TMutex>
            static void lock( TMutex & pMutex )
            {
                pMutex.lock();
            }

            template <typename TMutex>
            static bool tryLock( TMutex & pMutex )
            {
                return pMutex.tryLock();
            }

            template <typename TMutex>
            static void unlock( TMutex & pMutex )
            {
                pMutex.unlock();
            }

            template <typename TMutex>
            static void lockShared( TMutex & pMutex )
            {
                pMutex.lockShared();
            }

            template <typename TMutex>
            static bool tryLockShared( TMutex & pMutex )
            {
                return pMutex.tryLockShared();
            }

            template <typename TMutex>
            static void unlockShared( TMutex & pMutex )
            {
                pMutex.unlockShared();
            }
        };

        struct MutexInterfaceProxyStd
        {
            template <typename TMutex>
            static void lock( TMutex & pMutex )
            {
                pMutex.lock();
            }

            template <typename TMutex>
            static bool tryLock( TMutex & pMutex )
            {
                return pMutex.try_mutex();
            }

            template <typename TMutex>
            static void unlock( TMutex & pMutex )
            {
                pMutex.unlock();
            }

            template <typename TMutex>
            static void lockShared( TMutex & pMutex )
            {
                pMutex.lock_shared();
            }

            template <typename TMutex>
            static bool tryLockShared( TMutex & pMutex )
            {
                return pMutex.try_mutex_shared();
            }

            template <typename TMutex>
            static void unlockShared( TMutex & pMutex )
            {
                pMutex.unlock_shared();
            }
        };


        template <typename TMutex>
        struct MutexInterface
        {
            using Type = MutexInterfaceProxyDefault;
        };

        template <>
        struct MutexInterface<std::mutex>
        {
            using Type = MutexInterfaceProxyStd;
        };

        template <>
        struct MutexInterface<std::shared_mutex>
        {
            using Type = MutexInterfaceProxyStd;
        };

    }

}

#endif // __TS3_CORE_MUTEX_COMMON_H__
