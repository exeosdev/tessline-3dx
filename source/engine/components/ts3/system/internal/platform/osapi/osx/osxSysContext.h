
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_OSX_SYS_CONTEXT_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_OSX_SYS_CONTEXT_H__

#include "osxCommon.h"
#include <ts3/system/sysContext.h>
#include <ts3/stdext/bitmaskAtomic.h>

namespace ts3::system
{

	namespace platform
	{

		struct OSXSysContextNativeData
		{
			id nsApplicationProxy = nil;

			id nsApplicationDelegate = nil;

			OSXSharedData osxSharedData;
		};

		struct SysContextCreateInfoNativeParams
		{
		};

	}

	class OSXSysContext : public NativeObject<SysContext, platform::OSXSysContextNativeData>
	{
	public:
		OSXSysContext();
		virtual ~OSXSysContext() noexcept;

		/// @override SysContext::createAssetLoader
		virtual AssetLoaderHandle createAssetLoader( const AssetLoaderCreateInfo & pCreateInfo ) override final;

		/// @override SysContext::createDisplayManager
		virtual DisplayManagerHandle createDisplayManager() override final;

		/// @override SysContext::createEventController
		virtual EventControllerHandle createEventController() override final;

		/// @override SysContext::createFileManager
		virtual FileManagerHandle createFileManager() override final;

		/// @override SysContext::createOpenGLSystemDriver
		virtual OpenGLSystemDriverHandle createOpenGLSystemDriver( DisplayManagerHandle pDisplayManager ) override final;

		/// @override SysContext::createWindowManager
		virtual WindowManagerHandle createWindowManager( DisplayManagerHandle pDisplayManager ) override final;

		/// @override SysContext::queryCurrentProcessExecutableFilePath
		virtual std::string queryCurrentProcessExecutableFilePath() const override final;

		bool isNSAppProxyRegistered() const;

	private:
		void _initializeOSXContextState();
		void _releaseOSXContextState();

		void _registerNSAppProxy();

	private:
		enum : uint32
		{
			E_STATE_NS_APP_PROXY_REGISTERED = 0x1000,
		};

		AtomicBitmask<uint32> _stateMask;
	};

} // namespace ts3::system

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_OSX_SYS_CONTEXT_H__
