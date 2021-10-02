
#include <ts3/system/openGLNative.h>

namespace ts3::system
{

	static EGLContext _eglCreateCoreContextDefault( EGLRenderContextNativeData & pEGLContextNativeData,
													const EGLDisplaySurfaceNativeData & pEGLSurfaceNativeData,
													const GLRenderContextCreateInfo & pCreateInfo,
													EGLContext pEGLShareContext );
	static EGLContext _eglCreateCoreContextES( EGLRenderContextNativeData & pEGLContextNativeData,
											   const EGLDisplaySurfaceNativeData & pEGLSurfaceNativeData,
											   const GLRenderContextCreateInfo & pCreateInfo,
											   EGLContext pEGLShareContext );
	static std::vector<EGLConfig> _eglQueryCompatibleEGLConfigList( EGLDisplay pDisplay, const VisualConfig & pVisualConfig, const Version & pTargetAPIVersion );
	static int _eglGetEGLConfigMatchRate( EGLDisplay pDisplay, EGLConfig pEGLConfig, const VisualConfig & pVisualConfig );
	static void _eglGetAttribArrayForVisualConfig( const VisualConfig & pVisualConfig, const Version & pTargetAPIVersion, int * pAttribArray );
	static void _eglValidateRequestedContextVersion( Version & pVersion );

