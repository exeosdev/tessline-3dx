
#include <ts3/system/openGL.h>

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_X11 )
namespace ts3
{
	// Creates X11 OpenGL surface using provided window create attributes and visual config.
	void _x11CreateGLWindowAndSurface( SysGLSurfaceNativeData & pGLSurfaceNativeData,
	                                   X11SysWindowCreateInfo & pWindowCreateInfo,
	                                   const SysVisualConfig & pVisualConfig );

	// Destroys existing surface and corresponding window.
	void _x11DestroyGLWindowAndSurface( SysGLSurfaceNativeData & pGLSurfaceNativeData );

	// Selects matching FBConfig for a GL surface. Format is selected using current system's configuration. Uses legacy API.
	GLXFBConfig _x11ChooseLegacyGLFBConfig( Display * pDisplay, int pScreenIndex );

	// Selects matching pixel format for surface described with a VisualConfig. Uses new EXT API and supports stuff like MSAA.
	GLXFBConfig _x11ChooseCoreGLFBConfig( Display * pDisplay, int pScreenIndex, const SysVisualConfig & pVisualConfig );

	// Returns an array of FBConfigs matching specified VisualConfig structure.
	std::vector<GLXFBConfig> _x11QueryCompatibleFBConfigList( Display * pDisplay, int pScreenIndex, const SysVisualConfig & pVisualConfig );

	// Computes a "compatibility rate", i.e. how much the specified FBConfig matches the visual.
	int _x11GetFBConfigMatchRate( Display * pDisplay, int pScreenIndex, GLXFBConfig pFBConfig, const SysVisualConfig & pVisualConfig );

	// Translation: SysVisualConfig --> array of GLX_* attributes required by the system API. Used for surface/context creation.
	void _x11GetAttribArrayForVisualConfig( const SysVisualConfig & pVisualConfig, int * pAttribArray );


	void SysGLSurface::_sysDestroy() noexcept
	{
		_x11DestroyGLWindowAndSurface( mNativeData );
	}

	void SysGLSurface::_sysSwapBuffers()
	{
		glXSwapBuffers( mNativeData.display, mNativeData.xwindow );
	}

	void SysGLSurface::_sysQueryCurrentSize( SysWindowSize & pSize ) const
	{
		XWindowAttributes windowAttributes;
		XGetWindowAttributes( mNativeData.display,
		                      mNativeData.xwindow,
		                      &windowAttributes );

		pSize.x = windowAttributes.width;
		pSize.y = windowAttributes.height;
	}


	void SysGLContext::_sysDestroy() noexcept
	{
		if( mNativeData.contextHandle != nullptr )
		{
			if( _sysValidateCurrentBinding() )
			{
				::glXMakeContextCurrent( mNativeData.display, cvXIDNone, cvXIDNone, nullptr );
			}

			::glXDestroyContext( mNativeData.display, mNativeData.contextHandle );

			mNativeData.contextHandle = nullptr;
		}
	}

	void SysGLContext::_sysBindForCurrentThread( SysGLSurface & pTargetSurface )
	{
		::glXMakeContextCurrent( mNativeData.display,
		                         pTargetSurface.mNativeData.xwindow,
		                         pTargetSurface.mNativeData.xwindow,
		                         mNativeData.contextHandle );
	}

	bool SysGLContext::_sysValidateCurrentBinding() const
	{
		auto currentContext = ::glXGetCurrentContext();
		return mNativeData.contextHandle == currentContext;
	}


