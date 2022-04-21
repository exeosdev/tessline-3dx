
#pragma once

#ifndef __TS3_CORE_MUTEX_LOCK_TYPES_H__
#define __TS3_CORE_MUTEX_LOCK_TYPES_H__

#include "mutexCommon.h"
#include <mutex>
#include <shared_mutex>

namespace ts3
{

    namespace sync
    {

        template <typename TpMutex, typename TpMutexInterface>
        class UniqueLock
        {
        public:
            UniqueLock( TpMutex & pMutex )
            : _mutex( &pMutex )
            , _ownsLock( false )
            {
                lock();
            }

            UniqueLock( TpMutex & pMutex, const std::defer_lock_t & )
            : _mutex( &pMutex )
            , _ownsLock( false )
            {}

            UniqueLock( TpMutex & pMutex, const std::try_to_lock_t & )
            : _mutex( &pMutex )
            , _ownsLock( false )
            {
                _ownsLock = tryLock();
            }

            UniqueLock( TpMutex & pMutex, const std::adopt_lock_t & )
            : _mutex( &pMutex )
            , _ownsLock( true )
            {}

            UniqueLock( UniqueLock && pSource )
            : _mutex( pSource._mutex )
            , _ownsLock( pSource._ownsLock )
            {
                pSource._mutex = nullptr;
                pSource._ownsLock = false;
            }

            ~UniqueLock()
            {
                if( _ownsLock )
                {
                    unlock();
                }
            }

            void lock()
            {
                TpMutexInterface::lock( *_mutex );
                _ownsLock = true;
            }

            bool tryLock()
            {
                _ownsLock = TpMutexInterface::tryLock( *_mutex );
                return _ownsLock;
            }

            void unlock()
            {
                TpMutexInterface::unlock( *_mutex );
                _ownsLock = false;
            }

            TpMutex * releaseMutex()
            {
                TpMutex * mutexPtr = _mutex;
                _mutex = nullptr;
                _ownsLock = false;
                return mutexPtr;

            }

            TS3_FUNC_NO_DISCARD bool checkLockOwnership() const
            {
                return _ownsLock;
            }

            void swap( UniqueLock & pOther )
            {
                std::swap( _mutex, pOther._mutex );
                std::swap( _ownsLock, pOther._ownsLock );
            }

        private:
            TpMutex * _mutex;
            bool _ownsLock;
        };

        template <typename TpMutex, typename TpMutexInterface>
        class SharedLock
        {
        public:
            explicit SharedLock( TpMutex & pMutex )
            : _mutex( &pMutex )
            , _ownsLock( false )
            {
                lock();
            }

            SharedLock( TpMutex & pMutex, const std::defer_lock_t & )
            : _mutex( &pMutex )
            , _ownsLock( false )
            {}

            SharedLock( TpMutex & pMutex, const std::try_to_lock_t & )
            : _mutex( &pMutex )
            , _ownsLock( false )
            {
                _ownsLock = tryLock();
            }

            SharedLock( TpMutex & pMutex, const std::adopt_lock_t & )
            : _mutex( &pMutex )
            , _ownsLock( true )
            {}

            SharedLock( SharedLock && pSource ) noexcept
            : _mutex( pSource._mutex )
            , _ownsLock( pSource._ownsLock )
            {
            	pSource._mutex = nullptr;
            	pSource._ownsLock = false;
            }

            ~SharedLock()
            {
                if( _ownsLock )
                {
                    unlock();
                }
            }

            SharedLock & operator=( SharedLock && pRhs ) noexcept
            {
                SharedLock( std::move( pRhs ) ).swap( *this );
                return *this;
            }

            void lock()
            {
                TpMutexInterface::lockShared( *_mutex );
                _ownsLock = true;
            }

            bool tryLock()
            {
                _ownsLock = TpMutexInterface::tryLockShared( *_mutex );
                return _ownsLock;
            }

            void unlock()
            {
                TpMutexInterface::unlockShared( *_mutex );
                _ownsLock = false;
            }

            TpMutex * releaseMutex()
            {
                TpMutex * mutexPtr = _mutex;
                _mutex = nullptr;
                _ownsLock = false;
                return mutexPtr;

            }

            TS3_FUNC_NO_DISCARD bool checkLockOwnership() const
            {
                return _ownsLock;
            }

            void swap( SharedLock & pOther )
            {
                std::swap( _mutex, pOther._mutex );
                std::swap( _ownsLock, pOther._ownsLock );
            }

        private:
            TpMutex * _mutex;
            bool _ownsLock;
        };


        template <typename TpMutex>
        using AutoUniqueLock = UniqueLock<TpMutex, typename MutexInterface<TpMutex>::Type>;

        template <typename TpMutex>
        using AutoSharedLock = SharedLock<TpMutex, typename MutexInterface<TpMutex>::Type>;

    }

}

#endif // __TS3_CORE_MUTEX_LOCK_TYPES_H__
