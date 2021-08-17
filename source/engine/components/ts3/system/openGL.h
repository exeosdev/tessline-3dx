
#ifndef __TS3_SYSTEM_GFX_OPENGL_H__
#define __TS3_SYSTEM_GFX_OPENGL_H__

#include "openGLCommon.h"
#include "sysObject.h"

namespace ts3
{
namespace system
{

    ts3SysDeclareHandle( DisplayManager );

    /// @brief Contains parameters used to specify how a GL display surface should be created.
    struct GLDisplaySurfaceCreateInfo
    {
        //
        VisualConfig visualConfig;
        //
        WindowGeometry windowGeometry;
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
        DisplayManagerHandle const mDisplayManager;

    public:
        GLSystemDriver( DisplayManagerHandle pDisplayManager );
        virtual ~GLSystemDriver();

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
        TS3_PCL_ATTR_NO_DISCARD GLDisplaySurfaceHandle createDisplaySurface( const GLDisplaySurfaceCreateInfo & pCreateInfo );

        /// @brief Creates a GLDisplaySurface object, that wraps currently bound surface for the current thread.
        /// @param pTargetSurface Existing surface object to be used. If null, a new GLDisplaySurface will be created.
        /// This function creates a logical surface object without creating an actual (system-level) surface. It enables
        /// usage of the library API in environments where low-level GL stuff is created externally (a good example is
        /// the editor, where wxWidgets manages underlying GL surface/context). Important note: on some environments there
        /// might be no way to obtain all data required to create a wrapper around surface. If that is the case, this
        /// method will throw an exception with EXC_NOT_SUPPORTED code.
        /// @note The surface specified as pTargetSurface is just a hint and may be discarded. Always use returned handle!
        /// @note If no surface is currently bound, null handle is returned, regardless of the value of pTargetSurface.
        TS3_PCL_ATTR_NO_DISCARD GLDisplaySurfaceHandle createDisplaySurfaceForCurrentThread( GLDisplaySurfaceHandle pTargetSurface = nullptr );

        /// @brief Creates a system OpenGL render context with a configuration matching specified requirements.
        /// @param pSurface Surface to be used for context creation. Context can be bound to any surface compatible with this one.
        /// @param pCreateInfo CreateInfo struct with a context specification (OpenGL API version, profile, etc.)
        TS3_PCL_ATTR_NO_DISCARD GLRenderContextHandle createRenderContext( GLDisplaySurface & pSurface,
                                                                           const GLRenderContextCreateInfo & pCreateInfo );

        /// @brief Creates a GLRenderContext object, that wraps currently bound context for the current thread.
        /// @param pTargetContext Existing context object to be used. If null, a new GLRenderContext will be created.
        /// This functions works similar to createDisplaySurfaceForCurrentThread(), but does that for the currently
        /// bound OpenGL render context. See description of createDisplaySurfaceForCurrentThread() for details.
        /// @note The context specified as pTargetContext is just a hint and may be discarded. Always use returned handle!
        /// @note If no context is currently bound, null handle is returned, regardless of the value of pTargetContext.
        TS3_PCL_ATTR_NO_DISCARD GLRenderContextHandle createRenderContextForCurrentThread( GLRenderContextHandle pTargetContext = nullptr );

        /// @brief
        TS3_PCL_ATTR_NO_DISCARD std::vector<DepthStencilFormat> querySupportedDepthStencilFormats( ColorFormat pColorFormat ) const;

        /// @brief
        TS3_PCL_ATTR_NO_DISCARD std::vector<MSAAMode> querySupportedMSAAModes( ColorFormat pColorFormat,
                                                                               DepthStencilFormat pDepthStencilFormat ) const;

        /// @brief
        TS3_PCL_ATTR_NO_DISCARD bool isRenderContextBound() const;

        /// @brief
        TS3_PCL_ATTR_NO_DISCARD bool isRenderContextBound( GLRenderContextHandle pRenderContext ) const;

    friendapi: // For friended types
        GLSystemDriverNativeData & getNativeData();

        const GLSystemDriverNativeData & getNativeData() const;

    private: // For implementation

        virtual void _nativeInitializePlatform() = 0;
        virtual void _nativeReleaseInitState( GLRenderContext & pRenderContext ) = 0;
        virtual void _nativeCreateDisplaySurface( GLDisplaySurface & pDisplaySurface, const GLDisplaySurfaceCreateInfo & pCreateInfo ) = 0;
        virtual void _nativeCreateDisplaySurfaceForCurrentThread( GLDisplaySurface & pDisplaySurface ) = 0;
        virtual void _nativeCreateRenderContext( GLRenderContext & pRenderContext, const GLRenderContextCreateInfo & pCreateInfo ) = 0;
        virtual void _nativeCreateRenderContextForCurrentThread( GLRenderContext & pRenderContext ) = 0;
        virtual bool _nativeIsRenderContextBound() const = 0;
        virtual bool _nativeIsRenderContextBound( const GLRenderContext & pRenderContext ) const = 0;
    };

    class GLDisplaySurface : public SysObject
    {
    public:
        GLSystemDriverHandle const mDriver;

    public:
        GLDisplaySurface( GLSystemDriverHandle pDriver );
        virtual ~GLDisplaySurface();

        /// @brief
        void swapBuffers();

        /// @brief
        void destroy();

        /// @brief
        TS3_PCL_ATTR_NO_DISCARD WindowSize queryRenderAreaSize() const;

        /// @brief
        TS3_PCL_ATTR_NO_DISCARD bool checkDriver( const GLSystemDriver & pDriver ) const;

        /// @brief
        TS3_PCL_ATTR_NO_DISCARD bool isValid() const;

    private:
        virtual void _nativeSwapBuffers() = 0;
        virtual void _nativeDestroy() = 0;
        virtual void _nativeQueryRenderAreaSize( WindowSize & pOutSize ) const = 0;
    };

    class GLRenderContext : public SysObject
    {
    public:
        GLSystemDriverHandle const mDriver;

    public:
        GLRenderContext( GLSystemDriverHandle pDriver );
        virtual ~GLRenderContext();

        /// @brief
        void bindForCurrentThread( const GLDisplaySurface & pSurface );

        /// @brief
        void destroy();

        /// @brief
        TS3_PCL_ATTR_NO_DISCARD GLSystemVersionInfo querySystemVersionInfo() const;

        /// @brief
        TS3_PCL_ATTR_NO_DISCARD bool checkDriver( const GLSystemDriver & pDriver ) const;

        /// @brief
        TS3_PCL_ATTR_NO_DISCARD bool isCurrent() const;

        /// @brief
        TS3_PCL_ATTR_NO_DISCARD bool isValid() const;

    private:
        virtual void _nativeBindForCurrentThread( const GLDisplaySurface & pSurface ) = 0;
        virtual void _nativeDestroy() = 0;
        virtual bool _nativeIsCurrent() const = 0;
        virtual bool _nativeIsValid() const = 0;
    };

} // namespace system
} // namespace ts3

#endif // __TS3_SYSTEM_GFX_OPENGL_H__