	void SysGLSubsystem::_sysInitializePlatform()
	{
		auto & scNativeData = mSysContext->mNativeData;
		auto & openglSysInitState = mNativeData.initState;

		int glxVersionMajor = 0;
		int glxVersionMinor = 0;
		::glXQueryVersion( scNativeData.display, &glxVersionMajor, &glxVersionMinor );
		if( ( glxVersionMajor == 0 ) || ( ( glxVersionMajor == 1 ) && ( glxVersionMinor < 3 ) ) )
		{
			throw 0;
		}

		SysVisualConfig legacyVisualConfig;
		legacyVisualConfig = sysDsmGetDefaultVisualConfigForSystemWindow();
		legacyVisualConfig.flags.set( SYS_VISUAL_ATTRIB_FLAG_LEGACY_BIT );

		X11SysWindowCreateInfo x11WindowCreateInfo;
		x11WindowCreateInfo.display = scNativeData.display;
		x11WindowCreateInfo.screenIndex = scNativeData.screenIndex;
		x11WindowCreateInfo.rootWindow = scNativeData.rootWindow;
		x11WindowCreateInfo.wmpDeleteWindow = scNativeData.wmpDeleteWindow;

		_x11CreateGLWindowAndSurface( openglSysInitState.surfaceData, x11WindowCreateInfo, legacyVisualConfig );

		auto tempContextHandle = ::glXCreateContext( scNativeData.display,
		                                             openglSysInitState.surfaceData.xvisualInfo,
		                                             nullptr,
		                                             True );
		if ( tempContextHandle == nullptr )
		{
			throw 0;
		}

		openglSysInitState.contextData.contextHandle = tempContextHandle;
		openglSysInitState.contextData.display = scNativeData.display;
		openglSysInitState.contextData.targetSurface = openglSysInitState.surfaceData.xwindow;

		auto makeCurrentResult = ::glXMakeCurrent( openglSysInitState.contextData.display,
		                                           openglSysInitState.contextData.targetSurface,
		                                           openglSysInitState.contextData.contextHandle );
		if ( makeCurrentResult == False )
		{
			throw 0;
		}

        auto glewResult = glewInit();
        if ( glewResult != GLEW_OK )
        {
            throw 0;
        }
	}

	void SysGLSubsystem::_sysReleaseInitState()
	{
		auto & openglSysInitState = mNativeData.initState;

		if( openglSysInitState.contextData.contextHandle != nullptr )
		{
			::glXDestroyContext( openglSysInitState.contextData.display, openglSysInitState.contextData.contextHandle );
			openglSysInitState.contextData.contextHandle = nullptr;
			openglSysInitState.contextData.targetSurface = cvXIDNone;
		}

		if( openglSysInitState.surfaceData.xwindow != cvXIDNone )
		{
			_x11DestroyGLWindowAndSurface( openglSysInitState.surfaceData );
			openglSysInitState.surfaceData.glxFBConfig = nullptr;
		}

		if( openglSysInitState.surfaceData.xvisualInfo != nullptr )
		{
			XFree( openglSysInitState.surfaceData.xvisualInfo );
			openglSysInitState.surfaceData.xvisualInfo = nullptr;
		}
	}

	void SysGLSubsystem::_sysCreateDisplaySurface( SysGLSurface & pGLSurface, const SysGLSurfaceCreateInfo & pCreateInfo )
	{
		auto & scNativeData = mSysContext->mNativeData;

		X11SysWindowCreateInfo x11WindowCreateInfo;
		x11WindowCreateInfo.commonProperties.geometry = pCreateInfo.windowGeometry;
		x11WindowCreateInfo.commonProperties.title = "Exeos Framework OpenGL Window";
		x11WindowCreateInfo.display = scNativeData.display;
		x11WindowCreateInfo.screenIndex = scNativeData.screenIndex;
		x11WindowCreateInfo.rootWindow = scNativeData.rootWindow;
		x11WindowCreateInfo.wmpDeleteWindow = scNativeData.wmpDeleteWindow;
		x11WindowCreateInfo.fullscreenMode = pCreateInfo.flags.isSet( E_SYS_GFX_GL_DISPLAY_SURFACE_CREATE_FLAG_FULLSCREEN_BIT );

		_x11CreateGLWindowAndSurface( pGLSurface.mNativeData, x11WindowCreateInfo, pCreateInfo.visualConfig );
		sysX11UpdateNewWindowState( pGLSurface.mNativeData, x11WindowCreateInfo );
	}

	void SysGLSubsystem::_sysCreateDisplaySurfaceForCurrentThread( SysGLSurface & pGLSurface )
	{
		pGLSurface.mNativeData.display = glXGetCurrentDisplay();
		pGLSurface.mNativeData.xwindow = glXGetCurrentDrawable();

		XWindowAttributes windowAttributes;
		XGetWindowAttributes( pGLSurface.mNativeData.display,
		                      pGLSurface.mNativeData.xwindow,
		                      &windowAttributes );

		pGLSurface.mNativeData.colormap = windowAttributes.colormap;
	}

