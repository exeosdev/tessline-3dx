
#include <ts3/system/openGLNative.h>
#include <ts3/system/windowNative.h>

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_WIN32 )
namespace ts3::system
{


	// Creates Win32 OpenGL surface using provided visual config.
	void _win32CreateGLSurface( GLDisplaySurfaceNativeData & pDisplaySurfaceNativeData, const VisualConfig & pVisualConfig );

	// Destroys existing surface.
	void _win32DestroyGLSurface( GLDisplaySurfaceNativeData & pDisplaySurfaceNativeData );

	// Selects matching pixel format for surface described with a PFD. Uses legacy API.
	int _win32ChooseLegacyGLPixelFormat( HDC pDisplaySurfaceDC, PIXELFORMATDESCRIPTOR & pPfmtDescriptor );

	// Selects matching pixel format for surface described with a VisualConfig. Uses new EXT API and supports stuff like MSAA.
	int _win32ChooseCoreGLPixelFormat( HDC pDisplaySurfaceDC, const VisualConfig & pVisualConfig, PIXELFORMATDESCRIPTOR & pPfmtDescriptor );

	// Returns an array of pixel format IDs matching specified set of Win32 PF attributes.
	std::vector<int> _win32QueryCompatiblePixelFormatList( HDC pDisplaySurfaceDC, int * pPixelFormatAttribList = nullptr );

	// Returns an array of pixel format IDs matching specified VisualConfig structure.
	std::vector<int> _win32QueryCompatiblePixelFormatList( HDC pDisplaySurfaceDC, const VisualConfig & pVisualConfig );

	// Computes a "compatibility rate", i.e. how much the specified pixel format matches the visual.
	int _win32GetPixelFormatMatchRate( HDC pDisplaySurfaceDC, int pPixelFormatIndex, const VisualConfig & pVisualConfig );

	// Translation: VisualConfig --> array of WGL_* attributes required by the system API. Used for surface/context creation.
	void _win32GetWGLAttribArrayForVisualConfig( const VisualConfig & pVisualConfig, int * pAttribArray );

	// Useful utility. Fetches values for a set of PF attributes. Uses templated array definition to resolve the size.
	template <size_t tSize, typename TpOutput>
	inline bool _win32QueryPixelFormatAttributes( HDC pDisplaySurfaceDC, int pPixelFormatIndex, const int( &pAttributes )[tSize], TpOutput & pOutput )
	{
		BOOL result = wglGetPixelFormatAttribivARB( pDisplaySurfaceDC, pPixelFormatIndex, 0, tSize, pAttributes, &( pOutput[0] ) );
		return result != FALSE;
	}


	void GLSystemDriver::_nativeInitializePlatform()
	{
	    auto & openglInitState = mPrivate->nativeDataPriv.initState;

	    WindowCreateInfo tempWindowCreateInfo;
	    tempWindowCreateInfo.properties.geometry.position = cvWindowPositionOrigin;
	    tempWindowCreateInfo.properties.geometry.size = WindowSize( 600, 600 );
	    tempWindowCreateInfo.properties.geometry.frameStyle = WindowFrameStyle::Overlay;

	    VisualConfig legacyVisualConfig;
	    legacyVisualConfig = vsxGetDefaultVisualConfigForSysWindow();
	    legacyVisualConfig.flags.set( VISUAL_ATTRIB_FLAG_LEGACY_BIT );

	    // Create a surface window. In case of Win32, GLDisplaySurfaceNativeData inherits from WindowNativeData
	    // for this very reason: to allow treating surfaces as windows (as that's exactly the case on desktop).
	    nativeWin32CreateWindow( openglInitState.surfaceData, tempWindowCreateInfo );

	    // Create a surface. This sets up the PFD and configures HDC properly.
	    _win32CreateGLSurface( openglInitState.surfaceData, legacyVisualConfig );

	    // Create legacy OpenGL context for initialization phase.
	    openglInitState.contextData.contextHandle = ::wglCreateContext( openglInitState.surfaceData.hdc );
	    if ( openglInitState.contextData.contextHandle == nullptr )
	    {
	        throw 0;
	    }

	    // Bind context as current, so GL calls may be used normally.
	    BOOL makeCurrentResult = ::wglMakeCurrent( openglInitState.surfaceData.hdc,
                                                   openglInitState.contextData.contextHandle );

	    if ( makeCurrentResult == FALSE )
	    {
	        throw 0;
	    }

	    auto glewResult = glewInit();
	    if ( glewResult != GLEW_OK )
	    {
	        throw 0;
	    }

	    glewResult = wglewInit();
	    if ( glewResult != GLEW_OK )
	    {
	        throw 0;
	    }
	}

