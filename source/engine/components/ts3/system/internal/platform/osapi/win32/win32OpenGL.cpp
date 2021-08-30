
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


	void GLSystemDriverNativeImpl::_nativeInitializePlatform()
	{
	    auto & openglInitState = mNativeData.initState;

	    WindowCreateInfo tempWindowCreateInfo;
	    tempWindowCreateInfo.properties.geometry.position = cvWindowPositionOrigin;
	    tempWindowCreateInfo.properties.geometry.size = WindowSize( 600, 600 );
	    tempWindowCreateInfo.properties.geometry.frameStyle = WindowFrameStyle::Overlay;

	    VisualConfig legacyVisualConfig;
	    legacyVisualConfig = visualGetDefaultVisualConfigForSysWindow();
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

	void GLSystemDriverNativeImpl::_nativeReleaseInitState( GLRenderContext & pRenderContext )
	{
	    auto & openglInitState = mNativeData.initState;

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

	void GLSystemDriverNativeImpl::_nativeCreateDisplaySurface( GLDisplaySurface & pDisplaySurface, const GLDisplaySurfaceCreateInfo & pCreateInfo )
	{
	    auto * displaySurfaceNative = pDisplaySurface.getInterface<GLDisplaySurfaceNativeImpl>();

	    WindowCreateInfo surfaceWindowCreateInfo;
	    surfaceWindowCreateInfo.properties.geometry = pCreateInfo.windowGeometry;
	    surfaceWindowCreateInfo.properties.title = "TS3 OpenGL Window";

	    nativeWin32CreateWindow( displaySurfaceNative->mNativeData, surfaceWindowCreateInfo );
	    _win32CreateGLSurface( displaySurfaceNative->mNativeData, pCreateInfo.visualConfig );

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

	    ::ShowWindow( displaySurfaceNative->mNativeData.hwnd, SW_SHOWNORMAL );

	    // TODO: Workaround to properly work with current engine's implementation. That should be turned into an explicit flag.
	    if( pCreateInfo.flags.isSet( E_GL_DISPLAY_SURFACE_CREATE_FLAG_FULLSCREEN_BIT ) )
	    {
	        ::SetCapture( displaySurfaceNative->mNativeData.hwnd );
	        ::ShowCursor( FALSE );
	    }
	}

	void GLSystemDriverNativeImpl::_nativeCreateDisplaySurfaceForCurrentThread( GLDisplaySurface & pDisplaySurface )
	{
	    auto * displaySurfaceNative = pDisplaySurface.getInterface<GLDisplaySurfaceNativeImpl>();
	    
	    auto hdc = ::wglGetCurrentDC();
	    if ( hdc == nullptr )
	    {
	        throw 0;
	    }

	    displaySurfaceNative->mNativeData.hdc = hdc;
	    displaySurfaceNative->mNativeData.hwnd = ::WindowFromDC( hdc );
	    displaySurfaceNative->mNativeData.pixelFormatIndex = ::GetPixelFormat( hdc );

	    CHAR surfaceWindowClassName[256];
	    ::GetClassNameA( displaySurfaceNative->mNativeData.hwnd, surfaceWindowClassName, 255 );
	    auto wndProcModuleHandle = ::GetWindowLongPtrA( displaySurfaceNative->mNativeData.hwnd, GWLP_HINSTANCE );

	    displaySurfaceNative->mNativeData.wndClsName = surfaceWindowClassName;
	    displaySurfaceNative->mNativeData.moduleHandle = reinterpret_cast<HMODULE>( wndProcModuleHandle );
	}

	void GLSystemDriverNativeImpl::_nativeCreateRenderContext( GLRenderContext & pRenderContext, const GLDisplaySurface & pDisplaySurface, const GLRenderContextCreateInfo & pCreateInfo )
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
	            auto * shareContextNative = pCreateInfo.shareContext->getInterface<GLRenderContextNativeImpl>();
	            shareContextHandle = shareContextNative->mNativeData.contextHandle;
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

	    auto * displaySurfaceNative = pDisplaySurface.getInterface<GLDisplaySurfaceNativeImpl>();
	    HGLRC contextHandle = wglCreateContextAttribsARB( displaySurfaceNative->mNativeData.hdc,
                                                          shareContextHandle,
                                                          &( contextAttribs[0] ) );

	    if ( contextHandle == nullptr )
	    {
	        throw 0;
	    }

	    auto * renderContextNative = pRenderContext.getInterface<GLRenderContextNativeImpl>();
	    renderContextNative->mNativeData.contextHandle = contextHandle;
	}

	void GLSystemDriverNativeImpl::_nativeCreateRenderContextForCurrentThread( GLRenderContext & pRenderContext )
	{
	    auto * renderContextNative = pRenderContext.getInterface<GLRenderContextNativeImpl>();

	    auto contextHandle = ::wglGetCurrentContext();
	    if ( contextHandle == nullptr )
	    {
	        throw 0;
	    }
	    renderContextNative->mNativeData.contextHandle = contextHandle;
	}

	bool GLSystemDriverNativeImpl::_nativeIsRenderContextBound() const
	{
	    auto currentContext = ::wglGetCurrentContext();
	    return currentContext != nullptr;
	}

	bool GLSystemDriverNativeImpl::_nativeIsRenderContextBound( const GLRenderContext & pRenderContext ) const
	{
	    auto * renderContextNative = pRenderContext.getInterface<GLRenderContextNativeImpl>();

	    auto currentContext = ::wglGetCurrentContext();
	    return currentContext == renderContextNative->mNativeData.contextHandle;
	}


	void GLDisplaySurfaceNativeImpl::_nativeSwapBuffers()
	{
	    ::SwapBuffers( mNativeData.hdc );
	}

	void GLDisplaySurfaceNativeImpl::_nativeDestroy()
	{
	    _win32DestroyGLSurface( mNativeData );
	}

	void GLDisplaySurfaceNativeImpl::_nativeQueryRenderAreaSize( WindowSize & pOutSize ) const
	{
	    RECT clientRect;
	    ::GetClientRect( mNativeData.hwnd, &clientRect );

	    pOutSize.x = clientRect.right - clientRect.left;
	    pOutSize.y = clientRect.bottom - clientRect.top;
	}

	bool GLDisplaySurfaceNativeImpl::_nativeIsValid() const
	{
	    return mNativeData.hdc != nullptr;
	}


	void GLRenderContextNativeImpl::_nativeBindForCurrentThread( const GLDisplaySurface & pDisplaySurface )
	{
	    auto * displaySurfaceNative = pDisplaySurface.getInterface<GLDisplaySurfaceNativeImpl>();
	    ::wglMakeCurrent( displaySurfaceNative->mNativeData.hdc, mNativeData.contextHandle );
	}

	void GLRenderContextNativeImpl::_nativeDestroy()
	{
	    if( mNativeData.contextHandle != nullptr )
	    {
	        if( _nativeIsCurrent() )
	        {
	            // Explicitly unbind the context if it's currently bound.
	            // TODO: What about other threads? What if a context is destroyed in one thread while other uses it?
	            ::wglMakeCurrent( nullptr, nullptr );
	        }

	        ::wglDeleteContext( mNativeData.contextHandle );

	        mNativeData.contextHandle = nullptr;
	    }
	}

	bool GLRenderContextNativeImpl::_nativeIsCurrent() const
	{
	    auto currentContext = ::wglGetCurrentContext();
	    return mNativeData.contextHandle == currentContext;
	}

	bool GLRenderContextNativeImpl::_nativeIsValid() const
	{
	    return mNativeData.contextHandle != nullptr;
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
