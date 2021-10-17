
#include "androidSysContext.h"
#include "androidAssetSystem.h"
#include "androidDisplaySystem.h"
#include "androidEventCore.h"
#include "androidFileManager.h"
#include "androidOpenGLDriver.h"
#include "androidWindowSystem.h"
#include <ts3/system/sysContextNative.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_ANDROID )
namespace ts3::system
{

	SysContextHandle createSysContext( const SysContextCreateInfo & pCreateInfo )
	{
		if( !pCreateInfo.nativeParams.aCommonAppState )
		{
			return nullptr;
		}
		return createDynamicInterfaceObject<AndroidSysContext>( pCreateInfo.nativeParams.aCommonAppState );
	}


	AndroidSysContext::AndroidSysContext( AndroidAppState * pAppState )
	: mJVMInstance( std::make_unique<JavaVMInstance>( pAppState->activity->vm ) )
	, mSysThreadJNIObject( mJVMInstance->acquireJNIForCurrentThread() )
	{
		_initializeAndroidContextState( pAppState );
	}

	AndroidSysContext::~AndroidSysContext() noexcept
	{
		_releaseAndroidContextState();
	}

	AssetLoaderHandle AndroidSysContext::createAssetLoader( const AssetLoaderCreateInfo & pCreateInfo )
	{
		return createSysObject<AndroidAssetLoader>( getHandle<AndroidSysContext>() );
	}

	DisplayManagerHandle AndroidSysContext::createDisplayManager()
	{
		return createSysObject<AndroidDisplayManager>( getHandle<AndroidSysContext>() );
	}

	EventControllerHandle AndroidSysContext::createEventController()
	{
		return createSysObject<AndroidEventController>( getHandle<AndroidSysContext>() );
	}

	FileManagerHandle AndroidSysContext::createFileManager()
	{
		return createSysObject<PosixFileManager>( getHandle<AndroidSysContext>() );
	}

	OpenGLSystemDriverHandle AndroidSysContext::createOpenGLSystemDriver( DisplayManagerHandle pDisplayManager )
	{
		if( !pDisplayManager )
		{
			pDisplayManager = createDisplayManager();
		}

		auto androidDisplayManager = pDisplayManager->queryHandle<AndroidDisplayManager>();
		return createSysObject<AndroidOpenGLSystemDriver>( androidDisplayManager );
	}

	WindowManagerHandle AndroidSysContext::createWindowManager( DisplayManagerHandle pDisplayManager )
	{
		if( !pDisplayManager )
		{
			pDisplayManager = createDisplayManager();
		}

		return createSysObject<AndroidWindowManager>( pDisplayManager );
	}

	std::string AndroidSysContext::queryCurrentProcessExecutableFilePath() const
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

	void AndroidSysContext::_initializeAndroidContextState( AndroidAppState * pAppState )
	{
		mNativeData.aSessionData.aCommonAppState = pAppState;
		mNativeData.aSessionData.aCommonAppState->ts3SetUserData(
			platform::E_ANDROID_APP_STATE_USER_DATA_INDEX_SYS_CONTEXT, this );
	}

	void AndroidSysContext::_releaseAndroidContextState()
	{
		mNativeData.aSessionData.aCommonAppState->ts3SetUserData(
			platform::E_ANDROID_APP_STATE_USER_DATA_INDEX_SYS_CONTEXT, nullptr );
		mNativeData.aSessionData.aCommonAppState = nullptr;
	}

	void AndroidSysContext::updateANativeWindowReference( ANativeWindow * pANativeWindow )
	{
		if( pANativeWindow != mNativeData.aSessionData.aNativeWindow )
		{
			ANativeWindow_acquire( pANativeWindow );
			if( mNativeData.aSessionData.aNativeWindow )
			{
				ANativeWindow_release( mNativeData.aSessionData.aNativeWindow );
			}
			mNativeData.aSessionData.aNativeWindow = pANativeWindow;
		}
	}

	platform::ASessionData & AndroidSysContext::getASessionData()
	{
		return mNativeData.aSessionData;
	}

	const platform::ASessionData & AndroidSysContext::getASessionData() const
	{
		return mNativeData.aSessionData;
	}

} // namespace ts3::system
#endif // TS3_PCL_TARGET_SYSAPI_ANDROID