	void GLSystemDriver::_nativeReleaseInitState( GLRenderContext & pRenderContext )
	{
	    auto & openglInitState = mPrivate->nativeDataPriv.initState;

	    if( openglInitState.contextData.contextHandle != nullptr )
	    {
	        ::wglDeleteContext( openglInitState.contextData.contextHandle );
	        openglInitState.contextData.contextHandle = nullptr;
	    }

	    if( openglInitState.surfaceData.hdc != nullptr )
	    {
	        _win32DestroyGLSurface( openglInitState.surfaceData );
	        openglInitState.surfaceData.hdc = nullptr;
	        openglInitState.surfaceData.pixelFormatIndex = cvWin32InvalidPixelFormatIndex;
	    }

	    if( openglInitState.surfaceData.hwnd != nullptr )
	    {
	        nativeWin32DestroyWindow( openglInitState.surfaceData );
	        openglInitState.surfaceData.hwnd = nullptr;
	    }
	}

	void GLSystemDriver::_nativeCreateDisplaySurface( GLDisplaySurface & pDisplaySurface, const GLDisplaySurfaceCreateInfo & pCreateInfo )
	{
	    WindowCreateInfo surfaceWindowCreateInfo;
	    surfaceWindowCreateInfo.properties.geometry = pCreateInfo.windowGeometry;
	    surfaceWindowCreateInfo.properties.title = "TS3 OpenGL Window";

	    nativeWin32CreateWindow( pDisplaySurface.mPrivate->nativeDataPriv, surfaceWindowCreateInfo );
	    _win32CreateGLSurface( pDisplaySurface.mPrivate->nativeDataPriv, pCreateInfo.visualConfig );

	    if( pCreateInfo.flags.isSet( E_GL_DISPLAY_SURFACE_CREATE_FLAG_SYNC_ADAPTIVE_BIT ) )
	    {
	        wglSwapIntervalEXT( -1 );
	    }
	    else if( pCreateInfo.flags.isSet( E_GL_DISPLAY_SURFACE_CREATE_FLAG_SYNC_VERTICAL_BIT ) )
	    {
	        wglSwapIntervalEXT( 1 );
	    }
	    else
	    {
	        wglSwapIntervalEXT( 0 );
	    }

	    ::ShowWindow( pDisplaySurface.mPrivate->nativeDataPriv.hwnd, SW_SHOWNORMAL );

	    // TODO: Workaround to properly work with current engine's implementation. That should be turned into an explicit flag.
	    if( pCreateInfo.flags.isSet( E_GL_DISPLAY_SURFACE_CREATE_FLAG_FULLSCREEN_BIT ) )
	    {
	        ::SetCapture( pDisplaySurface.mPrivate->nativeDataPriv.hwnd );
	        ::ShowCursor( FALSE );
	    }
	}

