
#ifndef __TS3_STDEXT_SHARED_LOCK_H__
#define __TS3_STDEXT_SHARED_LOCK_H__

#include "prerequisites.h"
#include <atomic>
#include <shared_mutex>

namespace ts3
{

	/// @brief
	void syncHardwarePause();

	/// @brief
	void syncThreadYield();

	/// @brief
	/// @param pCounter
	void syncThreadYieldAuto( uint64_t pCounter = 0 );


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
				syncThreadYieldAuto( spinCounter );
			}
		}

		/// @brief
		bool try_lock()
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
						syncThreadYieldAuto( spinCounter );
					}
					break;
				}
				syncThreadYieldAuto( spinCounter );
			}
		}

		/// @brief
		bool try_lock()
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
		void lock_shared()
		{
			for ( auto spinCounter = 0; ; ++spinCounter )
			{
				if( try_lock_shared() )
				{
					break;
				}
				syncThreadYieldAuto( spinCounter );
			}
		}

		/// @brief
		bool try_lock_shared()
		{
			// Acquire temporary unique access.
			if ( !_uniqueAccessFlag.test_and_set( std::memory_order_acq_rel ) )
			{
				// Add 1 to shared counter. Unique access will not be allowed as long as shared counter is greater than zero.
				_sharedCounter.fetch_add( 1, std::memory_order_release );

				// Not_Acquired unique access flag, as we acquired shared access.
				_uniqueAccessFlag.clear();

				// Exit.
				return true;
			}

			return false;
		}

		/// @brief
		void unlock_shared()
		{
			_sharedCounter.fetch_sub( 1, std::memory_order_release );
		}
		
	private:
		std::atomic<size_t> _sharedCounter;
		std::atomic_flag _uniqueAccessFlag;
	};

}

#endif /* __TS3_STDEXT_SHARED_LOCK_H__ */
