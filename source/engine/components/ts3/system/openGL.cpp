
#include "openGLNative.h"
#include "displayManager.h"

namespace ts3::system
{

    GLSystemDriver::GLSystemDriver( DisplayManagerHandle pDisplayManager )
    : SysObject( pDisplayManager->mSysContext )
    , mDisplayManager( std::move( pDisplayManager ) )
    , mPrivate( std::make_unique<ObjectPrivateData>() )
    , mNativeData( &( mPrivate->nativeDataPriv ) )
    {}

    GLSystemDriver::~GLSystemDriver() = default;

    void GLSystemDriver::initializePlatform()
    {}

    void GLSystemDriver::releaseInitState( GLRenderContext & pGLRenderContext )
    {}

    GLDisplaySurface * GLSystemDriver::createDisplaySurface( const GLDisplaySurfaceCreateInfo & pCreateInfo )
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

        mDisplayManager->validateWindowGeometry( validatedCreateInfo.windowGeometry.position,
                                                 validatedCreateInfo.windowGeometry.size );

        auto surfaceRef = mPrivate->displaySurfaceList.emplace( mPrivate->displaySurfaceList.end(), this );
        surfaceRef->mPrivate->driverListRef = surfaceRef;
        surfaceRef->mPrivate->internalOwnershipFlag = true;

        _nativeCreateDisplaySurface( *surfaceRef, validatedCreateInfo );

        return &( *surfaceRef );
    }

    GLDisplaySurface * GLSystemDriver::createDisplaySurfaceForCurrentThread( GLDisplaySurface * pTargetSurface )
    {
        if( pTargetSurface )
        {
            if( !isDisplaySurfaceValid( *pTargetSurface ) )
            {
                pTargetSurface = nullptr;
            }

            if( pTargetSurface->isValid() && pTargetSurface->mPrivate->internalOwnershipFlag )
            {
                _nativeDestroyDisplaySurface( *pTargetSurface );
            }
        }

        if( !pTargetSurface )
        {
            auto surfaceRef = mPrivate->displaySurfaceList.emplace( mPrivate->displaySurfaceList.end(), this );
            surfaceRef->mPrivate->driverListRef = surfaceRef;
            pTargetSurface = &( *surfaceRef );
        }

        pTargetSurface->mPrivate->internalOwnershipFlag = false;

        _nativeCreateDisplaySurfaceForCurrentThread( *pTargetSurface );

        return pTargetSurface;
    }

    GLRenderContext * GLSystemDriver::createRenderContext( GLDisplaySurface & pSurface,
                                                           const GLRenderContextCreateInfo & pCreateInfo )
    {
        GLRenderContextCreateInfo validatedCreateInfo = pCreateInfo;

        if( ( pCreateInfo.requiredAPIVersion == cvVersionInvalid  ) || ( pCreateInfo.requiredAPIVersion == cvVersionUnknown  ) )
        {
            validatedCreateInfo.requiredAPIVersion.major = 1;
            validatedCreateInfo.requiredAPIVersion.minor = 0;
        }

        auto contextRef = mPrivate->renderContextList.emplace( mPrivate->renderContextList.end(), this );
        contextRef->mPrivate->driverListRef = contextRef;
        contextRef->mPrivate->internalOwnershipFlag = true;

        _nativeCreateRenderContext( *contextRef, pSurface, validatedCreateInfo );

        return &( *contextRef );
    }

    GLRenderContext * GLSystemDriver::createRenderContextForCurrentThread( GLRenderContext * pTargetContext )
    {
        // If the caller specifies target context, it means it should be used
        // as a container for the context data instead of creating a new one.
        // We perform a simple validation first to see if its state is correct.
        if( pTargetContext )
        {
            if( !isRenderContextValid( *pTargetContext ) )
            {
                pTargetContext = nullptr;
            }

            if( pTargetContext->isValid() && pTargetContext->mPrivate->internalOwnershipFlag )
            {
                _nativeDestroyRenderContext( *pTargetContext );
            }
        }

        if( !pTargetContext )
        {
            auto contextRef = mPrivate->renderContextList.emplace( mPrivate->renderContextList.end(), this );
            contextRef->mPrivate->driverListRef = contextRef;
            pTargetContext = &( *contextRef );
        }

        pTargetContext->mPrivate->internalOwnershipFlag = false;

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
        return false;
    }

    bool GLSystemDriver::isRenderContextBound( GLRenderContext & pRenderContext ) const
    {
        return false;
    }

    bool GLSystemDriver::isDisplaySurfaceValid( GLDisplaySurface & pDisplaySurface ) const
    {
        auto displaySurfaceIter = mPrivate->displaySurfaceList.begin();
        auto displaySurfaceEndIter = mPrivate->displaySurfaceList.end();

        while( displaySurfaceIter != displaySurfaceEndIter )
        {
            if( pDisplaySurface.mPrivate->driverListRef == displaySurfaceIter )
            {
                if( &( *displaySurfaceIter ) == &pDisplaySurface )
                {
                    return true;
                }
            }
            ++displaySurfaceIter;
        }

        return false;
    }

    bool GLSystemDriver::isRenderContextValid( GLRenderContext & pRenderContext ) const
    {
        auto renderContextIter = mPrivate->renderContextList.begin();
        auto renderContextEndIter = mPrivate->renderContextList.end();

        while( renderContextIter != renderContextEndIter )
        {
            if( pRenderContext.mPrivate->driverListRef == renderContextIter )
            {
                if( &( *renderContextIter ) == &pRenderContext )
                {
                    return true;
                }
            }
            ++renderContextIter;
        }

        return false;
    }


    GLDisplaySurface::GLDisplaySurface( GLSystemDriver * pDriver )
    : mDriver( pDriver )
    , mPrivate( std::make_unique<ObjectPrivateData>() )
    , mNativeData( &( mPrivate->nativeDataPriv ) )
    {}

    GLDisplaySurface::~GLDisplaySurface() = default;

    void GLDisplaySurface::swapBuffers()
    {
        if( !isValid() )
        {
            throw 0;
        }

        _nativeSwapBuffers();
    }

    bool GLDisplaySurface::checkDriver( const GLSystemDriver & pDriver ) const
    {
        return mDriver == &pDriver;
    }

    WindowSize GLDisplaySurface::queryRenderAreaSize() const
    {
        if( !isValid() )
        {
            throw 0;
        }

        WindowSize result;
        _nativeQueryRenderAreaSize( result );

        return result;
    }


    GLRenderContext::GLRenderContext( GLSystemDriver * pDriver )
    : mDriver( pDriver )
    , mPrivate( std::make_unique<ObjectPrivateData>() )
    , mNativeData( &( mPrivate->nativeDataPriv ) )
    {}

    GLRenderContext::~GLRenderContext() = default;

    void GLRenderContext::bindForCurrentThread( const GLDisplaySurface & pSurface )
    {
        if( !isValid() )
        {
            throw 0;
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

    bool GLRenderContext::checkDriver( const GLSystemDriver & pDriver ) const
    {
        return mDriver == &pDriver;
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
