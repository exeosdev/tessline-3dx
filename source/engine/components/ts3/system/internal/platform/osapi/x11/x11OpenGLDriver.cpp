
#include "x11OpenGLDriver.h"
#include "x11DisplaySystem.h"

#define ts3X11OpenGLContextAttribAppend( pArray, pIndex, pAttrib ) \
	ts3DebugAssert( pIndex < CX_X11_MAX_GLX_FBCONFIG_ATTRIBUTES_NUM ); \
	pArray[pIndex++] = pAttrib

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_X11 )
namespace ts3::system
{

	namespace platform
	{

		PFNGLXCREATECONTEXTATTRIBSARBPROC _x11QueryGLXCreateContextAttribsProc();

		PFNGLXSWAPINTERVALEXTPROC _x11QueryGLXSwapIntervalProc();

		// Creates X11 OpenGL surface using provided window create attributes and visual config.
		void _x11CreateGLWindowAndSurface( X11OpenGLDisplaySurfaceNativeData & pGLSurfaceNativeData,
		                                   X11WindowCreateInfo & pWindowCreateInfo,
		                                   const VisualConfig & pVisualConfig );

		// Destroys existing surface and corresponding window.
		void _x11DestroyGLWindowAndSurface( X11OpenGLDisplaySurfaceNativeData & pGLSurfaceNativeData );

		// Destroys existing context.
		void _x11DestroyGLContext( X11OpenGLRenderContextNativeData & pGLContextNativeData );

		// Destroys existing surface and corresponding window.
		void _x11CreateAndBindLegacyRenderContext( X11OpenGLRenderContextNativeData & pGLContextNativeData,
		                                           const X11OpenGLDisplaySurfaceNativeData & pGLSurfaceNativeData );

		// Selects matching FBConfig for a GL surface. Format is selected using current system's configuration. Uses legacy API.
		X11OpenGLVisualConfig _x11ChooseLegacyGLFBConfig( XDisplay pDisplay, int pScreenIndex );

		// Selects matching pixel format for surface described with a VisualConfig. Uses new EXT API and supports stuff like MSAA.
		X11OpenGLVisualConfig _x11ChooseCoreGLFBConfig( XDisplay pDisplay, int pScreenIndex, const VisualConfig & pVisualConfig );

		// Returns an array of FBConfigs matching specified VisualConfig definition.
		GLXFBConfigArray _x11QueryCompatibleFBConfigList( XDisplay pDisplay,
		                                                  int pScreenIndex,
		                                                  const VisualConfig & pVisualConfig );

		// Computes a "compatibility rate", i.e. how much the specified FBConfig matches the visual.
		int _x11GetFBConfigMatchRate( XDisplay pDisplay, GLXFBConfig pFBConfig, const VisualConfig & pVisualConfig );

		// Translation: VisualConfig --> array of GLX_* attributes required by the system API.
		// Used for surface/context creation.
		void _x11GetAttribArrayForVisualConfig( const VisualConfig & pVisualConfig, int * pAttribArray );

	}


	X11OpenGLSystemDriver::X11OpenGLSystemDriver( X11DisplayManagerHandle pDisplayManager )
	: X11NativeObject( std::move( pDisplayManager ) )
	{}

	X11OpenGLSystemDriver::~X11OpenGLSystemDriver() noexcept
	{
		_nativeReleaseInitState();
	}

	void X11OpenGLSystemDriver::_nativeInitializePlatform()
	{
		ts3DebugAssert( !mNativeData.initState );
		// Init state should be first created here and destroyed as soon as proper GL
		// contexts are created (this is not enforce, though, and controlled explicitly
		// by the user and done by calling releaseInitState() method od the driver).
		mNativeData.initState = std::make_unique<platform::X11OpenGLSystemDriverNativeData::InitState>();

		auto & xSessionData = platform::x11GetXSessionData( *this );

		int glxVersionMajor = 0;
		int glxVersionMinor = 0;

		::glXQueryVersion( xSessionData.display, &glxVersionMajor, &glxVersionMinor );
		// We need at least version 1.3 of the GLX runtime (that's rather a pretty old one...).
		if( ( glxVersionMajor <= 0 ) || ( ( glxVersionMajor == 1 ) && ( glxVersionMinor < 3 ) ) )
		{
			ts3Throw( E_EXC_DEBUG_PLACEHOLDER );
		}

		VisualConfig legacyVisualConfig;
		legacyVisualConfig = vsxGetDefaultVisualConfigForSysWindow();
		legacyVisualConfig.flags.set( E_VISUAL_ATTRIB_FLAG_LEGACY_BIT );

		platform::X11WindowCreateInfo windowCreateInfo;
		windowCreateInfo.frameGeometry.position = { 0, 0 };
		windowCreateInfo.frameGeometry.size = { 600, 600 };
		windowCreateInfo.frameGeometry.style = EFrameStyle::Overlay;

		auto & tmpSurfaceNativeData = mNativeData.initState->surfaceData;
		tmpSurfaceNativeData.setSessionData( xSessionData );
		platform::_x11CreateGLWindowAndSurface( tmpSurfaceNativeData, windowCreateInfo, legacyVisualConfig );

		auto & tmpContextNativeData = mNativeData.initState->contextData;
		tmpContextNativeData.setSessionData( xSessionData );
		platform::_x11CreateAndBindLegacyRenderContext( tmpContextNativeData, tmpSurfaceNativeData );
	}

