
#ifndef __TS3_SYSTEM_OPENGL_SYSTEM_DRIVER_H__
#define __TS3_SYSTEM_OPENGL_SYSTEM_DRIVER_H__

#include "openGLCommon.h"

namespace ts3::system
{

    ts3SysDeclareHandle( DisplayManager );
    ts3SysDeclareHandle( OpenGLDisplaySurface );
    ts3SysDeclareHandle( OpenGLRenderContext );

    struct GLRuntimeVersionDesc
    {
        EGLAPIProfile apiProfile = EGLAPIProfile::OpenGL;

        Version apiVersion = CX_GL_VERSION_BEST_SUPPORTED;
    };

    /// @brief Contains parameters used to specify how a GL display surface should be created.
    struct GLDisplaySurfaceCreateInfo
    {
        //
        FrameGeometry frameGeometry;
        //
        std::string title;
        //
        VisualConfig visualConfig;
        //
        GLRuntimeVersionDesc runtimeVersionDesc;
        //
        Bitmask<EGLSurfaceCreateFlags> flags = 0;
    };

    /// @brief
    struct GLRenderContextCreateInfo
    {
        //
        OpenGLRenderContextHandle shareContext = nullptr;
        // Target API version (minimum) a context must support.
        GLRuntimeVersionDesc runtimeVersionDesc;
        // Target API profile which will be used by the app.
        EGLContextProfile contextProfile = EGLContextProfile::Auto;
        //
        Bitmask<EGLRenderContextCreateFlags> flags = 0;
    };

    /// @brief
    class OpenGLSystemDriver : public SysObject
    {
        friend class OpenGLDisplaySurface;
        friend class OpenGLRenderContext;

    public:
        DisplayManagerHandle const mDisplayManager;

    public:
        virtual ~OpenGLSystemDriver() noexcept;

        /// @brief Initializes core OpenGL state and system-level interfaces.
        /// This method also creates any additionally required
        void initializePlatform();

        /// @brief Releases temporary init state created automatically by the library. Context must be a valid GL context.
        /// Call this method after you have created actual display surface and context. It's not mandatory, (everything
        /// will be released at shutdown anyway), but on some platforms this could free some extra memory and release
        /// couple system-level interfaces.
        void releaseInitState( OpenGLRenderContext & pRenderContext );

        /// @brief Creates a system OpenGL surface (usually - a window) with a configuration matching specified requirements.
        /// @param pCreateInfo CreateInfo struct with a surface specification (VisualConfig, window geometry, etc.)
        TS3_FUNC_NO_DISCARD OpenGLDisplaySurfaceHandle createDisplaySurface( const GLDisplaySurfaceCreateInfo & pCreateInfo );

        /// @brief Creates a OpenGLDisplaySurface object, that wraps currently bound surface for the current thread.
        /// @param pTargetSurface Existing surface object to be used. If null, a new OpenGLDisplaySurface will be created.
        ///
        /// This function creates a logical surface object without creating an actual (system-level) surface. It enables
        /// usage of the library API in environments where low-level GL stuff is created externally (a good example is
        /// the editor, where wxWidgets manages underlying GL surface/context). Important note: on some environments there
        /// might be no way to obtain all data required to create a wrapper around surface. If that is the case, this
        /// method will throw an exception with EXC_NOT_SUPPORTED code.
        /// @note
        /// >> The surface specified as pTargetSurface is just a hint and may be discarded. Always use returned handle!
        /// @note
        /// >> If no surface is currently bound, null handle is returned, regardless of the value of pTargetSurface.
        TS3_FUNC_NO_DISCARD OpenGLDisplaySurfaceHandle createDisplaySurfaceForCurrentThread();

        /// @brief Creates a system OpenGL render context with a configuration matching specified requirements.
        /// @param pSurface Surface to be used for context creation. Context can be bound to any surface compatible with this one.
        /// @param pCreateInfo CreateInfo struct with a context specification (OpenGL API version, profile, etc.)
        TS3_FUNC_NO_DISCARD OpenGLRenderContextHandle createRenderContext( OpenGLDisplaySurface & pSurface,
                                                                           const GLRenderContextCreateInfo & pCreateInfo );

