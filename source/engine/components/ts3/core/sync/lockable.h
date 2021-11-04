
#pragma once

#ifndef __TS3_CORE_LOCKABLE_H__
#define __TS3_CORE_LOCKABLE_H__

#include "mutexCommon.h"

namespace ts3
{

    namespace sync
    {

        /// @brief Specifies how lock can be accessed and retrieved.
        enum class ELockableMutexAccess : enum_default_value_t
        {
            // Lock is always retrieved as non-const reference, which allows the object to be locked
            // in any possible state. Note, that this implies lock member to be declared as mutable.
            Relaxed,

            // The type of retrieved lock (in terms of its const-ness) matches the object's type.
            // This makes impossible to lock an object, that was declared/retrieved as const-qualified.
            Strict
        };


        /// @brief Helper base class for types representing lockable objects.
        /// @tparam TpMutex       Type of the mutex an instance of the class will contain.
        /// @tparam tpLockAccess Specifies how the mutex can be accessed. Modifies how the public interface is defined.
        /// Lockable<> is a simple base class which can be used to inject a locking behaviour. Type of the actual lock
        /// (like std::mutex or some custom ones) can be controlled via first template parameter. The second one controls
        /// the lock access (see ELockableMutexAccess for more details).
        /// @example
        /// Lockable<> also works with ts3SyncAcquire*() macros. This enables a simple syntax to acquire locks:
        /// @code
        /// class SharedResource : public Lockable<std::mutex> {};
        /// SharedResource res;
        /// // Some time later:
        /// auto lockGuard = ts3SyncAcquireUnique( res );
        /// @endcode
        template <typename TpMutex, ELockableMutexAccess tpLockAccess = ELockableMutexAccess::Strict>
        class Lockable;

        /// @brief Specialization of Lockable<> for ELockableMutexAccess::Relaxed.
        /// Relaxed lock access means, that internal lock is a mutable variable and all public
        /// lock-related methods are defined as const-qualified (const objects can be locked).
        template <typename TpMutex>
        class Lockable<TpMutex, ELockableMutexAccess::Relaxed>
        {
        public:
            using MutexType = TpMutex;
            using MutexInterface = typename MutexInterface<TpMutex>::Type;

        public:
            Lockable() = default;

            virtual void lock() const
            {
                MutexInterface::lock( _mutex );
            }

            virtual bool tryLock() const
            {
                return MutexInterface::tryLock( _mutex );
            }

            virtual void unlock() const
            {
                MutexInterface::unlock( _mutex );
            }

            TpMutex & getMutex() const
            {
                return _mutex;
            }

        protected:
            mutable TpMutex _mutex;
        };

        /// @brief Specialization of Lockable<> for ELockableMutexAccess::Strict.
        /// Strict lock access means, that internal lock is a "normal" variable and all public
        /// lock-related methods are defined as non-const-qualified (const objects CANNOT be locked).
        template <typename TpMutex>
        class Lockable<TpMutex, ELockableMutexAccess::Strict>
        {
        public:
            using MutexType = TpMutex;
            using MutexInterface = typename MutexInterface<TpMutex>::Type;

        public:
            Lockable() = default;

            virtual void lock()
            {
                MutexInterface::lock( _mutex );
            }

            virtual bool tryLock()
            {
                return MutexInterface::tryLock( _mutex );
            }

            virtual void unlock()
            {
                MutexInterface::unlock( _mutex );
            }

            TpMutex & getMutex()
            {
                return _mutex;
            }

            const TpMutex & getMutex() const
            {
                return _mutex;
            }

        protected:
            TpMutex _mutex;
        };

        // getMutex() implementation for Lockable<>. Returns the internal lock. It enables all
        // Lockable objects to be used with ts3SyncAcquireUnique()/ts3SyncAcquireShared() macros.

        template <typename TpMutex>
        inline constexpr TpMutex & getLock( const Lockable<TpMutex, ELockableMutexAccess::Relaxed> & pLock )
        {
            return pLock.getMutex();
        }

        template <typename TpMutex>
        inline constexpr TpMutex & getLock( Lockable<TpMutex, ELockableMutexAccess::Strict> & pLock )
        {
            return pLock.getMutex();
        }

    }

}

#endif // __TS3_CORE_LOCKABLE_H__
