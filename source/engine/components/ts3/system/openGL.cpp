
#include "openGL.h"
#include "displayManager.h"

namespace ts3
{

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



	GLDriver::GLDriver( DisplayManagerHandle pDisplayManager ) noexcept
	: BaseObject( pDisplayManager->mContext )
	, mDisplayManager( pDisplayManager )
	, _primaryContext( nullptr )
	{}

	GLDriver::~GLDriver() noexcept = default;

	GLDriverHandle GLDriver::create( DisplayManagerHandle pDisplayManager )
	{
		auto openglSubsystem = sysCreateObject<GLDriver>( pDisplayManager );
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
			validatedCreateInfo.windowGeometry.frameStyle = WmWindowFrameStyle::Overlay;
		}
		else
		{
			validatedCreateInfo.windowGeometry.position = pCreateInfo.windowGeometry.position;
			validatedCreateInfo.windowGeometry.size = pCreateInfo.windowGeometry.size;
			validatedCreateInfo.windowGeometry.frameStyle = pCreateInfo.windowGeometry.frameStyle;
		}

		mDisplayManager->validateWindowGeometry( validatedCreateInfo.windowGeometry );

		auto openglSurface = sysCreateObject<GLDisplaySurface>( getHandle<GLDriver>() );

		_sysCreateDisplaySurface( *openglSurface, validatedCreateInfo );

		return openglSurface;
	}

	GLDisplaySurfaceHandle GLDriver::createDisplaySurfaceForCurrentThread()
	{
		auto openglSurface = sysCreateObject<GLDisplaySurface>( getHandle<GLDriver>() );

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

		auto openglContext = sysCreateObject<GLRenderContext>( getHandle<GLDriver>() );

		_sysCreateRenderContext( *openglContext, pSurface, validatedCreateInfo );

		return openglContext;
	}

	GLRenderContextHandle GLDriver::createRenderContextForCurrentThread()
	{
		auto openglContext = sysCreateObject<GLRenderContext>( getHandle<GLDriver>() );

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
	}

}
