
#include "openGLNative.h"
#include "display.h"

namespace ts3
{
namespace system
{

    GLSystemDriver::GLSystemDriver( DisplayManagerHandle pDisplayManager )
    : SysObject( pDisplayManager->mSysContext )
    , mDisplayManager( std::move( pDisplayManager ) )
    , _nativeProxy( new GLSystemDriverNativeProxy( this ) )
    {}

    GLSystemDriver::~GLSystemDriver() = default;

    void GLSystemDriver::initializePlatform()
    {}

    void GLSystemDriver::releaseInitState( GLRenderContext & pGLRenderContext )
    {}

    GLDisplaySurfaceHandle GLSystemDriver::createDisplaySurface( const GLSurfaceCreateInfo & pCreateInfo )
    {
        GLSurfaceCreateInfo validatedCreateInfo = pCreateInfo;

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

        auto openglSurface = createSysObject<GLDisplaySurface>( getHandle<GLSystemDriver>() );

        _nativeProxy->createDisplaySurface( *openglSurface, validatedCreateInfo );

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
            pTargetSurface = createSysObject<GLDisplaySurface>( getHandle<GLSystemDriver>() );
        }

        _nativeProxy->createDisplaySurfaceForCurrentThread( *pTargetSurface );

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

        auto openglContext =  createSysObject<GLRenderContext>( getHandle<GLSystemDriver>() );

        _nativeProxy->createRenderContext( *openglContext, pSurface, validatedCreateInfo );

        return openglContext;
    }

    GLRenderContextHandle GLSystemDriver::createRenderContextForCurrentThread( GLRenderContextHandle pTargetContext )
    {
        if( pTargetContext )
        {
            if( pTargetContext->isValid() )
            {
                pTargetContext = nullptr;
            }
            if( !pTargetContext->checkDriver( *this ) )
            {
                pTargetContext = nullptr;
            }
        }

        if( !pTargetContext )
        {
            pTargetContext = createSysObject<GLRenderContext>( getHandle<GLSystemDriver>() );
        }

        return pTargetContext;
    }

    std::vector<DepthStencilFormat> GLSystemDriver::querySupportedDepthStencilFormats( ColorFormat pColorFormat ) const
    {
        return {};
    }

    std::vector<MSAAMode> GLSystemDriver::uerySupportedMSAAModes( ColorFormat pColorFormat,
                                                                  DepthStencilFormat pDepthStencilFormat ) const
    {
        return {};
    }


    GLRenderContext::GLDisplaySurface( GLSystemDriverHandle pDriver )
    {}

    GLRenderContext::~GLDisplaySurface() = default;

    void GLRenderContext::swapBuffers()
    {}

    WindowSize GLRenderContext::queryRenderAreaSize() const
    {}

    bool GLRenderContext::querySurfaceBindStatus() const
    {}



    GLRenderContext::GLRenderContext( GLSystemDriverHandle pDriver )
    {}

    GLRenderContext::~GLRenderContext() = default;

    void GLRenderContext::bindForCurrentThread()
    {}

    bool GLRenderContext::validateCurrentBinding() const
    {}

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



	GLDisplaySurface::GLDisplaySurface( GLDriverHandle pGLDriver ) noexcept
	: BaseObject( pGLDriver->mContext )
	, EventSource( mNativeData )
	, mGLDriver( pGLDriver )
	{}

	GLDisplaySurface::~GLDisplaySurface() noexcept
	{
		_sysDestroy();
	}

	void GLDisplaySurface::swapBuffers()
	{
		_sysSwapBuffers();
	}

	WmWindowSize GLDisplaySurface::queryCurrentSize() const
	{
		WmWindowSize windowSize;
		_sysQueryCurrentSize( windowSize );
		return windowSize;
	}


	GLRenderContext::GLRenderContext( GLDriverHandle pGLDriver ) noexcept
	: BaseObject( pGLDriver->mContext )
	, mGLDriver( pGLDriver )
	{}