	void GLSystemDriver::_nativeCreateDisplaySurfaceForCurrentThread( GLDisplaySurface & pDisplaySurface )
	{
	    auto hdc = ::wglGetCurrentDC();
	    if ( hdc == nullptr )
	    {
	        throw 0;
	    }

	    pDisplaySurface.mPrivate->nativeDataPriv.hdc = hdc;
	    pDisplaySurface.mPrivate->nativeDataPriv.hwnd = ::WindowFromDC( hdc );
	    pDisplaySurface.mPrivate->nativeDataPriv.pixelFormatIndex = ::GetPixelFormat( hdc );

	    CHAR surfaceWindowClassName[256];
	    ::GetClassNameA( pDisplaySurface.mPrivate->nativeDataPriv.hwnd, surfaceWindowClassName, 255 );
	    auto wndProcModuleHandle = ::GetWindowLongPtrA( pDisplaySurface.mPrivate->nativeDataPriv.hwnd, GWLP_HINSTANCE );

	    pDisplaySurface.mPrivate->nativeDataPriv.wndClsName = surfaceWindowClassName;
	    pDisplaySurface.mPrivate->nativeDataPriv.moduleHandle = reinterpret_cast<HMODULE>( wndProcModuleHandle );
	}

	void GLSystemDriver::_nativeCreateRenderContext( GLRenderContext & pRenderContext, const GLDisplaySurface & pDisplaySurface, const GLRenderContextCreateInfo & pCreateInfo )
	{
	    int contextProfile = WGL_CONTEXT_CORE_PROFILE_BIT_ARB;
	    Bitmask<int> contextCreateFlags = 0;
	    HGLRC shareContextHandle = nullptr;

	    if ( pCreateInfo.targetAPIProfile == EGLAPIProfile::GLES )
	    {
	        contextProfile = WGL_CONTEXT_ES_PROFILE_BIT_EXT;
	    }
	    if ( pCreateInfo.targetAPIProfile == EGLAPIProfile::Legacy )
	    {
	        contextProfile = WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB;
	    }
	    if ( pCreateInfo.flags.isSet( E_GL_RENDER_CONTEXT_CREATE_FLAG_ENABLE_DEBUG_BIT ) )
	    {
	        contextCreateFlags |= WGL_CONTEXT_DEBUG_BIT_ARB;
	    }
	    if ( pCreateInfo.flags.isSet( E_GL_RENDER_CONTEXT_CREATE_FLAG_FORWARD_COMPATIBLE_BIT ) )
	    {
	        contextCreateFlags |= WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB;
	    }
	    if ( pCreateInfo.flags.isSet( E_GL_RENDER_CONTEXT_CREATE_FLAG_ENABLE_SHARING_BIT ) )
	    {
	        if( pCreateInfo.shareContext )
	        {
	            shareContextHandle = pCreateInfo.shareContext->mPrivate->nativeDataPriv.contextHandle;
	        }
	    }

	    const int contextAttribs[] =
        {
            // Requested OpenGL API version: major part
            WGL_CONTEXT_MAJOR_VERSION_ARB, pCreateInfo.requiredAPIVersion.major,
            // Requested OpenGL API version: minor part
            WGL_CONTEXT_MINOR_VERSION_ARB, pCreateInfo.requiredAPIVersion.minor,
            //
            WGL_CONTEXT_PROFILE_MASK_ARB, contextProfile,
            //
            WGL_CONTEXT_FLAGS_ARB, contextCreateFlags,
            // Terminator
            FALSE
        };

	    HGLRC contextHandle = wglCreateContextAttribsARB( pDisplaySurface.mPrivate->nativeDataPriv.hdc,
                                                          shareContextHandle,
                                                          &( contextAttribs[0] ) );

	    if ( contextHandle == nullptr )
	    {
	        throw 0;
	    }

	    pRenderContext.mPrivate->nativeDataPriv.contextHandle = contextHandle;
	}

	void GLSystemDriver::_nativeCreateRenderContextForCurrentThread( GLRenderContext & pRenderContext )
	{
	    auto contextHandle = ::wglGetCurrentContext();
	    if ( contextHandle == nullptr )
	    {
	        throw 0;
	    }
	    pRenderContext.mPrivate->nativeDataPriv.contextHandle = contextHandle;
	}

