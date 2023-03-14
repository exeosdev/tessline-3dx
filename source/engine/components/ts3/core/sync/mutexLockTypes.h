
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

        template <typename TMutex, typename TMutexInterface>
        class UniqueLock
        {
        public:
            UniqueLock( TMutex & pMutex )
            : _mutex( &pMutex )
            , _ownsLock( false )
            {
                lock();
            }

            UniqueLock( TMutex & pMutex, const std::defer_lock_t & )
            : _mutex( &pMutex )
            , _ownsLock( false )
            {}

            UniqueLock( TMutex & pMutex, const std::try_to_lock_t & )
            : _mutex( &pMutex )
            , _ownsLock( false )
            {
                _ownsLock = tryLock();
            }

            UniqueLock( TMutex & pMutex, const std::adopt_lock_t & )
            : _mutex( &pMutex )
            , _ownsLock( true )
            {}

            UniqueLock( UniqueLock && pSrcObject )
            : _mutex( pSrcObject._mutex )
            , _ownsLock( pSrcObject._ownsLock )
            {
	            pSrcObject._mutex = nullptr;
	            pSrcObject._ownsLock = false;
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
                TMutexInterface::lock( *_mutex );
                _ownsLock = true;
            }

            bool tryLock()
            {
                _ownsLock = TMutexInterface::tryLock( *_mutex );
                return _ownsLock;
            }

            void unlock()
            {
                TMutexInterface::unlock( *_mutex );
                _ownsLock = false;
            }

            TMutex * releaseMutex()
            {
                TMutex * mutexPtr = _mutex;
                _mutex = nullptr;
                _ownsLock = false;
                return mutexPtr;

            }

            TS3_ATTR_NO_DISCARD bool checkLockOwnership() const
            {
                return _ownsLock;
            }

            void swap( UniqueLock & pOther )
            {
                std::swap( _mutex, pOther._mutex );
                std::swap( _ownsLock, pOther._ownsLock );
            }

        private:
            TMutex * _mutex;
            bool _ownsLock;
        };

        template <typename TMutex, typename TMutexInterface>
        class SharedLock
        {
        public:
            explicit SharedLock( TMutex & pMutex )
            : _mutex( &pMutex )
            , _ownsLock( false )
            {
                lock();
            }

            SharedLock( TMutex & pMutex, const std::defer_lock_t & )
            : _mutex( &pMutex )
            , _ownsLock( false )
            {}

            SharedLock( TMutex & pMutex, const std::try_to_lock_t & )
            : _mutex( &pMutex )
            , _ownsLock( false )
            {
                _ownsLock = tryLock();
            }

            SharedLock( TMutex & pMutex, const std::adopt_lock_t & )
            : _mutex( &pMutex )
            , _ownsLock( true )
            {}

            SharedLock( SharedLock && pSrcObject ) noexcept
            : _mutex( pSrcObject._mutex )
            , _ownsLock( pSrcObject._ownsLock )
            {
	            pSrcObject._mutex = nullptr;
	            pSrcObject._ownsLock = false;
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
                TMutexInterface::lockShared( *_mutex );
                _ownsLock = true;
            }

            bool tryLock()
            {
                _ownsLock = TMutexInterface::tryLockShared( *_mutex );
                return _ownsLock;
            }

            void unlock()
            {
                TMutexInterface::unlockShared( *_mutex );
                _ownsLock = false;
            }

            TMutex * releaseMutex()
            {
                TMutex * mutexPtr = _mutex;
                _mutex = nullptr;
                _ownsLock = false;
                return mutexPtr;

            }

            TS3_ATTR_NO_DISCARD bool checkLockOwnership() const
            {
                return _ownsLock;
            }

            void swap( SharedLock & pOther )
            {
                std::swap( _mutex, pOther._mutex );
                std::swap( _ownsLock, pOther._ownsLock );
            }

        private:
            TMutex * _mutex;
            bool _ownsLock;
        };


        template <typename TMutex>
        using AutoUniqueLock = UniqueLock<TMutex, typename MutexInterface<TMutex>::Type>;

        template <typename TMutex>
        using AutoSharedLock = SharedLock<TMutex, typename MutexInterface<TMutex>::Type>;

    }

}

#endif // __TS3_CORE_MUTEX_LOCK_TYPES_H__
