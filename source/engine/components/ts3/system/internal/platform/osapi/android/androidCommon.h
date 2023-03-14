
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_ANDROID_COMMON_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_ANDROID_COMMON_H__

#include <ts3/platform/android/androidNativeAppProxy.h>
#include <ts3/system/prerequisites.h>
#include <ts3/system/internal/platform/shared/java/javaCommon.h>
#include <ts3/math/vector.h>

namespace ts3::system
{

	class SysContext;
	class EventController;

	namespace platform
	{

		enum : uint32
		{
			E_ANDROID_APP_STATE_USER_DATA_INDEX_SYS_CONTEXT,
			E_ANDROID_APP_STATE_USER_DATA_INDEX_EVENT_CONTROLLER
		};

		struct ASessionData
		{
			AndroidAppState * aCommonAppState = nullptr;
			ANativeWindow * aNativeWindow = nullptr;
		};

		struct AndroidNativeDataCommon
		{
		public:
			void setSessionData( ASessionData & pSessionData )
			{
				ts3DebugAssert( aSessionDataPtr == nullptr );
				aSessionDataPtr = &pSessionData;
			}

			void resetSessionData()
			{
				ts3DebugAssert( aSessionDataPtr != nullptr );
				aSessionDataPtr = nullptr;
			}

			ASessionData & getSessionData() const
			{
				ts3DebugAssert( aSessionDataPtr != nullptr );
				return *aSessionDataPtr;
			}

		private:
			ASessionData * aSessionDataPtr = nullptr;
		};

		TS3_SYSTEM_API_NODISCARD ASessionData & androidGetASessionData( SysContext & pSysContext );

		TS3_SYSTEM_API_NODISCARD inline ASessionData & androidGetASessionData( const AndroidNativeDataCommon & pNativeData )
		{
			return pNativeData.getSessionData();
		}

		template <typename TNativeData>
		TS3_SYSTEM_API_NODISCARD inline ASessionData & androidGetASessionData( const NativeObject<TNativeData> & pNativeObject )
		{
			return androidGetASessionData( static_cast<AndroidNativeDataCommon>( pNativeObject.mNativeData ) );
		}
		
	}

	template <typename TBaseType, typename TNativeData>
	class AndroidNativeObject : public TBaseType, public NativeObject<TNativeData>
	{
	public:
		template <typename... TBaseTypeArgs>
		explicit AndroidNativeObject( SysContextHandle pSysContext, TBaseTypeArgs && ...pBaseTypeArgs )
		: TBaseType( pSysContext, std::forward<TBaseTypeArgs>( pBaseTypeArgs )... )
		{
			this->mNativeData.setSessionData( platform::androidGetASessionData( *pSysContext ) );
		}

		template <typename TParentSysObject, typename... TBaseTypeArgs>
		explicit AndroidNativeObject( TParentSysObject & pParentSysObject, TBaseTypeArgs && ...pBaseTypeArgs )
		: TBaseType( pParentSysObject, std::forward<TBaseTypeArgs>( pBaseTypeArgs )... )
		{
			this->mNativeData.setSessionData( platform::androidGetASessionData( pParentSysObject ) );
		}

		template <typename TParentSysObject, typename... TBaseTypeArgs>
		explicit AndroidNativeObject( SysHandle<TParentSysObject> pParentSysObject, TBaseTypeArgs && ...pBaseTypeArgs )
		: TBaseType( pParentSysObject, std::forward<TBaseTypeArgs>( pBaseTypeArgs )... )
		{
			this->mNativeData.setSessionData( platform::androidGetASessionData( *pParentSysObject ) );
		}

		virtual ~AndroidNativeObject()
		{
			this->mNativeData.resetSessionData();
		}
	};

} // namespace ts3::system

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_ANDROID_COMMON_H__
