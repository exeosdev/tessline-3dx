
#include <ts3/system/openGLNative.h>
#include <ts3/system/sysContextNative.h>

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_X11 )
namespace ts3::system
{

	// Creates X11 OpenGL surface using provided window create attributes and visual config.
	void _x11CreateGLWindowAndSurface( GLDisplaySurfaceNativeData & pGLSurfaceNativeData,
	                                   X11WindowCreateInfo & pWindowCreateInfo,
	                                   const VisualConfig & pVisualConfig );

	// Destroys existing surface and corresponding window.
	void _x11DestroyGLWindowAndSurface( GLDisplaySurfaceNativeData & pGLSurfaceNativeData );

    // Destroys existing surface and corresponding window.
    void _x11CreateAndBindLegacyRenderContext( GLRenderContextNativeData & pGLContextNativeData,
                                               const GLDisplaySurfaceNativeData & pGLSurfaceNativeData );

	// Selects matching FBConfig for a GL surface. Format is selected using current system's configuration. Uses legacy API.
	GLXFBConfig _x11ChooseLegacyGLFBConfig( XDisplay pDisplay, int pScreenIndex );

	// Selects matching pixel format for surface described with a VisualConfig. Uses new EXT API and supports stuff like MSAA.
	GLXFBConfig _x11ChooseCoreGLFBConfig( XDisplay pDisplay, int pScreenIndex, const VisualConfig & pVisualConfig );

	// Returns an array of FBConfigs matching specified VisualConfig structure.
	std::vector<GLXFBConfig> _x11QueryCompatibleFBConfigList( XDisplay pDisplay, int pScreenIndex, const VisualConfig & pVisualConfig );

	// Computes a "compatibility rate", i.e. how much the specified FBConfig matches the visual.
	int _x11GetFBConfigMatchRate( XDisplay pDisplay, GLXFBConfig pFBConfig, const VisualConfig & pVisualConfig );

	// Translation: VisualConfig --> array of GLX_* attributes required by the system API. Used for surface/context creation.
	void _x11GetAttribArrayForVisualConfig( const VisualConfig & pVisualConfig, int * pAttribArray );


    void GLSystemDriver::_nativeCtor()
    {
        mInternal->nativeDataPriv.setSessionData( nativeX11GetXSessionData( *mSysContext ) );
    }

    void GLSystemDriver::_nativeDtor() noexcept
    {
        mInternal->nativeDataPriv.resetSessionData();
    }