	GLRenderContext::~GLRenderContext() noexcept
	{
		_sysDestroy();
	}

	void GLRenderContext::bindForCurrentThread( GLDisplaySurface & pTargetSurface )
	{
		_sysBindForCurrentThread( pTargetSurface );
	}

	bool GLRenderContext::validateCurrentBinding() const
	{
		return _sysValidateCurrentBinding();
	}

	GLSystemVersionInfo GLRenderContext::querytemVersionInfo() const
	{
	}



	/*GLDriver::GLDriver( DisplayManagerHandle pDisplayManager ) noexcept
	: BaseObject( pDisplayManager->mContext )
	, mDisplayManager( pDisplayManager )
	, _primaryContext( nullptr )
	{}

	GLDriver::~GLDriver() noexcept = default;

	GLDriverHandle GLDriver::create( DisplayManagerHandle pDisplayManager )
	{
		auto openglSubsystem =  createSysObject<GLDriver>( pDisplayManager );
		return openglSubsystem;
	}

	void GLDriver::initializePlatform()
	{
		_sysInitializePlatform();
	}

	void GLDriver::releaseInitState( GLRenderContext & pGLRenderContext )
	{
		if( pGLRenderContext.mGLDriver.get() != this )
		{
			throw 0;
		}
		_sysReleaseInitState();
	}

	GLDisplaySurfaceHandle GLDriver::createDisplaySurface( const GLSurfaceCreateInfo & pCreateInfo )
	{
		GLSurfaceCreateInfo validatedCreateInfo = pCreateInfo;

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

		mDisplayManager->validateWindowGeometry( validatedCreateInfo.windowGeometry );

		auto openglSurface =  createSysObject<GLDisplaySurface>( getHandle<GLDriver>() );

		_sysCreateDisplaySurface( *openglSurface, validatedCreateInfo );

		return openglSurface;
	}

	GLDisplaySurfaceHandle GLDriver::createDisplaySurfaceForCurrentThread()
	{
		auto openglSurface =  createSysObject<GLDisplaySurface>( getHandle<GLDriver>() );

		_sysCreateDisplaySurfaceForCurrentThread( *openglSurface );

		return openglSurface;
	}

	GLRenderContextHandle GLDriver::createRenderContext( GLDisplaySurface & pSurface, const GLRenderContextCreateInfo & pCreateInfo )
	{
		GLRenderContextCreateInfo validatedCreateInfo = pCreateInfo;

		auto & targetAPIVersion = pCreateInfo.requiredAPIVersion;
		if( ( targetAPIVersion == cvVersionInvalid  ) || ( targetAPIVersion == cvVersionUnknown  ) )
		{
			validatedCreateInfo.requiredAPIVersion.major = 1;
			validatedCreateInfo.requiredAPIVersion.minor = 0;
		}

		auto openglContext =  createSysObject<GLRenderContext>( getHandle<GLDriver>() );

		_sysCreateRenderContext( *openglContext, pSurface, validatedCreateInfo );

		return openglContext;
	}

	GLRenderContextHandle GLDriver::createRenderContextForCurrentThread()
	{
		auto openglContext =  createSysObject<GLRenderContext>( getHandle<GLDriver>() );

		_sysCreateRenderContextForCurrentThread( *openglContext );

		return openglContext;
	}

	void GLDriver::setPrimaryContext( GLRenderContext & pPrimaryContext )
	{
		_primaryContext = &pPrimaryContext;
	}

	void GLDriver::resetPrimaryContext()
	{
		_primaryContext = nullptr;
	}

	std::vector<DepthStencilFormat> GLDriver::querySupportedDepthStencilFormats( ColorFormat pColorFormat )
	{
		return {};
	}

	std::vector<MSAAMode> GLDriver::querySupportedMSAAModes( ColorFormat pColorFormat, DepthStencilFormat pDepthStencilFormat )
	{
		return {};
	}

	GLRenderContext * GLDriver::getPrimaryContext() const
	{
		return _primaryContext;
	}*/

} // namespace system
} // namespace ts3
