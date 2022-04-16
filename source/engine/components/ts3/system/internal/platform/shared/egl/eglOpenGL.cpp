
#include "eglOpenGL.h"
#include <ts3/system/openGLDriver.h>

namespace ts3::system
{

	namespace platform
	{

		// Returns an array of EGLConfigs matching specified VisualConfig definition and API version (ES).
		std::vector<EGLConfig> _eglQueryCompatibleEGLConfigList( EGLDisplay pDisplay,
		                                                         const VisualConfig & pVisualConfig,
		                                                         const Version & pTargetAPIVersion );

		// Computes a "compatibility rate", i.e. how much the specified FBConfig matches the visual.
		int _eglGetEGLConfigMatchRate( EGLDisplay pDisplay, EGLConfig pEGLConfig,
		                               const VisualConfig & pVisualConfig );

		// Translation: VisualConfig+APIVersion --> array of EGL_* attributes required by the system API.
		// Used for surface/context creation.
		void _eglGetAttribArrayForVisualConfig( const VisualConfig & pVisualConfig,
		                                        const Version & pTargetAPIVersion,
		                                        int * pAttribArray );

		// Creates an EGL rendering context for the Core/Legacy API profile.
		// Uses full set of context creation features (debug and forward compatibility flags, detailed version, etc).
		EGLContext _eglCreateCoreContextDefault( EGLRenderContextNativeData & pEGLContextNativeData,
		                                         const EGLDisplaySurfaceNativeData & pEGLSurfaceNativeData,
		                                         const GLRenderContextCreateInfo & pCreateInfo,
		                                         EGLContext pEGLShareContext );

		// Creates an EGL rendering context for the ES API profile.
		// Uses only the client API version number.
		EGLContext _eglCreateCoreContextES( EGLRenderContextNativeData & pEGLContextNativeData,
		                                    const EGLDisplaySurfaceNativeData & pEGLSurfaceNativeData,
		                                    const Version & pTargetAPIVersion,
		                                    EGLContext pEGLShareContext );

		// Validates specified API version using selected target API profile (Core/ES).
		// 'Version' parameter is an IN/OUT parameter and updated with a valid version number.
		void _eglValidateRequestedContextVersion( EGLAPIProfile pTargetAPIProfile, Version & pVersion );


		void platform::eglInitializeGLDriver( EGLDriverNativeData & pEGLDriverNativeData )
		{
			EGLDisplay eglDisplay = ::eglGetDisplay( EGL_DEFAULT_DISPLAY );
			if( eglDisplay == EGL_NO_DISPLAY )
			{
				ts3EGLThrowLastError();
			}

			EGLint eglVersionMajor = 0;
			EGLint eglVersionMinor = 0;
			auto initResult = ::eglInitialize( eglDisplay, &eglVersionMajor, &eglVersionMinor );
			if( initResult == EGL_FALSE )
			{
				ts3EGLThrowLastError();
			}

			auto eglQueriedVersion = EGLCoreAPI::queryRuntimeVersion();

			pEGLDriverNativeData.eDisplay = eglDisplay;
			pEGLDriverNativeData.eglVersion.major = eglVersionMajor;
			pEGLDriverNativeData.eglVersion.minor = eglVersionMinor;
		}

		void platform::eglReleaseGLDriver( EGLDriverNativeData & pEGLDriverNativeData )
		{
			auto eglResult = ::eglTerminate( pEGLDriverNativeData.eDisplay );
			pEGLDriverNativeData.eDisplay = EGL_NO_DISPLAY;
			pEGLDriverNativeData.eglVersion.major = 0;
			pEGLDriverNativeData.eglVersion.minor = 0;
		}

		EGLConfig platform::eglChooseCoreFBConfig( EGLDisplay pDisplay,
		                                           const VisualConfig & pVisualConfig,
		                                           const Version & pTargetAPIVersion )
		{
			auto eglConfigList = _eglQueryCompatibleEGLConfigList( pDisplay, pVisualConfig, pTargetAPIVersion );

			int bestMatchRate = 0;
			EGLConfig bestEGLConfig = nullptr;

			for( auto eglConfig : eglConfigList )
			{
				int matchRate = _eglGetEGLConfigMatchRate( pDisplay, eglConfig, pVisualConfig );
				if( matchRate > bestMatchRate )
				{
					bestMatchRate = matchRate;
					bestEGLConfig = eglConfig;
				}
			}

			// return bestEGLConfig;
			return eglConfigList[0];
		}