	void GLSystemDriver::_nativeInitializePlatform()
	{
        ts3DebugAssert( !mInternal->nativeDataPriv.initState );
        // Init state should be first created here and destroyed as soon as proper GL
        // contexts are created (this is not enforce, though, and controlled explicitly
        // by the user and done by calling releaseInitState() method od the driver).
        mInternal->nativeDataPriv.initState = new GLSystemDriverNativeData::InitState();

        auto & xSessionData = nativeX11GetXSessionData( mInternal->nativeDataPriv );

		int glxVersionMajor = 0;
		int glxVersionMinor = 0;

		::glXQueryVersion( xSessionData.display, &glxVersionMajor, &glxVersionMinor );
        // We need at least version 1.3 of the GLX runtime (that's rather a pretty old one...).
		if( ( glxVersionMajor <= 0 ) || ( ( glxVersionMajor == 1 ) && ( glxVersionMinor < 3 ) ) )
		{
			ts3ThrowAuto( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
		}

		VisualConfig legacyVisualConfig;
		legacyVisualConfig = vsxGetDefaultVisualConfigForSysWindow();
		legacyVisualConfig.flags.set( VISUAL_ATTRIB_FLAG_LEGACY_BIT );

		X11WindowCreateInfo windowCreateInfo;
        windowCreateInfo.setSessionData( xSessionData );

        auto & tmpSurfaceNativeData = mInternal->nativeDataPriv.initState->surfaceData;
		_x11CreateGLWindowAndSurface( tmpSurfaceNativeData, windowCreateInfo, legacyVisualConfig );

        auto & tmpContextNativeData = mInternal->nativeDataPriv.initState->contextData;
        _x11CreateAndBindLegacyRenderContext( tmpContextNativeData, tmpSurfaceNativeData );

        auto glewResult = glewInit();
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

        auto & xSessionData = nativeX11GetXSessionData( mInternal->nativeDataPriv );

        auto & tmpSurfaceNativeData = mInternal->nativeDataPriv.initState->surfaceData;
        auto & tmpContextNativeData = mInternal->nativeDataPriv.initState->contextData;

		if( tmpContextNativeData.contextHandle != nullptr )
		{
			::glXDestroyContext( xSessionData.display, tmpContextNativeData.contextHandle );
			tmpContextNativeData.contextHandle = nullptr;
		}

		if( tmpSurfaceNativeData.windowXID != E_X11_XID_NONE )
		{
			_x11DestroyGLWindowAndSurface( tmpSurfaceNativeData );
			tmpSurfaceNativeData.fbConfig = nullptr;
		}

		if( tmpSurfaceNativeData.visualInfo != nullptr )
		{
			XFree( tmpSurfaceNativeData.visualInfo );
			tmpSurfaceNativeData.visualInfo = nullptr;
		}

        // Explicitly reset X11SessionData pointers. Not really important in terms of resource/memory
        // management (it's just a pointer to the actual data in the SysContext), but useful for debugging.
        tmpContextNativeData.resetSessionData();
        tmpSurfaceNativeData.resetSessionData();

        delete mInternal->nativeDataPriv.initState;
        mInternal->nativeDataPriv.initState = nullptr;
	}

	void GLSystemDriver::_nativeCreateDisplaySurface( GLDisplaySurface & pDisplaySurface, const GLDisplaySurfaceCreateInfo & pCreateInfo )
	{
        auto & xSessionData = nativeX11GetXSessionData( mInternal->nativeDataPriv );

		X11WindowCreateInfo x11WindowCreateInfo;
        x11WindowCreateInfo.setSessionData( xSessionData );
		x11WindowCreateInfo.commonProperties.geometry = pCreateInfo.windowGeometry;
		x11WindowCreateInfo.commonProperties.title = "TS3 OpenGL Window";
		x11WindowCreateInfo.fullscreenMode = pCreateInfo.flags.isSet( E_GL_DISPLAY_SURFACE_CREATE_FLAG_FULLSCREEN_BIT );

		_x11CreateGLWindowAndSurface( pDisplaySurface.mInternal->nativeDataPriv, x11WindowCreateInfo, pCreateInfo.visualConfig );

		nativeX11UpdateNewWindowState( pDisplaySurface.mInternal->nativeDataPriv, x11WindowCreateInfo );
	}

	void GLSystemDriver::_nativeCreateDisplaySurfaceForCurrentThread( GLDisplaySurface & pDisplaySurface )
	{
        auto & xSessionData = nativeX11GetXSessionData( mInternal->nativeDataPriv );

        auto & surfaceNativeData = pDisplaySurface.mInternal->nativeDataPriv;
        surfaceNativeData.setSessionData( xSessionData );
        surfaceNativeData.windowXID = glXGetCurrentDrawable();

		XWindowAttributes windowAttributes;
		XGetWindowAttributes( xSessionData.display, surfaceNativeData.windowXID, &windowAttributes );

        surfaceNativeData.xColormap = windowAttributes.colormap;
	}

    void GLSystemDriver::_nativeDestroyDisplaySurface( GLDisplaySurface & pDisplaySurface )
    {
        nativeX11DestroyWindow( pDisplaySurface.mInternal->nativeDataPriv );
    }

	void GLSystemDriver::_nativeCreateRenderContext( GLRenderContext & pRenderContext,
                                                     const GLDisplaySurface & pDisplaySurface,
                                                     const GLRenderContextCreateInfo & pCreateInfo )
	{
        auto & xSessionData = nativeX11GetXSessionData( mInternal->nativeDataPriv );

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

		Bitmask<int> contextCreateFlags = 0;
		GLXContext shareContextHandle = nullptr;
        int contextProfile = GLX_CONTEXT_CORE_PROFILE_BIT_ARB;

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
				shareContextHandle = pCreateInfo.shareContext->mNativeData->contextHandle;
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

        const auto & surfaceNativeData = pDisplaySurface.mInternal->nativeDataPriv;
		GLXContext contextHandle = glXCreateContextAttribsProc( xSessionData.display,
                                                                surfaceNativeData.fbConfig,
                                                                shareContextHandle,
                                                                True,
                                                                &( contextAttribs[0] ) );
		if ( contextHandle == nullptr )
		{
			ts3ThrowAuto( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
		}

        auto & contextNativeData = pRenderContext.mInternal->nativeDataPriv;
		contextNativeData.setSessionData( xSessionData );
		contextNativeData.contextHandle = contextHandle;
	}

	void GLSystemDriver::_nativeCreateRenderContextForCurrentThread( GLRenderContext & pRenderContext )
	{
        auto & xSessionData = nativeX11GetXSessionData( mInternal->nativeDataPriv );

		auto contextHandle = ::glXGetCurrentContext();
		if ( contextHandle == nullptr )
		{
			ts3ThrowAuto( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
		}

        auto & contextNativeData = pRenderContext.mInternal->nativeDataPriv;
        contextNativeData.setSessionData( xSessionData );
        contextNativeData.contextHandle = contextHandle;
	}

    void GLSystemDriver::_nativeDestroyRenderContext( GLRenderContext & pRenderContext )
    {
    }

    void GLSystemDriver::_nativeResetContextBinding()
    {
    }

    bool GLSystemDriver::_nativeIsRenderContextBound() const
    {
        auto currentContext = ::glXGetCurrentContext();
        return currentContext != nullptr;
    }

    bool GLSystemDriver::_nativeIsRenderContextBound( const GLRenderContext & pRenderContext ) const
    {
        auto currentContext = ::glXGetCurrentContext();
        return currentContext == pRenderContext.mInternal->nativeDataPriv.contextHandle;
    }

    bool GLSystemDriver::_nativeIsDisplaySurfaceValid( GLDisplaySurface & pDisplaySurface ) const
    {
        return pDisplaySurface.mInternal->nativeDataPriv.fbConfig != nullptr;
    }

    bool GLSystemDriver::_nativeIsRenderContextValid( const GLRenderContext & pRenderContext ) const
    {
        return pRenderContext.mInternal->nativeDataPriv.contextHandle != nullptr;
    }


    void GLDisplaySurface::_nativeSwapBuffers()
    {
        auto & xSessionData = nativeX11GetXSessionData( mInternal->nativeDataPriv );
        glXSwapBuffers( xSessionData.display, mInternal->nativeDataPriv.windowXID );
    }

    void GLDisplaySurface::_nativeDestroy() noexcept
    {
        _x11DestroyGLWindowAndSurface( mInternal->nativeDataPriv );
    }

    void GLDisplaySurface::_nativeQueryRenderAreaSize( WindowSize & pOutSize ) const
    {
        auto & xSessionData = nativeX11GetXSessionData( mInternal->nativeDataPriv );

        XWindowAttributes windowAttributes;
        XGetWindowAttributes( xSessionData.display,
                              mInternal->nativeDataPriv.windowXID,
                              &windowAttributes );

        pOutSize.x = windowAttributes.width;
        pOutSize.y = windowAttributes.height;
    }

    bool GLDisplaySurface::_nativeIsValid() const
    {
        return mInternal->nativeDataPriv.windowXID != E_X11_XID_NONE;
    }


    void GLRenderContext::_nativeBindForCurrentThread( const GLDisplaySurface & pTargetSurface )
    {
        auto & xSessionData = nativeX11GetXSessionData( mInternal->nativeDataPriv );

        ::glXMakeContextCurrent( xSessionData.display,
                                 pTargetSurface.mInternal->nativeDataPriv.windowXID,
                                 pTargetSurface.mInternal->nativeDataPriv.windowXID,
                                 mInternal->nativeDataPriv.contextHandle );
    }

    void GLRenderContext::_nativeDestroy() noexcept
    {
        auto & xSessionData = mInternal->nativeDataPriv.getSessionData();

        if( mInternal->nativeDataPriv.contextHandle != nullptr )
        {
            if( _nativeIsCurrent() )
            {
                ::glXMakeContextCurrent( xSessionData.display,
                                         E_X11_XID_NONE,
                                         E_X11_XID_NONE,
                                         nullptr );
            }

            ::glXDestroyContext( xSessionData.display, mInternal->nativeDataPriv.contextHandle );

            mInternal->nativeDataPriv.resetSessionData();
            mInternal->nativeDataPriv.contextHandle = nullptr;
        }
    }

    bool GLRenderContext::_nativeIsCurrent() const
    {
        auto currentContext = ::glXGetCurrentContext();
        return mInternal->nativeDataPriv.contextHandle == currentContext;
    }

    bool GLRenderContext::_nativeIsValid() const
    {
        return mInternal->nativeDataPriv.contextHandle != nullptr;
    }

	
	void _x11CreateGLWindowAndSurface( GLDisplaySurfaceNativeData & pGLSurfaceNativeData,
	                                   X11WindowCreateInfo & pWindowCreateInfo,
	                                   const VisualConfig & pVisualConfig )
	{
        auto & xSessionData = nativeX11GetXSessionData( pWindowCreateInfo );

		GLXFBConfig windowFBConfig = nullptr;
		if( pVisualConfig.flags.isSet( VISUAL_ATTRIB_FLAG_LEGACY_BIT ) )
		{
			windowFBConfig = _x11ChooseLegacyGLFBConfig( xSessionData.display,
                                                         xSessionData.screenIndex );
		}
		else
		{
			windowFBConfig = _x11ChooseCoreGLFBConfig( xSessionData.display,
                                                       xSessionData.screenIndex,
                                                       pVisualConfig );
		}

		if( windowFBConfig == nullptr )
		{
			ts3ThrowAuto( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
		}

		auto * fbConfigVisualInfo = glXGetVisualFromFBConfig( xSessionData.display, windowFBConfig );
		if( fbConfigVisualInfo == nullptr )
		{
			ts3ThrowAuto( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
		}

		pGLSurfaceNativeData.fbConfig = windowFBConfig;
		pGLSurfaceNativeData.visualInfo = fbConfigVisualInfo;

		pWindowCreateInfo.colorDepth = fbConfigVisualInfo->depth;
		pWindowCreateInfo.windowVisual = fbConfigVisualInfo->visual;

        nativeX11CreateWindow( pGLSurfaceNativeData, pWindowCreateInfo );
	}

	void _x11DestroyGLWindowAndSurface( GLDisplaySurfaceNativeData & pGLSurfaceNativeData )
	{
        auto & xSessionData = nativeX11GetXSessionData( pGLSurfaceNativeData );

		XUnmapWindow( xSessionData.display, pGLSurfaceNativeData.windowXID );
        nativeX11DestroyWindow( pGLSurfaceNativeData );
	}

    void _x11CreateAndBindLegacyRenderContext( GLRenderContextNativeData & pGLContextNativeData,
                                               const GLDisplaySurfaceNativeData & pGLSurfaceNativeData )
    {
        auto & xSessionData = nativeX11GetXSessionData( pGLSurfaceNativeData );

        auto tempContextHandle = ::glXCreateContext( xSessionData.display,
                                                     pGLSurfaceNativeData.visualInfo,
                                                     nullptr,
                                                     True );
        if ( tempContextHandle == nullptr )
        {
            ts3ThrowAuto( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
        }

        pGLContextNativeData.setSessionData( xSessionData );
        pGLContextNativeData.contextHandle = tempContextHandle;

        auto makeCurrentResult = ::glXMakeCurrent( xSessionData.display,
                                                   pGLSurfaceNativeData.windowXID,
                                                   pGLContextNativeData.contextHandle );
        if ( makeCurrentResult == False )
        {
            ts3ThrowAuto( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
        }
    }

	GLXFBConfig _x11ChooseLegacyGLFBConfig( XDisplay pDisplay, int pScreenIndex )
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
            E_X11_XID_NONE,
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
			E_X11_XID_NONE,
		};

		int fbConfigListSize = 0;
		GLXFBConfig * fbConfigList = glXChooseFBConfig( pDisplay, pScreenIndex, defaultVisualAttribs, &fbConfigListSize );

		if( ( fbConfigList == nullptr ) || ( fbConfigListSize == 0 ) )
		{
			fbConfigList = glXChooseFBConfig( pDisplay, pScreenIndex, defaultVisualAttribsNoDepthStencil, &fbConfigListSize );
			if( ( fbConfigList == nullptr ) || ( fbConfigListSize == 0 ) )
			{
				ts3ThrowAuto( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
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

	GLXFBConfig _x11ChooseCoreGLFBConfig( XDisplay pDisplay, int pScreenIndex, const VisualConfig & pVisualConfig )
	{
		auto fbConfigList = _x11QueryCompatibleFBConfigList( pDisplay, pScreenIndex, pVisualConfig );

		int bestMatchRate = 0;
		GLXFBConfig bestFBConfig = nullptr;

		for ( auto fbConfig : fbConfigList )
		{
			int matchRate = _x11GetFBConfigMatchRate( pDisplay, fbConfig, pVisualConfig );
			if ( matchRate > bestMatchRate )
			{
				bestMatchRate = matchRate;
				bestFBConfig = fbConfig;
			}
		}

		return bestFBConfig;
	}

    static constexpr size_t cxX11MaxGLXFBConfigAttributesNum = 64u;

	std::vector<GLXFBConfig> _x11QueryCompatibleFBConfigList( XDisplay pDisplay, int pScreenIndex, const VisualConfig & pVisualConfig )
	{
		std::vector<GLXFBConfig> result;

		int fbConfigAttribArray[cxX11MaxGLXFBConfigAttributesNum];
		_x11GetAttribArrayForVisualConfig( pVisualConfig, fbConfigAttribArray );

		int pFBConfigListSize = 0;
		auto * pFBConfigList = glXChooseFBConfig( pDisplay, pScreenIndex, fbConfigAttribArray, &pFBConfigListSize );

		if ( ( pFBConfigList == nullptr ) || ( pFBConfigListSize == 0 ) )
		{
			ts3ThrowAuto( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
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

	int _x11GetFBConfigMatchRate( XDisplay pDisplay, GLXFBConfig pFBConfig, const VisualConfig & pVisualConfig )
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

		pAttribArray[attribIndex++] = E_X11_XID_NONE;
	}

} // namespace ts3::system
#endif
