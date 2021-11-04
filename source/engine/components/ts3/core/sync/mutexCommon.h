
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
            template <typename TpMutex>
            static void lock( TpMutex & pMutex )
            {
                pMutex.lock();
            }

            template <typename TpMutex>
            static bool tryLock( TpMutex & pMutex )
            {
                return pMutex.tryLock();
            }

            template <typename TpMutex>
            static void unlock( TpMutex & pMutex )
            {
                pMutex.unlock();
            }

            template <typename TpMutex>
            static void lockShared( TpMutex & pMutex )
            {
                pMutex.lockShared();
            }

            template <typename TpMutex>
            static bool tryLockShared( TpMutex & pMutex )
            {
                return pMutex.tryLockShared();
            }

            template <typename TpMutex>
            static void unlockShared( TpMutex & pMutex )
            {
                pMutex.unlockShared();
            }
        };

        struct MutexInterfaceProxyStd
        {
            template <typename TpMutex>
            static void lock( TpMutex & pMutex )
            {
                pMutex.lock();
            }

            template <typename TpMutex>
            static bool tryLock( TpMutex & pMutex )
            {
                return pMutex.try_mutex();
            }

            template <typename TpMutex>
            static void unlock( TpMutex & pMutex )
            {
                pMutex.unlock();
            }

            template <typename TpMutex>
            static void lockShared( TpMutex & pMutex )
            {
                pMutex.lock_shared();
            }

            template <typename TpMutex>
            static bool tryLockShared( TpMutex & pMutex )
            {
                return pMutex.try_mutex_shared();
            }

            template <typename TpMutex>
            static void unlockShared( TpMutex & pMutex )
            {
                pMutex.unlock_shared();
            }
        };


        template <typename TpMutex>
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