	bool GLSystemDriver::_nativeIsRenderContextBound() const
	{
	    auto currentContext = ::wglGetCurrentContext();
	    return currentContext != nullptr;
	}

	bool GLSystemDriver::_nativeIsRenderContextBound( const GLRenderContext & pRenderContext ) const
	{
	    auto currentContext = ::wglGetCurrentContext();
	    return currentContext == pRenderContext.mPrivate->nativeDataPriv.contextHandle;
	}


	void GLDisplaySurface::_nativeSwapBuffers()
	{
	    ::SwapBuffers( mPrivate->nativeDataPriv.hdc );
	}

	void GLDisplaySurface::_nativeDestroy()
	{
	    _win32DestroyGLSurface( mPrivate->nativeDataPriv );
	}

	void GLDisplaySurface::_nativeQueryRenderAreaSize( WindowSize & pOutSize ) const
	{
	    RECT clientRect;
	    ::GetClientRect( mPrivate->nativeDataPriv.hwnd, &clientRect );

	    pOutSize.x = clientRect.right - clientRect.left;
	    pOutSize.y = clientRect.bottom - clientRect.top;
	}

	bool GLDisplaySurface::_nativeIsValid() const
	{
	    return mPrivate->nativeDataPriv.hdc != nullptr;
	}


	void GLRenderContext::_nativeBindForCurrentThread( const GLDisplaySurface & pDisplaySurface )
	{
	    ::wglMakeCurrent( pDisplaySurface.mPrivate->nativeDataPriv.hdc,
                          mPrivate->nativeDataPriv.contextHandle );
	}

	void GLRenderContext::_nativeDestroy()
	{
	    if( mPrivate->nativeDataPriv.contextHandle != nullptr )
	    {
	        if( _nativeIsCurrent() )
	        {
	            // Explicitly unbind the context if it's currently bound.
	            // TODO: What about other threads? What if a context is destroyed in one thread while other uses it?
	            ::wglMakeCurrent( nullptr, nullptr );
	        }

	        ::wglDeleteContext( mPrivate->nativeDataPriv.contextHandle );

	        mPrivate->nativeDataPriv.contextHandle = nullptr;
	    }
	}

	bool GLRenderContext::_nativeIsCurrent() const
	{
	    auto currentContext = ::wglGetCurrentContext();
	    return mPrivate->nativeDataPriv.contextHandle == currentContext;
	}

	bool GLRenderContext::_nativeIsValid() const
	{
	    return mPrivate->nativeDataPriv.contextHandle != nullptr;
	}


	void _win32CreateGLSurface( GLDisplaySurfaceNativeData & pDisplaySurfaceNativeData, const VisualConfig & pVisualConfig )
	{
		auto hdc = ::GetWindowDC( pDisplaySurfaceNativeData.hwnd );
		pDisplaySurfaceNativeData.hdc = hdc;

		PIXELFORMATDESCRIPTOR pixelFormatDescriptor;
		memset( &pixelFormatDescriptor, 0, sizeof( PIXELFORMATDESCRIPTOR ) );
		pixelFormatDescriptor.nSize = sizeof( PIXELFORMATDESCRIPTOR );
		pixelFormatDescriptor.nVersion = 1;

		if ( pVisualConfig.flags.isSet( VISUAL_ATTRIB_FLAG_LEGACY_BIT ) )
		{
			pDisplaySurfaceNativeData.pixelFormatIndex = _win32ChooseLegacyGLPixelFormat( hdc, pixelFormatDescriptor );
		}
		else
		{
			pDisplaySurfaceNativeData.pixelFormatIndex = _win32ChooseCoreGLPixelFormat( hdc, pVisualConfig, pixelFormatDescriptor );
		}

		BOOL spfResult = ::SetPixelFormat( hdc, pDisplaySurfaceNativeData.pixelFormatIndex, &pixelFormatDescriptor );
		if ( spfResult == FALSE )
		{
			throw 0;
		}
	}

