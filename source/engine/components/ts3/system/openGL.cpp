
#include "openGL.h"
#include "displayManager.h"

namespace ts3
{

	SysGLSurface::SysGLSurface( SysGLCoreDeviceHandle pGLCoreDevice ) noexcept
	: SysBaseObject( pGLCoreDevice->mSysContext )
	, SysEventSource( mNativeData )
	, mGLCoreDevice( pGLCoreDevice )
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


	SysGLRenderContext::SysGLRenderContext( SysGLCoreDeviceHandle pGLCoreDevice ) noexcept
	: SysBaseObject( pGLCoreDevice->mSysContext )
	, mGLCoreDevice( pGLCoreDevice )
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



	SysGLCoreDevice::SysGLCoreDevice( SysDisplayManagerHandle pDisplayManager ) noexcept
	: SysBaseObject( pDisplayManager->mSysContext )
	, mDisplayManager( pDisplayManager )
	, _primaryContext( nullptr )
	{}

	SysGLCoreDevice::~SysGLCoreDevice() noexcept = default;

	SysGLCoreDeviceHandle SysGLCoreDevice::create( SysDisplayManagerHandle pDisplayManager )
	{
		auto openglSubsystem = sysCreateObject<SysGLCoreDevice>( pDisplayManager );
		return openglSubsystem;
	}

	void SysGLCoreDevice::initializePlatform()
	{
		_sysInitializePlatform();
	}

	void SysGLCoreDevice::releaseInitState( SysGLRenderContext & pGLRenderContext )
	{
		if( pGLRenderContext.mGLCoreDevice.get() != this )
		{
			throw 0;
		}
		_sysReleaseInitState();
	}

	SysGLSurfaceHandle SysGLCoreDevice::createDisplaySurface( const SysGLSurfaceCreateInfo & pCreateInfo )
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

		auto openglSurface = sysCreateObject<SysGLSurface>( getHandle<SysGLCoreDevice>() );

		_sysCreateDisplaySurface( *openglSurface, validatedCreateInfo );

		return openglSurface;
	}

	SysGLSurfaceHandle SysGLCoreDevice::createDisplaySurfaceForCurrentThread()
	{
		auto openglSurface = sysCreateObject<SysGLSurface>( getHandle<SysGLCoreDevice>() );

		_sysCreateDisplaySurfaceForCurrentThread( *openglSurface );

		return openglSurface;
	}

	SysGLRenderContextHandle SysGLCoreDevice::createRenderContext( SysGLSurface & pSurface, const SysGLRenderContextCreateInfo & pCreateInfo )
	{
		SysGLRenderContextCreateInfo validatedCreateInfo = pCreateInfo;

		auto & targetAPIVersion = pCreateInfo.requiredAPIVersion;
		if( ( targetAPIVersion == cvVersionInvalid  ) || ( targetAPIVersion == cvVersionUnknown  ) )
		{
			validatedCreateInfo.requiredAPIVersion.major = 1;
			validatedCreateInfo.requiredAPIVersion.minor = 0;
		}

		auto openglContext = sysCreateObject<SysGLRenderContext>( getHandle<SysGLCoreDevice>() );

		_sysCreateRenderContext( *openglContext, pSurface, validatedCreateInfo );

		return openglContext;
	}

	SysGLRenderContextHandle SysGLCoreDevice::createRenderContextForCurrentThread()
	{
		auto openglContext = sysCreateObject<SysGLRenderContext>( getHandle<SysGLCoreDevice>() );

		_sysCreateRenderContextForCurrentThread( *openglContext );

		return openglContext;
	}

	void SysGLCoreDevice::setPrimaryContext( SysGLRenderContext & pPrimaryContext )
	{
		_primaryContext = &pPrimaryContext;
	}

	void SysGLCoreDevice::resetPrimaryContext()
	{
		_primaryContext = nullptr;
	}

	std::vector<SysDepthStencilFormat> SysGLCoreDevice::querySupportedDepthStencilFormats( SysColorFormat pColorFormat )
	{
		return {};
	}

	std::vector<SysMSAAMode> SysGLCoreDevice::querySupportedMSAAModes( SysColorFormat pColorFormat, SysDepthStencilFormat pDepthStencilFormat )
	{
		return {};
	}

	SysGLRenderContext * SysGLCoreDevice::getPrimaryContext() const
	{
		return _primaryContext;
	}

}
