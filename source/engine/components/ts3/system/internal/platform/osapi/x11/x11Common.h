
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_X11_COMMON_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_X11_COMMON_H__

#include <ts3/system/prerequisites.h>
#include <X11/X.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>

namespace ts3::system
{

	class SysContext;

	namespace platform
	{

		using XDisplay = Display *;
		using XWindow = XID;
		using XColormap = XID;

		// Having these pretty common words replaced with a numbers tends
		// to be an issue, so we use some explicit enum constants instead
		// and remove them from the global scope.
		enum : XID
		{
			E_X11_XID_ALWAYS = Always,
			E_X11_XID_NONE = None,
			E_X11_XID_SUCCESS = Success,
		};

		#undef Always
		#undef None
		#undef Success

		inline constexpr auto CX_X11_DISPLAY_NULL = static_cast<XDisplay>( nullptr );

		struct X11SessionInfo
		{
			int connectionNumber;
			std::string vendorName;
			std::string displayString;
		};

		struct X11SessionData
		{
			struct AtomCache
			{
				Atom wmProtocol = 0;
				Atom wmProtocolDelete = 0;
				Atom wmProtocolDestroy = 0;
				Atom wmState = 0;
				Atom wmStateFullscreen = 0;
			};
			XDisplay display = CX_X11_DISPLAY_NULL;
			XWindow rootWindowXID = E_X11_XID_NONE;
			int screenIndex = -1;
			AtomCache atomCache;
			X11SessionInfo sessionInfo;
		};

		struct X11NativeDataCommon
		{
		public:
			void setSessionData( X11SessionData & pSessionData )
			{
				ts3DebugAssert( xSessionDataPtr == nullptr );
				xSessionDataPtr = &pSessionData;
			}

			void resetSessionData()
			{
				ts3DebugAssert( xSessionDataPtr != nullptr );
				xSessionDataPtr = nullptr;
			}

			TS3_ATTR_NO_DISCARD X11SessionData & getSessionData() const
			{
				ts3DebugAssert( xSessionDataPtr != nullptr );
				return *xSessionDataPtr;
			}

		private:
			X11SessionData * xSessionDataPtr = nullptr;
		};

		TS3_SYSTEM_API_NODISCARD X11SessionData & x11GetXSessionData( SysContext & pSysContext );

		TS3_SYSTEM_API_NODISCARD inline X11SessionData & x11GetXSessionData( const X11NativeDataCommon & pNativeData )
		{
			return pNativeData.getSessionData();
		}

		template <typename TBaseType, typename TNativeData>
		TS3_SYSTEM_API_NODISCARD inline X11SessionData & x11GetXSessionData( const NativeObject<TBaseType, TNativeData> & pNativeObject )
		{
			return x11GetXSessionData( static_cast<X11NativeDataCommon>( pNativeObject.mNativeData ) );
		}

	}

	template <typename TBaseType, typename TNativeData>
	class X11NativeObject : public NativeObject<TBaseType, TNativeData>
	{
	public:
		template <typename... TBaseTypeArgs>
		explicit X11NativeObject( SysContextHandle pSysContext, TBaseTypeArgs && ...pBaseTypeArgs )
		: NativeObject<TBaseType, TNativeData>( pSysContext, std::forward<TBaseTypeArgs>( pBaseTypeArgs )... )
		{
			this->mNativeData.setSessionData( platform::x11GetXSessionData( *pSysContext ) );
		}

		template <typename TParentSysObject, typename... TBaseTypeArgs>
		explicit X11NativeObject( TParentSysObject & pParentSysObject, TBaseTypeArgs && ...pBaseTypeArgs )
		: NativeObject<TBaseType, TNativeData>( pParentSysObject, std::forward<TBaseTypeArgs>( pBaseTypeArgs )... )
		{
			this->mNativeData.setSessionData( platform::x11GetXSessionData( pParentSysObject ) );
		}

		template <typename TParentSysObject, typename... TBaseTypeArgs>
		explicit X11NativeObject( SysHandle<TParentSysObject> pParentSysObject, TBaseTypeArgs && ...pBaseTypeArgs )
		: NativeObject<TBaseType, TNativeData>( pParentSysObject, std::forward<TBaseTypeArgs>( pBaseTypeArgs )... )
		{
			this->mNativeData.setSessionData( platform::x11GetXSessionData( *pParentSysObject ) );
		}

		virtual ~X11NativeObject()
		{
			this->mNativeData.resetSessionData();
		}
	};

} // namespace ts3::system

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_X11_COMMON_H__
