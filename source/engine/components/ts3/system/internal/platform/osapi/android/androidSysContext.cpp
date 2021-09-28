
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

		return;
		auto * aAssetManager = pCreateInfo.nativeParams.aCommonAppState->activity->assetManager;
		auto * asset = AAssetManager_open( aAssetManager, "SampleFxVK.bin", AASSET_MODE_RANDOM );
		const auto aLength = AAsset_getLength( asset );
		const void * aBuffer = AAsset_getBuffer( asset );

		auto * rDir1 = AAssetManager_openDir( aAssetManager, "shaders" );
		auto * fName11 = AAssetDir_getNextFileName( rDir1 );
		auto * fName12 = AAssetDir_getNextFileName( rDir1 );
		auto * fName13 = AAssetDir_getNextFileName( rDir1 );

		auto * rDir2 = AAssetManager_openDir( aAssetManager, "shaders/vulkan" );
		auto * fName21 = AAssetDir_getNextFileName( rDir2 );
		auto * fName22 = AAssetDir_getNextFileName( rDir2 );

		auto content = std::string( (const char*)aBuffer, aLength );
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

} // namespace ts3::system
#endif // TS3_PCL_TARGET_SYSAPI_ANDROID
