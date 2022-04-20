
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_SYS_CONTEXT_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_SYS_CONTEXT_H__

#include <ts3/system/sysContext.h>
#include "win32Common.h"

namespace ts3::system
{

	namespace platform
	{

		struct Win32SysContextNativeData
		{
			HINSTANCE appExecModuleHandle;
		};

		struct SysContextCreateInfoNativeParams
		{
			HINSTANCE appExecModuleHandle;
		};

	}

	class Win32SysContext : public NativeObject<SysContext, platform::Win32SysContextNativeData>
	{
	public:
		Win32SysContext();
		virtual ~Win32SysContext() noexcept;

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

	private:
		void _initializeWin32ContextState();
		void _releaseWin32ContextState();
	};

} // namespace ts3::system

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_SYS_CONTEXT_H__
