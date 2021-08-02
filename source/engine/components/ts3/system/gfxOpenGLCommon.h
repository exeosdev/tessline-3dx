
#ifndef __TS3_SYSTEM_GFX_OPENGL_COMMON_H__
#define __TS3_SYSTEM_GFX_OPENGL_COMMON_H__

#include "visual.h"
#include "windowDefs.h"
#include <GL/glew.h>

namespace ts3
{

    struct SysGLDriverNativeData;
    struct SysGLSurfaceNativeData;
    struct SysGLRenderContextNativeData;

    ts3DeclareSysHandle( SysGLDriver );
    ts3DeclareSysHandle( SysGLSurface );
    ts3DeclareSysHandle( SysGLRenderContext );

    /// @brief
    enum ESysGLSurfaceCreateFlags : uint32
    {
        // Specifies a full-screen surface, covering the whole screen.
        // Ignored for mobile (iOS and Android), where fullscreen is always used.
        E_SYS_GFX_GL_DISPLAY_SURFACE_CREATE_FLAG_FULLSCREEN_BIT = 0x1000,
        E_SYS_GFX_GL_DISPLAY_SURFACE_CREATE_FLAG_SYNC_ADAPTIVE_BIT = 0x2000,
        E_SYS_GFX_GL_DISPLAY_SURFACE_CREATE_FLAG_SYNC_VERTICAL_BIT = 0x4000,
    };

    /// @brief
    enum ESysGLRenderContextCreateFlags : uint32
    {
        E_SYS_GFX_GL_RENDER_CONTEXT_CREATE_FLAG_ENABLE_DEBUG_BIT = 0x1000,
        E_SYS_GFX_GL_RENDER_CONTEXT_CREATE_FLAG_FORWARD_COMPATIBLE_BIT = 0x2000,
        E_SYS_GFX_GL_RENDER_CONTEXT_CREATE_FLAG_ENABLE_SHARING_BIT = 0x0010,
        E_SYS_GFX_GL_RENDER_CONTEXT_CREATE_FLAG_SHARE_WITH_CURRENT_BIT = 0x0020
    };

    /// @brief
    enum class ESysGLAPIProfile : enum_default_value_t
    {
        Auto,
        Core,
        GLES,
        Legacy
    };

    /// @brief Represents combined info about the current OpenGL subsystem version.
    /// Basically, this struct stores the output from all version-related GL queries.
    struct SysGLSystemVersionInfo
    {
    public:
        // Numeric version of the GL (GL_VERSION_MAJOR.GL_VERSION_MINOR)
        Version apiVersion;
        // Text version of the GL (GL_VERSION)
        std::string apiVersionStr;
        // Text version of the GLSL (GL_SHADING_LANGUAGE_VERSION)
        std::string glslVersionStr;
        // Name of the renderer (GL_RENDERER_NAME)
        std::string rendererName;
        // Name of the vendor (GL_VENDOR_NAME)
        std::string vendorName;

    public:
        std::string toString() const;
    };

    /// @brief Contains parameters used to specify how a GL display surface should be created.
    struct SysGLSurfaceCreateInfo
    {
        //
        SysVisualConfig visualConfig;
        //
        SysWindowGeometry windowGeometry;
        //
        Bitmask<ESysGLSurfaceCreateFlags> flags = 0;
    };

    /// @brief
    struct SysGLRenderContextCreateInfo
    {
        //
        SysGLRenderContextHandle shareContext = nullptr;
        // Target API version (minimum) a context must support.
        Version requiredAPIVersion;
        // Target API profile which will be used by the app.
        ESysGLAPIProfile targetAPIProfile;
        //
        Bitmask<ESysGLRenderContextCreateFlags> flags = 0;
    };


}

#endif // __TS3_SYSTEM_GFX_OPENGL_COMMON_H__