	void X11OpenGLSystemDriver::_nativeReleaseInitState() noexcept
	{
		if( !mNativeData.initState )
		{
			return;
		}

		auto & xSessionData = platform::x11GetXSessionData( *this );

		auto & tmpSurfaceNativeData = mNativeData.initState->surfaceData;
		auto & tmpContextNativeData = mNativeData.initState->contextData;

		if( tmpContextNativeData.contextHandle != nullptr )
		{
			::glXDestroyContext( xSessionData.display, tmpContextNativeData.contextHandle );
			tmpContextNativeData.contextHandle = nullptr;
			tmpContextNativeData.resetSessionData();
		}

		if( tmpSurfaceNativeData.windowXID != platform::E_X11_XID_NONE )
		{
			platform::_x11DestroyGLWindowAndSurface( tmpSurfaceNativeData );
			tmpSurfaceNativeData.fbConfig = nullptr;
			tmpSurfaceNativeData.resetSessionData();
		}

		if( tmpSurfaceNativeData.visualInfo != nullptr )
		{
			XFree( tmpSurfaceNativeData.visualInfo );
			tmpSurfaceNativeData.visualInfo = nullptr;
		}

		mNativeData.initState.reset();
	}

	OpenGLDisplaySurfaceHandle X11OpenGLSystemDriver::_nativeCreateDisplaySurface( const OpenGLDisplaySurfaceCreateInfo & pCreateInfo )
	{
		auto displaySurface = createSysObject<X11OpenGLDisplaySurface>( getHandle<X11OpenGLSystemDriver>() );

		platform::X11WindowCreateInfo x11WindowCreateInfo;
		x11WindowCreateInfo.frameGeometry = pCreateInfo.frameGeometry;
		x11WindowCreateInfo.title = "TS3 OpenGL Window";
		x11WindowCreateInfo.fullscreenMode = pCreateInfo.flags.isSet( E_OPENGL_DISPLAY_SURFACE_CREATE_FLAG_FULLSCREEN_BIT );

		platform::_x11CreateGLWindowAndSurface( displaySurface->mNativeData, x11WindowCreateInfo, pCreateInfo.visualConfig );

		platform::x11WindowPostCreateUpdate( displaySurface->mNativeData, x11WindowCreateInfo );

		if( auto glXSwapIntervalEXTProc = platform::_x11QueryGLXSwapIntervalProc() )
		{
			auto & xSessionData = platform::x11GetXSessionData( *this );

			if( pCreateInfo.flags.isSet( E_OPENGL_DISPLAY_SURFACE_CREATE_FLAG_SYNC_DISABLED_BIT ) )
			{
				glXSwapIntervalEXTProc( xSessionData.display, displaySurface->mNativeData.windowXID, 0 );
			}
			else if( pCreateInfo.flags.isSet( E_OPENGL_DISPLAY_SURFACE_CREATE_FLAG_SYNC_ADAPTIVE_BIT ) )
			{
				glXSwapIntervalEXTProc( xSessionData.display, displaySurface->mNativeData.windowXID, -1 );
			}
			else if( pCreateInfo.flags.isSet( E_OPENGL_DISPLAY_SURFACE_CREATE_FLAG_SYNC_VERTICAL_BIT ) )
			{
				glXSwapIntervalEXTProc( xSessionData.display, displaySurface->mNativeData.windowXID, 1 );
			}
		}

		return displaySurface;
	}

	OpenGLDisplaySurfaceHandle X11OpenGLSystemDriver::_nativeCreateDisplaySurfaceForCurrentThread()
	{
		auto & xSessionData = platform::x11GetXSessionData( *this );

		auto currentWindowXID = glXGetCurrentDrawable();

		XWindowAttributes windowAttributes;
		XGetWindowAttributes( xSessionData.display, currentWindowXID, &windowAttributes );

		auto displaySurface = createSysObject<X11OpenGLDisplaySurface>( getHandle<X11OpenGLSystemDriver>() );

		displaySurface->mNativeData.windowXID = currentWindowXID;
		displaySurface->mNativeData.xColormap = windowAttributes.colormap;

		return displaySurface;
	}

