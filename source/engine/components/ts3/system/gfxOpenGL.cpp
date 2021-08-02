
#include "openGL.h"
#include "displayManager.h"

namespace ts3
{

	SysGLSurface::SysGLSurface( SysGLDriverHandle pGLDriver ) noexcept
	: SysBaseObject( pGLDriver->mSysContext )
	, SysEventSource( mNativeData )
	, mGLDriver( pGLDriver )
	{}

	SysGLSurface::~SysGLSurface() noexcept
	{
		_sysDestroy();
	}

	void SysGLSurface::swapBuffers()
	{
		_sysSwapBuffers();
	}

	SysWindowSize SysGLSurface::queryCurrentSize() const
	{
		SysWindowSize windowSize;
		_sysQueryCurrentSize( windowSize );
		return windowSize;
	}


	SysGLRenderContext::SysGLRenderContext( SysGLDriverHandle pGLDriver ) noexcept
	: SysBaseObject( pGLDriver->mSysContext )
	, mGLDriver( pGLDriver )
	{}

	SysGLRenderContext::~SysGLRenderContext() noexcept
	{
		_sysDestroy();
	}

	void SysGLRenderContext::bindForCurrentThread( SysGLSurface & pTargetSurface )
	{
		_sysBindForCurrentThread( pTargetSurface );
	}

	bool SysGLRenderContext::validateCurrentBinding() const
	{
		return _sysValidateCurrentBinding();
	}

	SysGLSystemVersionInfo SysGLRenderContext::querySystemVersionInfo() const
	{
		SysGLSystemVersionInfo systemVersionInfo;

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



	SysGLDriver::SysGLDriver( SysDisplayManagerHandle pDisplayManager ) noexcept
	: SysBaseObject( pDisplayManager->mSysContext )
	, mDisplayManager( pDisplayManager )
	, _primaryContext( nullptr )
	{}

	SysGLDriver::~SysGLDriver() noexcept = default;

	SysGLDriverHandle SysGLDriver::create( SysDisplayManagerHandle pDisplayManager )
	{
		auto openglSubsystem = sysCreateObject<SysGLDriver>( pDisplayManager );
		return openglSubsystem;
	}

	void SysGLDriver::initializePlatform()
	{
		_sysInitializePlatform();
	}

	void SysGLDriver::releaseInitState( SysGLRenderContext & pGLRenderContext )
	{
		if( pGLRenderContext.mGLDriver.get() != this )
		{
			throw 0;
		}
		_sysReleaseInitState();
	}

	SysGLSurfaceHandle SysGLDriver::createDisplaySurface( const SysGLSurfaceCreateInfo & pCreateInfo )
	{
		SysGLSurfaceCreateInfo validatedCreateInfo = pCreateInfo;

		if( pCreateInfo.flags.isSet( E_SYS_GFX_GL_DISPLAY_SURFACE_CREATE_FLAG_FULLSCREEN_BIT ) )
		{
			validatedCreateInfo.windowGeometry.size = cvSysWindowSizeMax;
			validatedCreateInfo.windowGeometry.frameStyle = SysWindowFrameStyle::Overlay;
		}
		else
		{
			validatedCreateInfo.windowGeometry.position = pCreateInfo.windowGeometry.position;
			validatedCreateInfo.windowGeometry.size = pCreateInfo.windowGeometry.size;
			validatedCreateInfo.windowGeometry.frameStyle = pCreateInfo.windowGeometry.frameStyle;
		}

		mDisplayManager->validateWindowGeometry( validatedCreateInfo.windowGeometry );

		auto openglSurface = sysCreateObject<SysGLSurface>( getHandle<SysGLDriver>() );

		_sysCreateDisplaySurface( *openglSurface, validatedCreateInfo );

		return openglSurface;
	}

	SysGLSurfaceHandle SysGLDriver::createDisplaySurfaceForCurrentThread()
	{
		auto openglSurface = sysCreateObject<SysGLSurface>( getHandle<SysGLDriver>() );

		_sysCreateDisplaySurfaceForCurrentThread( *openglSurface );

		return openglSurface;
	}

	SysGLRenderContextHandle SysGLDriver::createRenderContext( SysGLSurface & pSurface, const SysGLRenderContextCreateInfo & pCreateInfo )
	{
		SysGLRenderContextCreateInfo validatedCreateInfo = pCreateInfo;

		auto & targetAPIVersion = pCreateInfo.requiredAPIVersion;
		if( ( targetAPIVersion == cvVersionInvalid  ) || ( targetAPIVersion == cvVersionUnknown  ) )
		{
			validatedCreateInfo.requiredAPIVersion.major = 1;
			validatedCreateInfo.requiredAPIVersion.minor = 0;
		}

		auto openglContext = sysCreateObject<SysGLRenderContext>( getHandle<SysGLDriver>() );

		_sysCreateRenderContext( *openglContext, pSurface, validatedCreateInfo );

		return openglContext;
	}

	SysGLRenderContextHandle SysGLDriver::createRenderContextForCurrentThread()
	{
		auto openglContext = sysCreateObject<SysGLRenderContext>( getHandle<SysGLDriver>() );

		_sysCreateRenderContextForCurrentThread( *openglContext );

		return openglContext;
	}

	void SysGLDriver::setPrimaryContext( SysGLRenderContext & pPrimaryContext )
	{
		_primaryContext = &pPrimaryContext;
	}

	void SysGLDriver::resetPrimaryContext()
	{
		_primaryContext = nullptr;
	}

	std::vector<SysDepthStencilFormat> SysGLDriver::querySupportedDepthStencilFormats( SysColorFormat pColorFormat )
	{
		return {};
	}

	std::vector<SysMSAAMode> SysGLDriver::querySupportedMSAAModes( SysColorFormat pColorFormat, SysDepthStencilFormat pDepthStencilFormat )
	{
		return {};
	}

	SysGLRenderContext * SysGLDriver::getPrimaryContext() const
	{
		return _primaryContext;
	}

}