	void _win32DestroyGLSurface( GLDisplaySurfaceNativeData & pDisplaySurfaceNativeData )
	{
		if ( pDisplaySurfaceNativeData.hdc != nullptr )
		{
			::ReleaseDC( pDisplaySurfaceNativeData.hwnd, pDisplaySurfaceNativeData.hdc );

			pDisplaySurfaceNativeData.hdc = nullptr;
			pDisplaySurfaceNativeData.pixelFormatIndex = 0;
		}
	}

	int _win32ChooseLegacyGLPixelFormat( HDC pDisplaySurfaceDC, PIXELFORMATDESCRIPTOR & pPfmtDescriptor )
	{
		// For legacy pixel format, we first try the most reasonable one:
		// 24-bit color (RGB8) with 8-bit alpha channel and 32-bit combined
		// depth/stencil buffer. That should be the best pick in most cases.
		pPfmtDescriptor.cRedBits = 8;
		pPfmtDescriptor.cGreenBits = 8;
		pPfmtDescriptor.cBlueBits = 8;
		pPfmtDescriptor.cAlphaBits = 8;
		pPfmtDescriptor.cDepthBits = 24;
		pPfmtDescriptor.cStencilBits = 8;
		pPfmtDescriptor.iLayerType = PFD_MAIN_PLANE;
		pPfmtDescriptor.iPixelType = PFD_TYPE_RGBA;
		pPfmtDescriptor.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;

		int pixelFormatIndex = ::ChoosePixelFormat( pDisplaySurfaceDC, &pPfmtDescriptor );

		if ( pixelFormatIndex == 0 )
		{
			pPfmtDescriptor.cColorBits = 24; // 24-bit color, no alpha, no explicit RGB
			pPfmtDescriptor.cDepthBits = 32; // 32-bit depth buffer, no stencil attachment
			pixelFormatIndex = ::ChoosePixelFormat( pDisplaySurfaceDC, &pPfmtDescriptor );
		}

		return ( pixelFormatIndex > 0 ) ? pixelFormatIndex : cvWin32InvalidPixelFormatIndex;
	}

	int _win32ChooseCoreGLPixelFormat( HDC pDisplaySurfaceDC, const VisualConfig & pVisualConfig, PIXELFORMATDESCRIPTOR & pPfmtDescriptor )
	{
		auto pixelFormatList = _win32QueryCompatiblePixelFormatList( pDisplaySurfaceDC, pVisualConfig );

		int bestMatchRate = 0;
		int bestPixelFormatID = 0;

		for ( auto pixelFormatID : pixelFormatList )
		{
			int matchRate = _win32GetPixelFormatMatchRate( pDisplaySurfaceDC, pixelFormatID, pVisualConfig );
			if ( matchRate > bestMatchRate )
			{
				bestMatchRate = matchRate;
				bestPixelFormatID = pixelFormatID;
			}
		}

		int maxPixelFormatIndex = ::DescribePixelFormat( pDisplaySurfaceDC, bestPixelFormatID, sizeof( PIXELFORMATDESCRIPTOR ), &pPfmtDescriptor );

		return ( maxPixelFormatIndex > 0 ) ? bestPixelFormatID : cvWin32InvalidPixelFormatIndex;
	}

	std::vector<int> _win32QueryCompatiblePixelFormatList( HDC pDisplaySurfaceDC, int * pPixelFormatAttribList )
	{
		// Output array where system will store IDs of enumerated pixel formats.
		int pixelFormatArray[cvWin32MaxWGLPixelFormatsNum];
		// Number of pixel formats returned by the system.
		UINT returnedPixelFormatsNum = 0U;

		// Enumerate pixel formats.
		BOOL enumResult = ::wglChoosePixelFormatARB( pDisplaySurfaceDC,
		                                             pPixelFormatAttribList,
		                                             nullptr,
		                                             cvWin32MaxWGLPixelFormatsNum,
		                                             pixelFormatArray,
		                                             &returnedPixelFormatsNum );

		if ( ( enumResult == FALSE ) || ( returnedPixelFormatsNum == 0 ) )
		{
			throw 0;
		}

		std::vector<int> result;
		result.reserve( returnedPixelFormatsNum );
		result.assign( &( pixelFormatArray[0] ), &( pixelFormatArray[returnedPixelFormatsNum] ) );

		return result;
	}