	void X11OpenGLSystemDriver::_nativeDestroyDisplaySurface( OpenGLDisplaySurface & pDisplaySurface )
	{
	}

	OpenGLRenderContextHandle X11OpenGLSystemDriver::_nativeCreateRenderContext( OpenGLDisplaySurface & pDisplaySurface,
	                                                                             const OpenGLRenderContextCreateInfo & pCreateInfo )
	{
		auto & xSessionData = platform::x11GetXSessionData( *this );

		auto * x11DisplaySurface = pDisplaySurface.queryInterface<X11OpenGLDisplaySurface>();

		auto glXCreateContextAttribsProc = platform::_x11QueryGLXCreateContextAttribsProc();

		int contextAPIProfile = 0;
		Bitmask<int> contextCreateFlags = 0;
		GLXContext shareContextHandle = nullptr;

		if( pCreateInfo.contextAPIProfile == EOpenGLAPIProfile::Core )
		{
			contextAPIProfile = GLX_CONTEXT_CORE_PROFILE_BIT_ARB;
		}
		else if( pCreateInfo.contextAPIProfile == EOpenGLAPIProfile::Legacy )
		{
			contextAPIProfile = GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB;
		}
		else if( pCreateInfo.contextAPIProfile == EOpenGLAPIProfile::GLES )
		{
			contextAPIProfile = GLX_CONTEXT_ES_PROFILE_BIT_EXT;
		}

		if( pCreateInfo.flags.isSet( E_OPENGL_RENDER_CONTEXT_CREATE_FLAG_ENABLE_DEBUG_BIT ) )
		{
			contextCreateFlags |= GLX_CONTEXT_DEBUG_BIT_ARB;
		}
		if( pCreateInfo.flags.isSet( E_OPENGL_RENDER_CONTEXT_CREATE_FLAG_FORWARD_COMPATIBLE_BIT ) )
		{
			contextCreateFlags |= GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB;
		}
		if( pCreateInfo.flags.isSet( E_OPENGL_RENDER_CONTEXT_CREATE_FLAG_ENABLE_SHARING_BIT ) )
		{
			if( pCreateInfo.shareContext )
			{
				auto * x11ShareContext = pCreateInfo.shareContext->queryInterface<X11OpenGLRenderContext>();
				shareContextHandle = x11ShareContext->mNativeData.contextHandle;
			}
			else if( pCreateInfo.flags.isSet( E_OPENGL_RENDER_CONTEXT_CREATE_FLAG_SHARE_WITH_CURRENT_BIT ) )
			{
				if( auto * currentGLXContext = ::glXGetCurrentContext() )
				{
					shareContextHandle = currentGLXContext;
				}
			}
		}

		const int contextAttributes[] =
		{
			// Requested OpenGL API version: major part
			GLX_CONTEXT_MAJOR_VERSION_ARB, pCreateInfo.runtimeVersionDesc.apiVersion.major,
			// Requested OpenGL API version: minor part
			GLX_CONTEXT_MINOR_VERSION_ARB, pCreateInfo.runtimeVersionDesc.apiVersion.minor,
			//
			GLX_CONTEXT_PROFILE_MASK_ARB, contextAPIProfile,
			//
			GLX_CONTEXT_FLAGS_ARB, contextCreateFlags,
			// Terminator
			FALSE
		};

		GLXContext contextHandle = glXCreateContextAttribsProc( xSessionData.display,
		                                                        x11DisplaySurface->mNativeData.fbConfig,
		                                                        shareContextHandle,
		                                                        True,
		                                                        &( contextAttributes[0] ) );

		if( !contextHandle )
		{
			ts3Throw( E_EXC_DEBUG_PLACEHOLDER );
		}

		auto renderContext = createSysObject<X11OpenGLRenderContext>( getHandle<X11OpenGLSystemDriver>() );
		renderContext->mNativeData.contextHandle = contextHandle;

		return renderContext;
	}

	OpenGLRenderContextHandle X11OpenGLSystemDriver::_nativeCreateRenderContextForCurrentThread()
	{
		auto contextHandle = ::glXGetCurrentContext();
		if( contextHandle == nullptr )
		{
			ts3Throw( E_EXC_DEBUG_PLACEHOLDER );
		}

		auto renderContext = createSysObject<X11OpenGLRenderContext>( getHandle<X11OpenGLSystemDriver>() );
		renderContext->mNativeData.contextHandle = contextHandle;

		return renderContext;
	}

