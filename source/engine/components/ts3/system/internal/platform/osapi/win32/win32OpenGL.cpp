
#include <ts3/system/openGLNative.h>
#include <ts3/system/windowNative.h>

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_WIN32 )
namespace ts3::system
{


	// Creates Win32 OpenGL surface using provided visual config.
	void _win32CreateGLSurface( GLDisplaySurfaceNativeData & pGLSurfaceNativeData, const VisualConfig & pVisualConfig );

	// Destroys existing surface.
	void _win32DestroyGLSurface( GLDisplaySurfaceNativeData & pGLSurfaceNativeData );

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


	void GLSystemDriver::_nativeConstructor()
    {}

    void GLSystemDriver::_nativeDestructor() noexcept
    {}

	void GLSystemDriver::_nativeInitialize()
	{}

	void GLSystemDriver::_nativeRelease()
	{}

	void GLSystemDriver::_nativeInitializePlatform()
	{
	    ts3DebugAssert( !mInternal->nativeDataPriv.initState );
	    // Init state should be first created here and destroyed as soon as proper GL
	    // contexts are created (this is not enforce, though, and controlled explicitly
	    // by the user and done by calling releaseInitState() method od the driver).
	    mInternal->nativeDataPriv.initState = new GLSystemDriverNativeData::InitState();

	    WindowCreateInfo tempWindowCreateInfo;
	    tempWindowCreateInfo.properties.geometry.position = cvWindowPositionOrigin;
	    tempWindowCreateInfo.properties.geometry.size = WindowSize( 600, 600 );
	    tempWindowCreateInfo.properties.geometry.frameStyle = WindowFrameStyle::Overlay;

	    VisualConfig legacyVisualConfig;
	    legacyVisualConfig = vsxGetDefaultVisualConfigForSysWindow();
	    legacyVisualConfig.flags.set( E_VISUAL_ATTRIB_FLAG_LEGACY_BIT );

	    auto & tmpSurfaceNativeData = mInternal->nativeDataPriv.initState->surfaceData;

	    // Create a surface window. In case of Win32, GLDisplaySurfaceNativeData inherits from WindowNativeData
	    // for this very reason: to allow treating surfaces as windows (as that's exactly the case on desktop).
	    nativeWin32CreateWindow( tmpSurfaceNativeData, tempWindowCreateInfo );

	    // Create a surface. This sets up the PFD and configures HDC properly.
	    _win32CreateGLSurface( tmpSurfaceNativeData, legacyVisualConfig );

	    auto & tmpContextNativeData = mInternal->nativeDataPriv.initState->contextData;
	    
	    // Create legacy OpenGL context for initialization phase.
	    tmpContextNativeData.contextHandle = ::wglCreateContext( tmpSurfaceNativeData.hdc );
	    if ( tmpContextNativeData.contextHandle == nullptr )
	    {
	        ts3ThrowAuto( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
	    }

	    // Bind context as current, so GL calls may be used normally.
	    BOOL makeCurrentResult = ::wglMakeCurrent( tmpSurfaceNativeData.hdc,
                                                   tmpContextNativeData.contextHandle );

	    if ( makeCurrentResult == FALSE )
	    {
	        ts3ThrowAuto( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
	    }

	    auto glewResult = glewInit();
	    if ( glewResult != GLEW_OK )
	    {
	        ts3ThrowAuto( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
	    }

	    glewResult = wglewInit();
	    if ( glewResult != GLEW_OK )
	    {
	        ts3ThrowAuto( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
	    }
	}

	void GLSystemDriver::_nativeReleaseInitState()
	{
	    if( !mInternal->nativeDataPriv.initState )
	    {
	        return;
	    }

	    auto & tmpSurfaceNativeData = mInternal->nativeDataPriv.initState->surfaceData;
	    auto & tmpContextNativeData = mInternal->nativeDataPriv.initState->contextData;

	    if( tmpContextNativeData.contextHandle != nullptr )
	    {
	        ::wglDeleteContext( tmpContextNativeData.contextHandle );
	        tmpContextNativeData.contextHandle = nullptr;
	    }

	    if( tmpSurfaceNativeData.hdc != nullptr )
	    {
	        _win32DestroyGLSurface( tmpSurfaceNativeData );
	        tmpSurfaceNativeData.hdc = nullptr;
	        tmpSurfaceNativeData.pixelFormatIndex = cvWin32InvalidPixelFormatIndex;
	    }

	    if( tmpSurfaceNativeData.hwnd != nullptr )
	    {
	        nativeWin32DestroyWindow( tmpSurfaceNativeData );
	        tmpSurfaceNativeData.hwnd = nullptr;
	    }
	}

	void GLSystemDriver::_nativeCreateDisplaySurface( GLDisplaySurface & pDisplaySurface, const GLDisplaySurfaceCreateInfo & pCreateInfo )
	{
	    WindowCreateInfo surfaceWindowCreateInfo;
	    surfaceWindowCreateInfo.properties.geometry = pCreateInfo.windowGeometry;
	    surfaceWindowCreateInfo.properties.title = "TS3 OpenGL Window";

	    nativeWin32CreateWindow( pDisplaySurface.mInternal->nativeDataPriv, surfaceWindowCreateInfo );

	    _win32CreateGLSurface( pDisplaySurface.mInternal->nativeDataPriv, pCreateInfo.visualConfig );

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
	        // wglSwapIntervalEXT( 0 );
	    }

	    ::ShowWindow( pDisplaySurface.mInternal->nativeDataPriv.hwnd, SW_SHOWNORMAL );

	    // TODO: Workaround to properly work with current engine's implementation. That should be turned into an explicit flag.
	    if( pCreateInfo.flags.isSet( E_GL_DISPLAY_SURFACE_CREATE_FLAG_FULLSCREEN_BIT ) )
	    {
	        ::SetCapture( pDisplaySurface.mInternal->nativeDataPriv.hwnd );
	        ::ShowCursor( FALSE );
	    }
	}

	void GLSystemDriver::_nativeCreateDisplaySurfaceForCurrentThread( GLDisplaySurface & pDisplaySurface )
	{
	    auto hdc = ::wglGetCurrentDC();
	    if ( hdc == nullptr )
	    {
	        ts3ThrowAuto( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
	    }

	    pDisplaySurface.mInternal->nativeDataPriv.hdc = hdc;
	    pDisplaySurface.mInternal->nativeDataPriv.hwnd = ::WindowFromDC( hdc );
	    pDisplaySurface.mInternal->nativeDataPriv.pixelFormatIndex = ::GetPixelFormat( hdc );

	    CHAR surfaceWindowClassName[256];
	    ::GetClassNameA( pDisplaySurface.mInternal->nativeDataPriv.hwnd, surfaceWindowClassName, 255 );
	    auto wndProcModuleHandle = ::GetWindowLongPtrA( pDisplaySurface.mInternal->nativeDataPriv.hwnd, GWLP_HINSTANCE );

	    pDisplaySurface.mInternal->nativeDataPriv.wndClsName = surfaceWindowClassName;
	    pDisplaySurface.mInternal->nativeDataPriv.moduleHandle = reinterpret_cast<HMODULE>( wndProcModuleHandle );
	}

	void GLSystemDriver::_nativeDestroyDisplaySurface( GLDisplaySurface & pDisplaySurface )
	{
	    _win32DestroyGLSurface( pDisplaySurface.mInternal->nativeDataPriv );

	    nativeWin32DestroyWindow( pDisplaySurface.mInternal->nativeDataPriv );
	}

	void GLSystemDriver::_nativeCreateRenderContext( GLRenderContext & pRenderContext,
                                                     const GLDisplaySurface & pDisplaySurface,
                                                     const GLRenderContextCreateInfo & pCreateInfo )
	{
	    int contextProfile = 0;
	    Bitmask<int> contextCreateFlags = 0;
	    HGLRC shareContextHandle = nullptr;

	    if ( pCreateInfo.targetAPIProfile == EGLAPIProfile::Core )
	    {
	        contextProfile = WGL_CONTEXT_CORE_PROFILE_BIT_ARB;
	    }
	    else if ( pCreateInfo.targetAPIProfile == EGLAPIProfile::Legacy )
	    {
	        contextProfile = WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB;
	    }
	    else if ( pCreateInfo.targetAPIProfile == EGLAPIProfile::GLES )
	    {
	        contextProfile = WGL_CONTEXT_ES_PROFILE_BIT_EXT;
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
	            shareContextHandle = pCreateInfo.shareContext->mInternal->nativeDataPriv.contextHandle;
	        }
	    }

	    const int contextAttributes[] =
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

	    const auto & surfaceNativeData = pDisplaySurface.mInternal->nativeDataPriv;
	    HGLRC contextHandle = wglCreateContextAttribsARB( surfaceNativeData.hdc,
                                                          shareContextHandle,
                                                          &( contextAttributes[0] ) );

	    if ( contextHandle == nullptr )
	    {
	        ts3ThrowAuto( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
	    }

	    auto & contextNativeData = pRenderContext.mInternal->nativeDataPriv;
	    contextNativeData.contextHandle = contextHandle;
	}

	void GLSystemDriver::_nativeCreateRenderContextForCurrentThread( GLRenderContext & pRenderContext )
	{
	    auto contextHandle = ::wglGetCurrentContext();
	    if ( contextHandle == nullptr )
	    {
	        ts3ThrowAuto( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
	    }
	    pRenderContext.mInternal->nativeDataPriv.contextHandle = contextHandle;
	}

	void GLSystemDriver::_nativeDestroyRenderContext( GLRenderContext & pRenderContext )
	{
	    if( pRenderContext.mInternal->nativeDataPriv.contextHandle != nullptr )
	    {
	        if( _nativeIsRenderContextBound( pRenderContext ) )
	        {
	            // Explicitly unbind the context if it's currently bound.
	            // TODO: What about other threads? What if a context is destroyed in one thread while other uses it?
	            ::wglMakeCurrent( nullptr, nullptr );
	        }

	        ::wglDeleteContext( pRenderContext.mInternal->nativeDataPriv.contextHandle );

	        pRenderContext.mInternal->nativeDataPriv.contextHandle = nullptr;
	    }
	}

	void GLSystemDriver::_nativeResetContextBinding()
    {
        ::wglMakeCurrent( nullptr, nullptr );
    }

	bool GLSystemDriver::_nativeIsRenderContextBound() const
	{
	    auto currentContext = ::wglGetCurrentContext();
	    return currentContext != nullptr;
	}

	bool GLSystemDriver::_nativeIsRenderContextBound( const GLRenderContext & pRenderContext ) const
	{
	    auto currentContext = ::wglGetCurrentContext();
	    return currentContext == pRenderContext.mInternal->nativeDataPriv.contextHandle;
	}

	bool GLSystemDriver::_nativeIsDisplaySurfaceValid( const GLDisplaySurface & pDisplaySurface ) const
    {
	    return pDisplaySurface.mInternal->nativeDataPriv.hwnd && pDisplaySurface.mInternal->nativeDataPriv.hdc;
    }

    bool GLSystemDriver::_nativeIsRenderContextValid( const GLRenderContext & pRenderContext ) const
    {
        return pRenderContext.mInternal->nativeDataPriv.contextHandle != nullptr;
    }


    void GLDisplaySurface::_nativeSwapBuffers()
	{
	    ::SwapBuffers( mInternal->nativeDataPriv.hdc );
	}

	void GLDisplaySurface::_nativeQueryRenderAreaSize( WindowSize & pOutSize ) const
	{
	    RECT clientRect;
	    ::GetClientRect( mInternal->nativeDataPriv.hwnd, &clientRect );

	    pOutSize.x = clientRect.right - clientRect.left;
	    pOutSize.y = clientRect.bottom - clientRect.top;
	}


	void GLRenderContext::_nativeBindForCurrentThread( const GLDisplaySurface & pDisplaySurface )
	{
	    const auto & surfaceNativeData = pDisplaySurface.mInternal->nativeDataPriv;
	    ::wglMakeCurrent( surfaceNativeData.hdc, mNativeData->contextHandle );
	}


	void _win32CreateGLSurface( GLDisplaySurfaceNativeData & pGLSurfaceNativeData,
                                const VisualConfig & pVisualConfig )
	{
		auto hdc = ::GetWindowDC( pGLSurfaceNativeData.hwnd );
		pGLSurfaceNativeData.hdc = hdc;

		PIXELFORMATDESCRIPTOR pixelFormatDescriptor;
		memset( &pixelFormatDescriptor, 0, sizeof( PIXELFORMATDESCRIPTOR ) );
		pixelFormatDescriptor.nSize = sizeof( PIXELFORMATDESCRIPTOR );
		pixelFormatDescriptor.nVersion = 1;

		if ( pVisualConfig.flags.isSet( E_VISUAL_ATTRIB_FLAG_LEGACY_BIT ) )
		{
			pGLSurfaceNativeData.pixelFormatIndex = _win32ChooseLegacyGLPixelFormat( hdc, pixelFormatDescriptor );
		}
		else
		{
			pGLSurfaceNativeData.pixelFormatIndex = _win32ChooseCoreGLPixelFormat( hdc, pVisualConfig, pixelFormatDescriptor );
		}

		BOOL spfResult = ::SetPixelFormat( hdc, pGLSurfaceNativeData.pixelFormatIndex, &pixelFormatDescriptor );
		if ( spfResult == FALSE )
		{
			ts3ThrowAuto( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
		}
	}

	void _win32DestroyGLSurface( GLDisplaySurfaceNativeData & pGLSurfaceNativeData )
	{
		if ( pGLSurfaceNativeData.hdc != nullptr )
		{
			::ReleaseDC( pGLSurfaceNativeData.hwnd, pGLSurfaceNativeData.hdc );

			pGLSurfaceNativeData.hdc = nullptr;
			pGLSurfaceNativeData.pixelFormatIndex = 0;
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
			ts3ThrowAuto( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
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

		if ( pVisualConfig.flags.isSet( E_VISUAL_ATTRIB_FLAG_SINGLE_BUFFER_BIT ) &&
			!pVisualConfig.flags.isSet( E_VISUAL_ATTRIB_FLAG_DOUBLE_BUFFER_BIT ) )
		{
			doubleBufferRequestedState = FALSE;
		}

		if ( pVisualConfig.flags.isSet( E_VISUAL_ATTRIB_FLAG_STEREO_DISPLAY_BIT ) &&
			!pVisualConfig.flags.isSet( E_VISUAL_ATTRIB_FLAG_MONO_DISPLAY_BIT ) )
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

		if ( pVisualConfig.flags.isSet( E_VISUAL_ATTRIB_FLAG_DOUBLE_BUFFER_BIT ) )
		{
			pAttribArray[attribIndex++] = WGL_DOUBLE_BUFFER_ARB;
			pAttribArray[attribIndex++] = GL_TRUE;
		}
		else if ( pVisualConfig.flags.isSet( E_VISUAL_ATTRIB_FLAG_SINGLE_BUFFER_BIT ) )
		{
			pAttribArray[attribIndex++] = WGL_DOUBLE_BUFFER_ARB;
			pAttribArray[attribIndex++] = GL_FALSE;
		}

		if ( pVisualConfig.flags.isSet( E_VISUAL_ATTRIB_FLAG_MONO_DISPLAY_BIT ) )
		{
			pAttribArray[attribIndex++] = WGL_STEREO_ARB;
			pAttribArray[attribIndex++] = GL_FALSE;
		}
		else if ( pVisualConfig.flags.isSet( E_VISUAL_ATTRIB_FLAG_STEREO_DISPLAY_BIT ) )
		{
			pAttribArray[attribIndex++] = WGL_STEREO_ARB;
			pAttribArray[attribIndex++] = GL_TRUE;
		}

		if ( pVisualConfig.flags.isSet( E_VISUAL_ATTRIB_FLAG_SRGB_CAPABLE_BIT ) )
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
#endif // TS3_PCL_TARGET_SYSAPI_WIN32