		EGLint platform::eglQueryFBConfigAttribute( EGLDisplay pEGLDisplay, EGLConfig pEGLConfig, EGLenum pAttribute )
		{
			EGLint fbConfigAttribute = 0;

			auto eglResult = ::eglGetConfigAttrib( pEGLDisplay, pEGLConfig, pAttribute, &fbConfigAttribute );
			if( eglResult == EGL_FALSE )
			{
				ts3EGLThrowLastError();
			}

			return fbConfigAttribute;
		}

		void platform::eglCreateSurface( EGLDisplaySurfaceNativeData & pEGLSurfaceNativeData,
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
			if( surfaceHandle == EGL_NO_SURFACE )
			{
				ts3EGLThrowLastError();
			}

			pEGLSurfaceNativeData.eDisplay = pEGLDisplay;
			pEGLSurfaceNativeData.eNativeWindow = pWindow;
			pEGLSurfaceNativeData.eSurfaceHandle = surfaceHandle;
			pEGLSurfaceNativeData.eFBConfig = pEGLConfig;
		}

		void platform::eglCreateSurfaceForCurrentThread( EGLDisplaySurfaceNativeData & pEGLSurfaceNativeData )
		{
			auto eDisplay = ::eglGetCurrentDisplay();
			auto eSurfaceHandle = ::eglGetCurrentSurface( EGL_DRAW );

			EGLint surfaceConfigID = 0;

			auto eglResult = ::eglQuerySurface( eDisplay, eSurfaceHandle, EGL_CONFIG_ID, &surfaceConfigID );
			if( eglResult == EGL_FALSE )
			{
				ts3EGLThrowLastError();
			}

			EGLConfig surfaceConfig;
			EGLint resultConfigsNum;
			EGLint scAttributeList[] = { EGL_CONFIG_ID, surfaceConfigID, EGL_NONE };

			eglResult = ::eglChooseConfig( eDisplay, &( scAttributeList[0] ), &surfaceConfig, 1, &resultConfigsNum );
			if( eglResult == EGL_FALSE )
			{
				ts3EGLThrowLastError();
			}

			pEGLSurfaceNativeData.eDisplay = eDisplay;
			pEGLSurfaceNativeData.eSurfaceHandle = eSurfaceHandle;
			pEGLSurfaceNativeData.eFBConfig = surfaceConfig;
		}

		void platform::eglDestroySurface( EGLDisplaySurfaceNativeData & pEGLSurfaceNativeData )
		{
			auto eglResult = ::eglDestroySurface( pEGLSurfaceNativeData.eDisplay, pEGLSurfaceNativeData.eSurfaceHandle );

			pEGLSurfaceNativeData.eDisplay = nullptr;
			pEGLSurfaceNativeData.eFBConfig = nullptr;
			pEGLSurfaceNativeData.eSurfaceHandle = nullptr;
			pEGLSurfaceNativeData.eNativeWindow = nullptr;
		}

		void platform::eglCreateCoreContext( EGLRenderContextNativeData & pEGLContextNativeData,
		                                     const EGLDisplaySurfaceNativeData & pEGLSurfaceNativeData,
		                                     const GLRenderContextCreateInfo & pCreateInfo )
		{
			auto createInfo = pCreateInfo;

			_eglValidateRequestedContextVersion( pCreateInfo.runtimeVersionDesc.apiProfile,
			                                 createInfo.runtimeVersionDesc.apiVersion );

			EGLContext shareContextHandle = EGL_NO_CONTEXT;
			EGLContext contextHandle = EGL_NO_CONTEXT;

			if( createInfo.flags.isSet( E_GL_RENDER_CONTEXT_CREATE_FLAG_ENABLE_SHARING_BIT ) )
			{
				// if( createInfo.shareContext )
				// {
				// 	shareContextHandle = createInfo.shareContext->mNativeData->eContextHandle;
				// }
			}

			if( createInfo.runtimeVersionDesc.apiProfile != EGLAPIProfile::OpenGLES )
			{
				contextHandle = _eglCreateCoreContextDefault( pEGLContextNativeData,
				                                              pEGLSurfaceNativeData,
				                                              createInfo,
				                                              shareContextHandle );
			}

			if( ( createInfo.runtimeVersionDesc.apiProfile == EGLAPIProfile::OpenGLES ) || !contextHandle )
			{
				contextHandle = _eglCreateCoreContextES( pEGLContextNativeData,
				                                         pEGLSurfaceNativeData,
				                                         createInfo.runtimeVersionDesc.apiVersion,
				                                         shareContextHandle );
			}

			if( !contextHandle )
			{
				ts3EGLThrowError( "Failed to create EGL context" );
			}

			pEGLContextNativeData.eDisplay = pEGLSurfaceNativeData.eDisplay;
			pEGLContextNativeData.eContextHandle = contextHandle;
		}

