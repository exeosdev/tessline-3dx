#pragma once

#ifndef __TS3_CORE_DYNAMIC_INTERFACE_H__
#define __TS3_CORE_DYNAMIC_INTERFACE_H__

#include <memory>

namespace ts3
{

	template <typename TClass>
	using SharedHandle = std::shared_ptr<TClass>;

	template <typename TClass>
	using WeakHandle = std::weak_ptr<TClass>;

	inline constexpr std::nullptr_t cvNullHandle { nullptr };

	class DynamicInterface : public std::enable_shared_from_this<DynamicInterface>
	{
	public:
		virtual ~DynamicInterface() = default;

		template <typename TInterfaceSubClass>
		TS3_ATTR_NO_DISCARD TInterfaceSubClass * getInterface()
		{
		#if( TS3_DEBUG )
			return dynamic_cast_check<TInterfaceSubClass>( this );
		#else
			return static_cast<TInterfaceSubClass *>( this );
		#endif
		}

		template <typename TInterfaceSubClass>
		TS3_ATTR_NO_DISCARD const TInterfaceSubClass * getInterface() const
		{
		#if( TS3_DEBUG )
		    return dynamic_cast_check<const TInterfaceSubClass>( this );
		#else
		    return static_cast<const TInterfaceSubClass *>( this );
		#endif
		}

		template <typename TInterfaceSubClass>
		TS3_ATTR_NO_DISCARD TInterfaceSubClass * queryInterface()
		{
		#if( TS3_DEBUG )
			return dynamic_cast_check<TInterfaceSubClass>( this );
		#else
			return dynamic_cast_throw<TInterfaceSubClass>( this );
		#endif
		}

		template <typename TInterfaceSubClass>
		TS3_ATTR_NO_DISCARD const TInterfaceSubClass * queryInterface() const
		{
		#if( TS3_DEBUG )
			return dynamic_cast_check<const TInterfaceSubClass>( this );
		#else
			return dynamic_cast_throw<const TInterfaceSubClass>( this );
		#endif
		}

		template <typename TInterfaceSubClass>
		TS3_ATTR_NO_DISCARD SharedHandle<TInterfaceSubClass> getHandle()
		{
		#if( TS3_DEBUG )
			return dynamic_ptr_cast_check<TInterfaceSubClass>( shared_from_this() );
		#else
			return std::static_pointer_cast<TInterfaceSubClass>( shared_from_this() );
		#endif
		}

		template <typename TInterfaceSubClass>
		TS3_ATTR_NO_DISCARD SharedHandle<TInterfaceSubClass> queryHandle()
		{
		#if( TS3_DEBUG )
			return dynamic_ptr_cast_throw<TInterfaceSubClass>( shared_from_this() );
		#else
			return std::static_pointer_cast<TInterfaceSubClass>( shared_from_this() );
		#endif
		}

	private:
		using std::enable_shared_from_this<DynamicInterface>::shared_from_this;
		using std::enable_shared_from_this<DynamicInterface>::weak_from_this;
	};

	template <typename TClass, typename... TArgs>
	TS3_ATTR_NO_DISCARD inline SharedHandle<TClass> createDynamicInterfaceObject( TArgs && ...pArgs )
	{
		auto objectHandle = std::make_shared<TClass>( std::forward<TArgs>( pArgs )... );
	#if( TS3_DEBUG )
		// This will trigger a compile-time error if TClass is not a subclass of DynamicInterface.
		static_cast<DynamicInterface *>( objectHandle.get() );
	#endif
		return objectHandle;
	}

    template <typename TClass, typename TDeleter, typename... TArgs>
    TS3_ATTR_NO_DISCARD inline SharedHandle<TClass> createDynamicInterfaceObjectWithDeleter( TDeleter pDeleter, TArgs && ...pArgs )
    {
        auto objectHandle = std::shared_ptr<TClass>{ new TClass( std::forward<TArgs>( pArgs )... ), std::forward<TDeleter>( pDeleter ) };
    #if( TS3_DEBUG )
        // This will trigger a compile-time error if TClass is not a subclass of DynamicInterface.
        static_cast<DynamicInterface *>( objectHandle.get() );
    #endif
        return objectHandle;
    }

	template <typename TResultType, typename TInputType>
	TS3_ATTR_NO_DISCARD inline std::unique_ptr<TResultType> moveInterfaceUniquePtr( std::unique_ptr<TInputType> pUPtr )
	{
		std::unique_ptr<TResultType> result;
		if( pUPtr )
		{
			if( TResultType * targetPtr = pUPtr->template queryInterface<TResultType>() )
			{
				result = std::unique_ptr<TResultType>{ targetPtr };
				pUPtr.release();
			}
		}
		return result;
	}

	template <typename TInterfaceSubClass, typename TInputType>
	TS3_ATTR_NO_DISCARD inline TInterfaceSubClass * dynamic_interface_cast_get( TInputType * pInterfacePtr )
	{
		return reinterpret_cast<DynamicInterface *>( pInterfacePtr )->template getInterface<TInterfaceSubClass>();
	}

	template <typename TInterfaceSubClass, typename TInputType>
	TS3_ATTR_NO_DISCARD inline const TInterfaceSubClass * dynamic_interface_cast_get( const TInputType * pInterfacePtr )
	{
		return reinterpret_cast<const DynamicInterface *>( pInterfacePtr )->template getInterface<TInterfaceSubClass>();
	}

	template <typename TInterfaceSubClass, typename TInputType>
	TS3_ATTR_NO_DISCARD inline TInterfaceSubClass * dynamic_interface_cast_query( TInputType * pInterfacePtr )
	{
		return reinterpret_cast<DynamicInterface *>( pInterfacePtr )->template queryInterface<TInterfaceSubClass>();
	}

	template <typename TInterfaceSubClass, typename TInputType>
	TS3_ATTR_NO_DISCARD inline const TInterfaceSubClass * dynamic_interface_cast_query( const TInputType * pInterfacePtr )
	{
		return reinterpret_cast<const DynamicInterface *>( pInterfacePtr )->template queryInterface<TInterfaceSubClass>();
	}

#define ts3DeclareClassHandle( pClassName ) \
    class pClassName; \
    using pClassName##Handle = SharedHandle<pClassName>; \
    using pClassName##WeakHandle = WeakHandle<pClassName>

#define ts3DeclareTypedefHandle( pAliasName, pTypeName ) \
    using pAliasName = pTypeName; \
    using pAliasName##Handle = SharedHandle<pAliasName>; \
    using pAliasName##WeakHandle = WeakHandle<pAliasName>

}

#endif // __TS3_CORE_DYNAMIC_INTERFACE_H__