	void X11OpenGLSystemDriver::_nativeDestroyRenderContext( OpenGLRenderContext & pRenderContext )
	{
		auto * x11RenderContext = pRenderContext.queryInterface<X11OpenGLRenderContext>();
		platform::_x11DestroyGLContext( x11RenderContext->mNativeData );
	}

	void X11OpenGLSystemDriver::_nativeResetContextBinding()
	{
		auto & xSessionData = platform::x11GetXSessionData( *this );
		::glXMakeContextCurrent( xSessionData.display, platform::E_X11_XID_NONE, platform::E_X11_XID_NONE, nullptr );
	}

	std::vector<EDepthStencilFormat> X11OpenGLSystemDriver::_nativeQuerySupportedDepthStencilFormats( EColorFormat pColorFormat ) const
	{
		return {};
	}

	std::vector<EMSAAMode> X11OpenGLSystemDriver::_nativeQuerySupportedMSAAModes( EColorFormat pColorFormat,
	                                                                              EDepthStencilFormat pDepthStencilFormat ) const
	{
		return {};
	}

	bool X11OpenGLSystemDriver::_nativeIsAPIClassSupported( EOpenGLAPIClass pAPIClass ) const
	{
		if( pAPIClass == EOpenGLAPIClass::OpenGL )
		{
			return true;
		}
		return false;
	}

	bool X11OpenGLSystemDriver::_nativeIsRenderContextBound() const
	{
		auto currentContext = ::glXGetCurrentContext();
		return currentContext != nullptr;
	}


	X11OpenGLDisplaySurface::X11OpenGLDisplaySurface( X11OpenGLSystemDriverHandle pGLSystemDriver )
	: X11NativeObject( std::move( pGLSystemDriver ), &mNativeData )
	{}

	X11OpenGLDisplaySurface::~X11OpenGLDisplaySurface() noexcept
	{
		_releaseX11SurfaceState();
	}

	void X11OpenGLDisplaySurface::_releaseX11SurfaceState()
	{
		platform::_x11DestroyGLWindowAndSurface( mNativeData );
	}

	void X11OpenGLDisplaySurface::_nativeSwapBuffers()
	{
		auto & xSessionData = platform::x11GetXSessionData( *this );
		glXSwapBuffers( xSessionData.display, mNativeData.windowXID );
	}

	FrameSize X11OpenGLDisplaySurface::_nativeQueryRenderAreaSize() const
	{
		return platform::x11GetFrameSize( mNativeData, EFrameSizeMode::ClientArea );
	}

	bool X11OpenGLDisplaySurface::_nativeSysValidate() const
	{
		return ( mNativeData.windowXID != platform::E_X11_XID_NONE ) && mNativeData.fbConfig;
	}

	void X11OpenGLDisplaySurface::_nativeResize( const FrameSize & pFrameSize, EFrameSizeMode pSizeMode )
	{}

	void X11OpenGLDisplaySurface::_nativeSetFullscreenMode( bool pEnable )
	{
		platform::x11SetWindowFullscreenState( mNativeData, pEnable );
	}

	void X11OpenGLDisplaySurface::_nativeSetTitle( const std::string & pTitle )
	{
		return platform::x11SetFrameTitle( mNativeData, pTitle );
	}

	void X11OpenGLDisplaySurface::_nativeUpdateGeometry( const FrameGeometry & pFrameGeometry,
	                                                     Bitmask<EFrameGeometryUpdateFlags> pUpdateFlags )
	{
		return platform::x11UpdateFrameGeometry( mNativeData, pFrameGeometry, pUpdateFlags );
	}

	FrameSize X11OpenGLDisplaySurface::_nativeGetSize( EFrameSizeMode pSizeMode ) const
	{
		return platform::x11GetFrameSize( mNativeData, pSizeMode );
	}

	bool X11OpenGLDisplaySurface::_nativeIsFullscreen() const
	{
		return platform::x11IsFullscreenWindow( mNativeData );
	}


	X11OpenGLRenderContext::X11OpenGLRenderContext( X11OpenGLSystemDriverHandle pGLSystemDriver )
	: X11NativeObject( std::move( pGLSystemDriver ) )
	{}

	X11OpenGLRenderContext::~X11OpenGLRenderContext() noexcept
	{
		_releaseX11ContextState();
	}

