
#include "openGLDriver.h"
#include "displaySystem.h"

namespace ts3::system
{

	OpenGLSystemDriver::OpenGLSystemDriver( DisplayManagerHandle pDisplayManager )
	: SysObject( pDisplayManager->mSysContext )
	, mDisplayManager( std::move( pDisplayManager ) )
	{}

	OpenGLSystemDriver::~OpenGLSystemDriver() noexcept = default;

	OpenGLVersionSupportInfo OpenGLSystemDriver::initializePlatform()
	{
		_nativeInitializePlatform();

		_versionSupportInfo = _nativeQueryVersionSupportInfo();

		return _versionSupportInfo;
	}

	const OpenGLVersionSupportInfo & OpenGLSystemDriver::getVersionSupportInfo() const
	{
		return _versionSupportInfo;
	}

	void OpenGLSystemDriver::releaseInitState( OpenGLRenderContext & /* pGLRenderContext */ )
	{
		_nativeReleaseInitState();
	}

	OpenGLDisplaySurfaceHandle OpenGLSystemDriver::createDisplaySurface( const OpenGLDisplaySurfaceCreateInfo & pCreateInfo )
	{
		OpenGLDisplaySurfaceCreateInfo surfaceCreateInfo = pCreateInfo;

		if( pCreateInfo.flags.isSet( E_OPENGL_DISPLAY_SURFACE_CREATE_FLAG_FULLSCREEN_BIT ) )
		{
			surfaceCreateInfo.frameGeometry.size = CX_FRAME_SIZE_MAX;
			surfaceCreateInfo.frameGeometry.style = EFrameStyle::Overlay;
		}
		else
		{
			surfaceCreateInfo.frameGeometry.position = pCreateInfo.frameGeometry.position;
			surfaceCreateInfo.frameGeometry.size = pCreateInfo.frameGeometry.size;
			surfaceCreateInfo.frameGeometry.style = pCreateInfo.frameGeometry.style;
		}

		if( pCreateInfo.minimumAPIVersion == CX_GL_VERSION_BEST_SUPPORTED )
		{
			const auto versionSupportInfo = _nativeQueryVersionSupportInfo();
			surfaceCreateInfo.minimumAPIVersion = versionSupportInfo.apiVersion;
		}

		surfaceCreateInfo.frameGeometry = mDisplayManager->validateFrameGeometry( surfaceCreateInfo.frameGeometry );

		auto displaySurface = _nativeCreateDisplaySurface( surfaceCreateInfo );
		displaySurface->setInternalOwnershipFlag( true );

		return displaySurface;
	}

	OpenGLDisplaySurfaceHandle OpenGLSystemDriver::createDisplaySurfaceForCurrentThread()
	{
		auto displaySurface = _nativeCreateDisplaySurfaceForCurrentThread();
		displaySurface->setInternalOwnershipFlag( false );

		return displaySurface;
	}

	OpenGLRenderContextHandle OpenGLSystemDriver::createRenderContext( OpenGLDisplaySurface & pSurface,
                                                                       const OpenGLRenderContextCreateInfo & pCreateInfo )
	{
		OpenGLRenderContextCreateInfo contextCreateInfo = pCreateInfo;

		if( contextCreateInfo.requestedAPIVersion == CX_VERSION_UNKNOWN  )
		{
			contextCreateInfo.requestedAPIVersion = Version{ 1, 0 };
		}
		else if( contextCreateInfo.requestedAPIVersion == CX_GL_VERSION_BEST_SUPPORTED )
		{
			contextCreateInfo.requestedAPIVersion = _versionSupportInfo.apiVersion;
		}

		const auto surfaceAPIClass = pSurface.querySupportedAPIClass();
		if( surfaceAPIClass == EOpenGLAPIClass::OpenGLES )
		{
			if( contextCreateInfo.requestedAPIVersion > CX_GL_VERSION_MAX_ES )
			{
				return nullptr;
			}
		}
		else
		{
			if( contextCreateInfo.requestedAPIVersion > CX_GL_VERSION_MAX_DESKTOP )
			{
				return nullptr;
			}
		}

		if( contextCreateInfo.requestedAPIVersion == Version{ 3, 1 } )
		{
			if( ( contextCreateInfo.contextAPIProfile == EOpenGLAPIProfile::Auto ) || ( contextCreateInfo.contextAPIProfile == EOpenGLAPIProfile::Core ) )
			{
				contextCreateInfo.flags.set( E_OPENGL_RENDER_CONTEXT_CREATE_FLAG_FORWARD_COMPATIBLE_BIT );
			}
			else
			{
				contextCreateInfo.flags.unset( E_OPENGL_RENDER_CONTEXT_CREATE_FLAG_FORWARD_COMPATIBLE_BIT );
			}
		}

		if( contextCreateInfo.requestedAPIVersion >= Version{ 3, 2 } )
		{
			if( contextCreateInfo.contextAPIProfile == EOpenGLAPIProfile::Auto )
			{
				contextCreateInfo.contextAPIProfile = EOpenGLAPIProfile::Core;
			}
		}

		auto renderContext = _nativeCreateRenderContext( pSurface, contextCreateInfo );
		renderContext->setInternalOwnershipFlag( true );

		return renderContext;
	}

