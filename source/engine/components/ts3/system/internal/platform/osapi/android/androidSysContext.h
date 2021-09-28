
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_ANDROID_CORE_SESSION_CONTEXT_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_ANDROID_CORE_SESSION_CONTEXT_H__

#include "androidCommon.h"

namespace ts3::system
{

    struct SysContextNativeData
    {
		ASessionData aSessionData;
    };

	struct SysContextCreateInfoNativeParams
	{
		AndroidAppState * aCommonAppState = nullptr;
	};

	void nativeAndroidUpdateNativeWindowRef( SysContext & pSysContext, ANativeWindow * pANativeWindow );

} // namespace ts3::system

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_ANDROID_CORE_SESSION_CONTEXT_H__