	void SysGLSubsystem::_sysCreateRenderContext( SysGLContext & pGLContext, SysGLSurface & pGLSurface, const SysGLContextCreateInfo & pCreateInfo )
	{
		static PFNGLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribsProc = nullptr;
		static PFNGLXSWAPINTERVALEXTPROC glXSwapIntervalEXTProc = nullptr;

		if( glXCreateContextAttribsProc == nullptr )
		{
			glXCreateContextAttribsProc = (PFNGLXCREATECONTEXTATTRIBSARBPROC)glXGetProcAddressARB( (const GLubyte *) "glXCreateContextAttribsARB" );
		}

		if( glXSwapIntervalEXTProc == nullptr )
		{
			glXSwapIntervalEXTProc = (PFNGLXSWAPINTERVALEXTPROC)glXGetProcAddressARB( (const GLubyte *) "glXSwapIntervalEXT" );
		}

		int contextProfile = GLX_CONTEXT_CORE_PROFILE_BIT_ARB;
		Bitmask<int> contextCreateFlags = 0;
		GLXContext shareContextHandle = nullptr;

		if ( pCreateInfo.targetAPIProfile == ESysGLAPIProfile::GLES )
		{
			contextProfile = GLX_CONTEXT_ES_PROFILE_BIT_EXT;
		}
		if ( pCreateInfo.targetAPIProfile == ESysGLAPIProfile::Legacy )
		{
			contextProfile = GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB;
		}
		if ( pCreateInfo.flags.isSet( E_SYS_GFX_GL_RENDER_CONTEXT_CREATE_FLAG_ENABLE_DEBUG_BIT ) )
		{
			contextCreateFlags |= GLX_CONTEXT_DEBUG_BIT_ARB;
		}
		if ( pCreateInfo.flags.isSet( E_SYS_GFX_GL_RENDER_CONTEXT_CREATE_FLAG_FORWARD_COMPATIBLE_BIT ) )
		{
			contextCreateFlags |= GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB;
		}
		if ( pCreateInfo.flags.isSet( E_SYS_GFX_GL_RENDER_CONTEXT_CREATE_FLAG_ENABLE_SHARING_BIT ) )
		{
			if( pCreateInfo.shareContext != nullptr )
			{
				shareContextHandle = pCreateInfo.shareContext->mNativeData.contextHandle;
			}
		}

		const int contextAttribs[] =
		{
			// Requested OpenGL API version: major part
			GLX_CONTEXT_MAJOR_VERSION_ARB, pCreateInfo.requiredAPIVersion.major,
			// Requested OpenGL API version: minor part
			GLX_CONTEXT_MINOR_VERSION_ARB, pCreateInfo.requiredAPIVersion.minor,
			//
			GLX_CONTEXT_PROFILE_MASK_ARB, contextProfile,
			//
			GLX_CONTEXT_FLAGS_ARB, contextCreateFlags,
			// Terminator
			FALSE
		};

		GLXContext contextHandle = glXCreateContextAttribsProc( pGLSurface.mNativeData.display,
		                                                        pGLSurface.mNativeData.glxFBConfig,
		                                                        shareContextHandle,
		                                                        True,
		                                                        &( contextAttribs[0] ) );

		if ( contextHandle == nullptr )
		{
			throw 0;
		}

		pGLContext.mNativeData.display = pGLSurface.mNativeData.display;
		pGLContext.mNativeData.targetSurface = pGLSurface.mNativeData.xwindow;
		pGLContext.mNativeData.contextHandle = contextHandle;
	}

	void SysGLSubsystem::_sysCreateRenderContextForCurrentThread( SysGLContext & pGLContext )
	{
		auto contextHandle = ::glXGetCurrentContext();
		if ( contextHandle == nullptr )
		{
			throw 0;
		}
		pGLContext.mNativeData.contextHandle = contextHandle;
	}

	
	void _x11CreateGLWindowAndSurface( SysGLSurfaceNativeData & pGLSurfaceNativeData,
	                                   X11SysWindowCreateInfo & pWindowCreateInfo,
	                                   const SysVisualConfig & pVisualConfig )
	{
		GLXFBConfig windowFBConfig = nullptr;

		if( pVisualConfig.flags.isSet( SYS_VISUAL_ATTRIB_FLAG_LEGACY_BIT ) )
		{
			windowFBConfig = _x11ChooseLegacyGLFBConfig( pWindowCreateInfo.display, pWindowCreateInfo.screenIndex );
		}
		else
		{
			windowFBConfig = _x11ChooseCoreGLFBConfig( pWindowCreateInfo.display, pWindowCreateInfo.screenIndex, pVisualConfig );
		}

		if( windowFBConfig == nullptr )
		{
			throw 0;
		}

		auto * fbConfigVisualInfo = glXGetVisualFromFBConfig( pWindowCreateInfo.display, windowFBConfig );
		if( fbConfigVisualInfo == nullptr )
		{
			throw 0;
		}

		pGLSurfaceNativeData.glxFBConfig = windowFBConfig;
		pGLSurfaceNativeData.xvisualInfo = fbConfigVisualInfo;

		pWindowCreateInfo.colorDepth = fbConfigVisualInfo->depth;
		pWindowCreateInfo.windowVisual = fbConfigVisualInfo->visual;

		sysX11CreateWindow( pGLSurfaceNativeData, pWindowCreateInfo );
	}

