#pragma once

#ifndef __TS3_CORE_DYNAMIC_INTERFACE_H__
#define __TS3_CORE_DYNAMIC_INTERFACE_H__

#include <memory>

namespace ts3
{

	template <typename TpClass>
	using SharedHandle = std::shared_ptr<TpClass>;

	template <typename TpClass>
	using WeakHandle = std::weak_ptr<TpClass>;

	class DynamicInterface : public std::enable_shared_from_this<DynamicInterface>
	{
	public:
		virtual ~DynamicInterface() = default;

		template <typename TpSubclass>
		TpSubclass * getInterface()
		{
		#if( TS3_DEBUG )
			return dynamic_cast_check<TpSubclass>( this );
		#else
			return static_cast<TpSubclass *>( this );
		#endif
		}

		template <typename TpSubclass>
		const TpSubclass * getInterface() const
		{
		#if( TS3_DEBUG )
		    return dynamic_cast_check<const TpSubclass>( this );
		#else
		    return static_cast<const TpSubclass *>( this );
		#endif
		}

		template <typename TpSubclass>
		TpSubclass * queryInterface()
		{
		#if( TS3_DEBUG )
			return dynamic_cast_throw<TpSubclass>( this );
		#else
			return static_cast<TpSubclass *>( this );
		#endif
		}

		template <typename TpSubclass>
		const TpSubclass * queryInterface() const
		{
		#if( TS3_DEBUG )
			return dynamic_cast_throw<const TpSubclass>( this );
		#else
			return static_cast<const TpSubclass *>( this );
		#endif
		}

		template <typename TpSubclass>
		SharedHandle<TpSubclass> getHandle()
		{
		#if( TS3_DEBUG )
			return dynamic_shared_ptr_cast_check<TpSubclass>( shared_from_this() );
		#else
			return std::static_pointer_cast<TpSubclass>( shared_from_this() );
		#endif
		}

		template <typename TpSubclass>
		SharedHandle<TpSubclass> queryHandle()
		{
		#if( TS3_DEBUG )
			return dynamic_shared_ptr_cast_throw<TpSubclass>( shared_from_this() );
		#else
			return std::static_pointer_cast<TpSubclass>( shared_from_this() );
		#endif
		}

	private:
		using std::enable_shared_from_this<DynamicInterface>::shared_from_this;
		using std::enable_shared_from_this<DynamicInterface>::weak_from_this;
	};

	template <typename TpClass, typename... TpArgs>
	inline SharedHandle<TpClass> createDynamicInterfaceObject( TpArgs && ...pArgs )
	{
		auto objectHandle = std::make_shared<TpClass>( std::forward<TpArgs>( pArgs )... );
	#if( TS3_DEBUG )
		// This will trigger a compile-time error if TpClass is not a subclass of DynamicInterface.
		static_cast<DynamicInterface *>( objectHandle.get() );
	#endif
		return objectHandle;
	}

    template <typename TpClass, typename TpDeleter, typename... TpArgs>
    inline SharedHandle<TpClass> createDynamicInterfaceObjectWithDeleter( TpDeleter pDeleter, TpArgs && ...pArgs )
    {
        auto objectHandle = std::shared_ptr<TpClass>{ new TpClass( std::forward<TpArgs>( pArgs )... ), std::forward<TpDeleter>( pDeleter ) };
    #if( TS3_DEBUG )
        // This will trigger a compile-time error if TpClass is not a subclass of DynamicInterface.
        static_cast<DynamicInterface *>( objectHandle.get() );
    #endif
        return objectHandle;
    }

	template <typename TpOutInterface, typename TpInInterface>
	inline std::unique_ptr<TpOutInterface> moveInterfaceUniquePtr( std::unique_ptr<TpInInterface> pUPtr )
	{
		TpOutInterface * targetPtr = pUPtr->template queryInterface<TpOutInterface>();
		pUPtr.release();
		return std::unique_ptr<TpOutInterface>{ targetPtr };
	}

#define ts3DeclareClassHandle( pClassName ) \
    class pClassName; \
    using pClassName##Handle = SharedHandle<pClassName>; \
    using pClassName##WeakHandle = WeakHandle<pClassName>

#define ts3DeclareTypedefHandle( pAliasName, pTypeName ) \
    using pAliasName = pTypeName; using pAliasName##Handle = SharedHandle<pAliasName>

}

#endif // __TS3_CORE_DYNAMIC_INTERFACE_H__
