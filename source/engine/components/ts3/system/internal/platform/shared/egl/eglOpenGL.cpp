
#include <ts3/system/openGLNative.h>

namespace ts3::system
{

	std::vector<EGLConfig> _eglQueryCompatibleEGLConfigList( EGLDisplay pDisplay, const VisualConfig & pVisualConfig );
	int _eglGetEGLConfigMatchRate( EGLDisplay pDisplay, EGLConfig pEGLConfig, const VisualConfig & pVisualConfig );
	void _eglGetAttribArrayForVisualConfig( const VisualConfig & pVisualConfig, int * pAttribArray );
	void _eglValidateRequestedContextVersion( Version & pVersion );

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
		::eglTerminate( pEGLDriverNativeData.eDisplay );
		pEGLDriverNativeData.eDisplay = EGL_NO_DISPLAY;
		pEGLDriverNativeData.eglVersion.major = 0;
		pEGLDriverNativeData.eglVersion.minor = 0;
	}

	EGLConfig nativeEGLChooseCoreFBConfig( EGLDisplay pDisplay, const VisualConfig & pVisualConfig )
	{
		auto EGLConfigList = _eglQueryCompatibleEGLConfigList( pDisplay, pVisualConfig );

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

		return bestEGLConfig;
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
		if( pVisualConfig.colorDesc.colorSpace == ColorSpace::SRGB )
		{
			surfaceAttributeList = &( sRGBSurfaceAttributeList[0] );
		}

		auto surfaceHandle = ::eglCreateWindowSurface( pEGLDisplay, pEGLConfig, pWindow, surfaceAttributeList );
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
		::eglDestroySurface( pEGLSurfaceNativeData.eDisplay, pEGLSurfaceNativeData.eSurfaceHandle );

		pEGLSurfaceNativeData.eDisplay = nullptr;
		pEGLSurfaceNativeData.eFBConfig = nullptr;
		pEGLSurfaceNativeData.eSurfaceHandle = nullptr;
		pEGLSurfaceNativeData.eNativeWindow = nullptr;
	}

	void nativeEGLCreateCoreContext( EGLRenderContextNativeData & pEGLContextNativeData,
									 const EGLDisplaySurfaceNativeData & pEGLSurfaceNativeData,
									 const GLRenderContextCreateInfo & pCreateInfo )
	{
		int contextProfile = EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT;
		Bitmask<int> contextCreateFlags = 0;
		EGLContext shareContextHandle = nullptr;

		if ( pCreateInfo.targetAPIProfile == EGLAPIProfile::Core )
		{
			contextProfile = EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT;
		}
		else if ( pCreateInfo.targetAPIProfile == EGLAPIProfile::Legacy )
		{
			contextProfile = EGL_CONTEXT_OPENGL_COMPATIBILITY_PROFILE_BIT;
		}

		if ( pCreateInfo.flags.isSet( E_GL_RENDER_CONTEXT_CREATE_FLAG_ENABLE_DEBUG_BIT ) )
		{
			contextCreateFlags |= EGL_CONTEXT_OPENGL_DEBUG_BIT_KHR;
		}
		if ( pCreateInfo.flags.isSet( E_GL_RENDER_CONTEXT_CREATE_FLAG_FORWARD_COMPATIBLE_BIT ) )
		{
			contextCreateFlags |= EGL_CONTEXT_OPENGL_FORWARD_COMPATIBLE_BIT_KHR;
		}
		if ( pCreateInfo.flags.isSet( E_GL_RENDER_CONTEXT_CREATE_FLAG_ENABLE_SHARING_BIT ) )
		{
			if( pCreateInfo.shareContext )
			{
				shareContextHandle = pCreateInfo.shareContext->mNativeData->eContextHandle;
			}
		}

		Version contextVersion;
		contextVersion.major = pCreateInfo.requiredAPIVersion.major;
		contextVersion.minor = pCreateInfo.requiredAPIVersion.minor;
		_eglValidateRequestedContextVersion( contextVersion );

		const EGLint contextAttributes[] =
		{
			// Requested OpenGL API version: major part
			EGL_CONTEXT_MAJOR_VERSION_KHR, contextVersion.major,
			// Requested OpenGL API version: minor part
			EGL_CONTEXT_MINOR_VERSION_KHR, contextVersion.minor,
			//
			EGL_CONTEXT_OPENGL_PROFILE_MASK_KHR, contextProfile,
			//
			EGL_CONTEXT_FLAGS_KHR, contextCreateFlags,
			// Terminator
			EGL_NONE
		};

		EGLContext contextHandle = ::eglCreateContext( pEGLSurfaceNativeData.eDisplay,
													   pEGLSurfaceNativeData.eFBConfig,
													   shareContextHandle,
													   contextAttributes );

		if( contextHandle == EGL_NO_CONTEXT )
		{
			if( shareContextHandle && pCreateInfo.flags.isSet( E_GL_RENDER_CONTEXT_CREATE_FLAG_SHARING_OPTIONAL_BIT ) )
			{
				contextHandle = ::eglCreateContext( pEGLSurfaceNativeData.eDisplay,
													pEGLSurfaceNativeData.eFBConfig,
													nullptr,
													contextAttributes );
			}
			if( contextHandle == EGL_NO_CONTEXT )
			{
				ts3ThrowAuto( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
			}
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

			::eglDestroyContext( pEGLContextNativeData.eDisplay, pEGLContextNativeData.eContextHandle );

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

    std::vector<EGLConfig> _eglQueryCompatibleEGLConfigList( EGLDisplay pDisplay, const VisualConfig & pVisualConfig )
    {
        std::vector<EGLConfig> result;

        int eglConfigAttribArray[cvEGLMaxEGLConfigAttributesNum];
		_eglGetAttribArrayForVisualConfig( pVisualConfig, eglConfigAttribArray );

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

		if ( ( enumResult == EGL_FALSE ) || ( returnedEGLConfigsNum <= 0 ) )
		{
			ts3ThrowAuto( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER );
		}

        result.reserve( static_cast<size_t>( returnedEGLConfigsNum ) );
        result.assign( &( eglConfigArray[0] ), &( eglConfigArray[returnedEGLConfigsNum] ) );

        return result;
    }

	int _eglGetEGLConfigMatchRate( EGLDisplay pDisplay, EGLConfig pEGLConfig, const VisualConfig & pVisualConfig )
	{
		int matchRate = 0;
        int EGLConfigAttribValue = 0;

        ::eglGetConfigAttrib( pDisplay, pEGLConfig, EGL_DEPTH_SIZE, &EGLConfigAttribValue );
        matchRate += ( EGLConfigAttribValue == pVisualConfig.depthStencilDesc.depthBufferSize );

        ::eglGetConfigAttrib( pDisplay, pEGLConfig, EGL_STENCIL_SIZE, &EGLConfigAttribValue );
        matchRate += ( EGLConfigAttribValue == pVisualConfig.depthStencilDesc.stencilBufferSize );

        ::eglGetConfigAttrib( pDisplay, pEGLConfig, EGL_SAMPLES, &EGLConfigAttribValue );
        matchRate += ( EGLConfigAttribValue == pVisualConfig.msaaDesc.quality );

		return matchRate;
	}

	void _eglGetAttribArrayForVisualConfig( const VisualConfig & pVisualConfig, int * pAttribArray )
	{
		int attribIndex = 0;

		pAttribArray[attribIndex++] = EGL_RENDERABLE_TYPE;
		pAttribArray[attribIndex++] = EGL_OPENGL_ES_BIT;

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
