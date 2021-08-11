
#ifndef __TS3_SYSTEM_GFX_OPENGL_H__
#define __TS3_SYSTEM_GFX_OPENGL_H__

#include "openGLCommon.h"
#include "sysObject.h"

namespace ts3
{
namespace system
{

    /// @brief Contains parameters used to specify how a GL display surface should be created.
    struct GLSurfaceCreateInfo
    {
        //
        VisualConfig visualConfig;
        //
        WmWindowGeometry windowGeometry;
        //
        Bitmask<EGLSurfaceCreateFlags> flags = 0;
    };

    /// @brief
    struct GLRenderContextCreateInfo
    {
        //
        GLRenderContextHandle shareContext = nullptr;
        // Target API version (minimum) a context must support.
        Version requiredAPIVersion;
        // Target API profile which will be used by the app.
        EGLAPIProfile targetAPIProfile;
        //
        Bitmask<EGLRenderContextCreateFlags> flags = 0;
    };

    class GLSystemDriver : public SysObject
    {
    public:
        GLSystemDriver();
        virtual ~GLSystemDriver() = default;

        /// @brief Initializes core OpenGL state and system-level interfaces.
        /// This method also creates any additionally required
        void initializePlatform();

        /// @brief Releases temporary init state created automatically by the library. Context must be a valid GL context.
        /// Call this method after you have created actual display surface and context. It's not mandatory, (everything
        /// will be released at shutdown anyway), but on some platforms this could free some extra memory and release
        /// couple system-level interfaces.
        void releaseInitState( GLRenderContext & pGLRenderContext );

        /// @brief Creates a system OpenGL surface (usually - a window) with a configuration matching specified requirements.
        /// @param pCreateInfo CreateInfo struct with a surface specification (VisualConfig, window geometry, etc.)
        TS3_PCL_ATTR_NO_DISCARD GLDisplaySurfaceHandle createDisplaySurface( const GLSurfaceCreateInfo & pCreateInfo ) const;

        /// @brief Creates a GLDisplaySurface object, that wraps currently bound surface for the current thread.
        /// @param pTargetSurface Existing surface object to be used. If null, a new GLDisplaySurface will be created.
        /// This function creates a logical surface object without creating an actual (system-level) surface. It enables
        /// usage of the library API in environments where low-level GL stuff is created externally (a good example is
        /// the editor, where wxWidgets manages underlying GL surface/context). Important note: on some environments there
        /// might be no way to obtain all data required to create a wrapper around surface. If that is the case, this
        /// method will throw an exception with EXC_NOT_SUPPORTED code.
        /// @note The surface specified as pTargetSurface is just a hint and may be discarded. Always use returned handle!
        /// @note If no surface is currently bound, null handle is returned, regardless of the value of pTargetSurface.
        TS3_PCL_ATTR_NO_DISCARD GLDisplaySurfaceHandle createDisplaySurfaceForCurrentThread( GLDisplaySurfaceHandle pTargetSurface = nullptr ) const;

        /// @brief Creates a system OpenGL render context with a configuration matching specified requirements.
        /// @param pSurface Surface to be used for context creation. Context can be bound to any surface compatible with this one.
        /// @param pCreateInfo CreateInfo struct with a context specification (OpenGL API version, profile, etc.)
        TS3_PCL_ATTR_NO_DISCARD GLRenderContextHandle createRenderContext( GLDisplaySurface & pSurface, const GLRenderContextCreateInfo & pCreateInfo ) const;

        /// @brief Creates a GLRenderContext object, that wraps currently bound context for the current thread.
        /// @param pTargetContext Existing context object to be used. If null, a new GLRenderContext will be created.
        /// This functions works similar to createDisplaySurfaceForCurrentThread(), but does that for the currently
        /// bound OpenGL render context. See description of createDisplaySurfaceForCurrentThread() for details.
        /// @note The context specified as pTargetContext is just a hint and may be discarded. Always use returned handle!
        /// @note If no context is currently bound, null handle is returned, regardless of the value of pTargetContext.
        TS3_PCL_ATTR_NO_DISCARD GLRenderContextHandle createRenderContextForCurrentThread( GLRenderContextHandle pTargetContext = nullptr ) const;

        /// @brief
        TS3_PCL_ATTR_NO_DISCARD std::vector<DepthStencilFormat> querySupportedDepthStencilFormats( ColorFormat pColorFormat ) const;

        /// @brief
        TS3_PCL_ATTR_NO_DISCARD std::vector<MSAAMode> querySupportedMSAAModes( ColorFormat pColorFormat,
                                                                               DepthStencilFormat pDepthStencilFormat ) const;
    };

    class GLDisplaySurface : public SysObject
    {
    public:
        GLSystemDriverHandle const mDriver;

    public:
        GLDisplaySurface( GLSystemDriverHandle pDriver );
        virtual ~GLDisplaySurface() = default;

        /// @brief
        void swapBuffers();

        /// @brief
        TS3_PCL_ATTR_NO_DISCARD WmWindowSize queryRenderAreaSize() const;

        /// @brief Returns a boolean value indicating whether this surface is currently bound as the target surface.
        /// This method checks currently bound surface handle (for the current thread) against internal handle of this
        /// surface object. Result indicates whether they match (i,e, if the surface is bound for rendering via GL API).
        TS3_PCL_ATTR_NO_DISCARD bool querySurfaceBindStatus() const;
    };

    class GLRenderContext
    {
    public:
        GLSystemDriverHandle const mDriver;

    public:
        GLRenderContext( GLSystemDriverHandle pDriver );
        virtual ~GLRenderContext() = default;

        /// @brief
        void bindForCurrentThread();

        /// @brief
        TS3_PCL_ATTR_NO_DISCARD bool validateCurrentBinding() const;

        /// @brief
        TS3_PCL_ATTR_NO_DISCARD GLSystemVersionInfo querySystemVersionInfo() const;
    };

} // namespace system
} // namespace ts3

#endif // __TS3_SYSTEM_GFX_OPENGL_H__
