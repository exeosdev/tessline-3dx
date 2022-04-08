
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_OSX_COMMON_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_OSX_COMMON_H__

#include <ts3/system/prerequisites.h>
#include <ts3/stdext/bitmaskAtomic.h>
#include <objc/objc.h>
#import <CoreGraphics/CGError.h>

namespace ts3::system
{

	class OSXSysContext;

    namespace platform
    {

		enum EOSXCommonStateFlags : uint32
		{
			E_OSX_COMMON_STATE_APP_FINISHED_LAUNCHING_BIT = 0x0001
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

			TS3_FUNC_NO_DISCARD OSXSharedData & getSharedData() const
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

		template <typename TpBaseType, typename TpNativeData>
		TS3_SYSTEM_API_NODISCARD inline OSXSharedData & osxGetOSXSharedData( const NativeObject<TpBaseType, TpNativeData> & pNativeObject )
		{
			return osxGetOSXSharedData( static_cast<OSXNativeDataCommon>( pNativeObject.mNativeData ) );
		}

        TS3_SYSTEM_API_NODISCARD const char * osxQueryCGErrorMessage( CGError pCGError );

		bool osxNibLoadMenuNibFile();

		void osxNibCreateDefaultApplicationMenu();

    }


	template <typename TpBaseType, typename TpNativeData>
	class OSXNativeObject : public NativeObject<TpBaseType, TpNativeData>
	{
	public:
		template <typename... TpBaseTypeArgs>
		explicit OSXNativeObject( SysContextHandle pSysContext, TpBaseTypeArgs && ...pBaseTypeArgs )
		: NativeObject<TpBaseType, TpNativeData>( pSysContext, std::forward<TpBaseTypeArgs>( pBaseTypeArgs )... )
		{
			this->mNativeData.setSharedData( platform::osxGetOSXSharedData( *pSysContext ) );
		}

		template <typename TpParentSysObject, typename... TpBaseTypeArgs>
		explicit OSXNativeObject( TpParentSysObject & pParentSysObject, TpBaseTypeArgs && ...pBaseTypeArgs )
		: NativeObject<TpBaseType, TpNativeData>( pParentSysObject, std::forward<TpBaseTypeArgs>( pBaseTypeArgs )... )
		{
			this->mNativeData.setSharedData( platform::osxGetOSXSharedData( pParentSysObject ) );
		}

		template <typename TpParentSysObject, typename... TpBaseTypeArgs>
		explicit OSXNativeObject( Handle<TpParentSysObject> pParentSysObject, TpBaseTypeArgs && ...pBaseTypeArgs )
		: NativeObject<TpBaseType, TpNativeData>( pParentSysObject, std::forward<TpBaseTypeArgs>( pBaseTypeArgs )... )
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
