
#include "openGL.h"
#include "displayManager.h"

namespace ts3
{

	GfxGLSurface::GfxGLSurface( GfxGLDriverHandle pGLDriver ) noexcept
	: BaseObject( pGLDriver->mContext )
	, EventSource( mNativeData )
	, mGLDriver( pGLDriver )
	{}

	GfxGLSurface::~GfxGLSurface() noexcept
	{
		_sysDestroy();
	}

	void GfxGLSurface::swapBuffers()
	{
		_sysSwapBuffers();
	}

	WindowSize GfxGLSurface::queryCurrentSize() const
	{
		WindowSize windowSize;
		_sysQueryCurrentSize( windowSize );
		return windowSize;
	}


	GfxGLRenderContext::GfxGLRenderContext( GfxGLDriverHandle pGLDriver ) noexcept
	: BaseObject( pGLDriver->mContext )
	, mGLDriver( pGLDriver )
	{}

	GfxGLRenderContext::~GfxGLRenderContext() noexcept
	{
		_sysDestroy();
	}

	void GfxGLRenderContext::bindForCurrentThread( GfxGLSurface & pTargetSurface )
	{
		_sysBindForCurrentThread( pTargetSurface );
	}

	bool GfxGLRenderContext::validateCurrentBinding() const
	{
		return _sysValidateCurrentBinding();
	}

	GfxGLtemVersionInfo GfxGLRenderContext::querytemVersionInfo() const
	{
		GfxGLtemVersionInfo systemVersionInfo;

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



	GfxGLDriver::GfxGLDriver( DisplayManagerHandle pDisplayManager ) noexcept
	: BaseObject( pDisplayManager->mContext )
	, mDisplayManager( pDisplayManager )
	, _primaryContext( nullptr )
	{}

	GfxGLDriver::~GfxGLDriver() noexcept = default;

	GfxGLDriverHandle GfxGLDriver::create( DisplayManagerHandle pDisplayManager )
	{
		auto openglSubsystem = sysCreateObject<GfxGLDriver>( pDisplayManager );
		return openglSubsystem;
	}

	void GfxGLDriver::initializePlatform()
	{
		_sysInitializePlatform();
	}

	void GfxGLDriver::releaseInitState( GfxGLRenderContext & pGLRenderContext )
	{
		if( pGLRenderContext.mGLDriver.get() != this )
		{
			throw 0;
		}
		_sysReleaseInitState();
	}

	GfxGLSurfaceHandle GfxGLDriver::createDisplaySurface( const GfxGLSurfaceCreateInfo & pCreateInfo )
	{
		GfxGLSurfaceCreateInfo validatedCreateInfo = pCreateInfo;

		if( pCreateInfo.flags.isSet( E_GFX_GL_DISPLAY_SURFACE_CREATE_FLAG_FULLSCREEN_BIT ) )
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

		auto openglSurface = sysCreateObject<GfxGLSurface>( getHandle<GfxGLDriver>() );

		_sysCreateDisplaySurface( *openglSurface, validatedCreateInfo );

		return openglSurface;
	}

	GfxGLSurfaceHandle GfxGLDriver::createDisplaySurfaceForCurrentThread()
	{
		auto openglSurface = sysCreateObject<GfxGLSurface>( getHandle<GfxGLDriver>() );

		_sysCreateDisplaySurfaceForCurrentThread( *openglSurface );

		return openglSurface;
	}

	GfxGLRenderContextHandle GfxGLDriver::createRenderContext( GfxGLSurface & pSurface, const GfxGLRenderContextCreateInfo & pCreateInfo )
	{
		GfxGLRenderContextCreateInfo validatedCreateInfo = pCreateInfo;

		auto & targetAPIVersion = pCreateInfo.requiredAPIVersion;
		if( ( targetAPIVersion == cvVersionInvalid  ) || ( targetAPIVersion == cvVersionUnknown  ) )
		{
			validatedCreateInfo.requiredAPIVersion.major = 1;
			validatedCreateInfo.requiredAPIVersion.minor = 0;
		}

		auto openglContext = sysCreateObject<GfxGLRenderContext>( getHandle<GfxGLDriver>() );

		_sysCreateRenderContext( *openglContext, pSurface, validatedCreateInfo );

		return openglContext;
	}

	GfxGLRenderContextHandle GfxGLDriver::createRenderContextForCurrentThread()
	{
		auto openglContext = sysCreateObject<GfxGLRenderContext>( getHandle<GfxGLDriver>() );

		_sysCreateRenderContextForCurrentThread( *openglContext );

		return openglContext;
	}

	void GfxGLDriver::setPrimaryContext( GfxGLRenderContext & pPrimaryContext )
	{
		_primaryContext = &pPrimaryContext;
	}

	void GfxGLDriver::resetPrimaryContext()
	{
		_primaryContext = nullptr;
	}

	std::vector<GfxDepthStencilFormat> GfxGLDriver::querySupportedDepthStencilFormats( GfxColorFormat pColorFormat )
	{
		return {};
	}

	std::vector<GfxMSAAMode> GfxGLDriver::querySupportedMSAAModes( GfxColorFormat pColorFormat, GfxDepthStencilFormat pDepthStencilFormat )
	{
		return {};
	}

	GfxGLRenderContext * GfxGLDriver::getPrimaryContext() const
	{
		return _primaryContext;
	}

}
