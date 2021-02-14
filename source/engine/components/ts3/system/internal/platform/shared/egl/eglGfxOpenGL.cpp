
#include <ts3/system/internal/internalGfxOpenGL.h>

namespace ts3
{

	std::vector<EGLConfig> _internalEGLQueryCompatibleEGLConfigList( EGLDisplay pDisplay, const SysVisualConfig & pVisualConfig );
	int _internalEGLGetEGLConfigMatchRate( EGLDisplay pDisplay, EGLConfig pEGLConfig, const SysVisualConfig & pVisualConfig );
	void _internalEGLGetAttribArrayForVisualConfig( const SysVisualConfig & pVisualConfig, int * pAttribArray );
	void _internalEGLValidateRequestedContextVersion( Version & pVersion );

	void eglInitializeGLSubsystem( SysGLSubsystem & pDriver )
	{
		EGLDisplay eglDisplay = ::eglGetDisplay( EGL_DEFAULT_DISPLAY );
		
		EGLint eglVersionMajor = 0;
		EGLint eglVersionMinor = 0;
		auto initResult = ::eglInitialize( eglDisplay, &eglVersionMajor, &eglVersionMinor );

		if( initResult == EGL_FALSE )
		{
			// auto errorMessage = eglQueryLastErrorMessage();
			throw 0;
		}

		pDriver.nativeData->display = eglDisplay;
		pDriver.nativeData->eglVersion.major = eglVersionMajor;
		pDriver.nativeData->eglVersion.minor = eglVersionMinor;
	}

	void eglReleaseGLSubsystem( SysGLSubsystem & pDriver )
	{
		::eglTerminate( pDriver.nativeData->display );
		pDriver.nativeData->display = EGL_NO_DISPLAY;
	}

	EGLConfig eglChooseCoreEGLConfig( EGLDisplay pDisplay, const SysVisualConfig & pVisualConfig )
	{
		auto EGLConfigList = _internalEGLQueryCompatibleEGLConfigList( pDisplay, pVisualConfig );

		int bestMatchRate = 0;
		EGLConfig bestEGLConfig = nullptr;

		for ( auto EGLConfig : EGLConfigList )
		{
			int matchRate = _internalEGLGetEGLConfigMatchRate( pDisplay, EGLConfig, pVisualConfig );
			if ( matchRate > bestMatchRate )
			{
				bestMatchRate = matchRate;
				bestEGLConfig = EGLConfig;
			}
		}

		return bestEGLConfig;
	}

	void eglCreateSurface( SysGLSurface & pSurface, EGLDisplay pEGLDisplay, EGLNativeWindowType pWindow, EGLConfig pEGLConfig )
	{
		EGLSurface surfaceHandle = ::eglCreateWindowSurface( pEGLDisplay,
															 pEGLConfig,
															 pWindow,
															 nullptr );

		if( surfaceHandle == EGL_NO_SURFACE )
		{
			throw 0;
		}

		pSurface.nativeData->display = pEGLDisplay;
		pSurface.nativeData->surfaceHandle = surfaceHandle;
		pSurface.nativeData->nativeWindow = pWindow;
		pSurface.nativeData->fbConfig = pEGLConfig;
	}

