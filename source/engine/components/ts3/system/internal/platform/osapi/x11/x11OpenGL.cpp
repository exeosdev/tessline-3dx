
#include <ts3/system/openGL.h>

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_X11 )
namespace ts3
{
	// Creates X11 OpenGL surface using provided window create attributes and visual config.
	void _x11CreateGLWindowAndSurface( GLDisplaySurfaceNativeData & pGLSurfaceNativeData,
	                                   X11WindowCreateInfo & pWindowCreateInfo,
	                                   const VisualConfig & pVisualConfig );

	// Destroys existing surface and corresponding window.
	void _x11DestroyGLWindowAndSurface( GLDisplaySurfaceNativeData & pGLSurfaceNativeData );

	// Selects matching FBConfig for a GL surface. Format is selected using current system's configuration. Uses legacy API.
	GLXFBConfig _x11ChooseLegacyGLFBConfig( Display * pDisplay, int pScreenIndex );

	// Selects matching pixel format for surface described with a VisualConfig. Uses new EXT API and supports stuff like MSAA.
	GLXFBConfig _x11ChooseCoreGLFBConfig( Display * pDisplay, int pScreenIndex, const VisualConfig & pVisualConfig );

	// Returns an array of FBConfigs matching specified VisualConfig structure.
	std::vector<GLXFBConfig> _x11QueryCompatibleFBConfigList( Display * pDisplay, int pScreenIndex, const VisualConfig & pVisualConfig );

	// Computes a "compatibility rate", i.e. how much the specified FBConfig matches the visual.
	int _x11GetFBConfigMatchRate( Display * pDisplay, int pScreenIndex, GLXFBConfig pFBConfig, const VisualConfig & pVisualConfig );

	// Translation: VisualConfig --> array of GLX_* attributes required by the system API. Used for surface/context creation.
	void _x11GetAttribArrayForVisualConfig( const VisualConfig & pVisualConfig, int * pAttribArray );


	void GLDisplaySurface::_sysDestroy() noexcept
	{
		_x11DestroyGLWindowAndSurface( mNativeData );
	}

	void GLDisplaySurface::_sysSwapBuffers()
	{
		glXSwapBuffers( mNativeData.display, mNativeData.xwindow );
	}

	void GLDisplaySurface::_sysQueryCurrentSize( WindowSize & pSize ) const
	{
		XWindowAttributes windowAttributes;
		XGetWindowAttributes( mNativeData.display,
		                      mNativeData.xwindow,
		                      &windowAttributes );

		pSize.x = windowAttributes.width;
		pSize.y = windowAttributes.height;
	}


	void GLRenderContext::_sysDestroy() noexcept
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

	void GLRenderContext::_sysBindForCurrentThread( GLDisplaySurface & pTargetSurface )
	{
		::glXMakeContextCurrent( mNativeData.display,
		                         pTargetSurface.mNativeData.xwindow,
		                         pTargetSurface.mNativeData.xwindow,
		                         mNativeData.contextHandle );
	}

	bool GLRenderContext::_sysValidateCurrentBinding() const
	{
		auto currentContext = ::glXGetCurrentContext();
		return mNativeData.contextHandle == currentContext;
	}


