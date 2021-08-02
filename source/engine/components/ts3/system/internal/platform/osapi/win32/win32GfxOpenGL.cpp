
#include <ts3/system/openGL.h>
#include <ts3/system/windowtem.h>

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_WIN32 )
namespace ts3
{

	// Creates Win32 OpenGL surface using provided visual config.
	void _win32CreateGLSurface( GfxGLSurfaceNativeData & pGLSurfaceNativeData, const GfxVisualConfig & pVisualConfig );

	// Destroys existing surface.
	void _win32DestroyGLSurface( GfxGLSurfaceNativeData & pGLSurfaceNativeData );

	// Selects matching pixel format for surface described with a PFD. Uses legacy API.
	int _win32ChooseLegacyGLPixelFormat( HDC pGLSurfaceDC, PIXELFORMATDESCRIPTOR & pPfmtDescriptor );

	// Selects matching pixel format for surface described with a VisualConfig. Uses new EXT API and supports stuff like MSAA.
	int _win32ChooseCoreGLPixelFormat( HDC pGLSurfaceDC, const GfxVisualConfig & pVisualConfig, PIXELFORMATDESCRIPTOR & pPfmtDescriptor );

	// Returns an array of pixel format IDs matching specified set of Win32 PF attributes.
	std::vector<int> _win32QueryCompatiblePixelFormatList( HDC pGLSurfaceDC, int * pPixelFormatAttribList = nullptr );

	// Returns an array of pixel format IDs matching specified VisualConfig structure.
	std::vector<int> _win32QueryCompatiblePixelFormatList( HDC pGLSurfaceDC, const GfxVisualConfig & pVisualConfig );

	// Computes a "compatibility rate", i.e. how much the specified pixel format matches the visual.
	int _win32GetPixelFormatMatchRate( HDC pGLSurfaceDC, int pPixelFormatIndex, const GfxVisualConfig & pVisualConfig );

	// Translation: GfxVisualConfig --> array of WGL_* attributes required by the system API. Used for surface/context creation.
	void _win32GetWGLAttribArrayForVisualConfig( const GfxVisualConfig & pVisualConfig, int * pAttribArray );

	// Useful utility. Fetches values for a set of PF attributes. Uses templated array definition to resolve the size.
	template <size_t tSize, typename TpOutput>
	inline bool _win32QueryPixelFormatAttributes( HDC pGLSurfaceDC, int pPixelFormatIndex, const int( &pAttributes )[tSize], TpOutput & pOutput )
	{
		BOOL result = wglGetPixelFormatAttribivARB( pGLSurfaceDC, pPixelFormatIndex, 0, tSize, pAttributes, &( pOutput[0] ) );
		return result != FALSE;
	}


	void GfxGLSurface::_sysDestroy() noexcept
	{
		_win32DestroyGLSurface( mNativeData );
	}

	void GfxGLSurface::_sysSwapBuffers()
	{
		::SwapBuffers( mNativeData.surfaceHandle );
	}

	void GfxGLSurface::_sysQueryCurrentSize( WindowSize & pSize ) const
	{
		RECT clientRect;
		::GetClientRect( mNativeData.hwnd, &clientRect );
		pSize.x = clientRect.right - clientRect.left;
		pSize.y = clientRect.bottom - clientRect.top;
	}


	void GfxGLRenderContext::_sysDestroy() noexcept
	{
		if( mNativeData.contextHandle != nullptr )
		{
			if( _sysValidateCurrentBinding() )
			{
				::wglMakeCurrent( nullptr, nullptr );
			}

			::wglDeleteContext( mNativeData.contextHandle );

			mNativeData.contextHandle = nullptr;
		}
	}

	void GfxGLRenderContext::_sysBindForCurrentThread( GfxGLSurface & pTargetSurface )
	{
		::wglMakeCurrent( pTargetSurface.mNativeData.surfaceHandle, mNativeData.contextHandle );
	}