        /// @brief Creates a OpenGLRenderContext object, that wraps currently bound context for the current thread.
        /// @param pTargetContext Existing context object to be used. If null, a new OpenGLRenderContext will be created.
        /// This functions works similar to createDisplaySurfaceForCurrentThread(), but does that for the currently
        /// bound OpenGL render context. See description of createDisplaySurfaceForCurrentThread() for details.
        /// @note
        /// >> The context specified as pTargetContext is just a hint and may be discarded. Always use returned handle!
        /// @note
        /// >> If no context is currently bound, null handle is returned, regardless of the value of pTargetContext.
        TS3_FUNC_NO_DISCARD OpenGLRenderContextHandle createRenderContextForCurrentThread();

        /// @brief
        void resetContextBinding();

        /// @brief
        TS3_FUNC_NO_DISCARD std::vector<EDepthStencilFormat> querySupportedDepthStencilFormats( EColorFormat pColorFormat ) const;

        /// @brief
        TS3_FUNC_NO_DISCARD std::vector<EMSAAMode> querySupportedMSAAModes( EColorFormat pColorFormat,
                                                                            EDepthStencilFormat pDepthStencilFormat ) const;

        ///
        TS3_FUNC_NO_DISCARD bool isGLAPIProfileSupported( EGLAPIProfile pGLAPIProfile ) const;

        /// @brief
        TS3_FUNC_NO_DISCARD bool isRenderContextBound() const;

    protected:
        explicit OpenGLSystemDriver( DisplayManagerHandle pDisplayManager );

    friendapi:
        // Used by the OpenGLDisplaySurface class. It is called inside its destructor.
        virtual void onDisplaySurfaceDestroy( OpenGLDisplaySurface & pDisplaySurface ) noexcept;

        // Used by the OpenGLRenderContext class. It is called inside its destructor.
        virtual void onRenderContextDestroy( OpenGLRenderContext & pRenderContext ) noexcept;

    private:
        virtual void _nativeInitializePlatform() = 0;

        virtual void _nativeReleaseInitState() noexcept = 0;

        virtual OpenGLDisplaySurfaceHandle _nativeCreateDisplaySurface( const GLDisplaySurfaceCreateInfo & pCreateInfo ) = 0;

        virtual OpenGLDisplaySurfaceHandle _nativeCreateDisplaySurfaceForCurrentThread() = 0;

        virtual void _nativeDestroyDisplaySurface( OpenGLDisplaySurface & pDisplaySurface ) = 0;

        virtual OpenGLRenderContextHandle _nativeCreateRenderContext( OpenGLDisplaySurface & pSurface,
                                                                      const GLRenderContextCreateInfo & pCreateInfo ) = 0;

        virtual OpenGLRenderContextHandle _nativeCreateRenderContextForCurrentThread() = 0;

        virtual void _nativeDestroyRenderContext( OpenGLRenderContext & pRenderContext ) = 0;

        virtual void _nativeResetContextBinding() = 0;

        virtual std::vector<EDepthStencilFormat> _nativeQuerySupportedDepthStencilFormats( EColorFormat pColorFormat ) const = 0;

        virtual std::vector<EMSAAMode> _nativeQuerySupportedMSAAModes( EColorFormat pColorFormat,
                                                                       EDepthStencilFormat pDepthStencilFormat ) const = 0;

        virtual bool _nativeIsGLAPIProfileSupported( EGLAPIProfile pGLAPIProfile ) const = 0;

        virtual bool _nativeIsRenderContextBound() const = 0;

    private:
        struct OpenGLSystemDriverPrivateData;
        std::unique_ptr<OpenGLSystemDriverPrivateData> _privateData;
    };

    /// @brief
    class OpenGLDisplaySurface : public Frame
    {
        friend class OpenGLSystemDriver;

