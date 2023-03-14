
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_OSX_COMMON_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_OSX_COMMON_H__

#include <ts3/system/prerequisites.h>
#include <ts3/stdext/bitmaskAtomic.h>

#import <AppKit/NSApplication.h>
#import <CoreGraphics/CGError.h>

namespace ts3::system
{

	class OSXSysContext;

	namespace platform
	{

		enum EOSXCommonStateFlags : uint32
		{
			E_OSX_COMMON_STATE_APP_DELEGATE_INITIALIZED_BIT = 0x0001,
			E_OSX_COMMON_STATE_APP_FINISHED_LAUNCHING_BIT = 0x0002
		};

		struct OSXSharedData
		{
			AtomicBitmask<EOSXCommonStateFlags> stateFlags = 0u;
		};

		struct OSXNativeDataCommon
		{
		public:
			void setSharedData( OSXSharedData & pSharedData )
			{
				ts3DebugAssert( osxSharedDataPtr == nullptr );
				osxSharedDataPtr = &pSharedData;
			}

			void resetSharedData()
			{
				ts3DebugAssert( osxSharedDataPtr != nullptr );
				osxSharedDataPtr = nullptr;
			}

			TS3_ATTR_NO_DISCARD OSXSharedData & getSharedData() const
			{
				ts3DebugAssert( osxSharedDataPtr != nullptr );
				return *osxSharedDataPtr;
			}

		private:
			OSXSharedData * osxSharedDataPtr = nullptr;
		};

		TS3_SYSTEM_API_NODISCARD OSXSharedData & osxGetOSXSharedData( SysContext & pSysContext );

		TS3_SYSTEM_API_NODISCARD OSXSharedData & osxGetOSXSharedData( OSXSysContext & pSysContext );

		TS3_SYSTEM_API_NODISCARD inline OSXSharedData & osxGetOSXSharedData( const OSXNativeDataCommon & pNativeData )
		{
			return pNativeData.getSharedData();
		}

		template <typename TBaseType, typename TNativeData>
		TS3_SYSTEM_API_NODISCARD inline OSXSharedData & osxGetOSXSharedData( const NativeObject<TBaseType, TNativeData> & pNativeObject )
		{
			return osxGetOSXSharedData( static_cast<OSXNativeDataCommon>( pNativeObject.mNativeData ) );
		}

		TS3_SYSTEM_API_NODISCARD bool osxCheckAppKitFrameworkVersion( NSAppKitVersion pRequiredVersion );

		TS3_SYSTEM_API_NODISCARD const char * osxQueryCGErrorMessage( CGError pCGError );

		bool osxNibLoadMenuNibFile();

		void osxNibCreateDefaultApplicationMenu();

	}


	template <typename TBaseType, typename TNativeData>
	class OSXNativeObject : public NativeObject<TBaseType, TNativeData>
	{
	public:
		template <typename... TBaseTypeArgs>
		explicit OSXNativeObject( SysContextHandle pSysContext, TBaseTypeArgs && ...pBaseTypeArgs )
		: NativeObject<TBaseType, TNativeData>( pSysContext, std::forward<TBaseTypeArgs>( pBaseTypeArgs )... )
		{
			this->mNativeData.setSharedData( platform::osxGetOSXSharedData( *pSysContext ) );
		}

		template <typename TParentSysObject, typename... TBaseTypeArgs>
		explicit OSXNativeObject( TParentSysObject & pParentSysObject, TBaseTypeArgs && ...pBaseTypeArgs )
		: NativeObject<TBaseType, TNativeData>( pParentSysObject, std::forward<TBaseTypeArgs>( pBaseTypeArgs )... )
		{
			this->mNativeData.setSharedData( platform::osxGetOSXSharedData( pParentSysObject ) );
		}

		template <typename TParentSysObject, typename... TBaseTypeArgs>
		explicit OSXNativeObject( SysHandle<TParentSysObject> pParentSysObject, TBaseTypeArgs && ...pBaseTypeArgs )
		: NativeObject<TBaseType, TNativeData>( pParentSysObject, std::forward<TBaseTypeArgs>( pBaseTypeArgs )... )
		{
			this->mNativeData.setSharedData( platform::osxGetOSXSharedData( *pParentSysObject ) );
		}

		virtual ~OSXNativeObject()
		{
			this->mNativeData.resetSharedData();
		}
	};

} // namespace ts3::system

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_OSX_COMMON_H__
