
#include <ts3/system/sysContextNative.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_ANDROID )
namespace ts3::system
{

    void nativeSysContextInternalInitialize( SysContext & pSysContext, const SysContextCreateInfo & pCreateInfo )
	{
		auto & nativeData = pSysContext.mInternal->nativeDataPriv;

		nativeData.aSessionData.aCommonAppState = pCreateInfo.nativeParams.aCommonAppState;
		nativeData.aSessionData.aCommonAppState->ts3SetUserData( E_ANDROID_APP_STATE_USER_DATA_INDEX_SYS_CONTEXT, &pSysContext );
	}

	void nativeSysContextInternalRelease( SysContext & pSysContext )
	{
        auto & nativeData = pSysContext.mInternal->nativeDataPriv;

        nativeData.aSessionData.aCommonAppState->ts3SetUserData( E_ANDROID_APP_STATE_USER_DATA_INDEX_SYS_CONTEXT, nullptr );
        nativeData.aSessionData.aCommonAppState = nullptr;
	}

	void nativeAndroidUpdateNativeWindowRef( SysContext & pSysContext, ANativeWindow * pANativeWindow )
	{
		auto & aSessionData = nativeAndroidGetASessionData( pSysContext );
		if( pANativeWindow != aSessionData.aNativeWindow )
		{
			ANativeWindow_acquire( pANativeWindow );
			if( aSessionData.aNativeWindow )
			{
				ANativeWindow_release( aSessionData.aNativeWindow );
			}
			aSessionData.aNativeWindow = pANativeWindow;
		}

	}


	std::string SysContext::queryCurrentProcessExecutableFilePath()
	{
		pid_t currentProcessID = getpid();

		std::string executableFilePath;
		std::string exeLink = std::string( "/proc/" ) + std::to_string( currentProcessID ) + std::string( "/exe" );

		if( char * bufferPtr = realpath( exeLink.c_str(), nullptr ) )
		{
			executableFilePath = bufferPtr;
			free( bufferPtr );
		}

		return executableFilePath;
	}

} // namespace ts3::system
#endif // TS3_PCL_TARGET_SYSAPI_ANDROID