    public:
        OpenGLSystemDriverHandle const mGLSystemDriver;

    public:
        virtual ~OpenGLDisplaySurface() noexcept;

        /// @brief
        void swapBuffers();

        /// @brief
        TS3_FUNC_NO_DISCARD FrameSize queryRenderAreaSize() const;

        /// @brief
        TS3_FUNC_NO_DISCARD bool isValid() const;

        /// @override Frame::resize
        virtual void resize( const FrameSize & pSize ) override final;

        /// @override Frame::resizeClientArea
        virtual void resizeClientArea( const FrameSize & pSize ) override final;

        /// @override Frame::setFullscreenMode
        virtual void setFullscreenMode( bool pEnable ) override final;

        /// @override Frame::setTitle
        virtual void setTitle( const std::string & pTitleText ) override final;

        /// @override Frame::updateGeometry
        virtual void updateGeometry( const FrameGeometry & pFrameGeometry,
                                     Bitmask<EFrameGeometryUpdateFlags> pUpdateFlags ) override final;

        /// @override Frame::getClientAreaSize
        TS3_FUNC_NO_DISCARD virtual FrameSize getClientAreaSize() const override final;

        /// @override Frame::getSize
        TS3_FUNC_NO_DISCARD virtual FrameSize getSize() const override final;

        /// @override Frame::isFullscreen
        TS3_FUNC_NO_DISCARD virtual bool isFullscreen() const override final;

    protected:
        explicit OpenGLDisplaySurface( OpenGLSystemDriverHandle pGLSystemDriver, void * pNativeData );

        void setInternalOwnershipFlag( bool pOwnershipFlag );

        bool hasInternalOwnershipFlag() const;

    private:
        virtual void _nativeSwapBuffers() = 0;

        virtual FrameSize _nativeQueryRenderAreaSize() const = 0;

        virtual bool _nativeIsValid() const = 0;

        virtual void _nativeResize( const FrameSize & pFrameSize, EFrameSizeMode pSizeMode ) = 0;

        virtual void _nativeSetFullscreenMode( bool pEnable ) = 0;

        virtual void _nativeSetTitle( const std::string & pTitle ) = 0;

        virtual void _nativeUpdateGeometry( const FrameGeometry & pFrameGeometry,
                                            Bitmask<EFrameGeometryUpdateFlags> pUpdateFlags ) = 0;

        virtual FrameSize _nativeGetSize( EFrameSizeMode pSizeMode ) const = 0;

    private:
        struct OpenGLDisplaySurfacePrivateData;
        std::unique_ptr<OpenGLDisplaySurfacePrivateData> _privateData;
    };

    /// @brief
    class OpenGLRenderContext : public SysObject
    {
        friend class OpenGLSystemDriver;

    public:
        OpenGLSystemDriverHandle const mGLSystemDriver;

    public:
        virtual ~OpenGLRenderContext() noexcept;

        /// @brief
        void bindForCurrentThread( const OpenGLDisplaySurface & pTargetSurface );

        /// @brief
        TS3_FUNC_NO_DISCARD bool isCurrent() const;

        /// @brief
        TS3_FUNC_NO_DISCARD bool isValid() const;

        /// @brief
        TS3_FUNC_NO_DISCARD GLSystemVersionInfo querySystemVersionInfo() const;

    protected:
        explicit OpenGLRenderContext( OpenGLSystemDriverHandle pGLSystemDriver );

        void setInternalOwnershipFlag( bool pOwnershipFlag );

        bool hasInternalOwnershipFlag() const;

    private:
        virtual void _nativeBindForCurrentThread( const OpenGLDisplaySurface & pTargetSurface ) = 0;

        virtual bool _nativeIsCurrent() const = 0;

        virtual bool _nativeIsValid() const = 0;

    private:
        struct OpenGLRenderContextPrivateData;
        std::unique_ptr<OpenGLRenderContextPrivateData> _privateData;
    };

} // namespace ts3::system

#endif // __TS3_SYSTEM_OPENGL_SYSTEM_DRIVER_H__