		void platform::eglCreateCoreContextForCurrentThread( EGLRenderContextNativeData & pEGLContextNativeData )
		{
			auto eDisplay = ::eglGetCurrentDisplay();
			auto eContextHandle = ::eglGetCurrentContext();

			pEGLContextNativeData.eDisplay = eDisplay;
			pEGLContextNativeData.eContextHandle = eContextHandle;
		}

		void platform::eglDestroyRenderContext( EGLRenderContextNativeData & pEGLContextNativeData )
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

		void platform::eglBindContextForCurrentThread( const EGLRenderContextNativeData & pEGLContextNativeData,
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

			int eglConfigAttribArray[CX_EGL_MAX_EGL_CONFIG_ATTRIBUTES_NUM];
			_eglGetAttribArrayForVisualConfig( pVisualConfig, pTargetAPIVersion, eglConfigAttribArray );

			// Output array where system will store IDs of enumerated pixel formats.
			EGLConfig eglConfigArray[CX_EGL_MAX_EGL_CONFIGS_NUM];
			// Number of pixel formats returned by the system.
			EGLint returnedEGLConfigsNum = 0U;

			// Enumerate framebuffer configs.
			EGLBoolean enumResult = ::eglChooseConfig( pDisplay,
			                                           eglConfigAttribArray,
			                                           eglConfigArray,
			                                           CX_EGL_MAX_EGL_CONFIGS_NUM,
			                                           &returnedEGLConfigsNum );
			if( enumResult == EGL_FALSE )
			{
				ts3EGLThrowLastError();
			}
			if( returnedEGLConfigsNum <= 0 )
			{
				ts3EGLThrowError( "No matching EGLConfigs found." );
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

			if( ( pVisualConfig.msaaDesc.bufferCount != 0 ) && ( pVisualConfig.msaaDesc.quality != 0 ) )
			{
				pAttribArray[attribIndex++] = EGL_SAMPLE_BUFFERS;
				pAttribArray[attribIndex++] = pVisualConfig.msaaDesc.bufferCount;

				pAttribArray[attribIndex++] = EGL_SAMPLES;
				pAttribArray[attribIndex++] = pVisualConfig.msaaDesc.quality;
			}

			if( pVisualConfig.colorDesc.rgba.u32Code != 0 )
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

			if( pVisualConfig.depthStencilDesc.depthBufferSize != 0 )
			{
				pAttribArray[attribIndex++] = EGL_DEPTH_SIZE;
				pAttribArray[attribIndex++] = pVisualConfig.depthStencilDesc.depthBufferSize;
			}

			if( pVisualConfig.depthStencilDesc.stencilBufferSize != 0 )
			{
				pAttribArray[attribIndex++] = EGL_STENCIL_SIZE;
				pAttribArray[attribIndex++] = pVisualConfig.depthStencilDesc.stencilBufferSize;
			}

			pAttribArray[attribIndex++] = EGL_NONE;
		}

		EGLContext _eglCreateCoreContextDefault( EGLRenderContextNativeData & pEGLContextNativeData,
		                                         const EGLDisplaySurfaceNativeData & pEGLSurfaceNativeData,
		                                         const GLRenderContextCreateInfo & pCreateInfo,
		                                         EGLContext pEGLShareContext )
		{
			auto currentAPI = ::eglQueryAPI();
			if( currentAPI == EGL_OPENGL_API )
			{
				auto eglBindResult = ::eglBindAPI( EGL_OPENGL_API );
				if( eglBindResult == EGL_FALSE )
				{
					ts3EGLThrowLastError();
				}
			}

			int contextProfile = 0;
			if( pCreateInfo.contextProfile == EGLContextProfile::Core )
			{
				contextProfile = EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT;
			}
			else if( pCreateInfo.contextProfile == EGLContextProfile::Legacy )
			{
				contextProfile = EGL_CONTEXT_OPENGL_COMPATIBILITY_PROFILE_BIT;
			}

			EGLint debugContextFlag = EGL_FALSE;
			if( pCreateInfo.flags.isSet( E_GL_RENDER_CONTEXT_CREATE_FLAG_ENABLE_DEBUG_BIT ) )
			{
				debugContextFlag = EGL_TRUE;
			}

			EGLint forwardCompatibleContextFlag = EGL_FALSE;
			if( pCreateInfo.flags.isSet( E_GL_RENDER_CONTEXT_CREATE_FLAG_FORWARD_COMPATIBLE_BIT ) )
			{
				forwardCompatibleContextFlag = EGL_TRUE;
			}

			const EGLint contextAttributesCore[] =
			{
				// Requested OpenGL API version: major part
				EGL_CONTEXT_MAJOR_VERSION, pCreateInfo.runtimeVersionDesc.apiVersion.major,
				// Requested OpenGL API version: minor part
				EGL_CONTEXT_MINOR_VERSION, pCreateInfo.runtimeVersionDesc.apiVersion.minor,
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
				ts3EGLThrowLastError();
			}

			return contextHandle;
		}

		EGLContext _eglCreateCoreContextES( EGLRenderContextNativeData & pEGLContextNativeData,
		                                    const EGLDisplaySurfaceNativeData & pEGLSurfaceNativeData,
		                                    const Version & pTargetAPIVersion,
		                                    EGLContext pEGLShareContext )
		{
			auto currentAPI = ::eglQueryAPI();
			if( currentAPI == EGL_OPENGL_ES_API )
			{
				auto eglBindResult = ::eglBindAPI( EGL_OPENGL_ES_API );
				if( eglBindResult == EGL_FALSE )
				{
					ts3EGLThrowLastError();
				}
			}

			const EGLint contextAttributesES[] =
			{
				//
				EGL_CONTEXT_CLIENT_VERSION, pTargetAPIVersion.major,
				// Terminator
				EGL_NONE
			};

			EGLContext contextHandle = ::eglCreateContext( pEGLSurfaceNativeData.eDisplay,
			                                               pEGLSurfaceNativeData.eFBConfig,
			                                               pEGLShareContext,
			                                               contextAttributesES );
			if( contextHandle == EGL_NO_CONTEXT )
			{
				ts3EGLThrowLastError();
			}

			return contextHandle;
		}

		void _eglValidateRequestedContextVersion( EGLAPIProfile pTargetAPIProfile, Version & pVersion )
		{
			auto validatedVersion = CX_VERSION_INVALID;

			if( pTargetAPIProfile == EGLAPIProfile::OpenGLES )
			{
				const Version esVersionList[] =
				{
					{ 1, 0 },
					{ 1, 1 },
					{ 2, 0 },
					{ 3, 0 },
					{ 3, 1 },
					{ 3, 2 }
				};

				for( const auto & esVersion : esVersionList )
				{
					if( pVersion == esVersion )
					{
						validatedVersion = pVersion;
						break;
					}
				}
			}
			else
			{
				const Version coreVersionList[] =
				{
					{ 1, 0 },
					{ 1, 1 },
					{ 1, 2 },
					{ 1, 3 },
					{ 1, 4 },
					{ 1, 5 },
					{ 2, 0 },
					{ 2, 1 },
					{ 3, 0 },
					{ 3, 1 },
					{ 3, 2 },
					{ 3, 3 },
					{ 4, 0 },
					{ 4, 1 },
					{ 4, 2 },
					{ 4, 3 },
					{ 4, 4 },
					{ 4, 5 },
					{ 4, 6 },
				};

				for( const auto & coreVersion : coreVersionList )
				{
					if( pVersion == coreVersion )
					{
						validatedVersion = pVersion;
						break;
					}
				}
			}

			if( validatedVersion == CX_VERSION_INVALID )
			{
				validatedVersion = { 1, 0 };
			}

			pVersion = validatedVersion;
		}

	}

} // namespace ts3::system