	OpenGLRenderContextHandle OpenGLSystemDriver::createRenderContextForCurrentThread()
	{
		auto renderContext = _nativeCreateRenderContextForCurrentThread();
		renderContext->setInternalOwnershipFlag( false );

		return renderContext;
	}

	std::vector<EDepthStencilFormat> OpenGLSystemDriver::querySupportedDepthStencilFormats( EColorFormat pColorFormat ) const
	{
		return _nativeQuerySupportedDepthStencilFormats( pColorFormat);
	}

	std::vector<EMSAAMode> OpenGLSystemDriver::querySupportedMSAAModes( EColorFormat pColorFormat,
	                                                                     EDepthStencilFormat pDepthStencilFormat ) const
	{
		return _nativeQuerySupportedMSAAModes( pColorFormat, pDepthStencilFormat );
	}

	void OpenGLSystemDriver::resetContextBinding()
	{
		_nativeResetContextBinding();
	}

	bool OpenGLSystemDriver::isAPIClassSupported( EOpenGLAPIClass pAPIClass ) const
	{
		return _nativeIsAPIClassSupported( pAPIClass );
	}

	bool OpenGLSystemDriver::isRenderContextBound() const
	{
		return _nativeIsRenderContextBound();
	}

	void OpenGLSystemDriver::releaseSystemDisplaySurface( OpenGLDisplaySurface & pDisplaySurface ) noexcept
	{
		try
		{
			if( pDisplaySurface.hasInternalOwnershipFlag() )
			{
				if( pDisplaySurface.sysValidate() )
				{
					_nativeDestroyDisplaySurface( pDisplaySurface );
				}

				pDisplaySurface.setInternalOwnershipFlag( false );
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

	void OpenGLSystemDriver::releaseSystemRenderContext( OpenGLRenderContext & pRenderContext ) noexcept
	{
		try
		{
			if( pRenderContext.hasInternalOwnershipFlag() )
			{
				if( pRenderContext.sysValidate() )
				{
					_nativeDestroyRenderContext( pRenderContext );
				}

				pRenderContext.setInternalOwnershipFlag( false );
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

	OpenGLVersionSupportInfo OpenGLSystemDriver::_nativeQueryVersionSupportInfo() const noexcept
	{
		OpenGLVersionSupportInfo openGLVersionSupportInfo{};

		openGLVersionSupportInfo.apiVersion = OpenGLCoreAPI::queryRuntimeVersion();
		openGLVersionSupportInfo.apiClass = EOpenGLAPIClass::OpenGLDesktop;
		openGLVersionSupportInfo.apiProfile = EOpenGLAPIProfile::Legacy;

		if( openGLVersionSupportInfo.apiVersion.major == 0 )
		{
			openGLVersionSupportInfo.apiVersion.major = 1;
			openGLVersionSupportInfo.apiVersion.minor = 0;
		}

		return openGLVersionSupportInfo;
	}


	OpenGLDisplaySurface::OpenGLDisplaySurface( OpenGLSystemDriverHandle pGLSystemDriver, void * pNativeData )
	: Frame( pGLSystemDriver->mSysContext )
	, mGLSystemDriver( std::move( pGLSystemDriver ) )
	{
		setEventSourceNativeData( pNativeData );
	}

	OpenGLDisplaySurface::~OpenGLDisplaySurface() noexcept
	{
		resetEventSourceNativeData();
	}

	void OpenGLDisplaySurface::clearColorBuffer()
	{
		glClearColor( 0.24f, 0.72f, 0.4f, 1.0f );
		ts3OpenGLHandleLastError();

		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		ts3OpenGLHandleLastError();
	}

	void OpenGLDisplaySurface::swapBuffers()
	{
		_nativeSwapBuffers();
	}

	EOpenGLAPIClass OpenGLDisplaySurface::querySupportedAPIClass() const
	{
		return _nativeQuerySupportedAPIClass();
	}

	FrameSize OpenGLDisplaySurface::queryRenderAreaSize() const
	{
		return _nativeQueryRenderAreaSize();
	}

	bool OpenGLDisplaySurface::sysValidate() const
	{
		return _nativeSysValidate();
	}

	void OpenGLDisplaySurface::resizeClientArea( const FrameSize & pSize )
	{
		FrameGeometry newFrameGeometry{};
		newFrameGeometry.position = CX_FRAME_POS_AUTO;
		newFrameGeometry.size = pSize;
		newFrameGeometry.style = EFrameStyle::Unspecified;

		newFrameGeometry = mGLSystemDriver->mDisplayManager->validateFrameGeometry( newFrameGeometry );

		const auto updateFlags = E_FRAME_GEOMETRY_UPDATE_FLAG_POSITION_BIT | E_FRAME_GEOMETRY_UPDATE_FLAG_SIZE_CLIENT_AREA_BIT;
		_nativeUpdateGeometry( newFrameGeometry, updateFlags );
	}

	void OpenGLDisplaySurface::resizeFrame( const FrameSize & pSize )
	{
		FrameGeometry newFrameGeometry{};
		newFrameGeometry.position = CX_FRAME_POS_AUTO;
		newFrameGeometry.size = pSize;
		newFrameGeometry.style = EFrameStyle::Unspecified;

		newFrameGeometry = mGLSystemDriver->mDisplayManager->validateFrameGeometry( newFrameGeometry );

		const auto updateFlags = E_FRAME_GEOMETRY_UPDATE_FLAG_POSITION_BIT | E_FRAME_GEOMETRY_UPDATE_FLAG_SIZE_OUTER_RECT_BIT;
		_nativeUpdateGeometry( newFrameGeometry, updateFlags );
	}

	void OpenGLDisplaySurface::setFullscreenMode( bool pEnable )
	{
		_nativeSetFullscreenMode( pEnable );
	}

	void OpenGLDisplaySurface::setTitle( const std::string & pTitleText )
	{
		_nativeSetTitle( pTitleText );
	}

	void OpenGLDisplaySurface::updateGeometry( const FrameGeometry & pFrameGeometry,
	                                           Bitmask<EFrameGeometryUpdateFlags> pUpdateFlags )
	{
		_nativeUpdateGeometry( pFrameGeometry, pUpdateFlags );
	}

	FrameSize OpenGLDisplaySurface::getClientAreaSize() const
	{
		return _nativeGetSize( EFrameSizeMode::ClientArea );
	}

	FrameSize OpenGLDisplaySurface::getFrameSize() const
	{
		return _nativeGetSize( EFrameSizeMode::OuterRect );
	}

	bool OpenGLDisplaySurface::isFullscreen() const
	{
		return _nativeIsFullscreen();
	}

	void OpenGLDisplaySurface::onDestroySystemObjectRequested()
	{
		EventSource::onDestroySystemObjectRequested();

		mGLSystemDriver->releaseSystemDisplaySurface( *this );
	}

	void OpenGLDisplaySurface::setInternalOwnershipFlag( bool pOwnershipFlag )
	{
		_internalOwnershipFlag = pOwnershipFlag;
	}

	bool OpenGLDisplaySurface::hasInternalOwnershipFlag() const
	{
		return _internalOwnershipFlag;
	}


	OpenGLRenderContext::OpenGLRenderContext( OpenGLSystemDriverHandle pDriver )
	: SysObject( pDriver->mSysContext )
	, mGLSystemDriver( std::move( pDriver ) )
	{}

	OpenGLRenderContext::~OpenGLRenderContext() noexcept = default;

	void OpenGLRenderContext::bindForCurrentThread( const OpenGLDisplaySurface & pTargetSurface )
	{
		_nativeBindForCurrentThread( pTargetSurface );
	}

	bool OpenGLRenderContext::sysCheckIsCurrent() const
	{
		return _nativeSysCheckIsCurrent();
	}

	bool OpenGLRenderContext::sysValidate() const
	{
		return _nativeSysValidate();
	}

	OpenGLSystemVersionInfo OpenGLRenderContext::querySystemVersionInfo() const
	{
		if( !sysValidate() )
		{
			ts3Throw( E_EXC_DEBUG_PLACEHOLDER );
		}

		OpenGLSystemVersionInfo systemVersionInfo;
		systemVersionInfo.apiVersion = OpenGLCoreAPI::queryRuntimeVersion();

		if( const auto * versionStr = glGetString( GL_VERSION ) )
		{
			systemVersionInfo.apiVersionStr.assign( reinterpret_cast<const char *>( versionStr ) );
		}
		if( const auto * glslVersionStr = glGetString( GL_SHADING_LANGUAGE_VERSION ) )
		{
			systemVersionInfo.glslVersionStr.assign( reinterpret_cast<const char *>( glslVersionStr ) );
		}
		if( const auto * rendererNameStr = glGetString( GL_RENDERER ) )
		{
			systemVersionInfo.rendererName.assign( reinterpret_cast<const char *>( rendererNameStr ) );
		}
		if( const auto * vendorNameStr = glGetString( GL_VENDOR ) )
		{
			systemVersionInfo.vendorName.assign( reinterpret_cast<const char *>( vendorNameStr ) );
		}

		ts3OpenGLResetErrorQueue();

		return systemVersionInfo;
	}

	void OpenGLRenderContext::onDestroySystemObjectRequested()
	{
		mGLSystemDriver->releaseSystemRenderContext( *this );
	}

	void OpenGLRenderContext::setInternalOwnershipFlag( bool pOwnershipFlag )
	{
		_internalOwnershipFlag = pOwnershipFlag;
	}

	bool OpenGLRenderContext::hasInternalOwnershipFlag() const
	{
		return _internalOwnershipFlag;
	}

} // namespace ts3::system
