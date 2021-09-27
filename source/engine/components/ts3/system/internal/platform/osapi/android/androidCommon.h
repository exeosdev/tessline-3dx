
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_ANDROID_COMMON_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_ANDROID_COMMON_H__

#include <ts3/platform/android/androidNativeAppProxy.h>
#include <ts3/math/vector.h>

namespace ts3::system
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
        ASessionData * aSessionDataPtr = nullptr;

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
    };

    TS3_SYSTEM_API_NODISCARD ASessionData & nativeAndroidGetASessionData( SysContext & pSysContext );

    TS3_SYSTEM_API_NODISCARD ASessionData & nativeAndroidGetASessionData( const AndroidNativeDataCommon & pNativeData );

} // namespace ts3::system

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_ANDROID_COMMON_H__
