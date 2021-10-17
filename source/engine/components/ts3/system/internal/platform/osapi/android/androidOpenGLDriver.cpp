
#include <ts3/system/openGLNative.h>
#include <ts3/system/windowNative.h>

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_ANDROID )
namespace ts3::system
{

    void _androidInitializeDriver( SysContext & pSysContext, OpenGLSystemDriverNativeData & pGLDriverNativeData );
    void _androidReleaseDriver( OpenGLSystemDriverNativeData & pGLDriverNativeData );


    void OpenGLSystemDriver::_nativeConstructor()
    {
        _nativeInitialize();
    }

    void OpenGLSystemDriver::_nativeDestructor() noexcept
    {
        _nativeRelease();
    }

    void OpenGLSystemDriver::_nativeInitialize()
    {
        _androidInitializeDriver( *mSysContext, mInternal->nativeDataPriv );
    }

    void OpenGLSystemDriver::_nativeRelease()
    {
        _androidReleaseDriver( mInternal->nativeDataPriv );
    }

    void OpenGLSystemDriver::_nativeInitializePlatform()
    {}

    void OpenGLSystemDriver::_nativeReleaseInitState()
    {}

    void OpenGLSystemDriver::_nativeCreateDisplaySurface( OpenGLDisplaySurface & pDisplaySurface, const GLDisplaySurfaceCreateInfo & pCreateInfo )
    {
        auto & aSessionData = nativeAndroidGetASessionData( *mSysContext );
        auto & driverNativeData = mInternal->nativeDataPriv;

        EGLConfig fbConfig = platform::eglChooseCoreFBConfig( driverNativeData.eDisplay, pCreateInfo.visualConfig, pCreateInfo.targetAPIVersion );

        // EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is guaranteed to be
        // accepted by ANativeWindow_setBuffersGeometry(). As soon as we retrieve an EGLConfig,
        // we can reconfigure the ANativeWindow buffers using the value of EGL_NATIVE_VISUAL_ID.
        EGLint fbConfigNativeVisualID = platform::eglQueryFBConfigAttribute( driverNativeData.eDisplay,
                                                                         fbConfig,
                                                                         EGL_NATIVE_VISUAL_ID );

        auto aNativeResult = ANativeWindow_setBuffersGeometry( driverNativeData.eNativeWindow,
                                                               0,
                                                               0,
                                                               fbConfigNativeVisualID );
        if( aNativeResult < 0 )
        {
            ts3Throw( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
        }

        platform::eglCreateSurface( pDisplaySurface.mInternal->nativeDataPriv,
                                driverNativeData.eDisplay,
                                driverNativeData.eNativeWindow,
                                fbConfig,
                                pCreateInfo.visualConfig );

        pDisplaySurface.mInternal->nativeDataPriv.setSessionData( aSessionData );
        pDisplaySurface.mInternal->nativeDataPriv.eNativeWindow = aSessionData.aNativeWindow;
    }

    void OpenGLSystemDriver::_nativeCreateDisplaySurfaceForCurrentThread( OpenGLDisplaySurface & pDisplaySurface )
    {
        auto & aSessionData = nativeAndroidGetASessionData( *mSysContext );

        platform::eglCreateSurfaceForCurrentThread( pDisplaySurface.mInternal->nativeDataPriv );

        pDisplaySurface.mInternal->nativeDataPriv.setSessionData( aSessionData );
        pDisplaySurface.mInternal->nativeDataPriv.eNativeWindow = aSessionData.aNativeWindow;
    }

    void OpenGLSystemDriver::_nativeDestroyDisplaySurface( OpenGLDisplaySurface & pDisplaySurface )
    {
        platform::eglDestroySurface( pDisplaySurface.mInternal->nativeDataPriv );
        pDisplaySurface.mInternal->nativeDataPriv.resetSessionData();
    }

    void OpenGLSystemDriver::_nativeCreateRenderContext( OpenGLRenderContext & pRenderContext,
                                                     const OpenGLDisplaySurface & pDisplaySurface,
                                                     const GLRenderContextCreateInfo & pCreateInfo )
    {
        auto & aSessionData = nativeAndroidGetASessionData( *mSysContext );

        platform::eglCreateCoreContext( pRenderContext.mInternal->nativeDataPriv,
                                    pDisplaySurface.mInternal->nativeDataPriv,
                                    pCreateInfo );

        pRenderContext.mInternal->nativeDataPriv.setSessionData( aSessionData );
    }

    void OpenGLSystemDriver::_nativeCreateRenderContextForCurrentThread( OpenGLRenderContext & pRenderContext )
    {
        auto & aSessionData = nativeAndroidGetASessionData( *mSysContext );

        platform::eglCreateCoreContextForCurrentThread( pRenderContext.mInternal->nativeDataPriv );

        pRenderContext.mInternal->nativeDataPriv.setSessionData( aSessionData );
    }

    void OpenGLSystemDriver::_nativeDestroyRenderContext( OpenGLRenderContext & pRenderContext )
    {
        platform::eglDestroyRenderContext( pRenderContext.mInternal->nativeDataPriv );
        pRenderContext.mInternal->nativeDataPriv.resetSessionData();
    }

    void OpenGLSystemDriver::_nativeResetContextBinding()
    {
        ::eglMakeCurrent( mInternal->nativeDataPriv.eDisplay,
                          EGL_NO_SURFACE,
                          EGL_NO_SURFACE,
                          EGL_NO_CONTEXT );
    }

    bool OpenGLSystemDriver::_nativeIsRenderContextBound() const
    {
        auto currentContext = ::eglGetCurrentContext();
        return currentContext != EGL_NO_CONTEXT;
    }

    bool OpenGLSystemDriver::_nativeIsRenderContextBound( const OpenGLRenderContext & pRenderContext ) const
    {
        auto currentContext = ::eglGetCurrentContext();
        return currentContext = pRenderContext.mInternal->nativeDataPriv.eContextHandle;
    }

    bool OpenGLSystemDriver::_nativeIsDisplaySurfaceValid( const OpenGLDisplaySurface & pDisplaySurface ) const
    {
        return pDisplaySurface.mInternal->nativeDataPriv.eSurfaceHandle != EGL_NO_SURFACE;
    }

    bool OpenGLSystemDriver::_nativeIsRenderContextValid( const OpenGLRenderContext & pRenderContext ) const
    {
        return pRenderContext.mInternal->nativeDataPriv.eContextHandle != EGL_NO_CONTEXT;
    }


    void OpenGLDisplaySurface::_nativeSwapBuffers()
    {
        auto & surfaceNativeData = mInternal->nativeDataPriv;
        ::eglSwapBuffers( surfaceNativeData.eDisplay, surfaceNativeData.eSurfaceHandle );
    }

    void OpenGLDisplaySurface::_nativeQueryRenderAreaSize( WindowSize & pOutSize ) const
    {
        auto & surfaceNativeData = mInternal->nativeDataPriv;
        pOutSize = nativeAndroidQueryNativeWindowSize( surfaceNativeData.eNativeWindow );
    }


    void OpenGLRenderContext::_nativeBindForCurrentThread( const OpenGLDisplaySurface & pTargetSurface )
    {
        platform::eglBindContextForCurrentThread( mInternal->nativeDataPriv, pTargetSurface.mInternal->nativeDataPriv );
    }


    void _androidInitializeDriver( SysContext & pSysContext, OpenGLSystemDriverNativeData & pGLDriverNativeData )
    {
        auto & aSessionData = nativeAndroidGetASessionData( pSysContext );

        pGLDriverNativeData.setSessionData( aSessionData );
        pGLDriverNativeData.eNativeWindow = aSessionData.aNativeWindow;

        platform::eglInitializeGLDriver( pGLDriverNativeData );
    }

    void _androidReleaseDriver( OpenGLSystemDriverNativeData & pGLDriverNativeData )
    {
        platform::eglReleaseGLDriver( pGLDriverNativeData );

        pGLDriverNativeData.eNativeWindow = nullptr;
        pGLDriverNativeData.resetSessionData();
    }

} // namespace ts3::system
#endif // TS3_PCL_TARGET_SYSAPI_ANDROID