	void X11OpenGLRenderContext::_nativeBindForCurrentThread( const OpenGLDisplaySurface & pTargetSurface )
	{
		auto & xSessionData = platform::x11GetXSessionData( *this );

		const auto * x11DisplaySurface = pTargetSurface.queryInterface<X11OpenGLDisplaySurface>();

		::glXMakeContextCurrent( xSessionData.display,
		                         x11DisplaySurface->mNativeData.windowXID,
		                         x11DisplaySurface->mNativeData.windowXID,
		                         mNativeData.contextHandle );
	}

	bool X11OpenGLRenderContext::_nativeSysCheckIsCurrent() const
	{
		auto currentContextHandle = glXGetCurrentContext();
		return mNativeData.contextHandle && ( mNativeData.contextHandle == currentContextHandle );
	}

	bool X11OpenGLRenderContext::_nativeSysValidate() const
	{
		return mNativeData.contextHandle != nullptr;
	}

	void X11OpenGLRenderContext::_releaseX11ContextState()
	{
		platform::_x11DestroyGLContext( mNativeData );
	}

	
	namespace platform
	{

		PFNGLXCREATECONTEXTATTRIBSARBPROC _x11QueryGLXCreateContextAttribsProc()
		{
			static PFNGLXCREATECONTEXTATTRIBSARBPROC cvGLXCreateContextAttribsProc = nullptr;

			if( !cvGLXCreateContextAttribsProc )
			{
				auto procAddress = glXGetProcAddressARB( (const GLubyte *) "glXCreateContextAttribsARB" );
				cvGLXCreateContextAttribsProc = reinterpret_cast<PFNGLXCREATECONTEXTATTRIBSARBPROC>( procAddress );
			}

			return cvGLXCreateContextAttribsProc;
		}

		PFNGLXSWAPINTERVALEXTPROC _x11QueryGLXSwapIntervalProc()
		{
			static PFNGLXSWAPINTERVALEXTPROC cvGLXSwapIntervalEXTProc = nullptr;

			if( !cvGLXSwapIntervalEXTProc )
			{
				auto procAddress = glXGetProcAddressARB( (const GLubyte *) "glXSwapIntervalEXT" );
				cvGLXSwapIntervalEXTProc = reinterpret_cast<PFNGLXSWAPINTERVALEXTPROC>( procAddress );
			}

			return cvGLXSwapIntervalEXTProc;
		}

		void _x11CreateGLWindowAndSurface( X11OpenGLDisplaySurfaceNativeData & pGLSurfaceNativeData,
		                                   X11WindowCreateInfo & pWindowCreateInfo,
		                                   const VisualConfig & pVisualConfig )
		{
			auto & xSessionData = platform::x11GetXSessionData( pGLSurfaceNativeData );

			X11OpenGLVisualConfig x11GLVisualConfig{};
			if( pVisualConfig.flags.isSet( E_VISUAL_ATTRIB_FLAG_LEGACY_BIT ) )
			{
				x11GLVisualConfig = _x11ChooseLegacyGLFBConfig( xSessionData.display, xSessionData.screenIndex );
			}
			else
			{
				x11GLVisualConfig = _x11ChooseCoreGLFBConfig( xSessionData.display, xSessionData.screenIndex, pVisualConfig );
			}

			if( !x11GLVisualConfig )
			{
				ts3Throw( E_EXC_DEBUG_PLACEHOLDER );
			}

			pGLSurfaceNativeData.fbConfig = x11GLVisualConfig.fbConfig;
			pGLSurfaceNativeData.visualInfo = x11GLVisualConfig.xVisualInfo;

			pWindowCreateInfo.colorDepth = x11GLVisualConfig.xVisualInfo->depth;
			pWindowCreateInfo.windowVisual = x11GLVisualConfig.xVisualInfo->visual;

			platform::x11CreateWindow( pGLSurfaceNativeData, pWindowCreateInfo );
		}

		void _x11DestroyGLWindowAndSurface( X11OpenGLDisplaySurfaceNativeData & pGLSurfaceNativeData )
		{
			platform::x11DestroyWindow( pGLSurfaceNativeData );
		}

		void _x11DestroyGLContext( X11OpenGLRenderContextNativeData & pGLContextNativeData )
		{
			auto & xSessionData = platform::x11GetXSessionData( pGLContextNativeData );
			if( pGLContextNativeData.contextHandle != nullptr )
			{
				auto currentContextHandle = ::glXGetCurrentContext();
				if( pGLContextNativeData.contextHandle == currentContextHandle )
				{
					::glXMakeContextCurrent( xSessionData.display, E_X11_XID_NONE, E_X11_XID_NONE, nullptr );
				}

				::glXDestroyContext( xSessionData.display, pGLContextNativeData.contextHandle );
				pGLContextNativeData.contextHandle = nullptr;
			}
		}