	void _x11DestroyGLWindowAndSurface( SysGLSurfaceNativeData & pGLSurfaceNativeData )
	{
		XUnmapWindow( pGLSurfaceNativeData.display, pGLSurfaceNativeData.xwindow );
		sysX11DestroyWindow( pGLSurfaceNativeData );
	}

	GLXFBConfig _x11ChooseLegacyGLFBConfig( Display * pDisplay, int pScreenIndex )
	{
		const int defaultVisualAttribs[] =
		{
			GLX_X_RENDERABLE,  True,
			GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
			GLX_RENDER_TYPE,   GLX_RGBA_BIT,
			GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
			GLX_RED_SIZE,      8,
			GLX_GREEN_SIZE,    8,
			GLX_BLUE_SIZE,     8,
			GLX_ALPHA_SIZE,    8,
			GLX_DEPTH_SIZE,    24,
			GLX_STENCIL_SIZE,  8,
			GLX_DOUBLEBUFFER,  True,
			cvXIDNone,
		};

		const int defaultVisualAttribsNoDepthStencil[] =
		{
			GLX_X_RENDERABLE,  True,
			GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
			GLX_RENDER_TYPE,   GLX_RGBA_BIT,
			GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
			GLX_RED_SIZE,      8,
			GLX_GREEN_SIZE,    8,
			GLX_BLUE_SIZE,     8,
			GLX_ALPHA_SIZE,    8,
			GLX_DOUBLEBUFFER,  True,
			cvXIDNone,
		};

		int fbConfigListSize = 0;
		GLXFBConfig * fbConfigList = glXChooseFBConfig( pDisplay, pScreenIndex, defaultVisualAttribs, &fbConfigListSize );

		if( ( fbConfigList == nullptr ) || ( fbConfigListSize == 0 ) )
		{
			fbConfigList = glXChooseFBConfig( pDisplay, pScreenIndex, defaultVisualAttribsNoDepthStencil, &fbConfigListSize );
			if( ( fbConfigList == nullptr ) || ( fbConfigListSize == 0 ) )
			{
				throw 0;
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

		return bestFBConfig;
	}

	GLXFBConfig _x11ChooseCoreGLFBConfig( Display * pDisplay, int pScreenIndex, const SysVisualConfig & pVisualConfig )
	{
		auto fbConfigList = _x11QueryCompatibleFBConfigList( pDisplay, pScreenIndex, pVisualConfig );

		int bestMatchRate = 0;
		GLXFBConfig bestFBConfig = nullptr;

		for ( auto fbConfig : fbConfigList )
		{
			int matchRate = _x11GetFBConfigMatchRate( pDisplay, pScreenIndex, fbConfig, pVisualConfig );
			if ( matchRate > bestMatchRate )
			{
				bestMatchRate = matchRate;
				bestFBConfig = fbConfig;
			}
		}

		return bestFBConfig;
	}

	std::vector<GLXFBConfig> _x11QueryCompatibleFBConfigList( Display * pDisplay, int pScreenIndex, const SysVisualConfig & pVisualConfig )
	{
		std::vector<GLXFBConfig> result;

		int fbConfigAttribArray[cvSysX11MaxGLXFBConfigAttributesNum];
		_x11GetAttribArrayForVisualConfig( pVisualConfig, fbConfigAttribArray );

		int pFBConfigListSize = 0;
		auto * pFBConfigList = glXChooseFBConfig( pDisplay, pScreenIndex, fbConfigAttribArray, &pFBConfigListSize );

		if ( ( pFBConfigList == nullptr ) || ( pFBConfigListSize == 0 ) )
		{
			throw 0;
		}

		for ( int pFBConfigIndex = 0; pFBConfigIndex < pFBConfigListSize; ++pFBConfigIndex )
		{
			auto pFBConfig = pFBConfigList[pFBConfigIndex];
			auto * visualInfo = glXGetVisualFromFBConfig( pDisplay, pFBConfig );

			if ( visualInfo != nullptr )
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

		XFree( pFBConfigList );

		return result;
	}

	int _x11GetFBConfigMatchRate( Display * pDisplay, int pScreenIndex, GLXFBConfig pFBConfig, const SysVisualConfig & pVisualConfig )
	{
		int doubleBufferRequestedState = True;
		int stereoModeRequestedState = False;

		if ( pVisualConfig.flags.isSet( SYS_VISUAL_ATTRIB_FLAG_SINGLE_BUFFER_BIT ) &&
		     !pVisualConfig.flags.isSet( SYS_VISUAL_ATTRIB_FLAG_DOUBLE_BUFFER_BIT ) )
		{
			doubleBufferRequestedState = False;
		}

		if ( pVisualConfig.flags.isSet( SYS_VISUAL_ATTRIB_FLAG_STEREO_DISPLAY_BIT ) &&
		     !pVisualConfig.flags.isSet( SYS_VISUAL_ATTRIB_FLAG_MONO_DISPLAY_BIT ) )
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

	void _x11GetAttribArrayForVisualConfig( const SysVisualConfig & pVisualConfig, int * pAttribArray )
	{
		int attribIndex = 0;

		pAttribArray[attribIndex++] = GLX_DRAWABLE_TYPE;
		pAttribArray[attribIndex++] = GLX_WINDOW_BIT;

		pAttribArray[attribIndex++] = GLX_RENDER_TYPE;
		pAttribArray[attribIndex++] = GLX_RGBA_BIT;

		pAttribArray[attribIndex++] = GLX_X_RENDERABLE;
		pAttribArray[attribIndex++] = True;

		pAttribArray[attribIndex++] = GLX_X_VISUAL_TYPE;
		pAttribArray[attribIndex++] = GLX_TRUE_COLOR;

		if ( pVisualConfig.flags.isSet( SYS_VISUAL_ATTRIB_FLAG_DOUBLE_BUFFER_BIT ) )
		{
			pAttribArray[attribIndex++] = GLX_DOUBLEBUFFER;
			pAttribArray[attribIndex++] = True;
		}
		else if ( pVisualConfig.flags.isSet( SYS_VISUAL_ATTRIB_FLAG_SINGLE_BUFFER_BIT ) )
		{
			pAttribArray[attribIndex++] = GLX_DOUBLEBUFFER;
			pAttribArray[attribIndex++] = False;
		}

		if ( pVisualConfig.flags.isSet( SYS_VISUAL_ATTRIB_FLAG_MONO_DISPLAY_BIT ) )
		{
			pAttribArray[attribIndex++] = GLX_STEREO;
			pAttribArray[attribIndex++] = False;
		}
		else if ( pVisualConfig.flags.isSet( SYS_VISUAL_ATTRIB_FLAG_STEREO_DISPLAY_BIT ) )
		{
			pAttribArray[attribIndex++] = GLX_STEREO;
			pAttribArray[attribIndex++] = True;
		}

		if ( ( pVisualConfig.msaaDesc.bufferCount != 0 ) && ( pVisualConfig.msaaDesc.quality != 0 ) )
		{
			pAttribArray[attribIndex++] = GLX_SAMPLE_BUFFERS;
			pAttribArray[attribIndex++] = pVisualConfig.msaaDesc.bufferCount;

			pAttribArray[attribIndex++] = GLX_SAMPLES;
			pAttribArray[attribIndex++] = pVisualConfig.msaaDesc.quality;
		}

		if ( pVisualConfig.colorDesc.rgba.u32Code != 0 )
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

		if ( pVisualConfig.depthStencilDesc.depthBufferSize != 0 )
		{
			pAttribArray[attribIndex++] = GLX_DEPTH_SIZE;
			pAttribArray[attribIndex++] = pVisualConfig.depthStencilDesc.depthBufferSize;
		}

		if ( pVisualConfig.depthStencilDesc.stencilBufferSize != 0 )
		{
			pAttribArray[attribIndex++] = GLX_STENCIL_SIZE;
			pAttribArray[attribIndex++] = pVisualConfig.depthStencilDesc.stencilBufferSize;
		}

		pAttribArray[attribIndex++] = cvXIDNone;
	}

}
#endif
