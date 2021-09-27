
#ifndef __TS3_SYSTEM_GFX_OPENGL_COMMON_H__
#define __TS3_SYSTEM_GFX_OPENGL_COMMON_H__

#include "visual.h"
#include "windowCommon.h"

#define TS3_SYSTEM_GL_PLATFORM_TYPE_CORE 0x7001
#define TS3_SYSTEM_GL_PLATFORM_TYPE_ES   0x7002

#if( TS3_PCL_TARGET_OS & TS3_PCL_TARGET_FLAG_OS_DESKTOP )
#  define TS3_SYSTEM_GL_PLATFORM_TYPE TS3_SYSTEM_GL_PLATFORM_TYPE_CORE
#  include <GL/glew.h>
#else
#  define TS3_SYSTEM_GL_PLATFORM_TYPE TS3_SYSTEM_GL_PLATFORM_TYPE_ES
#  include <GLES3/gl3platform.h>
#  include <GLES3/gl3.h>
#endif

#if !defined( TS3_SYSTEM_GL_ENABLE_ERROR_CHECKS )
#  if( TS3_DEBUG || TS3_SYSTEM_GL_ENABLE_ERROR_CHECKS_NON_DEBUG )
#    define TS3_SYSTEM_GL_ENABLE_ERROR_CHECKS 1
#  else
#    define TS3_SYSTEM_GL_ENABLE_ERROR_CHECKS 1
#  endif
#endif

namespace ts3::system
{

    ts3SysDeclareHandle( GLSystemDriver );
    ts3SysDeclareHandle( GLDisplaySurface );
    ts3SysDeclareHandle( GLRenderContext );

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
        E_GL_RENDER_CONTEXT_CREATE_FLAG_SHARING_OPTIONAL_BIT = 0x0020,
        E_GL_RENDER_CONTEXT_CREATE_FLAG_SHARE_WITH_CURRENT_BIT = 0x0040
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

    class GLCoreAPI
    {
    public:
        static Version queryRuntimeVersion();

        static bool checkLastResult();

        static bool checkLastError( GLenum pErrorCode );

        static void handleLastError();

        static void resetErrorQueue();

        static const char * translateErrorCode( GLenum pError );
    };

} // namespace ts3::system

#if( TS3_SYSTEM_GL_ENABLE_ERROR_CHECKS )
#  define ts3GLCheckLastResult()             GLCoreAPI::checkLastResult()
#  define ts3GLCheckLastError( pErrorCode )  GLCoreAPI::checkLastError( pErrorCode )
#  define ts3GLHandleLastError()             GLCoreAPI::handleLastError()
#  define ts3GLResetErrorQueue()             GLCoreAPI::resetErrorQueue()
#else
#  define ts3GLCheckLastResult()
#  define ts3GLCheckLastError( pErrorCode )
#  define ts3GLHandleLastError()
#  define ts3GLResetErrorQueue()
#endif

#endif // __TS3_SYSTEM_GFX_OPENGL_COMMON_H__
