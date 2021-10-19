
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_X11_CORE_SESSION_CONTEXT_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_X11_CORE_SESSION_CONTEXT_H__

#include "x11Common.h"
#include <ts3/system/sysContext.h>

namespace ts3::system
{

    namespace platform
    {

        struct X11SysContextNativeData
        {
            X11SessionData xSessionData;
        };

        struct SysContextCreateInfoNativeParams
        {
        };

    }

    class X11SysContext : public SysContext, public NativeObject<platform::X11SysContextNativeData>
    {
    public:
        X11SysContext();
        virtual ~X11SysContext() noexcept;

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
        void _initializeX11ContextState();
        void _releaseX11ContextState();
    };

} // namespace ts3::system

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_X11_CORE_SESSION_CONTEXT_H__