		void _x11CreateAndBindLegacyRenderContext( X11OpenGLRenderContextNativeData & pGLContextNativeData,
		                                           const X11OpenGLDisplaySurfaceNativeData & pGLSurfaceNativeData )
		{
			auto & xSessionData = platform::x11GetXSessionData( pGLSurfaceNativeData );

			auto tempContextHandle = ::glXCreateContext( xSessionData.display, pGLSurfaceNativeData.visualInfo, nullptr, True );
			if( tempContextHandle == nullptr )
			{
				ts3Throw( E_EXC_DEBUG_PLACEHOLDER );
			}

			pGLContextNativeData.contextHandle = tempContextHandle;

			auto makeCurrentResult = ::glXMakeCurrent( xSessionData.display, pGLSurfaceNativeData.windowXID, tempContextHandle );
			if( makeCurrentResult == False )
			{
				ts3Throw( E_EXC_DEBUG_PLACEHOLDER );
			}
		}

		X11OpenGLVisualConfig _x11ChooseLegacyGLFBConfig( XDisplay pDisplay, int pScreenIndex )
		{
			const int cvDefaultVisualAttribs[] =
			{
				GLX_X_RENDERABLE  , True,
				GLX_DRAWABLE_TYPE , GLX_WINDOW_BIT,
				GLX_RENDER_TYPE   , GLX_RGBA_BIT,
				GLX_X_VISUAL_TYPE , GLX_TRUE_COLOR,
				GLX_RED_SIZE      , 8,
				GLX_GREEN_SIZE    , 8,
				GLX_BLUE_SIZE     , 8,
				GLX_ALPHA_SIZE    , 8,
				GLX_DEPTH_SIZE    , 24,
				GLX_STENCIL_SIZE  , 8,
				GLX_DOUBLEBUFFER  , True,
				E_X11_XID_NONE,
			};

			const int cvDefaultVisualAttribsNoDepthStencil[] =
			{
				GLX_X_RENDERABLE  , True,
				GLX_DRAWABLE_TYPE , GLX_WINDOW_BIT,
				GLX_RENDER_TYPE   , GLX_RGBA_BIT,
				GLX_X_VISUAL_TYPE , GLX_TRUE_COLOR,
				GLX_RED_SIZE      , 8,
				GLX_GREEN_SIZE    , 8,
				GLX_BLUE_SIZE     , 8,
				GLX_ALPHA_SIZE    , 8,
				GLX_DOUBLEBUFFER  , True,
				E_X11_XID_NONE,
			};

			int fbConfigListSize = 0;
			auto * fbConfigList = glXChooseFBConfig( pDisplay, pScreenIndex, cvDefaultVisualAttribs, &fbConfigListSize );

			if( ( fbConfigList == nullptr ) || ( fbConfigListSize == 0 ) )
			{
				fbConfigList = glXChooseFBConfig( pDisplay, pScreenIndex, cvDefaultVisualAttribsNoDepthStencil, &fbConfigListSize );
				if( ( fbConfigList == nullptr ) || ( fbConfigListSize == 0 ) )
				{
					ts3Throw( E_EXC_DEBUG_PLACEHOLDER );
				}
			}

			GLXFBConfig bestFBConfig = nullptr;
			GLXFBConfig bestFBConfigRGB8 = nullptr;

			for( int fbcIndex = 0; fbcIndex < fbConfigListSize; ++fbcIndex )
			{
				GLXFBConfig fbConfig = fbConfigList[fbcIndex];
				XVisualInfo * fbcVisualInfo = glXGetVisualFromFBConfig( pDisplay, fbConfig );

				if( fbcVisualInfo != nullptr )
				{
					if( ( fbcVisualInfo->bits_per_rgb == 8 ) && ( bestFBConfigRGB8 == nullptr ) )
					{
						bestFBConfigRGB8 = fbConfig;
					}
					else if( bestFBConfig == nullptr )
					{
						bestFBConfig = fbConfig;
					}
					XFree( fbcVisualInfo );
				}

				if( bestFBConfigRGB8 != nullptr )
				{
					bestFBConfig = bestFBConfigRGB8;
					break;
				}
			}

			XFree( fbConfigList );

			X11OpenGLVisualConfig x11GLVisualConfig;
			x11GLVisualConfig.fbConfig = bestFBConfig;
			x11GLVisualConfig.xVisualInfo = glXGetVisualFromFBConfig( pDisplay, bestFBConfig );

			return x11GLVisualConfig;
		}

