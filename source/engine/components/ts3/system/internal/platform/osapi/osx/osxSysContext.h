
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_OSX_SYS_CONTEXT_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_OSX_SYS_CONTEXT_H__

#include <ts3/system/sysContext.h>
#include "osxCommon.h"

namespace ts3::system
{

	namespace platform
	{

		struct OSXSysContextNativeData
		{
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

	private:
		void _initializeOSXContextState();
		void _releaseOSXContextState();
	};

} // namespace ts3::system

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_OSX_SYS_CONTEXT_H__