	void GLDriver::_sysInitializePlatform()
	{
		auto & scNativeData = mContext->mNativeData;
		auto & openglInitState = mNativeData.initState;

		int glxVersionMajor = 0;
		int glxVersionMinor = 0;
		::glXQueryVersion( scNativeData.display, &glxVersionMajor, &glxVersionMinor );
		if( ( glxVersionMajor == 0 ) || ( ( glxVersionMajor == 1 ) && ( glxVersionMinor < 3 ) ) )
		{
			throw 0;
		}

		VisualConfig legacyVisualConfig;
		legacyVisualConfig = gfxGetDefaultVisualConfigForSysWindow();
		legacyVisualConfig.flags.set( VISUAL_ATTRIB_FLAG_LEGACY_BIT );

		X11WindowCreateInfo x11WindowCreateInfo;
		x11WindowCreateInfo.display = scNativeData.display;
		x11WindowCreateInfo.screenIndex = scNativeData.screenIndex;
		x11WindowCreateInfo.rootWindow = scNativeData.rootWindow;
		x11WindowCreateInfo.wmpDeleteWindow = scNativeData.wmpDeleteWindow;

		_x11CreateGLWindowAndSurface( openglInitState.surfaceData, x11WindowCreateInfo, legacyVisualConfig );

		auto tempContextHandle = ::glXCreateContext( scNativeData.display,
		                                             openglInitState.surfaceData.visualInfo,
		                                             nullptr,
		                                             True );
		if ( tempContextHandle == nullptr )
		{
			throw 0;
		}

		openglInitState.contextData.contextHandle = tempContextHandle;
		openglInitState.contextData.display = scNativeData.display;
		openglInitState.contextData.targetSurface = openglInitState.surfaceData.xwindow;

		auto makeCurrentResult = ::glXMakeCurrent( openglInitState.contextData.display,
		                                           openglInitState.contextData.targetSurface,
		                                           openglInitState.contextData.contextHandle );
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

	void GLDriver::_sysReleaseInitState()
	{
		auto & openglInitState = mNativeData.initState;

		if( openglInitState.contextData.contextHandle != nullptr )
		{
			::glXDestroyContext( openglInitState.contextData.display, openglInitState.contextData.contextHandle );
			openglInitState.contextData.contextHandle = nullptr;
			openglInitState.contextData.targetSurface = cvXIDNone;
		}

		if( openglInitState.surfaceData.xwindow != cvXIDNone )
		{
			_x11DestroyGLWindowAndSurface( openglInitState.surfaceData );
			openglInitState.surfaceData.fbConfig = nullptr;
		}

		if( openglInitState.surfaceData.visualInfo != nullptr )
		{
			XFree( openglInitState.surfaceData.visualInfo );
			openglInitState.surfaceData.visualInfo = nullptr;
		}
	}

	void GLDriver::_sysCreateDisplaySurface( GLDisplaySurface & pGLSurface, const GLDisplaySurfaceCreateInfo & pCreateInfo )
	{
		auto & scNativeData = mContext->mNativeData;

		X11WindowCreateInfo x11WindowCreateInfo;
		x11WindowCreateInfo.commonProperties.geometry = pCreateInfo.windowGeometry;
		x11WindowCreateInfo.commonProperties.title = "Exeos Framework OpenGL Window";
		x11WindowCreateInfo.display = scNativeData.display;
		x11WindowCreateInfo.screenIndex = scNativeData.screenIndex;
		x11WindowCreateInfo.rootWindow = scNativeData.rootWindow;
		x11WindowCreateInfo.wmpDeleteWindow = scNativeData.wmpDeleteWindow;
		x11WindowCreateInfo.fullscreenMode = pCreateInfo.flags.isSet( E_GL_DISPLAY_SURFACE_CREATE_FLAG_FULLSCREEN_BIT );

		_x11CreateGLWindowAndSurface( pGLSurface.mNativeData, x11WindowCreateInfo, pCreateInfo.visualConfig );
		sysX11UpdateNewWindowState( pGLSurface.mNativeData, x11WindowCreateInfo );
	}

	void GLDriver::_sysCreateDisplaySurfaceForCurrentThread( GLDisplaySurface & pGLSurface )
	{
		pGLSurface.mNativeData.display = glXGetCurrentDisplay();
		pGLSurface.mNativeData.xwindow = glXGetCurrentDrawable();

		XWindowAttributes windowAttributes;
		XGetWindowAttributes( pGLSurface.mNativeData.display,
		                      pGLSurface.mNativeData.xwindow,
		                      &windowAttributes );

		pGLSurface.mNativeData.colormap = windowAttributes.colormap;
	}

	void GLDriver::_sysCreateRenderContext( GLRenderContext & pGLRenderContext, GLDisplaySurface & pGLSurface, const GLRenderContextCreateInfo & pCreateInfo )
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

		if ( pCreateInfo.targetAPIProfile == EGLAPIProfile::GLES )
		{
			contextProfile = GLX_CONTEXT_ES_PROFILE_BIT_EXT;
		}
		if ( pCreateInfo.targetAPIProfile == EGLAPIProfile::Legacy )
		{
			contextProfile = GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB;
		}
		if ( pCreateInfo.flags.isSet( E_GL_RENDER_CONTEXT_CREATE_FLAG_ENABLE_DEBUG_BIT ) )
		{
			contextCreateFlags |= GLX_CONTEXT_DEBUG_BIT_ARB;
		}
		if ( pCreateInfo.flags.isSet( E_GL_RENDER_CONTEXT_CREATE_FLAG_FORWARD_COMPATIBLE_BIT ) )
		{
			contextCreateFlags |= GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB;
		}
		if ( pCreateInfo.flags.isSet( E_GL_RENDER_CONTEXT_CREATE_FLAG_ENABLE_SHARING_BIT ) )
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
		                                                        pGLSurface.mNativeData.fbConfig,
		                                                        shareContextHandle,
		                                                        True,
		                                                        &( contextAttribs[0] ) );

		if ( contextHandle == nullptr )
		{
			throw 0;
		}

