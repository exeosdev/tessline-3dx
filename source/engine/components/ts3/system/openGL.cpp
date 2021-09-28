
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
        if( !isValid() )
        {
            ts3ThrowAuto( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
        }

        glClearColor( 0.1f, 0.33f, 0.9f, 1.0f );
        ts3GLHandleLastError();

        glClear( GL_COLOR_BUFFER_BIT );
        ts3GLHandleLastError();
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
        return !mInternal->internalInvalidateFlag && mDriver->isDisplaySurfaceValid( *this );
    }

    void GLDisplaySurface::setInvalidState()
    {
        mInternal->internalInvalidateFlag = true;
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
        if( !isValid() )
        {
            ts3ThrowAuto( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
        }

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

        ts3GLResetErrorQueue();

        return systemVersionInfo;
    }

    bool GLRenderContext::isCurrent() const
    {
        return !mInternal->internalInvalidateFlag && mDriver->isRenderContextBound( *this );
    }

    bool GLRenderContext::isValid() const
    {
        return !mInternal->internalInvalidateFlag && mDriver->isRenderContextValid( *this );
    }

    void GLRenderContext::setInvalidState()
    {
        if( mDriver->isRenderContextBound( *this ) )
        {
            mDriver->resetContextBinding();
        }
        mInternal->internalInvalidateFlag = true;
    }


    GLSystemDriver::GLSystemDriver( DisplayManagerHandle pDisplayManager )
    : SysObject( pDisplayManager->mSysContext )
    , mDisplayManager( std::move( pDisplayManager ) )
    , mInternal( std::make_unique<ObjectInternalData>( this ) )
    , mNativeData( &( mInternal->nativeDataPriv ) )
    {
        _nativeConstructor();
    }

    GLSystemDriver::~GLSystemDriver() noexcept
    {
        _nativeReleaseInitState();
        _nativeDestructor();
    }

    void GLSystemDriver::initialize()
    {
        _nativeInitialize();
    }

    void GLSystemDriver::release()
    {
        for( auto * renderContext : mInternal->internalRenderContextList )
        {
            renderContext->setInvalidState();
        }
        for( auto * displaySurface : mInternal->internalDisplaySurfaceList )
        {
            displaySurface->setInvalidState();
        }
        _nativeReleaseInitState();
        _nativeRelease();
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

        _nativeCreateDisplaySurface( *displaySurface, ctxCreateInfo );
        displaySurface->mInternal->internalOwnershipFlag = true;

        mInternal->internalDisplaySurfaceList.push_back( displaySurface.get() );

        return displaySurface;
    }

    GLDisplaySurfaceHandle GLSystemDriver::createDisplaySurfaceForCurrentThread( GLDisplaySurfaceHandle pTargetSurface )
    {
        // 'pTargetSurface' is an optional, existing surface object that should be used as a target.
        // It is only a hint - if it's not suitable and/or valid, a new one is created and used.

        if( pTargetSurface )
        {
            if( pTargetSurface->mInternal->internalOwnershipFlag && _nativeIsDisplaySurfaceValid( *pTargetSurface ) )
            {
                // If the surface is valid, we need to destroy it and ensure couple things are handled.
                // destroyDisplaySurface() will also:
                // - remove the surface from the driver's list (we will register it again after it's re-created)
                // - unregister it from the event controller if it's an active event emitter

                _nativeDestroyDisplaySurface( *pTargetSurface );

                pTargetSurface->mInternal->internalOwnershipFlag = false;
            }
        }

        if( !pTargetSurface )
        {
            pTargetSurface = createSysObject<GLDisplaySurface>( getHandle<GLSystemDriver>() );
        }

        _nativeCreateDisplaySurfaceForCurrentThread( *pTargetSurface );
        pTargetSurface->mInternal->internalOwnershipFlag = false;
        pTargetSurface->mInternal->internalInvalidateFlag = false;

        mInternal->internalDisplaySurfaceList.push_back( pTargetSurface.get() );

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

        _nativeCreateRenderContext( *renderContext, pSurface, ctxCreateInfo );
        renderContext->mInternal->internalOwnershipFlag = true;

        mInternal->internalRenderContextList.push_back( renderContext.get() );

        return renderContext;
    }

    GLRenderContextHandle GLSystemDriver::createRenderContextForCurrentThread( GLRenderContextHandle pTargetContext )
    {
        if( pTargetContext )
        {
            if( pTargetContext->mInternal->internalOwnershipFlag && _nativeIsRenderContextValid( *pTargetContext ) )
            {
                _nativeDestroyRenderContext( *pTargetContext );

                pTargetContext->mInternal->internalOwnershipFlag = false;
            }
        }

        if( !pTargetContext )
        {
            pTargetContext = createSysObject<GLRenderContext>( getHandle<GLSystemDriver>() );
        }

        _nativeCreateRenderContextForCurrentThread( *pTargetContext );
        pTargetContext->mInternal->internalOwnershipFlag = false;
        pTargetContext->mInternal->internalInvalidateFlag = false;

        mInternal->internalRenderContextList.push_back( pTargetContext.get() );

        return pTargetContext;
    }

    std::vector<EDepthStencilFormat> GLSystemDriver::querySupportedDepthStencilFormats( EColorFormat pColorFormat ) const
    {
        return {};
    }

    std::vector<EMSAAMode> GLSystemDriver::querySupportedMSAAModes( EColorFormat pColorFormat,
                                                                   EDepthStencilFormat pDepthStencilFormat ) const
    {
        return {};
    }

    void GLSystemDriver::resetContextBinding()
    {
        _nativeResetContextBinding();
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
        return _nativeIsDisplaySurfaceValid( pDisplaySurface );
    }

    bool GLSystemDriver::isRenderContextValid( const GLRenderContext & pRenderContext ) const
    {
        return _nativeIsRenderContextValid( pRenderContext );
    }

    void GLSystemDriver::onDisplaySurfaceDestroy( GLDisplaySurface & pDisplaySurface ) noexcept
    {
        try
        {
            if( pDisplaySurface.mInternal->internalOwnershipFlag && _nativeIsDisplaySurfaceValid( pDisplaySurface ) )
            {
                _nativeDestroyDisplaySurface( pDisplaySurface );
                pDisplaySurface.mInternal->internalOwnershipFlag = false;
            }

            auto surfaceIter = mInternal->internalDisplaySurfaceList.begin();
            while( surfaceIter != mInternal->internalDisplaySurfaceList.end() )
            {
                if( *surfaceIter == &pDisplaySurface )
                {
                    mInternal->internalDisplaySurfaceList.erase( surfaceIter );
                    break;
                }
                ++surfaceIter;
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
            if( pRenderContext.mInternal->internalOwnershipFlag && _nativeIsRenderContextValid( pRenderContext ) )
            {
                _nativeDestroyRenderContext( pRenderContext );
                pRenderContext.mInternal->internalOwnershipFlag = false;
            }

            auto contextIter = mInternal->internalRenderContextList.begin();
            while( contextIter != mInternal->internalRenderContextList.end() )
            {
                if( *contextIter == &pRenderContext )
                {
                    mInternal->internalRenderContextList.erase( contextIter );
                    break;
                }
                ++contextIter;
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
