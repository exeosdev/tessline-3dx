
#pragma once

#ifndef __TS3_CORE_SPIN_LOCK_H__
#define __TS3_CORE_SPIN_LOCK_H__

#include "commonSyncDefs.h"
#include <atomic>

namespace ts3
{

    namespace sync
    {

        /// @brief
        class SpinLock
        {
        public:
            SpinLock()
            : _flag{ ATOMIC_FLAG_INIT }
            {}

            /// @brief
            void lock()
            {
                for( auto spinCounter = 0; _flag.test_and_set( std::memory_order_acq_rel ); ++spinCounter )
                {
                    yieldCurrentThreadAuto( spinCounter );
                }
            }

            /// @brief
            bool tryLock()
            {
                return !_flag.test_and_set( std::memory_order_acq_rel );
            }

            /// @brief
            void unlock()
            {
                _flag.clear( std::memory_order_release );
            }

        private:
            std::atomic_flag _flag;
        };

        /// @brief
        class SharedSpinLock
        {
        public:
            SharedSpinLock( const SharedSpinLock & ) = delete;
            SharedSpinLock& operator=( const SharedSpinLock & ) = delete;

            SharedSpinLock()
            : _sharedCounter{ ATOMIC_VAR_INIT( 0 ) }
            , _uniqueAccessFlag{ ATOMIC_FLAG_INIT }
            {}

            /// @brief
            void lock()
            {
                for ( auto spinCounter = 0; ; ++spinCounter )
                {
                    // Acquire unique access. If some other thread is holding it, this loop will block the current thread.
                    if ( !_uniqueAccessFlag.test_and_set( std::memory_order_acq_rel ) )
                    {
                        // Wait until all threads which hold shared access will release it.
                        for ( spinCounter = 0; _sharedCounter.load( std::memory_order_acquire ) > 0; ++spinCounter )
                        {
                            yieldCurrentThreadAuto( spinCounter );
                        }
                        break;
                    }
                    yieldCurrentThreadAuto( spinCounter );
                }
            }

            /// @brief
            bool tryLock()
            {
                if ( !_uniqueAccessFlag.test_and_set( std::memory_order_acq_rel ) )
                {
                    if ( _sharedCounter.load( std::memory_order_acquire ) == 0 )
                    {
                        // Unique access flag has been set, shared access counter is zero - unique access has been acquired.
                        return true;
                    }
                    _uniqueAccessFlag.clear();
                }

                return false;
            }

            /// @brief
            void unlock()
            {
                _uniqueAccessFlag.clear();
            }

            /// @brief
            void lockShared()
            {
                for ( auto spinCounter = 0; ; ++spinCounter )
                {
                    if( tryLockShared() )
                    {
                        break;
                    }
                    yieldCurrentThreadAuto( spinCounter );
                }
            }

            /// @brief
            bool tryLockShared()
            {
                // Acquire temporary unique access.
                if ( !_uniqueAccessFlag.test_and_set( std::memory_order_acq_rel ) )
                {
                    // Add 1 to shared counter. Unique access will not be allowed as long as shared counter is greater than zero.
                    _sharedCounter.fetch_add( 1, std::memory_order_acq_rel );

                    // Clear unique access flag, as we acquired shared access.
                    _uniqueAccessFlag.clear();

                    // Exit.
                    return true;
                }

                return false;
            }

            /// @brief
            void unlockShared()
            {
                _sharedCounter.fetch_sub( 1, std::memory_order_acq_rel );
            }

        private:
            std::atomic<native_uint> _sharedCounter;
            std::atomic_flag _uniqueAccessFlag;
        };

    }

}

#endif // __TS3_CORE_SPIN_LOCK_H__