	bool GfxGLRenderContext::_sysValidateCurrentBinding() const
	{
		auto currentContext = ::wglGetCurrentContext();
		return mNativeData.contextHandle == currentContext;
	}


	void GfxGLDriver::_sysInitializePlatform()
	{
		auto & openglInitState = mNativeData.initState;

		WindowCreateInfo tempWindowCreateInfo;
		tempWindowCreateInfo.properties.geometry.position = cvWindowPositionOrigin;
		tempWindowCreateInfo.properties.geometry.size = WindowSize( 600, 600 );
		tempWindowCreateInfo.properties.geometry.frameStyle = WindowFrameStyle::Overlay;

		GfxVisualConfig legacyVisualConfig;
		legacyVisualConfig = sysGfxGetDefaultVisualConfigFortemWindow();
		legacyVisualConfig.flags.set( SYS_GFX_VISUAL_ATTRIB_FLAG_LEGACY_BIT );

		// Create a surface window. In case of Win32, GfxGLSurfaceNativeData inherits from WindowNativeData
		// for this very reason: to allow treating surfaces as windows (as that's exactly the case on desktop).
		win32CreateWindow( openglInitState.surfaceData, tempWindowCreateInfo );

		// Create a surface. This sets up the PFD and configures HDC properly.
		_win32CreateGLSurface( openglInitState.surfaceData, legacyVisualConfig );

		// Create legacy OpenGL context for initialization phase.
		openglInitState.contextData.contextHandle = ::wglCreateContext( openglInitState.surfaceData.surfaceHandle );
		if ( openglInitState.contextData.contextHandle == nullptr )
		{
			throw 0;
		}

		// Bind context as current, so GL calls may be used normally.
		BOOL makeCurrentResult = ::wglMakeCurrent( openglInitState.surfaceData.surfaceHandle,
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

	void GfxGLDriver::_sysReleaseInitState()
	{
		auto & openglInitState = mNativeData.initState;

		if( openglInitState.contextData.contextHandle != nullptr )
		{
			::wglDeleteContext( openglInitState.contextData.contextHandle );
			openglInitState.contextData.contextHandle = nullptr;
		}

		if( openglInitState.surfaceData.surfaceHandle != nullptr )
		{
			_win32DestroyGLSurface( openglInitState.surfaceData );
			openglInitState.surfaceData.surfaceHandle = nullptr;
			openglInitState.surfaceData.pixelFormatIndex = cvWin32InvalidPixelFormatIndex;
		}

		if( openglInitState.surfaceData.hwnd != nullptr )
		{
			win32DestroyWindow( openglInitState.surfaceData );
			openglInitState.surfaceData.hwnd = nullptr;
		}
	}

	void GfxGLDriver::_sysCreateDisplaySurface( GfxGLSurface & pGLSurface, const GfxGLSurfaceCreateInfo & pCreateInfo )
	{
		WindowCreateInfo surfaceWindowCreateInfo;
		surfaceWindowCreateInfo.properties.geometry = pCreateInfo.windowGeometry;
		surfaceWindowCreateInfo.properties.title = "TS3 OpenGL Window";

		win32CreateWindow( pGLSurface.mNativeData, surfaceWindowCreateInfo );
		_win32CreateGLSurface( pGLSurface.mNativeData, pCreateInfo.visualConfig );

		if( pCreateInfo.flags.isSet( E_GFX_GL_DISPLAY_SURFACE_CREATE_FLAG_SYNC_ADAPTIVE_BIT ) )
		{
			wglSwapIntervalEXT( -1 );
		}
		else if( pCreateInfo.flags.isSet( E_GFX_GL_DISPLAY_SURFACE_CREATE_FLAG_SYNC_VERTICAL_BIT ) )
		{
			wglSwapIntervalEXT( 1 );
		}
		else
		{
			wglSwapIntervalEXT( 0 );
		}

		::ShowWindow( pGLSurface.mNativeData.hwnd, SW_SHOWNORMAL );

		// TODO: Workaround to properly work with current engine's implementation. That should be turned into an explicit flag.
		if( pCreateInfo.flags.isSet( E_GFX_GL_DISPLAY_SURFACE_CREATE_FLAG_FULLSCREEN_BIT ) )
		{
			::SetCapture( pGLSurface.mNativeData.hwnd );
			::ShowCursor( FALSE );
		}
	}

	void GfxGLDriver::_sysCreateDisplaySurfaceForCurrentThread( GfxGLSurface & pGLSurface )
	{
		auto surfaceHandle = ::wglGetCurrentDC();
		if ( surfaceHandle == nullptr )
		{
			throw 0;
		}

		pGLSurface.mNativeData.surfaceHandle = surfaceHandle;
		pGLSurface.mNativeData.hwnd = ::WindowFromDC( surfaceHandle );
		pGLSurface.mNativeData.pixelFormatIndex = ::GetPixelFormat( surfaceHandle );

		CHAR surfaceWindowClassName[256];
		::GetClassNameA( pGLSurface.mNativeData.hwnd, surfaceWindowClassName, 255 );
		auto wndProcModuleHandle = ::GetWindowLongPtrA( pGLSurface.mNativeData.hwnd, GWLP_HINSTANCE );

		pGLSurface.mNativeData.wndClassName = surfaceWindowClassName;
		pGLSurface.mNativeData.wndProcModuleHandle = reinterpret_cast<HMODULE>( wndProcModuleHandle );

	}

	void GfxGLDriver::_sysCreateRenderContext( GfxGLRenderContext & pGLRenderContext, GfxGLSurface & pGLSurface, const GfxGLRenderContextCreateInfo & pCreateInfo )
	{
		int contextProfile = WGL_CONTEXT_CORE_PROFILE_BIT_ARB;
		Bitmask<int> contextCreateFlags = 0;
		HGLRC shareContextHandle = nullptr;

		if ( pCreateInfo.targetAPIProfile == EGfxGLAPIProfile::GLES )
		{
			contextProfile = WGL_CONTEXT_ES_PROFILE_BIT_EXT;
		}
		if ( pCreateInfo.targetAPIProfile == EGfxGLAPIProfile::Legacy )
		{
			contextProfile = WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB;
		}
		if ( pCreateInfo.flags.isSet( E_GFX_GL_RENDER_CONTEXT_CREATE_FLAG_ENABLE_DEBUG_BIT ) )
		{
			contextCreateFlags |= WGL_CONTEXT_DEBUG_BIT_ARB;
		}
		if ( pCreateInfo.flags.isSet( E_GFX_GL_RENDER_CONTEXT_CREATE_FLAG_FORWARD_COMPATIBLE_BIT ) )
		{
			contextCreateFlags |= WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB;
		}
		if ( pCreateInfo.flags.isSet( E_GFX_GL_RENDER_CONTEXT_CREATE_FLAG_ENABLE_SHARING_BIT ) )
		{
			if( pCreateInfo.shareContext != nullptr )
			{
				shareContextHandle = pCreateInfo.shareContext->mNativeData.contextHandle;
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

		HGLRC contextHandle = wglCreateContextAttribsARB( pGLSurface.mNativeData.surfaceHandle,
		                                                  shareContextHandle,
		                                                  &( contextAttribs[0] ) );

		if ( contextHandle == nullptr )
		{
			throw 0;
		}

		pGLRenderContext.mNativeData.contextHandle = contextHandle;
	}

	void GfxGLDriver::_sysCreateRenderContextForCurrentThread( GfxGLRenderContext & pGLRenderContext )
	{
		auto contextHandle = ::wglGetCurrentContext();
		if ( contextHandle == nullptr )
		{
			throw 0;
		}
		pGLRenderContext.mNativeData.contextHandle = contextHandle;
	}


	void _win32CreateGLSurface( GfxGLSurfaceNativeData & pGLSurfaceNativeData, const GfxVisualConfig & pVisualConfig )
	{
		auto surfaceHandle = ::GetWindowDC( pGLSurfaceNativeData.hwnd );
		pGLSurfaceNativeData.surfaceHandle = surfaceHandle;

		PIXELFORMATDESCRIPTOR pixelFormatDescriptor;
		memset( &pixelFormatDescriptor, 0, sizeof( PIXELFORMATDESCRIPTOR ) );
		pixelFormatDescriptor.nSize = sizeof( PIXELFORMATDESCRIPTOR );
		pixelFormatDescriptor.nVersion = 1;

		if ( pVisualConfig.flags.isSet( SYS_GFX_VISUAL_ATTRIB_FLAG_LEGACY_BIT ) )
		{
			pGLSurfaceNativeData.pixelFormatIndex = _win32ChooseLegacyGLPixelFormat( surfaceHandle, pixelFormatDescriptor );
		}
		else
		{
			pGLSurfaceNativeData.pixelFormatIndex = _win32ChooseCoreGLPixelFormat( surfaceHandle, pVisualConfig, pixelFormatDescriptor );
		}

		BOOL spfResult = ::SetPixelFormat( surfaceHandle, pGLSurfaceNativeData.pixelFormatIndex, &pixelFormatDescriptor );
		if ( spfResult == FALSE )
		{
			throw 0;
		}
	}

	void _win32DestroyGLSurface( GfxGLSurfaceNativeData & pGLSurfaceNativeData )
	{
		if ( pGLSurfaceNativeData.surfaceHandle != nullptr )
		{
			::ReleaseDC( pGLSurfaceNativeData.hwnd, pGLSurfaceNativeData.surfaceHandle );

			pGLSurfaceNativeData.surfaceHandle = nullptr;
			pGLSurfaceNativeData.pixelFormatIndex = 0;
		}
	}

	int _win32ChooseLegacyGLPixelFormat( HDC pGLSurfaceDC, PIXELFORMATDESCRIPTOR & pPfmtDescriptor )
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

		int pixelFormatIndex = ::ChoosePixelFormat( pGLSurfaceDC, &pPfmtDescriptor );

		if ( pixelFormatIndex == 0 )
		{
			pPfmtDescriptor.cColorBits = 24; // 24-bit color, no alpha, no explicit RGB
			pPfmtDescriptor.cDepthBits = 32; // 32-bit depth buffer, no stencil attachment
			pixelFormatIndex = ::ChoosePixelFormat( pGLSurfaceDC, &pPfmtDescriptor );
		}

		return ( pixelFormatIndex > 0 ) ? pixelFormatIndex : cvWin32InvalidPixelFormatIndex;
	}

	int _win32ChooseCoreGLPixelFormat( HDC pGLSurfaceDC, const GfxVisualConfig & pVisualConfig, PIXELFORMATDESCRIPTOR & pPfmtDescriptor )
	{
		auto pixelFormatList = _win32QueryCompatiblePixelFormatList( pGLSurfaceDC, pVisualConfig );

		int bestMatchRate = 0;
		int bestPixelFormatID = 0;

		for ( auto pixelFormatID : pixelFormatList )
		{
			int matchRate = _win32GetPixelFormatMatchRate( pGLSurfaceDC, pixelFormatID, pVisualConfig );
			if ( matchRate > bestMatchRate )
			{
				bestMatchRate = matchRate;
				bestPixelFormatID = pixelFormatID;
			}
		}

		int maxPixelFormatIndex = ::DescribePixelFormat( pGLSurfaceDC, bestPixelFormatID, sizeof( PIXELFORMATDESCRIPTOR ), &pPfmtDescriptor );

		return ( maxPixelFormatIndex > 0 ) ? bestPixelFormatID : cvWin32InvalidPixelFormatIndex;
	}

	std::vector<int> _win32QueryCompatiblePixelFormatList( HDC pGLSurfaceDC, int * pPixelFormatAttribList )
	{
		// Output array where system will store IDs of enumerated pixel formats.
		int pixelFormatArray[cvWin32MaxWGLPixelFormatsNum];
		// Number of pixel formats returned by the system.
		UINT returnedPixelFormatsNum = 0U;

		// Enumerate pixel formats.
		BOOL enumResult = ::wglChoosePixelFormatARB( pGLSurfaceDC,
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

	std::vector<int> _win32QueryCompatiblePixelFormatList( HDC pGLSurfaceDC, const GfxVisualConfig & pVisualConfig )
	{
		// Array for WGL_..._ARB pixel format attributes.
		int pixelFormatAttributes[cvWin32MaxWGLPixelFormatAttributesNum * 2];
		// Translate provided visual config to a WGL attribute array.
		_win32GetWGLAttribArrayForVisualConfig( pVisualConfig, pixelFormatAttributes );

		return _win32QueryCompatiblePixelFormatList( pGLSurfaceDC, pixelFormatAttributes );
	}

	int _win32GetPixelFormatMatchRate( HDC pGLSurfaceDC, int pPixelFormatIndex, const GfxVisualConfig & pVisualConfig )
	{
		int doubleBufferRequestedState = TRUE;
		int stereoModeRequestedState = FALSE;

		if ( pVisualConfig.flags.isSet( SYS_GFX_VISUAL_ATTRIB_FLAG_SINGLE_BUFFER_BIT ) &&
			!pVisualConfig.flags.isSet( SYS_GFX_VISUAL_ATTRIB_FLAG_DOUBLE_BUFFER_BIT ) )
		{
			doubleBufferRequestedState = FALSE;
		}

		if ( pVisualConfig.flags.isSet( SYS_GFX_VISUAL_ATTRIB_FLAG_STEREO_DISPLAY_BIT ) &&
			!pVisualConfig.flags.isSet( SYS_GFX_VISUAL_ATTRIB_FLAG_MONO_DISPLAY_BIT ) )
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
		if( !_win32QueryPixelFormatAttributes( pGLSurfaceDC, pPixelFormatIndex, controlAttribArray, attribValueArray ) )
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

	void _win32GetWGLAttribArrayForVisualConfig( const GfxVisualConfig & pVisualConfig, int * pAttribArray )
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

		if ( pVisualConfig.flags.isSet( SYS_GFX_VISUAL_ATTRIB_FLAG_DOUBLE_BUFFER_BIT ) )
		{
			pAttribArray[attribIndex++] = WGL_DOUBLE_BUFFER_ARB;
			pAttribArray[attribIndex++] = GL_TRUE;
		}
		else if ( pVisualConfig.flags.isSet( SYS_GFX_VISUAL_ATTRIB_FLAG_SINGLE_BUFFER_BIT ) )
		{
			pAttribArray[attribIndex++] = WGL_DOUBLE_BUFFER_ARB;
			pAttribArray[attribIndex++] = GL_FALSE;
		}

		if ( pVisualConfig.flags.isSet( SYS_GFX_VISUAL_ATTRIB_FLAG_MONO_DISPLAY_BIT ) )
		{
			pAttribArray[attribIndex++] = WGL_STEREO_ARB;
			pAttribArray[attribIndex++] = GL_FALSE;
		}
		else if ( pVisualConfig.flags.isSet( SYS_GFX_VISUAL_ATTRIB_FLAG_STEREO_DISPLAY_BIT ) )
		{
			pAttribArray[attribIndex++] = WGL_STEREO_ARB;
			pAttribArray[attribIndex++] = GL_TRUE;
		}

		if ( pVisualConfig.flags.isSet( SYS_GFX_VISUAL_ATTRIB_FLAG_SRGB_CAPABLE_BIT ) )
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

}
#endif
