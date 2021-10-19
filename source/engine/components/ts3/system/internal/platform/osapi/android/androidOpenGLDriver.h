
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_ANDROID_OPENGL_DRIVER_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_ANDROID_OPENGL_DRIVER_H__

#include "androidCommon.h"
#include <ts3/system/openGLDriver.h>
#include <ts3/system/internal/platform/shared/egl/eglOpenGL.h>
#include <GLES3/gl31.h>

namespace ts3::system
{

    ts3SysDeclareHandle( AndroidDisplayManager );
    ts3SysDeclareHandle( AndroidOpenGLSystemDriver );
    ts3SysDeclareHandle( AndroidOpenGLRenderContext );
    ts3SysDeclareHandle( AndroidOpenGLRenderContext );

    namespace platform
    {

        struct AndroidOpenGLDisplaySurfaceNativeData :
                public EGLDisplaySurfaceNativeData,
                public AndroidNativeDataCommon
        {
        };

        struct AndroidOpenGLRenderContextNativeData :
                public EGLRenderContextNativeData,
                public AndroidNativeDataCommon
        {
        };

        struct AndroidOpenGLSystemDriverNativeData :
                public EGLDriverNativeData,
                public AndroidNativeDataCommon
        {
        };

    }

    /// @brief Android-specific implementation of the OpenGLSystemDriver class.
    class AndroidOpenGLSystemDriver : public AndroidNativeObject<OpenGLSystemDriver, platform::AndroidOpenGLSystemDriverNativeData>
    {
    public:
        AndroidOpenGLSystemDriver( AndroidDisplayManagerHandle pDisplayManager );
        virtual ~AndroidOpenGLSystemDriver() noexcept;

    private:
        void _initializeAndroidDriverState();

        void _releaseAndroidDriverState();

        /// @override OpenGLSystemDriver::_nativeInitializePlatform
        virtual void _nativeInitializePlatform() override final;

        /// @override OpenGLSystemDriver::_nativeReleaseInitState
        virtual void _nativeReleaseInitState() noexcept override final;

        /// @override OpenGLSystemDriver::_nativeCreateDisplaySurface
        virtual OpenGLDisplaySurfaceHandle _nativeCreateDisplaySurface( const GLDisplaySurfaceCreateInfo & pCreateInfo ) override final;

        /// @override OpenGLSystemDriver::_nativeCreateDisplaySurfaceForCurrentThread
        virtual OpenGLDisplaySurfaceHandle _nativeCreateDisplaySurfaceForCurrentThread() override final;

        /// @override OpenGLSystemDriver::_nativeDestroyDisplaySurface
        virtual void _nativeDestroyDisplaySurface( OpenGLDisplaySurface & pDisplaySurface ) override final;

        /// @override OpenGLSystemDriver::_nativeCreateRenderContext
        virtual OpenGLRenderContextHandle _nativeCreateRenderContext( OpenGLDisplaySurface & pDisplaySurface,
                                                                      const GLRenderContextCreateInfo & pCreateInfo ) override final;

        /// @override OpenGLSystemDriver::_nativeCreateRenderContextForCurrentThread
        virtual OpenGLRenderContextHandle _nativeCreateRenderContextForCurrentThread() override final;

        /// @override OpenGLSystemDriver::_nativeDestroyDisplaySurface
        virtual void _nativeDestroyRenderContext( OpenGLRenderContext & pRenderContext ) override final;

        /// @override OpenGLSystemDriver::_nativeResetContextBinding
        virtual void _nativeResetContextBinding() override final;

        /// @override OpenGLSystemDriver::_nativeQuerySupportedDepthStencilFormats
        virtual std::vector<EDepthStencilFormat> _nativeQuerySupportedDepthStencilFormats( EColorFormat pColorFormat ) const override final;

        /// @override OpenGLSystemDriver::_nativeQuerySupportedMSAAModes
        virtual std::vector<EMSAAMode> _nativeQuerySupportedMSAAModes( EColorFormat pColorFormat,
                                                                       EDepthStencilFormat pDepthStencilFormat ) const override final;

        /// @override OpenGLSystemDriver::_nativeIssGLAPIProfileSupported
        virtual bool _nativeIsGLAPIProfileSupported( EGLAPIProfile pGLAPIProfile ) const override final;;

        /// @override OpenGLSystemDriver::_nativeIsRenderContextBound
        virtual bool _nativeIsRenderContextBound() const override final;
    };

    /// @brief Android-specific implementation of the OpenGLDisplaySurface class.
    class AndroidOpenGLDisplaySurface : public AndroidNativeObject<OpenGLDisplaySurface, platform::AndroidOpenGLDisplaySurfaceNativeData>
    {
    public:
        explicit AndroidOpenGLDisplaySurface( AndroidOpenGLSystemDriverHandle pGLSystemDriver );
        virtual ~AndroidOpenGLDisplaySurface() noexcept;

    private:
        void _releaseAndroidSurfaceState();

        /// @override OpenGLDisplaySurface::_nativeSwapBuffers
        virtual void _nativeSwapBuffers() override final;

        /// @override OpenGLDisplaySurface::_nativeQueryRenderAreaSize
        virtual FrameSize _nativeQueryRenderAreaSize() const override final;

        /// @override OpenGLDisplaySurface::_nativeIsValid
        virtual bool _nativeIsValid() const override final;

        /// @override OpenGLDisplaySurface::_nativeResize
        virtual void _nativeResize( const FrameSize & pFrameSize, EFrameSizeMode pSizeMode ) override final;

        /// @override OpenGLDisplaySurface::_nativeSetFullscreenMode
        virtual void _nativeSetFullscreenMode( bool pEnable ) override final;

        /// @override OpenGLDisplaySurface::_nativeSetTitle
        virtual void _nativeSetTitle( const std::string & pTitle ) override final;

        /// @override OpenGLDisplaySurface::_nativeUpdateGeometry
        virtual void _nativeUpdateGeometry( const FrameGeometry & pFrameGeometry,
                                            Bitmask<EFrameGeometryUpdateFlags> pUpdateFlags ) override final;

        /// @override OpenGLDisplaySurface::_nativeGetSize
        virtual FrameSize _nativeGetSize( EFrameSizeMode pSizeMode ) const override final;
    };

    /// @brief Android-specific implementation of the OpenGLRenderContext class.
    class AndroidOpenGLRenderContext : public AndroidNativeObject<OpenGLRenderContext, platform::AndroidOpenGLRenderContextNativeData>
    {
    public:
        explicit AndroidOpenGLRenderContext( AndroidOpenGLSystemDriverHandle pGLSystemDriver );
        virtual ~AndroidOpenGLRenderContext() noexcept;

    private:
        void _releaseAndroidContextState();

        /// @override OpenGLRenderContext::_nativeBindForCurrentThread
        virtual void _nativeBindForCurrentThread( const OpenGLDisplaySurface & pTargetSurface ) override final;

        /// @override OpenGLRenderContext::_nativeIsCurrent
        virtual bool _nativeIsCurrent() const override final;

        /// @override OpenGLRenderContext::_nativeIsValid
        virtual bool _nativeIsValid() const override final;
    };

} // namespace ts3::system

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_ANDROID_OPENGL_DRIVER_H__