		X11OpenGLVisualConfig _x11ChooseCoreGLFBConfig( XDisplay pDisplay, int pScreenIndex, const VisualConfig & pVisualConfig )
		{
			auto fbConfigList = _x11QueryCompatibleFBConfigList( pDisplay, pScreenIndex, pVisualConfig );

			int bestMatchRate = 0;
			GLXFBConfig bestFBConfig = nullptr;

			for( auto fbConfig : fbConfigList )
			{
				int matchRate = _x11GetFBConfigMatchRate( pDisplay, fbConfig, pVisualConfig );
				if( matchRate > bestMatchRate )
				{
					bestMatchRate = matchRate;
					bestFBConfig = fbConfig;
				}
			}

			X11OpenGLVisualConfig x11GLVisualConfig;
			x11GLVisualConfig.fbConfig = bestFBConfig;
			x11GLVisualConfig.xVisualInfo = glXGetVisualFromFBConfig( pDisplay, bestFBConfig );

			return x11GLVisualConfig;
		}

		GLXFBConfigArray _x11QueryCompatibleFBConfigList( XDisplay pDisplay, int pScreenIndex, const VisualConfig & pVisualConfig )
		{
			GLXFBConfigArray result;

			int fbConfigAttribArray[CX_X11_MAX_GLX_FBCONFIG_ATTRIBUTES_NUM];
			_x11GetAttribArrayForVisualConfig( pVisualConfig, fbConfigAttribArray );

			int fbConfigListSize = 0;
			auto * fbConfigList = glXChooseFBConfig( pDisplay, pScreenIndex, fbConfigAttribArray, &fbConfigListSize );

			if( ( fbConfigList == nullptr ) || ( fbConfigListSize == 0 ) )
			{
				ts3Throw( E_EXC_DEBUG_PLACEHOLDER );
			}

			for( int pFBConfigIndex = 0; pFBConfigIndex < fbConfigListSize; ++pFBConfigIndex )
			{
				auto pFBConfig = fbConfigList[pFBConfigIndex];
				auto * visualInfo = glXGetVisualFromFBConfig( pDisplay, pFBConfig );

				if( visualInfo != nullptr )
				{
					int fbConfigAlphaSize = 0;
					glXGetFBConfigAttrib( pDisplay, pFBConfig, GLX_ALPHA_SIZE, &fbConfigAlphaSize );

					XFree( visualInfo );

					if( fbConfigAlphaSize > 0 )
					{
						result.push_back( pFBConfig );
					}
				}
			}

			XFree( fbConfigList );

			return result;
		}

		int _x11GetFBConfigMatchRate( XDisplay pDisplay, GLXFBConfig pFBConfig, const VisualConfig & pVisualConfig )
		{
			int doubleBufferRequestedState = True;
			int stereoModeRequestedState = False;

			if( pVisualConfig.flags.isSet( E_VISUAL_ATTRIB_FLAG_SINGLE_BUFFER_BIT ) &&
			    !pVisualConfig.flags.isSet( E_VISUAL_ATTRIB_FLAG_DOUBLE_BUFFER_BIT ) )
			{
				doubleBufferRequestedState = False;
			}

			if( pVisualConfig.flags.isSet( E_VISUAL_ATTRIB_FLAG_STEREO_DISPLAY_BIT ) &&
			    !pVisualConfig.flags.isSet( E_VISUAL_ATTRIB_FLAG_MONO_DISPLAY_BIT ) )
			{
				stereoModeRequestedState = True;
			}

			int matchRate = 0;
			int fbConfigAttribValue = 0;

			glXGetFBConfigAttrib( pDisplay, pFBConfig, GLX_BUFFER_SIZE, &fbConfigAttribValue );
			matchRate += ( fbConfigAttribValue == 32 );

			glXGetFBConfigAttrib( pDisplay, pFBConfig, GLX_DOUBLEBUFFER, &fbConfigAttribValue );
			matchRate += ( fbConfigAttribValue == doubleBufferRequestedState );

			glXGetFBConfigAttrib( pDisplay, pFBConfig, GLX_STEREO, &fbConfigAttribValue );
			matchRate += ( fbConfigAttribValue == stereoModeRequestedState );

			glXGetFBConfigAttrib( pDisplay, pFBConfig, GLX_DEPTH_SIZE, &fbConfigAttribValue );
			matchRate += ( fbConfigAttribValue == pVisualConfig.depthStencilDesc.depthBufferSize );

			glXGetFBConfigAttrib( pDisplay, pFBConfig, GLX_STENCIL_SIZE, &fbConfigAttribValue );
			matchRate += ( fbConfigAttribValue == pVisualConfig.depthStencilDesc.stencilBufferSize );

			glXGetFBConfigAttrib( pDisplay, pFBConfig, GLX_SAMPLES, &fbConfigAttribValue );
			matchRate += ( fbConfigAttribValue == pVisualConfig.msaaDesc.quality );

			auto * fbConfigXVisualInfo =  glXGetVisualFromFBConfig( pDisplay, pFBConfig );
			matchRate += ( fbConfigXVisualInfo->bits_per_rgb == 8 );
			XFree( fbConfigXVisualInfo );

			return matchRate;
		}