	void eglCreateCoreContext( SysGLContext & pContext, const SysGLContextCreateInfo & pCreateInfo )
	{
		auto * surfaceNativeData = pCreateInfo.displaySurface->nativeData;

		int contextProfile = EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT;
		Bitmask<int> contextCreateFlags = 0;
		EGLContext shareContextHandle = nullptr;

		if ( pCreateInfo.targetAPIProfile == ESysGLAPIProfile::Legacy )
		{
			contextProfile = EGL_CONTEXT_OPENGL_COMPATIBILITY_PROFILE_BIT;
		}
		if ( pCreateInfo.flags.isSet( E_SYS_GFX_GL_RENDER_CONTEXT_CREATE_FLAG_ENABLE_DEBUG_BIT ) )
		{
			contextCreateFlags |= EGL_CONTEXT_OPENGL_DEBUG_BIT_KHR;
		}
		if ( pCreateInfo.flags.isSet( E_SYS_GFX_GL_RENDER_CONTEXT_CREATE_FLAG_FORWARD_COMPATIBLE_BIT ) )
		{
			contextCreateFlags |= EGL_CONTEXT_OPENGL_FORWARD_COMPATIBLE_BIT_KHR;
		}
		if ( pCreateInfo.flags.isSet( E_SYS_GFX_GL_RENDER_CONTEXT_CREATE_FLAG_ENABLE_SHARING_BIT ) )
		{
			if( pCreateInfo.shareContext != nullptr )
			{
				shareContextHandle = pCreateInfo.shareContext->nativeData->contextHandle;
			}
		}

		Version contextVersion;
		contextVersion.major = pCreateInfo.requiredAPIVersion.major;
		contextVersion.minor = pCreateInfo.requiredAPIVersion.minor;
		_internalEGLValidateRequestedContextVersion( contextVersion );

		const EGLint contextAttribs[] =
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

		EGLContext contextHandle = ::eglCreateContext( surfaceNativeData->display,
													   surfaceNativeData->fbConfig,
													   shareContextHandle,
													   contextAttribs );

		if( contextHandle == EGL_NO_CONTEXT )
		{
			contextHandle = ::eglCreateContext( surfaceNativeData->display,
												surfaceNativeData->fbConfig,
												nullptr,
												nullptr );

			if( contextHandle == EGL_NO_CONTEXT )
			{
				throw 0;
			}
		}

		pContext.nativeData->display = surfaceNativeData->display;
		pContext.nativeData->surfaceHandle = surfaceNativeData->surfaceHandle;
		pContext.nativeData->contextHandle = contextHandle;

		::eglMakeCurrent( surfaceNativeData->display,
						  pContext.nativeData->surfaceHandle,
						  pContext.nativeData->surfaceHandle,
						  pContext.nativeData->contextHandle );
	}

    std::vector<EGLConfig> _internalEGLQueryCompatibleEGLConfigList( EGLDisplay pDisplay, const SysVisualConfig & pVisualConfig )
    {
        std::vector<EGLConfig> result;

        int eglConfigAttribArray[cvSysEGLMaxEGLConfigAttributesNum];
		_internalEGLGetAttribArrayForVisualConfig( pVisualConfig, eglConfigAttribArray );

		// Output array where system will store IDs of enumerated pixel formats.
		EGLConfig eglConfigArray[cvSysEGLMaxEGLConfigsNum];
		// Number of pixel formats returned by the system.
		EGLint returnedEGLConfigsNum = 0U;

		// Enumerate framebuffer configs.
		EGLBoolean enumResult = ::eglChooseConfig( pDisplay,
												   eglConfigAttribArray,
												   eglConfigArray,
												   cvSysEGLMaxEGLConfigsNum,
												   &returnedEGLConfigsNum );

		if ( ( enumResult == EGL_FALSE ) || ( returnedEGLConfigsNum <= 0 ) )
		{
			throw 0;
		}

        result.reserve( static_cast<size_t>( returnedEGLConfigsNum ) );
        result.assign( &( eglConfigArray[0] ), &( eglConfigArray[returnedEGLConfigsNum] ) );

        return result;
    }

	int _internalEGLGetEGLConfigMatchRate( EGLDisplay pDisplay, EGLConfig pEGLConfig, const SysVisualConfig & pVisualConfig )
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

	void _internalEGLGetAttribArrayForVisualConfig( const SysVisualConfig & pVisualConfig, int * pAttribArray )
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

		if ( pVisualConfig.colorDesc.rgba.hex != 0 )
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

		pAttribArray[attribIndex++] = 0;
	}

	void _internalEGLValidateRequestedContextVersion( Version & pVersion )
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

}
