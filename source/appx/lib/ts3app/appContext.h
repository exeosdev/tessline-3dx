
#ifndef __TS3APP_APP_CONTEXT_H__
#define __TS3APP_APP_CONTEXT_H__

#include "appCommon.h"

namespace ts3
{
    
    struct SysPlatformNativeData;
    struct SysGLSurfaceNativeData;
    struct SysGLContextNativeData;

    enum AppFeatureFlags : uint32
    {
        APP_FEATURE_CATEGORY_GFX_BIT = 0x010000,
        APP_FEATURE_ID_EVENT_CONTROLLER_BIT = 0x0001,
        APP_FEATURE_ID_GFX_OPENGL_BIT = APP_FEATURE_CATEGORY_GFX_BIT | 0x0100,
        APP_FEATURE_ID_GFX_METAL_BIT = APP_FEATURE_CATEGORY_GFX_BIT | 0x0200,
        APP_FEATURE_ID_GFX_VULKAN_BIT = APP_FEATURE_CATEGORY_GFX_BIT | 0x0400,
    };

    struct AppContextInitInfo
    {
        // Static Linking mode: wrapper (executable) is linked with a static library
        // containing a function which acts as an entry point. That function is set
        // here and saved by the app context during initialization.
        AppEntryPoint appEntryPoint;
        // Dynamic Loading support: Base dir used to load the app's module.
        std::string dlAppModuleBaseDir;
        // Dynamic Loading support: Name of the module (DLL) where the entry point is.
        std::string dlAppModuleName;
    };

    class AppContext
    {
    public:
        SysPlatformNativeData * mPlatform;
    public:
        void setIdleEventProcessingMode( bool pIdle );

    protected:
        enum AppStateFlags : uint32
        {
            APP_STATE_IDLE_EVENT_MODE_BIT = 0x0001
        };

    private:
        // Loaded entry point. Called as a part of the run() method.
        AppEntryPoint _appEntryPoint;
    };

}

#endif // __TS3APP_APP_CONTEXT_H__