		void _x11GetAttribArrayForVisualConfig( const VisualConfig & pVisualConfig, int * pAttribArray )
		{
			int attribIndex = 0;

			ts3X11OpenGLContextAttribAppend( pAttribArray, attribIndex, GLX_DRAWABLE_TYPE );
			ts3X11OpenGLContextAttribAppend( pAttribArray, attribIndex, GLX_WINDOW_BIT );

			ts3X11OpenGLContextAttribAppend( pAttribArray, attribIndex, GLX_RENDER_TYPE );
			ts3X11OpenGLContextAttribAppend( pAttribArray, attribIndex, GLX_RGBA_BIT );

			ts3X11OpenGLContextAttribAppend( pAttribArray, attribIndex, GLX_X_RENDERABLE );
			ts3X11OpenGLContextAttribAppend( pAttribArray, attribIndex, True );

			ts3X11OpenGLContextAttribAppend( pAttribArray, attribIndex, GLX_X_VISUAL_TYPE );
			ts3X11OpenGLContextAttribAppend( pAttribArray, attribIndex, GLX_TRUE_COLOR );

			if( pVisualConfig.flags.isSet( E_VISUAL_ATTRIB_FLAG_DOUBLE_BUFFER_BIT ) )
			{
				pAttribArray[attribIndex++] = GLX_DOUBLEBUFFER;
				pAttribArray[attribIndex++] = True;
			}
			else if( pVisualConfig.flags.isSet( E_VISUAL_ATTRIB_FLAG_SINGLE_BUFFER_BIT ) )
			{
				pAttribArray[attribIndex++] = GLX_DOUBLEBUFFER;
				pAttribArray[attribIndex++] = False;
			}

			if( pVisualConfig.flags.isSet( E_VISUAL_ATTRIB_FLAG_MONO_DISPLAY_BIT ) )
			{
				pAttribArray[attribIndex++] = GLX_STEREO;
				pAttribArray[attribIndex++] = False;
			}
			else if( pVisualConfig.flags.isSet( E_VISUAL_ATTRIB_FLAG_STEREO_DISPLAY_BIT ) )
			{
				pAttribArray[attribIndex++] = GLX_STEREO;
				pAttribArray[attribIndex++] = True;
			}

			if( ( pVisualConfig.msaaDesc.bufferCount != 0 ) && ( pVisualConfig.msaaDesc.quality != 0 ) )
			{
				pAttribArray[attribIndex++] = GLX_SAMPLE_BUFFERS;
				pAttribArray[attribIndex++] = pVisualConfig.msaaDesc.bufferCount;

				pAttribArray[attribIndex++] = GLX_SAMPLES;
				pAttribArray[attribIndex++] = pVisualConfig.msaaDesc.quality;
			}

			if( pVisualConfig.colorDesc.rgba.u32Code != 0 )
			{
				pAttribArray[attribIndex++] = GLX_RED_SIZE;
				pAttribArray[attribIndex++] = pVisualConfig.colorDesc.rgba.u8Red;

				pAttribArray[attribIndex++] = GLX_GREEN_SIZE;
				pAttribArray[attribIndex++] = pVisualConfig.colorDesc.rgba.u8Green;

				pAttribArray[attribIndex++] = GLX_BLUE_SIZE;
				pAttribArray[attribIndex++] = pVisualConfig.colorDesc.rgba.u8Blue;

				pAttribArray[attribIndex++] = GLX_ALPHA_SIZE;
				pAttribArray[attribIndex++] = pVisualConfig.colorDesc.rgba.u8Alpha;
			}

			if( pVisualConfig.depthStencilDesc.depthBufferSize != 0 )
			{
				pAttribArray[attribIndex++] = GLX_DEPTH_SIZE;
				pAttribArray[attribIndex++] = pVisualConfig.depthStencilDesc.depthBufferSize;
			}

			if( pVisualConfig.depthStencilDesc.stencilBufferSize != 0 )
			{
				pAttribArray[attribIndex++] = GLX_STENCIL_SIZE;
				pAttribArray[attribIndex++] = pVisualConfig.depthStencilDesc.stencilBufferSize;
			}

			pAttribArray[attribIndex++] = E_X11_XID_NONE;
		}

	}

} // namespace ts3::system
#endif // TS3_PCL_TARGET_SYSAPI_X11
