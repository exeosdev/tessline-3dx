
#include "openGLNative.h"
#include "displayNative.h"

namespace ts3::system
{

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
    }

    void GLSystemDriver::releaseInitState( GLRenderContext & /* pGLRenderContext */ )
    {
        _nativeReleaseInitState();
    }

    GLDisplaySurfaceHandle GLSystemDriver::createDisplaySurface( const GLDisplaySurfaceCreateInfo & pCreateInfo )
    {
        GLDisplaySurfaceCreateInfo validatedCreateInfo = pCreateInfo;

        if( pCreateInfo.flags.isSet( E_GL_DISPLAY_SURFACE_CREATE_FLAG_FULLSCREEN_BIT ) )
        {
            validatedCreateInfo.windowGeometry.size = cvWindowSizeMax;
            validatedCreateInfo.windowGeometry.frameStyle = WindowFrameStyle::Overlay;
        }
        else
        {
            validatedCreateInfo.windowGeometry.position = pCreateInfo.windowGeometry.position;
            validatedCreateInfo.windowGeometry.size = pCreateInfo.windowGeometry.size;
            validatedCreateInfo.windowGeometry.frameStyle = pCreateInfo.windowGeometry.frameStyle;
        }

        validatedCreateInfo.windowGeometry = mDisplayManager->validateWindowGeometry( validatedCreateInfo.windowGeometry );

        auto displaySurface = createSysObject<GLDisplaySurface>( getHandle<GLSystemDriver>() );
        displaySurface->mInternal->internalOwnershipFlag = true;

        _nativeCreateDisplaySurface( *displaySurface, validatedCreateInfo );

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
        GLRenderContextCreateInfo validatedCreateInfo = pCreateInfo;

        if( ( pCreateInfo.requiredAPIVersion == cvVersionInvalid  ) || ( pCreateInfo.requiredAPIVersion == cvVersionUnknown  ) )
        {
            validatedCreateInfo.requiredAPIVersion.major = 1;
            validatedCreateInfo.requiredAPIVersion.minor = 0;
        }

        auto renderContext = createSysObject<GLRenderContext>( getHandle<GLSystemDriver>() );
        renderContext->mInternal->internalOwnershipFlag = true;

        _nativeCreateRenderContext( *renderContext, pSurface, validatedCreateInfo );

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

    bool GLSystemDriver::isRenderContextBound( GLRenderContext & pRenderContext ) const
    {
        return _nativeIsRenderContextBound( pRenderContext );
    }

    bool GLSystemDriver::isDisplaySurfaceValid( GLDisplaySurface & pDisplaySurface ) const
    {
        return mInternal->parentDriver && _nativeIsDisplaySurfaceValid( pDisplaySurface );
    }

    bool GLSystemDriver::isRenderContextValid( GLRenderContext & pRenderContext ) const
    {
        return mInternal->parentDriver && _nativeIsRenderContextValid( pRenderContext );
    }

    void GLSystemDriver::onDisplaySurfaceDtor( GLDisplaySurface & pDisplaySurface ) noexcept
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
            ts3DebugInterrupt();
        }
        catch( ... )
        {
            ts3DebugInterrupt();
        }
    }

    void GLSystemDriver::onRenderContextDtor( GLRenderContext & pRenderContext ) noexcept
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
            ts3DebugInterrupt();
        }
        catch( ... )
        {
            ts3DebugInterrupt();
        }
    }


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
        mDriver->onDisplaySurfaceDtor( *this );
    }

    void GLDisplaySurface::clearColorBuffer()
    {
        glClearColor( 0.1f, 0.33f, 0.9f, 1.0f );
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
        return mInternal && _nativeIsValid();
    }


    GLRenderContext::GLRenderContext( GLSystemDriverHandle pDriver )
    : SysObject( pDriver->mSysContext )
    , mDriver( pDriver )
    , mInternal( std::make_unique<ObjectInternalData>( this ) )
    , mNativeData( &( mInternal->nativeDataPriv ) )
    {}

    GLRenderContext::~GLRenderContext() noexcept
    {
        mDriver->onRenderContextDtor( *this );
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

        int majorVersion = 0;
        glGetIntegerv( GL_MAJOR_VERSION, &majorVersion );
        systemVersionInfo.apiVersion.major = static_cast< uint16 >( majorVersion );

        int minorVersion = 0;
        glGetIntegerv( GL_MINOR_VERSION, &minorVersion );
        systemVersionInfo.apiVersion.minor = static_cast< uint16 >( minorVersion );

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
        if( !isValid() )
        {
            return false;
        }
        return _nativeIsCurrent();
    }

    bool GLRenderContext::isValid() const
    {
        return _nativeIsValid();
    }

} // namespace ts3::system
