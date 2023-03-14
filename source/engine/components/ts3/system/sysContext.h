
#ifndef __TS3_SYSTEM_CORE_SYS_CONTEXT_H__
#define __TS3_SYSTEM_CORE_SYS_CONTEXT_H__

#include "prerequisites.h"

namespace ts3::system
{

	namespace platform
	{

		struct AssetLoaderCreateInfoNativeParams;
		struct SysContextCreateInfoNativeParams;

	}

	struct MetalSystemDriverCreateInfo;

	ts3SysDeclareHandle( SysContext );
	ts3SysDeclareHandle( AssetLoader );
	ts3SysDeclareHandle( DisplayManager );
	ts3SysDeclareHandle( EventController );
	ts3SysDeclareHandle( FileManager );
	ts3SysDeclareHandle( MetalSystemDriver );
	ts3SysDeclareHandle( OpenGLSystemDriver );
	ts3SysDeclareHandle( WindowManager );

	struct AssetLoaderCreateInfo
	{
		platform::AssetLoaderCreateInfoNativeParams * nativeParams = nullptr;
	};

	struct SysContextCreateInfo
	{
		platform::SysContextCreateInfoNativeParams * nativeParams = nullptr;
	};

	/// @brief
	class SysContext : public DynamicInterface
	{
	public:
		SysContext();
		virtual ~SysContext() noexcept;

		virtual AssetLoaderHandle createAssetLoader( const AssetLoaderCreateInfo & pCreateInfo ) = 0;

		virtual DisplayManagerHandle createDisplayManager() = 0;

		virtual EventControllerHandle createEventController() = 0;

		virtual FileManagerHandle createFileManager() = 0;

		virtual MetalSystemDriverHandle createMetalSystemDriver( DisplayManagerHandle pDisplayManager,
																 const MetalSystemDriverCreateInfo & pCreateInfo );

		virtual OpenGLSystemDriverHandle createOpenGLSystemDriver( DisplayManagerHandle pDisplayManager );

		virtual WindowManagerHandle createWindowManager( DisplayManagerHandle pDisplayManager ) = 0;

		virtual std::string queryCurrentProcessExecutableFilePath() const = 0;

		std::string queryCurrentProcessExecutableDirectory() const;
	};

	namespace platform
	{

		/// @brief Creates a new SysContext instance and returns the handle.
		/// @param pCreateInfo
		TS3_SYSTEM_API_NODISCARD SysContextHandle createSysContext( const SysContextCreateInfo & pCreateInfo );

	}

} // namespace ts3::system

#endif // __TS3_SYSTEM_CORE_SYS_CONTEXT_H__
