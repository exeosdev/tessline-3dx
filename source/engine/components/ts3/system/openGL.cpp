
#include "openGL.h"
#include "displayManager.h"

namespace ts3
{

	SysGLSurface::SysGLSurface( SysGLSubsystemHandle pGLSubsystem ) noexcept
	: SysBaseObject( pGLSubsystem->mSysContext )
	, SysEventSource( mNativeData )
	, mGLSubsystem( pGLSubsystem )
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


	SysGLContext::SysGLContext( SysGLSubsystemHandle pGLSubsystem ) noexcept
	: SysBaseObject( pGLSubsystem->mSysContext )
	, mGLSubsystem( pGLSubsystem )
	{}

	SysGLContext::~SysGLContext() noexcept
	{
		_sysDestroy();
	}

	void SysGLContext::bindForCurrentThread( SysGLSurface & pTargetSurface )
	{
		_sysBindForCurrentThread( pTargetSurface );
	}

	bool SysGLContext::validateCurrentBinding() const
	{
		return _sysValidateCurrentBinding();
	}

	SysGLSystemVersionInfo SysGLContext::querySystemVersionInfo() const
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



	SysGLSubsystem::SysGLSubsystem( SysDisplayManagerHandle pDisplayManager ) noexcept
	: SysBaseObject( pDisplayManager->mSysContext )
	, mDisplayManager( pDisplayManager )
	, _primaryContext( nullptr )
	{}

	SysGLSubsystem::~SysGLSubsystem() noexcept = default;

	SysGLSubsystemHandle SysGLSubsystem::create( SysDisplayManagerHandle pDisplayManager )
	{
		auto openglSubsystem = sysCreateObject<SysGLSubsystem>( pDisplayManager );
		return openglSubsystem;
	}

	void SysGLSubsystem::initializePlatform()
	{
		_sysInitializePlatform();
	}

	void SysGLSubsystem::releaseInitState( SysGLContext & pGLContext )
	{
		if( pGLContext.mGLSubsystem.get() != this )
		{
			throw 0;
		}
		_sysReleaseInitState();
	}

	SysGLSurfaceHandle SysGLSubsystem::createDisplaySurface( const SysGLSurfaceCreateInfo & pCreateInfo )
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

		auto openglSurface = sysCreateObject<SysGLSurface>( getHandle<SysGLSubsystem>() );

		_sysCreateDisplaySurface( *openglSurface, validatedCreateInfo );

		return openglSurface;
	}

	SysGLSurfaceHandle SysGLSubsystem::createDisplaySurfaceForCurrentThread()
	{
		auto openglSurface = sysCreateObject<SysGLSurface>( getHandle<SysGLSubsystem>() );

		_sysCreateDisplaySurfaceForCurrentThread( *openglSurface );

		return openglSurface;
	}

	SysGLContextHandle SysGLSubsystem::createRenderContext( SysGLSurface & pSurface, const SysGLContextCreateInfo & pCreateInfo )
	{
		SysGLContextCreateInfo validatedCreateInfo = pCreateInfo;

		auto & targetAPIVersion = pCreateInfo.requiredAPIVersion;
		if( ( targetAPIVersion == cvVersionInvalid  ) || ( targetAPIVersion == cvVersionUnknown  ) )
		{
			validatedCreateInfo.requiredAPIVersion.major = 1;
			validatedCreateInfo.requiredAPIVersion.minor = 0;
		}

		auto openglContext = sysCreateObject<SysGLContext>( getHandle<SysGLSubsystem>() );

		_sysCreateRenderContext( *openglContext, pSurface, pCreateInfo );

		return openglContext;
	}

	SysGLContextHandle SysGLSubsystem::createRenderContextForCurrentThread()
	{
		auto openglContext = sysCreateObject<SysGLContext>( getHandle<SysGLSubsystem>() );

		_sysCreateRenderContextForCurrentThread( *openglContext );

		return openglContext;
	}

	void SysGLSubsystem::setPrimaryContext( SysGLContext & pPrimaryContext )
	{
		_primaryContext = &pPrimaryContext;
	}

	void SysGLSubsystem::resetPrimaryContext()
	{
		_primaryContext = nullptr;
	}

	std::vector<SysDepthStencilFormat> SysGLSubsystem::querySupportedDepthStencilFormats( SysColorFormat pColorFormat )
	{
		return {};
	}

	std::vector<SysMSAAMode> SysGLSubsystem::querySupportedMSAAModes( SysColorFormat pColorFormat, SysDepthStencilFormat pDepthStencilFormat )
	{
		return {};
	}

	SysGLContext * SysGLSubsystem::getPrimaryContext() const
	{
		return _primaryContext;
	}

}
