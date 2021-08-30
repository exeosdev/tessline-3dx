
#include "openGLNative.h"
#include "display.h"

namespace ts3::system
{

    GLSystemDriver::GLSystemDriver( DisplayManagerHandle pDisplayManager )
    : SysObject( pDisplayManager->mSysContext )
    , mDisplayManager( std::move( pDisplayManager ) )
    {}

    GLSystemDriver::~GLSystemDriver() = default;

    void GLSystemDriver::initializePlatform()
    {}

    void GLSystemDriver::releaseInitState( GLRenderContext & pGLRenderContext )
    {}


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

        mDisplayManager->validateWindowGeometry( validatedCreateInfo.windowGeometry.position,
                                                 validatedCreateInfo.windowGeometry.size );

        auto openglSurface = createSysObject<GLDisplaySurfaceNativeImpl>( getHandle<GLSystemDriver>() );

        _nativeCreateDisplaySurface( *openglSurface, validatedCreateInfo );

        return openglSurface;
    }

    GLDisplaySurfaceHandle GLSystemDriver::createDisplaySurfaceForCurrentThread( GLDisplaySurfaceHandle pTargetSurface )
    {
        if( pTargetSurface )
        {
            if( pTargetSurface->isValid() )
            {
                pTargetSurface = nullptr;
            }
            if( !pTargetSurface->checkDriver( *this ) )
            {
                pTargetSurface = nullptr;
            }
        }

        if( !pTargetSurface )
        {
            pTargetSurface = createSysObject<GLDisplaySurfaceNativeImpl>( getHandle<GLSystemDriver>() );
        }

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

        auto openglContext =  createSysObject<GLRenderContextNativeImpl>( getHandle<GLSystemDriver>() );

        _nativeCreateRenderContext( *openglContext, validatedCreateInfo );

        return openglContext;
    }

    GLRenderContextHandle GLSystemDriver::createRenderContextForCurrentThread( GLRenderContextHandle pTargetContext )
    {
        // If the caller specifies target context, it means it should be used
        // as a container for the context data instead of creating a new one.
        // We perform a simple validation first to see if its state is correct.
        if( pTargetContext )
        {
            // Check if the parent GL Driver is this driver. This must be true for all existing objects used
            // with a given driver. If that doesn't match, something is seriously broken. If that happens,
            // we simply discard the specified context which will cause creation of a new one.
            if( !pTargetContext->checkDriver( *this ) )
            {
                pTargetContext = nullptr;
            }
            // If the context is a valid context, we destroy the internal data so everything is properly
            // released at the system level too. We cannot destroy the whole GLContext, as that's the idea
            // behind this function - recreate a context without invalidating logical object's references.
            else if( pTargetContext->isValid() )
            {
                pTargetContext->destroy();
            }
        }

        if( !pTargetContext )
        {
            pTargetContext = createSysObject<GLRenderContextNativeImpl>( getHandle<GLSystemDriver>() );
        }

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


    GLDisplaySurface::GLDisplaySurface( GLSystemDriverHandle pDriver )
    : SysObject( pDriver->mSysContext )
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
        return mDriver.get() == &pDriver;
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


    GLRenderContext::GLRenderContext( GLSystemDriverHandle pDriver )
    : SysObject( pDriver->mSysContext )
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
        return mDriver.get() == &pDriver;
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