		pGLRenderContext.mNativeData.display = pGLSurface.mNativeData.display;
		pGLRenderContext.mNativeData.targetSurface = pGLSurface.mNativeData.xwindow;
		pGLRenderContext.mNativeData.contextHandle = contextHandle;
	}

	void GLDriver::_sysCreateRenderContextForCurrentThread( GLRenderContext & pGLRenderContext )
	{
		auto contextHandle = ::glXGetCurrentContext();
		if ( contextHandle == nullptr )
		{
			throw 0;
		}
		pGLRenderContext.mNativeData.contextHandle = contextHandle;
	}

	
	void _x11CreateGLWindowAndSurface( GLDisplaySurfaceNativeData & pGLSurfaceNativeData,
	                                   X11WindowCreateInfo & pWindowCreateInfo,
	                                   const VisualConfig & pVisualConfig )
	{
		GLXFBConfig windowFBConfig = nullptr;

		if( pVisualConfig.flags.isSet( VISUAL_ATTRIB_FLAG_LEGACY_BIT ) )
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

		pGLSurfaceNativeData.fbConfig = windowFBConfig;
		pGLSurfaceNativeData.visualInfo = fbConfigVisualInfo;

		pWindowCreateInfo.colorDepth = fbConfigVisualInfo->depth;
		pWindowCreateInfo.windowVisual = fbConfigVisualInfo->visual;

		sysX11CreateWindow( pGLSurfaceNativeData, pWindowCreateInfo );
	}

	void _x11DestroyGLWindowAndSurface( GLDisplaySurfaceNativeData & pGLSurfaceNativeData )
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

	GLXFBConfig _x11ChooseCoreGLFBConfig( Display * pDisplay, int pScreenIndex, const VisualConfig & pVisualConfig )
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

    static constexpr size_t cxX11MaxGLXFBConfigAttributesNum = 64u;

	std::vector<GLXFBConfig> _x11QueryCompatibleFBConfigList( Display * pDisplay, int pScreenIndex, const VisualConfig & pVisualConfig )
	{
		std::vector<GLXFBConfig> result;

		int fbConfigAttribArray[cxX11MaxGLXFBConfigAttributesNum];
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

	int _x11GetFBConfigMatchRate( Display * pDisplay, int pScreenIndex, GLXFBConfig pFBConfig, const VisualConfig & pVisualConfig )
	{
		int doubleBufferRequestedState = True;
		int stereoModeRequestedState = False;

		if ( pVisualConfig.flags.isSet( VISUAL_ATTRIB_FLAG_SINGLE_BUFFER_BIT ) &&
		     !pVisualConfig.flags.isSet( VISUAL_ATTRIB_FLAG_DOUBLE_BUFFER_BIT ) )
		{
			doubleBufferRequestedState = False;
		}

		if ( pVisualConfig.flags.isSet( VISUAL_ATTRIB_FLAG_STEREO_DISPLAY_BIT ) &&
		     !pVisualConfig.flags.isSet( VISUAL_ATTRIB_FLAG_MONO_DISPLAY_BIT ) )
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

		pAttribArray[attribIndex++] = GLX_DRAWABLE_TYPE;
		pAttribArray[attribIndex++] = GLX_WINDOW_BIT;

		pAttribArray[attribIndex++] = GLX_RENDER_TYPE;
		pAttribArray[attribIndex++] = GLX_RGBA_BIT;

		pAttribArray[attribIndex++] = GLX_X_RENDERABLE;
		pAttribArray[attribIndex++] = True;

		pAttribArray[attribIndex++] = GLX_X_VISUAL_TYPE;
		pAttribArray[attribIndex++] = GLX_TRUE_COLOR;

		if ( pVisualConfig.flags.isSet( VISUAL_ATTRIB_FLAG_DOUBLE_BUFFER_BIT ) )
		{
			pAttribArray[attribIndex++] = GLX_DOUBLEBUFFER;
			pAttribArray[attribIndex++] = True;
		}
		else if ( pVisualConfig.flags.isSet( VISUAL_ATTRIB_FLAG_SINGLE_BUFFER_BIT ) )
		{
			pAttribArray[attribIndex++] = GLX_DOUBLEBUFFER;
			pAttribArray[attribIndex++] = False;
		}

		if ( pVisualConfig.flags.isSet( VISUAL_ATTRIB_FLAG_MONO_DISPLAY_BIT ) )
		{
			pAttribArray[attribIndex++] = GLX_STEREO;
			pAttribArray[attribIndex++] = False;
		}
		else if ( pVisualConfig.flags.isSet( VISUAL_ATTRIB_FLAG_STEREO_DISPLAY_BIT ) )
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