	void nativeEGLInitializeGLDriver( EGLDriverNativeData & pEGLDriverNativeData )
	{
		EGLDisplay eglDisplay = ::eglGetDisplay( EGL_DEFAULT_DISPLAY );

        if( eglDisplay == EGL_NO_DISPLAY )
        {
            // auto errorMessage = eglQueryLastErrorMessage();
            ts3ThrowAuto( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
        }

		EGLint eglVersionMajor = 0;
		EGLint eglVersionMinor = 0;
		auto initResult = ::eglInitialize( eglDisplay, &eglVersionMajor, &eglVersionMinor );

		if( initResult == EGL_FALSE )
		{
			// auto errorMessage = eglQueryLastErrorMessage();
			ts3ThrowAuto( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
		}

		pEGLDriverNativeData.eDisplay = eglDisplay;
		pEGLDriverNativeData.eglVersion.major = eglVersionMajor;
		pEGLDriverNativeData.eglVersion.minor = eglVersionMinor;
	}

	void nativeEGLReleaseGLDriver( EGLDriverNativeData & pEGLDriverNativeData )
	{
		auto eglResult = ::eglTerminate( pEGLDriverNativeData.eDisplay );
		pEGLDriverNativeData.eDisplay = EGL_NO_DISPLAY;
		pEGLDriverNativeData.eglVersion.major = 0;
		pEGLDriverNativeData.eglVersion.minor = 0;
	}

	EGLConfig nativeEGLChooseCoreFBConfig( EGLDisplay pDisplay, const VisualConfig & pVisualConfig, const Version & pTargetAPIVersion )
	{
		auto EGLConfigList = _eglQueryCompatibleEGLConfigList( pDisplay, pVisualConfig, pTargetAPIVersion );

		int bestMatchRate = 0;
		EGLConfig bestEGLConfig = nullptr;

		for ( auto EGLConfig : EGLConfigList )
		{
			int matchRate = _eglGetEGLConfigMatchRate( pDisplay, EGLConfig, pVisualConfig );
			if ( matchRate > bestMatchRate )
			{
				bestMatchRate = matchRate;
				bestEGLConfig = EGLConfig;
			}
		}

		// return bestEGLConfig;
		return EGLConfigList[0];
	}

	EGLint nativeEGLQueryFBConfigAttribute( EGLDisplay pEGLDisplay, EGLConfig pEGLConfig, EGLenum pAttribute )
	{
		EGLint fbConfigAttribute = 0;

		auto eglResult = ::eglGetConfigAttrib( pEGLDisplay, pEGLConfig, pAttribute, &fbConfigAttribute );
		if( eglResult == EGL_FALSE )
		{
			ts3ThrowAuto( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
		}

		return fbConfigAttribute;
	}

	void nativeEGLCreateSurface( EGLDisplaySurfaceNativeData & pEGLSurfaceNativeData,
								 EGLDisplay pEGLDisplay,
								 EGLNativeWindowType pWindow,
								 EGLConfig pEGLConfig,
								 const VisualConfig & pVisualConfig )
	{
		const EGLint defaultSurfaceAttributeList[] = { EGL_NONE };
		const EGLint sRGBSurfaceAttributeList[] = { EGL_GL_COLORSPACE, EGL_GL_COLORSPACE_SRGB, EGL_NONE };

		const EGLint * surfaceAttributeList = &( defaultSurfaceAttributeList[0] );
		if( pVisualConfig.colorDesc.colorSpace == EColorSpace::SRGB )
		{
			surfaceAttributeList = &( sRGBSurfaceAttributeList[0] );
		}

		auto surfaceHandle = ::eglCreateWindowSurface( pEGLDisplay, pEGLConfig, pWindow, surfaceAttributeList );
		ts3EGLHandleLastError();

		if( surfaceHandle == EGL_NO_SURFACE )
		{
			ts3ThrowAuto( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
		}

		pEGLSurfaceNativeData.eDisplay = pEGLDisplay;
		pEGLSurfaceNativeData.eSurfaceHandle = surfaceHandle;
		pEGLSurfaceNativeData.eFBConfig = pEGLConfig;
	}

	void nativeEGLCreateSurfaceForCurrentThread( EGLDisplaySurfaceNativeData & pEGLSurfaceNativeData )
	{
		auto eDisplay = ::eglGetCurrentDisplay();
		auto eSurfaceHandle = ::eglGetCurrentSurface( EGL_DRAW );

		EGLint surfaceConfigID = 0;

		auto eglResult = ::eglQuerySurface( eDisplay, eSurfaceHandle, EGL_CONFIG_ID, &surfaceConfigID );
		if( eglResult == EGL_FALSE )
		{
			ts3ThrowAuto( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
		}

		EGLConfig surfaceConfig;
		EGLint resultConfigsNum;
		EGLint scAttributeList[] = { EGL_CONFIG_ID, surfaceConfigID, EGL_NONE };

		eglResult = ::eglChooseConfig( eDisplay, &( scAttributeList[0] ), &surfaceConfig, 1, &resultConfigsNum );
		if( eglResult == EGL_FALSE )
		{
			ts3ThrowAuto( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
		}

		pEGLSurfaceNativeData.eDisplay = eDisplay;
		pEGLSurfaceNativeData.eSurfaceHandle = eSurfaceHandle;
		pEGLSurfaceNativeData.eFBConfig = surfaceConfig;
	}

	void nativeEGLDestroySurface( EGLDisplaySurfaceNativeData & pEGLSurfaceNativeData )
	{
		auto eglResult = ::eglDestroySurface( pEGLSurfaceNativeData.eDisplay, pEGLSurfaceNativeData.eSurfaceHandle );

		pEGLSurfaceNativeData.eDisplay = nullptr;
		pEGLSurfaceNativeData.eFBConfig = nullptr;
		pEGLSurfaceNativeData.eSurfaceHandle = nullptr;
		pEGLSurfaceNativeData.eNativeWindow = nullptr;
	}

	void nativeEGLCreateCoreContext( EGLRenderContextNativeData & pEGLContextNativeData,
									 const EGLDisplaySurfaceNativeData & pEGLSurfaceNativeData,
									 const GLRenderContextCreateInfo & pCreateInfo )
	{
		auto createInfo = pCreateInfo;
		_eglValidateRequestedContextVersion( createInfo.requiredAPIVersion );

		EGLContext shareContextHandle = EGL_NO_CONTEXT;
		EGLContext contextHandle = EGL_NO_CONTEXT;

		if ( createInfo.flags.isSet( E_GL_RENDER_CONTEXT_CREATE_FLAG_ENABLE_SHARING_BIT ) )
		{
			if( createInfo.shareContext )
			{
				shareContextHandle = createInfo.shareContext->mNativeData->eContextHandle;
			}
		}

		if ( createInfo.targetAPIProfile != EGLAPIProfile::GLES )
		{
			contextHandle = _eglCreateCoreContextDefault( pEGLContextNativeData, pEGLSurfaceNativeData, createInfo, shareContextHandle );
		}

		if( ( createInfo.targetAPIProfile == EGLAPIProfile::GLES ) || !contextHandle )
		{
			contextHandle = _eglCreateCoreContextES( pEGLContextNativeData, pEGLSurfaceNativeData, createInfo, shareContextHandle );
		}

		pEGLContextNativeData.eDisplay = pEGLSurfaceNativeData.eDisplay;
		pEGLContextNativeData.eContextHandle = contextHandle;
	}

	void nativeEGLCreateCoreContextForCurrentThread( EGLRenderContextNativeData & pEGLContextNativeData )
	{
		auto eDisplay = ::eglGetCurrentDisplay();
		auto eContextHandle = ::eglGetCurrentContext();

		pEGLContextNativeData.eDisplay = eDisplay;
		pEGLContextNativeData.eContextHandle = eContextHandle;
	}

	void nativeEGLDestroyRenderContext( EGLRenderContextNativeData & pEGLContextNativeData )
	{
		if( pEGLContextNativeData.eContextHandle != nullptr )
		{
			auto eCurrentContext = ::eglGetCurrentContext();
			if( pEGLContextNativeData.eContextHandle == eCurrentContext )
			{
				::eglMakeCurrent( pEGLContextNativeData.eDisplay, nullptr, nullptr, nullptr );
			}

			auto eglResult = ::eglDestroyContext( pEGLContextNativeData.eDisplay, pEGLContextNativeData.eContextHandle );

			pEGLContextNativeData.eDisplay = nullptr;
			pEGLContextNativeData.eContextHandle = nullptr;
		}
	}

	void nativeEGLBindContextForCurrentThread( const EGLRenderContextNativeData & pEGLContextNativeData,
											   const EGLDisplaySurfaceNativeData & pEGLSurfaceNativeData )
	{
		::eglMakeCurrent( pEGLContextNativeData.eDisplay,
						  pEGLSurfaceNativeData.eSurfaceHandle,
						  pEGLSurfaceNativeData.eSurfaceHandle,
						  pEGLContextNativeData.eContextHandle );
	}

    std::vector<EGLConfig> _eglQueryCompatibleEGLConfigList( EGLDisplay pDisplay, const VisualConfig & pVisualConfig, const Version & pTargetAPIVersion )
    {
        std::vector<EGLConfig> result;

        int eglConfigAttribArray[cvEGLMaxEGLConfigAttributesNum];
		_eglGetAttribArrayForVisualConfig( pVisualConfig, pTargetAPIVersion, eglConfigAttribArray );

		// Output array where system will store IDs of enumerated pixel formats.
		EGLConfig eglConfigArray[cvEGLMaxEGLConfigsNum];
		// Number of pixel formats returned by the system.
		EGLint returnedEGLConfigsNum = 0U;

		// Enumerate framebuffer configs.
		EGLBoolean enumResult = ::eglChooseConfig( pDisplay,
												   eglConfigAttribArray,
												   eglConfigArray,
												   cvEGLMaxEGLConfigsNum,
												   &returnedEGLConfigsNum );
		ts3EGLHandleLastError();

		if ( ( enumResult == EGL_FALSE ) || ( returnedEGLConfigsNum <= 0 ) )
		{
			ts3ThrowAuto( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
		}

        result.reserve( static_cast<size_t>( returnedEGLConfigsNum ) );
        result.assign( &( eglConfigArray[0] ), &( eglConfigArray[returnedEGLConfigsNum] ) );

        return result;
    }


	EGLContext _eglCreateCoreContextDefault( EGLRenderContextNativeData & pEGLContextNativeData,
											 const EGLDisplaySurfaceNativeData & pEGLSurfaceNativeData,
											 const GLRenderContextCreateInfo & pCreateInfo,
											 EGLContext pEGLShareContext )
	{
		int contextProfile = 0;

		if ( pCreateInfo.targetAPIProfile == EGLAPIProfile::Core )
		{
			contextProfile = EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT;
		}
		else if ( pCreateInfo.targetAPIProfile == EGLAPIProfile::Legacy )
		{
			contextProfile = EGL_CONTEXT_OPENGL_COMPATIBILITY_PROFILE_BIT;
		}

		EGLint debugContextFlag = EGL_FALSE;
		if ( pCreateInfo.flags.isSet( E_GL_RENDER_CONTEXT_CREATE_FLAG_ENABLE_DEBUG_BIT ) )
		{
			debugContextFlag = EGL_TRUE;
		}

		EGLint forwardCompatibleContextFlag = EGL_FALSE;
		if ( pCreateInfo.flags.isSet( E_GL_RENDER_CONTEXT_CREATE_FLAG_FORWARD_COMPATIBLE_BIT ) )
		{
			forwardCompatibleContextFlag = EGL_TRUE;
		}

		const EGLint contextAttributesCore[] =
		{
			// Requested OpenGL API version: major part
			EGL_CONTEXT_MAJOR_VERSION, pCreateInfo.requiredAPIVersion.major,
			// Requested OpenGL API version: minor part
			EGL_CONTEXT_MINOR_VERSION, pCreateInfo.requiredAPIVersion.minor,
			//
			EGL_CONTEXT_OPENGL_PROFILE_MASK, contextProfile,
			//
			EGL_CONTEXT_OPENGL_DEBUG, debugContextFlag,
			//
			EGL_CONTEXT_OPENGL_FORWARD_COMPATIBLE, forwardCompatibleContextFlag,
			// Terminator
			EGL_NONE
		};

		EGLContext contextHandle = ::eglCreateContext( pEGLSurfaceNativeData.eDisplay,
													   pEGLSurfaceNativeData.eFBConfig,
													   pEGLShareContext,
													   contextAttributesCore );

		if( contextHandle == EGL_NO_CONTEXT )
		{
			ts3EGLHandleLastError();
			ts3ThrowAuto( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
		}

		return contextHandle;
	}

	EGLContext _eglCreateCoreContextES( EGLRenderContextNativeData & pEGLContextNativeData,
										const EGLDisplaySurfaceNativeData & pEGLSurfaceNativeData,
										const GLRenderContextCreateInfo & pCreateInfo,
										EGLContext pEGLShareContext )
	{
		const EGLint contextAttributesES[] =
		{
			//
			EGL_CONTEXT_CLIENT_VERSION, pCreateInfo.requiredAPIVersion.major,
			// Terminator
			EGL_NONE
		};

		EGLContext contextHandle = ::eglCreateContext( pEGLSurfaceNativeData.eDisplay,
													   pEGLSurfaceNativeData.eFBConfig,
													   pEGLShareContext,
													   contextAttributesES );

		if( contextHandle == EGL_NO_CONTEXT )
		{
			ts3EGLHandleLastError();
			ts3ThrowAuto( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
		}

		return contextHandle;
	}

	int _eglGetEGLConfigMatchRate( EGLDisplay pDisplay, EGLConfig pEGLConfig, const VisualConfig & pVisualConfig )
	{
		int matchRate = 0;
        int EGLConfigAttribValue = 0;

        ::eglGetConfigAttrib( pDisplay, pEGLConfig, EGL_DEPTH_SIZE, &EGLConfigAttribValue );
        ts3EGLHandleLastError();
        matchRate += ( EGLConfigAttribValue == pVisualConfig.depthStencilDesc.depthBufferSize );

        ::eglGetConfigAttrib( pDisplay, pEGLConfig, EGL_STENCIL_SIZE, &EGLConfigAttribValue );
		ts3EGLHandleLastError();
        matchRate += ( EGLConfigAttribValue == pVisualConfig.depthStencilDesc.stencilBufferSize );

        ::eglGetConfigAttrib( pDisplay, pEGLConfig, EGL_SAMPLES, &EGLConfigAttribValue );
		ts3EGLHandleLastError();
        matchRate += ( EGLConfigAttribValue == pVisualConfig.msaaDesc.quality );

		return matchRate;
	}

	void _eglGetAttribArrayForVisualConfig( const VisualConfig & pVisualConfig, const Version & pTargetAPIVersion, int * pAttribArray )
	{
		int attribIndex = 0;
		int renderableType = 0;

		if( pTargetAPIVersion.major == 1 )
		{
			renderableType = EGL_OPENGL_ES_BIT;
		}
		else if( pTargetAPIVersion.major == 2 )
		{
			renderableType = EGL_OPENGL_ES2_BIT;
		}
		else if( pTargetAPIVersion.major == 3 )
		{
			renderableType = EGL_OPENGL_ES3_BIT;
		}

		pAttribArray[attribIndex++] = EGL_RENDERABLE_TYPE;
		pAttribArray[attribIndex++] = renderableType;

		pAttribArray[attribIndex++] = EGL_SURFACE_TYPE;
		pAttribArray[attribIndex++] = EGL_WINDOW_BIT;

		pAttribArray[attribIndex++] = EGL_COLOR_BUFFER_TYPE;
		pAttribArray[attribIndex++] = EGL_RGB_BUFFER;

		if ( ( pVisualConfig.msaaDesc.bufferCount != 0 ) && ( pVisualConfig.msaaDesc.quality != 0 ) )
		{
			pAttribArray[attribIndex++] = EGL_SAMPLE_BUFFERS;
			pAttribArray[attribIndex++] = pVisualConfig.msaaDesc.bufferCount;

			pAttribArray[attribIndex++] = EGL_SAMPLES;
			pAttribArray[attribIndex++] = pVisualConfig.msaaDesc.quality;
		}

		if ( pVisualConfig.colorDesc.rgba.u32Code != 0 )
		{
			pAttribArray[attribIndex++] = EGL_RED_SIZE;
			pAttribArray[attribIndex++] = pVisualConfig.colorDesc.rgba.u8Red;

			pAttribArray[attribIndex++] = EGL_GREEN_SIZE;
			pAttribArray[attribIndex++] = pVisualConfig.colorDesc.rgba.u8Green;

			pAttribArray[attribIndex++] = EGL_BLUE_SIZE;
			pAttribArray[attribIndex++] = pVisualConfig.colorDesc.rgba.u8Blue;

			pAttribArray[attribIndex++] = EGL_ALPHA_SIZE;
			pAttribArray[attribIndex++] = pVisualConfig.colorDesc.rgba.u8Alpha;
		}

		if ( pVisualConfig.depthStencilDesc.depthBufferSize != 0 )
		{
			pAttribArray[attribIndex++] = EGL_DEPTH_SIZE;
			pAttribArray[attribIndex++] = pVisualConfig.depthStencilDesc.depthBufferSize;
		}

		if ( pVisualConfig.depthStencilDesc.stencilBufferSize != 0 )
		{
			pAttribArray[attribIndex++] = EGL_STENCIL_SIZE;
			pAttribArray[attribIndex++] = pVisualConfig.depthStencilDesc.stencilBufferSize;
		}

		pAttribArray[attribIndex++] = EGL_NONE;
	}

	void _eglValidateRequestedContextVersion( Version & pVersion )
	{
		if( pVersion.major == 1 )
		{
			if( pVersion.minor > 1 )
			{
				pVersion.minor = 1;
			}
		}
		else if( pVersion.major == 2 )
		{
			if( pVersion.minor > 0 )
			{
				pVersion.minor = 0;
			}
		}
		else if( pVersion.major == 3 )
		{
			if( pVersion.minor > 2 )
			{
				pVersion.minor = 2;
			}
		}
		else
		{
			pVersion.major = 3;
			pVersion.major = 1;
		}
	}

} // namespace ts3::system
