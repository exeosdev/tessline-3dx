
#pragma once

#ifndef __TS3_CORE_ATOMIC_RWLOCK_H__
#define __TS3_CORE_ATOMIC_RWLOCK_H__

#include "commonSyncDefs.h"
#include <atomic>

namespace ts3
{

    namespace sync
    {

        enum class RWLockState : native_uint
        {
            Available,
            Locked
        };

        /// @brief Atomic-based spin-lock that provides the ability of observing the status of the lock.
        class AtomicRWLock
        {
        public:
            AtomicRWLock() = default;

            void lock()
            {
                for( auto spinCounter = 0; ; ++spinCounter )
                {
                    auto expectedFlagState = RWLockState::Available;
                    if( _lockFlag.compare_exchange_weak( expectedFlagState,
                                                         RWLockState::Locked,
                                                         std::memory_order_acq_rel,
                                                         std::memory_order_relaxed ) )
                    {
                        break;
                    }
                    yieldCurrentThreadAuto( spinCounter );
                }
            }

            void unlock()
            {
                _lockFlag.store( RWLockState::Available, std::memory_order_release );
            }

            bool tryLock()
            {
                auto currentFlagState = _lockFlag.load( std::memory_order_acquire );
                if( currentFlagState == RWLockState::Available )
                {
                    if( _lockFlag.compare_exchange_strong( currentFlagState,
                                                           RWLockState::Locked,
                                                           std::memory_order_acq_rel,
                                                           std::memory_order_relaxed ) )
                    {
                        return true;
                    }
                }
                return false;
            }

            TS3_FUNC_NO_DISCARD RWLockState state() const
            {
                return _lockFlag.load( std::memory_order_acquire );
            }

            TS3_FUNC_NO_DISCARD bool locked() const
            {
                return state() == RWLockState::Locked;
            }

        private:
            std::atomic<RWLockState> _lockFlag;
        };

    }

}

#endif // __TS3_CORE_ATOMIC_RWLOCK_H__