	std::vector<int> _win32QueryCompatiblePixelFormatList( HDC pDisplaySurfaceDC, const VisualConfig & pVisualConfig )
	{
		// Array for WGL_..._ARB pixel format attributes.
		int pixelFormatAttributes[cvWin32MaxWGLPixelFormatAttributesNum * 2];
		// Translate provided visual config to a WGL attribute array.
		_win32GetWGLAttribArrayForVisualConfig( pVisualConfig, pixelFormatAttributes );

		return _win32QueryCompatiblePixelFormatList( pDisplaySurfaceDC, pixelFormatAttributes );
	}

	int _win32GetPixelFormatMatchRate( HDC pDisplaySurfaceDC, int pPixelFormatIndex, const VisualConfig & pVisualConfig )
	{
		int doubleBufferRequestedState = TRUE;
		int stereoModeRequestedState = FALSE;

		if ( pVisualConfig.flags.isSet( VISUAL_ATTRIB_FLAG_SINGLE_BUFFER_BIT ) &&
			!pVisualConfig.flags.isSet( VISUAL_ATTRIB_FLAG_DOUBLE_BUFFER_BIT ) )
		{
			doubleBufferRequestedState = FALSE;
		}

		if ( pVisualConfig.flags.isSet( VISUAL_ATTRIB_FLAG_STEREO_DISPLAY_BIT ) &&
			!pVisualConfig.flags.isSet( VISUAL_ATTRIB_FLAG_MONO_DISPLAY_BIT ) )
		{
			stereoModeRequestedState = TRUE;
		}

		constexpr int controlAttribArray[] =
		{
			WGL_DOUBLE_BUFFER_ARB,
			WGL_STEREO_ARB,
			WGL_DEPTH_BITS_ARB,
			WGL_STENCIL_BITS_ARB,
			WGL_SAMPLES_ARB
		};

		std::array<int, staticArraySize( controlAttribArray )> attribValueArray{};
		if( !_win32QueryPixelFormatAttributes( pDisplaySurfaceDC, pPixelFormatIndex, controlAttribArray, attribValueArray ) )
		{
			return -1;
		}

		int matchRate = 0;
		int attribIndex = 0;

		if ( attribValueArray[attribIndex++] == doubleBufferRequestedState )
		{
			++matchRate;
		}
		if ( attribValueArray[attribIndex++] == stereoModeRequestedState )
		{
			++matchRate;
		}
		if ( attribValueArray[attribIndex++] == pVisualConfig.depthStencilDesc.depthBufferSize )
		{
			++matchRate;
		}
		if ( attribValueArray[attribIndex++] == pVisualConfig.depthStencilDesc.stencilBufferSize )
		{
			++matchRate;
		}
		if ( attribValueArray[attribIndex] == pVisualConfig.msaaDesc.quality )
		{
			++matchRate;
		}

		return matchRate;
	}

