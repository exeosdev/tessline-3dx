
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_ANDROID_CORE_SESSION_CONTEXT_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_ANDROID_CORE_SESSION_CONTEXT_H__

#include "androidCommon.h"
#include <ts3/system/sysContext.h>
#include <ts3/system/internal/platform/shared/java/javaVM.h>
#include <ts3/system/internal/platform/shared/java/javaNativeInterface.h>
#include <thread>

namespace ts3::system
{

	namespace platform
	{

		struct AndroidSysContextNativeData
		{
			ASessionData aSessionData;
		};

		struct SysContextCreateInfoNativeParams
		{
			AndroidAppState * aCommonAppState = nullptr;
		};

	}

	class AndroidSysContext : public SysContext, public NativeObject<platform::AndroidSysContextNativeData>
	{
	public:
		std::unique_ptr<JavaVMInstance> const mJVMInstance;
		JavaNativeInterfacePtr const mSysThreadJNIObject;

	public:
		AndroidSysContext( AndroidAppState * pAppState );
		virtual ~AndroidSysContext() noexcept;

		/// @copybrief SysContext::createAssetLoader
		virtual AssetLoaderHandle createAssetLoader( const AssetLoaderCreateInfo & pCreateInfo ) override final;

		/// @copybrief SysContext::createDisplayManager
		virtual DisplayManagerHandle createDisplayManager() override final;

		/// @copybrief SysContext::createEventController
		virtual EventControllerHandle createEventController() override final;

		/// @copybrief SysContext::createFileManager
		virtual FileManagerHandle createFileManager() override final;

		/// @copybrief SysContext::createOpenGLSystemDriver
		virtual OpenGLSystemDriverHandle createOpenGLSystemDriver( DisplayManagerHandle pDisplayManager ) override final;

		/// @copybrief SysContext::createWindowManager
		virtual WindowManagerHandle createWindowManager( DisplayManagerHandle pDisplayManager ) override final;

		/// @copybrief SysContext::queryCurrentProcessExecutableFilePath
		virtual std::string queryCurrentProcessExecutableFilePath() const override final;

		void updateANativeWindowReference( ANativeWindow * pANativeWindow );

		platform::ASessionData & getASessionData();

		const platform::ASessionData & getASessionData() const;

	private:
		void _initializeAndroidContextState( AndroidAppState * pAppState );
		void _releaseAndroidContextState();
	};

} // namespace ts3::system

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_ANDROID_CORE_SESSION_CONTEXT_H__
