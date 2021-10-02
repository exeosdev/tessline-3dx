
#include <ts3/system/openGLNative.h>
#include <ts3/system/windowNative.h>

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_ANDROID )
namespace ts3::system
{

    void _androidInitializeDriver( SysContext & pSysContext, GLSystemDriverNativeData & pGLDriverNativeData );
    void _androidReleaseDriver( GLSystemDriverNativeData & pGLDriverNativeData );


    void GLSystemDriver::_nativeConstructor()
    {
        _nativeInitialize();
    }

    void GLSystemDriver::_nativeDestructor() noexcept
    {
        _nativeRelease();
    }

    void GLSystemDriver::_nativeInitialize()
    {
        _androidInitializeDriver( *mSysContext, mInternal->nativeDataPriv );
    }

    void GLSystemDriver::_nativeRelease()
    {
        _androidReleaseDriver( mInternal->nativeDataPriv );
    }

    void GLSystemDriver::_nativeInitializePlatform()
    {}

    void GLSystemDriver::_nativeReleaseInitState()
    {}

    void GLSystemDriver::_nativeCreateDisplaySurface( GLDisplaySurface & pDisplaySurface, const GLDisplaySurfaceCreateInfo & pCreateInfo )
    {
        auto & aSessionData = nativeAndroidGetASessionData( *mSysContext );
        auto & driverNativeData = mInternal->nativeDataPriv;

        EGLConfig fbConfig = nativeEGLChooseCoreFBConfig( driverNativeData.eDisplay, pCreateInfo.visualConfig, pCreateInfo.targetAPIVersion );

        // EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is guaranteed to be
        // accepted by ANativeWindow_setBuffersGeometry(). As soon as we retrieve an EGLConfig,
        // we can reconfigure the ANativeWindow buffers using the value of EGL_NATIVE_VISUAL_ID.
        EGLint fbConfigNativeVisualID = nativeEGLQueryFBConfigAttribute( driverNativeData.eDisplay,
                                                                         fbConfig,
                                                                         EGL_NATIVE_VISUAL_ID );

        auto aNativeResult = ANativeWindow_setBuffersGeometry( driverNativeData.eNativeWindow,
                                                               0,
                                                               0,
                                                               fbConfigNativeVisualID );
        if( aNativeResult < 0 )
        {
            ts3ThrowAuto( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
        }

        nativeEGLCreateSurface( pDisplaySurface.mInternal->nativeDataPriv,
                                driverNativeData.eDisplay,
                                driverNativeData.eNativeWindow,
                                fbConfig,
                                pCreateInfo.visualConfig );

        pDisplaySurface.mInternal->nativeDataPriv.setSessionData( aSessionData );
        pDisplaySurface.mInternal->nativeDataPriv.eNativeWindow = aSessionData.aNativeWindow;
    }

    void GLSystemDriver::_nativeCreateDisplaySurfaceForCurrentThread( GLDisplaySurface & pDisplaySurface )
    {
        auto & aSessionData = nativeAndroidGetASessionData( *mSysContext );

        nativeEGLCreateSurfaceForCurrentThread( pDisplaySurface.mInternal->nativeDataPriv );

        pDisplaySurface.mInternal->nativeDataPriv.setSessionData( aSessionData );
        pDisplaySurface.mInternal->nativeDataPriv.eNativeWindow = aSessionData.aNativeWindow;
    }

    void GLSystemDriver::_nativeDestroyDisplaySurface( GLDisplaySurface & pDisplaySurface )
    {
        nativeEGLDestroySurface( pDisplaySurface.mInternal->nativeDataPriv );
        pDisplaySurface.mInternal->nativeDataPriv.resetSessionData();
    }

    void GLSystemDriver::_nativeCreateRenderContext( GLRenderContext & pRenderContext,
                                                     const GLDisplaySurface & pDisplaySurface,
                                                     const GLRenderContextCreateInfo & pCreateInfo )
    {
        auto & aSessionData = nativeAndroidGetASessionData( *mSysContext );

        nativeEGLCreateCoreContext( pRenderContext.mInternal->nativeDataPriv,
                                    pDisplaySurface.mInternal->nativeDataPriv,
                                    pCreateInfo );

        pRenderContext.mInternal->nativeDataPriv.setSessionData( aSessionData );
    }

    void GLSystemDriver::_nativeCreateRenderContextForCurrentThread( GLRenderContext & pRenderContext )
    {
        auto & aSessionData = nativeAndroidGetASessionData( *mSysContext );

        nativeEGLCreateCoreContextForCurrentThread( pRenderContext.mInternal->nativeDataPriv );

        pRenderContext.mInternal->nativeDataPriv.setSessionData( aSessionData );
    }

    void GLSystemDriver::_nativeDestroyRenderContext( GLRenderContext & pRenderContext )
    {
        nativeEGLDestroyRenderContext( pRenderContext.mInternal->nativeDataPriv );
        pRenderContext.mInternal->nativeDataPriv.resetSessionData();
    }

    void GLSystemDriver::_nativeResetContextBinding()
    {
        ::eglMakeCurrent( mInternal->nativeDataPriv.eDisplay,
                          EGL_NO_SURFACE,
                          EGL_NO_SURFACE,
                          EGL_NO_CONTEXT );
    }

    bool GLSystemDriver::_nativeIsRenderContextBound() const
    {
        auto currentContext = ::eglGetCurrentContext();
        return currentContext != EGL_NO_CONTEXT;
    }

    bool GLSystemDriver::_nativeIsRenderContextBound( const GLRenderContext & pRenderContext ) const
    {
        auto currentContext = ::eglGetCurrentContext();
        return currentContext = pRenderContext.mInternal->nativeDataPriv.eContextHandle;
    }

    bool GLSystemDriver::_nativeIsDisplaySurfaceValid( const GLDisplaySurface & pDisplaySurface ) const
    {
        return pDisplaySurface.mInternal->nativeDataPriv.eSurfaceHandle != EGL_NO_SURFACE;
    }

    bool GLSystemDriver::_nativeIsRenderContextValid( const GLRenderContext & pRenderContext ) const
    {
        return pRenderContext.mInternal->nativeDataPriv.eContextHandle != EGL_NO_CONTEXT;
    }


    void GLDisplaySurface::_nativeSwapBuffers()
    {
        auto & surfaceNativeData = mInternal->nativeDataPriv;
        ::eglSwapBuffers( surfaceNativeData.eDisplay, surfaceNativeData.eSurfaceHandle );
    }

    void GLDisplaySurface::_nativeQueryRenderAreaSize( WindowSize & pOutSize ) const
    {
        auto & surfaceNativeData = mInternal->nativeDataPriv;
        pOutSize = nativeAndroidQueryNativeWindowSize( surfaceNativeData.eNativeWindow );
    }


    void GLRenderContext::_nativeBindForCurrentThread( const GLDisplaySurface & pTargetSurface )
    {
        nativeEGLBindContextForCurrentThread( mInternal->nativeDataPriv, pTargetSurface.mInternal->nativeDataPriv );
    }


    void _androidInitializeDriver( SysContext & pSysContext, GLSystemDriverNativeData & pGLDriverNativeData )
    {
        auto & aSessionData = nativeAndroidGetASessionData( pSysContext );

        pGLDriverNativeData.setSessionData( aSessionData );
        pGLDriverNativeData.eNativeWindow = aSessionData.aNativeWindow;

        nativeEGLInitializeGLDriver( pGLDriverNativeData );
    }

    void _androidReleaseDriver( GLSystemDriverNativeData & pGLDriverNativeData )
    {
        nativeEGLReleaseGLDriver( pGLDriverNativeData );

        pGLDriverNativeData.eNativeWindow = nullptr;
        pGLDriverNativeData.resetSessionData();
    }

} // namespace ts3::system
#endif // TS3_PCL_TARGET_SYSAPI_ANDROID
