
#include "openGLNative.h"
#include "displayNative.h"

namespace ts3::system
{

    GLDisplaySurface::GLDisplaySurface( GLSystemDriverHandle pDriver )
    : EventSource( pDriver->mSysContext )
    , mDriver( pDriver )
    , mInternal( std::make_unique<ObjectInternalData>( this ) )
    , mNativeData( &( mInternal->nativeDataPriv ) )
    {
        setEventSourceNativeData( &( mInternal->nativeDataPriv ) );
    }

    GLDisplaySurface::~GLDisplaySurface() noexcept
    {
        mDriver->onDisplaySurfaceDestroy( *this );
    }

    void GLDisplaySurface::clearColorBuffer()
    {
        glClearColor( 0.1f, 0.33f, 0.9f, 1.0f );
        //ts3GLHandleLastError();

        glClear( GL_COLOR_BUFFER_BIT );
        //ts3GLHandleLastError();
    }

    void GLDisplaySurface::swapBuffers()
    {
        if( !isValid() )
        {
            ts3ThrowAuto( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
        }

        _nativeSwapBuffers();
    }

    WindowSize GLDisplaySurface::queryRenderAreaSize() const
    {
        if( !isValid() )
        {
            ts3ThrowAuto( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
        }

        WindowSize result;
        _nativeQueryRenderAreaSize( result );

        return result;
    }

    bool GLDisplaySurface::isValid() const
    {
        return mDriver->isDisplaySurfaceValid( *this );
    }


    GLRenderContext::GLRenderContext( GLSystemDriverHandle pDriver )
    : SysObject( pDriver->mSysContext )
    , mDriver( pDriver )
    , mInternal( std::make_unique<ObjectInternalData>( this ) )
    , mNativeData( &( mInternal->nativeDataPriv ) )
    {}

    GLRenderContext::~GLRenderContext() noexcept
    {
        mDriver->onRenderContextDestroy( *this );
    }

    void GLRenderContext::bindForCurrentThread( const GLDisplaySurface & pSurface )
    {
        if( !isValid() )
        {
            ts3ThrowAuto( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
        }
        _nativeBindForCurrentThread( pSurface );
    }

    GLSystemVersionInfo GLRenderContext::querySystemVersionInfo() const
    {
        GLSystemVersionInfo systemVersionInfo;

        systemVersionInfo.apiVersion = GLCoreAPI::queryRuntimeVersion();

        if ( const auto * versionStr = glGetString( GL_VERSION ) )
        {
            systemVersionInfo.apiVersionStr.assign( reinterpret_cast<const char *>( versionStr ) );
        }
        if ( const auto * glslVersionStr = glGetString( GL_SHADING_LANGUAGE_VERSION ) )
        {
            systemVersionInfo.glslVersionStr.assign( reinterpret_cast<const char *>( glslVersionStr ) );
        }
        if ( const auto * rendererNameStr = glGetString( GL_RENDERER ) )
        {
            systemVersionInfo.rendererName.assign( reinterpret_cast<const char *>( rendererNameStr ) );
        }
        if ( const auto * vendorNameStr = glGetString( GL_VENDOR ) )
        {
            systemVersionInfo.vendorName.assign( reinterpret_cast<const char *>( vendorNameStr ) );
        }

        return systemVersionInfo;
    }

    bool GLRenderContext::isCurrent() const
    {
        return mDriver->isRenderContextBound( *this );
    }

    bool GLRenderContext::isValid() const
    {
        return mDriver->isRenderContextValid( *this );
    }


    GLSystemDriver::GLSystemDriver( DisplayManagerHandle pDisplayManager )
    : SysObject( pDisplayManager->mSysContext )
    , mDisplayManager( std::move( pDisplayManager ) )
    , mInternal( std::make_unique<ObjectInternalData>( this ) )
    , mNativeData( &( mInternal->nativeDataPriv ) )
    {
        _nativeCtor();
    }

    GLSystemDriver::~GLSystemDriver()
    {
        _nativeReleaseInitState();
        _nativeDtor();
    }

    void GLSystemDriver::initializePlatform()
    {
        _nativeInitializePlatform();

        mInternal->supportedRuntimeVersion = GLCoreAPI::queryRuntimeVersion();

        if( mInternal->supportedRuntimeVersion.major == 0 )
        {
            mInternal->supportedRuntimeVersion.major = 1;
            mInternal->supportedRuntimeVersion.minor = 0;
        }
    }

    void GLSystemDriver::releaseInitState( GLRenderContext & /* pGLRenderContext */ )
    {
        _nativeReleaseInitState();
    }

    GLDisplaySurfaceHandle GLSystemDriver::createDisplaySurface( const GLDisplaySurfaceCreateInfo & pCreateInfo )
    {
        GLDisplaySurfaceCreateInfo ctxCreateInfo = pCreateInfo;

        if( pCreateInfo.flags.isSet( E_GL_DISPLAY_SURFACE_CREATE_FLAG_FULLSCREEN_BIT ) )
        {
            ctxCreateInfo.windowGeometry.size = cvWindowSizeMax;
            ctxCreateInfo.windowGeometry.frameStyle = WindowFrameStyle::Overlay;
        }
        else
        {
            ctxCreateInfo.windowGeometry.position = pCreateInfo.windowGeometry.position;
            ctxCreateInfo.windowGeometry.size = pCreateInfo.windowGeometry.size;
            ctxCreateInfo.windowGeometry.frameStyle = pCreateInfo.windowGeometry.frameStyle;
        }

        ctxCreateInfo.windowGeometry = mDisplayManager->validateWindowGeometry( ctxCreateInfo.windowGeometry );

        auto displaySurface = createSysObject<GLDisplaySurface>( getHandle<GLSystemDriver>() );
        displaySurface->mInternal->internalOwnershipFlag = true;

        _nativeCreateDisplaySurface( *displaySurface, ctxCreateInfo );

        return displaySurface;
    }

    GLDisplaySurfaceHandle GLSystemDriver::createDisplaySurfaceForCurrentThread( GLDisplaySurfaceHandle pTargetSurface )
    {
        // 'pTargetSurface' is an optional, existing surface object that should be used as a target.
        // It is only a hint - if it's not suitable and/or valid, a new one is created and used.

        if( pTargetSurface )
        {
            if( !isDisplaySurfaceValid( *pTargetSurface ) )
            {
                // If the surface is not valid, just discard the object and clear the handle.
                // TODO: should there be any extra check here? Anything to destroy? (OS-level)

                pTargetSurface = nullptr;
            }

            if( pTargetSurface->isValid() && pTargetSurface->mInternal->internalOwnershipFlag )
            {
                // If the surface is valid, we need to destroy it and ensure couple things are handled.
                // destroyDisplaySurface() will also:
                // - remove the surface from the driver's list (we will register it again after it's re-created)
                // - unregister it from the event controller if it's an active event emitter

                _nativeDestroyDisplaySurface( *pTargetSurface );
            }
        }

        if( !pTargetSurface )
        {
            pTargetSurface = createSysObject<GLDisplaySurface>( getHandle<GLSystemDriver>() );
        }

        pTargetSurface->mInternal->internalOwnershipFlag = false;

        _nativeCreateDisplaySurfaceForCurrentThread( *pTargetSurface );

        return pTargetSurface;
    }

    GLRenderContextHandle GLSystemDriver::createRenderContext( GLDisplaySurface & pSurface,
                                                               const GLRenderContextCreateInfo & pCreateInfo )
    {
        GLRenderContextCreateInfo ctxCreateInfo = pCreateInfo;

        if( ctxCreateInfo.requiredAPIVersion == cvVersionUnknown  )
        {
            ctxCreateInfo.requiredAPIVersion = Version{ 1, 0 };
        }
        else if( ctxCreateInfo.requiredAPIVersion == cvGLVersionBestSupported )
        {
            ctxCreateInfo.requiredAPIVersion = mInternal->supportedRuntimeVersion;
        }

        if( ctxCreateInfo.targetAPIProfile == EGLAPIProfile::GLES )
        {
            if( ctxCreateInfo.requiredAPIVersion > cvGLVersionMaxES )
            {
                return nullptr;
            }
        }
        else
        {
            if( ctxCreateInfo.requiredAPIVersion > cvGLVersionMaxDesktop )
            {
                return nullptr;
            }
        }
        
        if( ctxCreateInfo.requiredAPIVersion == Version{ 3, 1 } )
        {
            if( ( ctxCreateInfo.targetAPIProfile == EGLAPIProfile::Auto ) || ( ctxCreateInfo.targetAPIProfile == EGLAPIProfile::Core ) )
            {
                ctxCreateInfo.flags.set( E_GL_RENDER_CONTEXT_CREATE_FLAG_FORWARD_COMPATIBLE_BIT );
            }
            else
            {
                ctxCreateInfo.flags.unset( E_GL_RENDER_CONTEXT_CREATE_FLAG_FORWARD_COMPATIBLE_BIT );
            }
        }

        if( ctxCreateInfo.requiredAPIVersion >= Version{ 3, 2 } )
        {
            if( ctxCreateInfo.targetAPIProfile == EGLAPIProfile::Auto )
            {
                ctxCreateInfo.targetAPIProfile = EGLAPIProfile::Core;
            }
        }

        auto renderContext = createSysObject<GLRenderContext>( getHandle<GLSystemDriver>() );
        renderContext->mInternal->internalOwnershipFlag = true;

        _nativeCreateRenderContext( *renderContext, pSurface, ctxCreateInfo );

        return renderContext;
    }

    GLRenderContextHandle GLSystemDriver::createRenderContextForCurrentThread( GLRenderContextHandle pTargetContext )
    {
        if( pTargetContext )
        {
            if( !isRenderContextValid( *pTargetContext ) )
            {
                pTargetContext = nullptr;
            }

            if( pTargetContext->isValid() && pTargetContext->mInternal->internalOwnershipFlag )
            {
                _nativeDestroyRenderContext( *pTargetContext );
            }
        }

        if( !pTargetContext )
        {
            pTargetContext = createSysObject<GLRenderContext>( getHandle<GLSystemDriver>() );
        }

        pTargetContext->mInternal->internalOwnershipFlag = false;

        _nativeCreateRenderContextForCurrentThread( *pTargetContext );

        return pTargetContext;
    }

    std::vector<DepthStencilFormat> GLSystemDriver::querySupportedDepthStencilFormats( ColorFormat pColorFormat ) const
    {
        return {};
    }

    std::vector<MSAAMode> GLSystemDriver::querySupportedMSAAModes( ColorFormat pColorFormat,
                                                                   DepthStencilFormat pDepthStencilFormat ) const
    {
        return {};
    }

    bool GLSystemDriver::isRenderContextBound() const
    {
        return _nativeIsRenderContextBound();
    }

    bool GLSystemDriver::isRenderContextBound( const GLRenderContext & pRenderContext ) const
    {
        return _nativeIsRenderContextBound( pRenderContext );
    }

    bool GLSystemDriver::isDisplaySurfaceValid( const GLDisplaySurface & pDisplaySurface ) const
    {
        return mInternal->parentDriver && _nativeIsDisplaySurfaceValid( pDisplaySurface );
    }

    bool GLSystemDriver::isRenderContextValid( const GLRenderContext & pRenderContext ) const
    {
        return mInternal->parentDriver && _nativeIsRenderContextValid( pRenderContext );
    }

    void GLSystemDriver::onDisplaySurfaceDestroy( GLDisplaySurface & pDisplaySurface ) noexcept
    {
        try
        {
            if( !isDisplaySurfaceValid( pDisplaySurface ) )
            {
                ts3ThrowAuto( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
            }

            pDisplaySurface.unregisterEventSourceAuto();

            if( pDisplaySurface.mInternal->internalOwnershipFlag )
            {
                _nativeDestroyDisplaySurface( pDisplaySurface );
                pDisplaySurface.mInternal->internalOwnershipFlag = false;
            }
        }
        catch( const Exception & pException )
        {
            ( pException );
            ts3DebugInterrupt();
        }
        catch( ... )
        {
            ts3DebugInterrupt();
        }
    }

    void GLSystemDriver::onRenderContextDestroy( GLRenderContext & pRenderContext ) noexcept
    {
        try
        {
            if( !isRenderContextValid( pRenderContext ) )
            {
                ts3ThrowAuto( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
            }

            if( _nativeIsRenderContextBound( pRenderContext ) )
            {
                _nativeResetContextBinding();
            }

            if( pRenderContext.mInternal->internalOwnershipFlag )
            {
                _nativeDestroyRenderContext( pRenderContext );
                pRenderContext.mInternal->internalOwnershipFlag = false;
            }
        }
        catch( const Exception & pException )
        {
            ( pException );
            ts3DebugInterrupt();
        }
        catch( ... )
        {
            ts3DebugInterrupt();
        }
    }

} // namespace ts3::system
