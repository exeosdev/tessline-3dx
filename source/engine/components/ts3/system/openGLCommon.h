
#ifndef __TS3_SYSTEM_GFX_OPENGL_COMMON_H__
#define __TS3_SYSTEM_GFX_OPENGL_COMMON_H__

#include "visual.h"
#include "windowCommon.h"
#include <GL/glew.h>

namespace ts3
{
namespace system
{

    ts3SysDeclareHandle( GLSystemDriver );
    ts3SysDeclareHandle( GLDisplaySurface );
    ts3SysDeclareHandle( GLRenderContext );

    ts3SysDeclareNativeTypes( GLSystemDriver );
    ts3SysDeclareNativeTypes( GLDisplaySurface );
    ts3SysDeclareNativeTypes( GLRenderContext );

    /// @brief
    enum EGLSurfaceCreateFlags : uint32
    {
        // Specifies a full-screen surface, covering the whole screen.
        // Ignored for mobile (iOS and Android), where fullscreen is always used.
        E_GL_DISPLAY_SURFACE_CREATE_FLAG_FULLSCREEN_BIT = 0x1000,
        E_GL_DISPLAY_SURFACE_CREATE_FLAG_SYNC_ADAPTIVE_BIT = 0x2000,
        E_GL_DISPLAY_SURFACE_CREATE_FLAG_SYNC_VERTICAL_BIT = 0x4000,
    };

    /// @brief
    enum EGLRenderContextCreateFlags : uint32
    {
        E_GL_RENDER_CONTEXT_CREATE_FLAG_ENABLE_DEBUG_BIT = 0x1000,
        E_GL_RENDER_CONTEXT_CREATE_FLAG_FORWARD_COMPATIBLE_BIT = 0x2000,
        E_GL_RENDER_CONTEXT_CREATE_FLAG_ENABLE_SHARING_BIT = 0x0010,
        E_GL_RENDER_CONTEXT_CREATE_FLAG_SHARE_WITH_CURRENT_BIT = 0x0020
    };

    /// @brief
    enum class EGLAPIProfile : enum_default_value_t
    {
        Auto,
        Core,
        GLES,
        Legacy
    };

    /// @brief Represents combined info about the current OpenGL subsystem version.
    /// Basically, this struct stores the output from all version-related GL queries.
    struct GLSystemVersionInfo
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

} // namespace system
} // namespace ts3

#endif // __TS3_SYSTEM_GFX_OPENGL_COMMON_H__
