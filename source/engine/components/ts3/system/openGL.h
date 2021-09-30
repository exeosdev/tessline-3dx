
#ifndef __TS3_SYSTEM_OPENGL_H__
#define __TS3_SYSTEM_OPENGL_H__

#include "openGLCommon.h"
#include "eventSystem.h"

namespace ts3::system
{

    struct GLSystemDriverNativeData;
    struct GLDisplaySurfaceNativeData;
    struct GLRenderContextNativeData;

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

    inline constexpr Version cvGLVersionBestSupported{ CX_UINT16_MAX, CX_UINT16_MAX };

    inline constexpr Version cvGLVersionUnknown{ 0, 0 };

    inline constexpr Version cvGLVersionMaxDesktop{ 4, 6 };

    inline constexpr Version cvGLVersionMaxES{ 3, 2 };

    /// @brief
    class GLDisplaySurface : public EventSource
    {
        friend class GLSystemDriver;

    public:
        struct ObjectInternalData;
        GLSystemDriverHandle const mDriver;
        std::unique_ptr<ObjectInternalData> const mInternal;
        const GLDisplaySurfaceNativeData * const mNativeData;

    public:
        explicit GLDisplaySurface( GLSystemDriverHandle pDriver );
        virtual ~GLDisplaySurface() noexcept;

        void clearColorBuffer();

        /// @brief
        void swapBuffers();

        /// @brief
        TS3_PCL_ATTR_NO_DISCARD WindowSize queryRenderAreaSize() const;

        /// @brief
        TS3_PCL_ATTR_NO_DISCARD bool isValid() const;

    friendapi:
        void setInvalidState();

    private:
        void _nativeSwapBuffers();
        void _nativeQueryRenderAreaSize( WindowSize & pOutSize ) const;
    };

    /// @brief
    class GLRenderContext : public SysObject
    {
        friend class GLSystemDriver;

    public:
        struct ObjectInternalData;
        GLSystemDriverHandle const mDriver;
        std::unique_ptr<ObjectInternalData> const mInternal;
        const GLRenderContextNativeData * const mNativeData;

    public:
        explicit GLRenderContext( GLSystemDriverHandle pDriver );
        virtual ~GLRenderContext() noexcept;

        /// @brief
        void bindForCurrentThread( const GLDisplaySurface & pSurface );

        /// @brief
        TS3_PCL_ATTR_NO_DISCARD GLSystemVersionInfo querySystemVersionInfo() const;

        /// @brief
        TS3_PCL_ATTR_NO_DISCARD bool isCurrent() const;

        /// @brief
        TS3_PCL_ATTR_NO_DISCARD bool isValid() const;

    friendapi:
        void setInvalidState();

    private:
        void _nativeBindForCurrentThread( const GLDisplaySurface & pSurface );
    };

    /// @brief
    class GLSystemDriver : public SysObject
    {
        friend class GLDisplaySurface;
        friend class GLRenderContext;

    public:
        struct ObjectInternalData;
        DisplayManagerHandle const mDisplayManager;
        std::unique_ptr<ObjectInternalData> const mInternal;
        const GLSystemDriverNativeData * const mNativeData;

    public:
        GLSystemDriver( DisplayManagerHandle pDisplayManager );
        virtual ~GLSystemDriver() noexcept;

        /// @brief Initializes the driver.
        /// The GL driver is always automatically initialized when it is created for the first time,
        /// so there is no need to explicitly call this function (calling it multiple times has no
        /// effect). However, if the driver was released by calling release() on it (but not destroyed),
        /// it can be re-initialized using this function. This enables the app to avoid re-creating
        /// the driver object on some platforms (like Android - Suspend/Resume events).
        void initialize();

        /// @brief Releases the driver. Invalidates all surfaces and contexts.
        /// This function destroys all driver resources and renders all existing surfaces and contexts
        /// created with this driver invalid (like if their release() method was called). Unlike the
        /// driver itself, surfaces and contexts cannot be re-initialized, so after the driver is
        /// released, they must be explicitly re-created.
        void release();

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
        TS3_PCL_ATTR_NO_DISCARD std::vector<EDepthStencilFormat> querySupportedDepthStencilFormats( EColorFormat pColorFormat ) const;

        /// @brief
        TS3_PCL_ATTR_NO_DISCARD std::vector<EMSAAMode> querySupportedMSAAModes( EColorFormat pColorFormat,
                                                                               EDepthStencilFormat pDepthStencilFormat ) const;

        /// @brief
        void resetContextBinding();

        /// @brief
        TS3_PCL_ATTR_NO_DISCARD bool isRenderContextBound() const;

        /// @brief
        TS3_PCL_ATTR_NO_DISCARD bool isRenderContextBound( const GLRenderContext & pRenderContext ) const;

        TS3_SYSTEM_API_NODISCARD bool isDisplaySurfaceValid( const GLDisplaySurface & pDisplaySurface ) const;

        TS3_SYSTEM_API_NODISCARD bool isRenderContextValid( const GLRenderContext & pRenderContext ) const;

    friendapi:
        // Used by the GLDisplaySurface class. It is called inside its destructor.
        void onDisplaySurfaceDestroy( GLDisplaySurface & pDisplaySurface ) noexcept;

        // Used by the GLRenderContext class. It is called inside its destructor.
        void onRenderContextDestroy( GLRenderContext & pRenderContext ) noexcept;

    private: // For implementation
        void _nativeConstructor();
        void _nativeDestructor() noexcept;
        void _nativeInitialize();
        void _nativeRelease();
        void _nativeInitializePlatform();
        void _nativeReleaseInitState();
        void _nativeCreateDisplaySurface( GLDisplaySurface & pDisplaySurface, const GLDisplaySurfaceCreateInfo & pCreateInfo );
        void _nativeCreateDisplaySurfaceForCurrentThread( GLDisplaySurface & pDisplaySurface );
        void _nativeDestroyDisplaySurface( GLDisplaySurface & pDisplaySurface );
        void _nativeCreateRenderContext( GLRenderContext & pRenderContext, const GLDisplaySurface & pSurface, const GLRenderContextCreateInfo & pCreateInfo );
        void _nativeCreateRenderContextForCurrentThread( GLRenderContext & pRenderContext );
        void _nativeDestroyRenderContext( GLRenderContext & pRenderContext );
        void _nativeResetContextBinding();
        bool _nativeIsRenderContextBound() const;
        bool _nativeIsRenderContextBound( const GLRenderContext & pRenderContext ) const;
        bool _nativeIsDisplaySurfaceValid( const GLDisplaySurface & pDisplaySurface ) const;
        bool _nativeIsRenderContextValid( const GLRenderContext & pRenderContext ) const;
    };

} // namespace ts3::system

#endif // __TS3_SYSTEM_OPENGL_H__