	void _win32GetWGLAttribArrayForVisualConfig( const VisualConfig & pVisualConfig, int * pAttribArray )
	{
		int attribIndex = 0;

		pAttribArray[attribIndex++] = WGL_SUPPORT_OPENGL_ARB;
		pAttribArray[attribIndex++] = GL_TRUE;

		pAttribArray[attribIndex++] = WGL_DRAW_TO_WINDOW_ARB;
		pAttribArray[attribIndex++] = GL_TRUE;

		pAttribArray[attribIndex++] = WGL_ACCELERATION_ARB;
		pAttribArray[attribIndex++] = WGL_FULL_ACCELERATION_ARB;

		pAttribArray[attribIndex++] = WGL_PIXEL_TYPE_ARB;
		pAttribArray[attribIndex++] = WGL_TYPE_RGBA_ARB;

		if ( pVisualConfig.flags.isSet( VISUAL_ATTRIB_FLAG_DOUBLE_BUFFER_BIT ) )
		{
			pAttribArray[attribIndex++] = WGL_DOUBLE_BUFFER_ARB;
			pAttribArray[attribIndex++] = GL_TRUE;
		}
		else if ( pVisualConfig.flags.isSet( VISUAL_ATTRIB_FLAG_SINGLE_BUFFER_BIT ) )
		{
			pAttribArray[attribIndex++] = WGL_DOUBLE_BUFFER_ARB;
			pAttribArray[attribIndex++] = GL_FALSE;
		}

		if ( pVisualConfig.flags.isSet( VISUAL_ATTRIB_FLAG_MONO_DISPLAY_BIT ) )
		{
			pAttribArray[attribIndex++] = WGL_STEREO_ARB;
			pAttribArray[attribIndex++] = GL_FALSE;
		}
		else if ( pVisualConfig.flags.isSet( VISUAL_ATTRIB_FLAG_STEREO_DISPLAY_BIT ) )
		{
			pAttribArray[attribIndex++] = WGL_STEREO_ARB;
			pAttribArray[attribIndex++] = GL_TRUE;
		}

		if ( pVisualConfig.flags.isSet( VISUAL_ATTRIB_FLAG_SRGB_CAPABLE_BIT ) )
		{
			pAttribArray[attribIndex++] = WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB;
			pAttribArray[attribIndex++] = GL_TRUE;
		}

		if ( pVisualConfig.colorDesc.rgba.u32Code != 0 )
		{
			pAttribArray[attribIndex++] = WGL_RED_BITS_ARB;
			pAttribArray[attribIndex++] = pVisualConfig.colorDesc.rgba.u8Red;

			pAttribArray[attribIndex++] = WGL_GREEN_BITS_ARB;
			pAttribArray[attribIndex++] = pVisualConfig.colorDesc.rgba.u8Green;

			pAttribArray[attribIndex++] = WGL_BLUE_BITS_ARB;
			pAttribArray[attribIndex++] = pVisualConfig.colorDesc.rgba.u8Blue;

			pAttribArray[attribIndex++] = WGL_ALPHA_BITS_ARB;
			pAttribArray[attribIndex++] = pVisualConfig.colorDesc.rgba.u8Alpha;
		}

		if ( ( pVisualConfig.msaaDesc.bufferCount != 0 ) && ( pVisualConfig.msaaDesc.quality != 0 ) )
		{
			pAttribArray[attribIndex++] = WGL_SAMPLE_BUFFERS_ARB;
			pAttribArray[attribIndex++] = pVisualConfig.msaaDesc.bufferCount;

			pAttribArray[attribIndex++] = WGL_SAMPLES_ARB;
			pAttribArray[attribIndex++] = pVisualConfig.msaaDesc.quality;
		}

		if ( pVisualConfig.depthStencilDesc.depthBufferSize != 0 )
		{
			pAttribArray[attribIndex++] = WGL_DEPTH_BITS_ARB;
			pAttribArray[attribIndex++] = pVisualConfig.depthStencilDesc.depthBufferSize;
		}

		if ( pVisualConfig.depthStencilDesc.stencilBufferSize != 0 )
		{
			pAttribArray[attribIndex++] = WGL_STENCIL_BITS_ARB;
			pAttribArray[attribIndex++] = pVisualConfig.depthStencilDesc.stencilBufferSize;
		}

		// Append zero as a list terminator.
		pAttribArray[attribIndex] = 0;
	}

} // namespace ts3::system
#endif
