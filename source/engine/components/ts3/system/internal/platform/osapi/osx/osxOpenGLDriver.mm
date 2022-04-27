
#include "osxOpenGLDriver.h"
#include "osxDisplaySystem.h"
#include "nsOSXOpenGL.h"
#include <ts3/core/exception.h>

#ifdef __clang__
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif

#define ts3OSXOpenGLContextAttribAppend( pArray, pIndex, pAttrib ) \
	ts3DebugAssert( pIndex < CX_OSX_MAX_NSGL_FBCONFIG_ATTRIBUTES_NUM ); \
	pArray[pIndex++] = pAttrib

namespace ts3::system
{

	namespace platform
	{

		//
		NSOpenGLPixelFormatAttribute _osxGetAPIProfileForSurface( const OpenGLDisplaySurfaceCreateInfo & pCreateInfo,
		                                                          NSOpenGLPixelFormatAttribute pNSMaxSupportedVersion );

		//
		void _osxGetAttribArrayForVisualConfig( const VisualConfig & pVisualConfig, NSOpenGLPixelFormatAttribute * pAttribArray );

	}

	OSXOpenGLSystemDriver::OSXOpenGLSystemDriver( OSXDisplayManagerHandle pDisplayManager )
	: OSXNativeObject( std::move( pDisplayManager ) )
	{
		_initializeOSXDriverState();
	}

	OSXOpenGLSystemDriver::~OSXOpenGLSystemDriver() noexcept
	{
		_releaseOSXDriverState();
	}

	void OSXOpenGLSystemDriver::_initializeOSXDriverState()
	{
		if( platform::osxCheckAppKitFrameworkVersion( NSAppKitVersionNumber10_10 ) )
		{
			_nsSupportedOpenGLVersion = NSOpenGLProfileVersion4_1Core;
		}
		else if( platform::osxCheckAppKitFrameworkVersion( NSAppKitVersionNumber10_7 ) )
		{
			_nsSupportedOpenGLVersion = NSOpenGLProfileVersion3_2Core;
		}
		else
		{
			_nsSupportedOpenGLVersion = NSOpenGLProfileVersionLegacy;
		}
	}

	void OSXOpenGLSystemDriver::_releaseOSXDriverState()
	{
	}

	void OSXOpenGLSystemDriver::_nativeInitializePlatform()
	{
	}

	void OSXOpenGLSystemDriver::_nativeReleaseInitState() noexcept
	{
	}

	OpenGLVersionSupportInfo OSXOpenGLSystemDriver::_nativeQueryVersionSupportInfo() const noexcept
	{
		OpenGLVersionSupportInfo openGLVersionSupportInfo{};

		switch( _nsSupportedOpenGLVersion )
		{
			case NSOpenGLProfileVersion4_1Core:
			{
				openGLVersionSupportInfo.apiVersion.major = 4;
				openGLVersionSupportInfo.apiVersion.minor = 1;
				break;
			}
			case NSOpenGLProfileVersion3_2Core:
			{
				openGLVersionSupportInfo.apiVersion.major = 3;
				openGLVersionSupportInfo.apiVersion.minor = 2;
				break;
			}
			default:
			{
				openGLVersionSupportInfo.apiVersion.major = 1;
				openGLVersionSupportInfo.apiVersion.minor = 0;
				break;
			}
		}

		openGLVersionSupportInfo.apiProfile = EOpenGLAPIProfile::Core;
		openGLVersionSupportInfo.apiClass = EOpenGLAPIClass::OpenGLDesktop;

		return openGLVersionSupportInfo;
	}

	OpenGLDisplaySurfaceHandle OSXOpenGLSystemDriver::_nativeCreateDisplaySurface( const OpenGLDisplaySurfaceCreateInfo & pCreateInfo )
	{
		NSOpenGLPixelFormatAttribute nsOpenGLPixelFormatAttribArray[platform::CX_OSX_MAX_NSGL_FBCONFIG_ATTRIBUTES_NUM + 2];
		nsOpenGLPixelFormatAttribArray[0] = NSOpenGLPFAOpenGLProfile;
		nsOpenGLPixelFormatAttribArray[1] = platform::_osxGetAPIProfileForSurface( pCreateInfo, _nsSupportedOpenGLVersion );

		platform::_osxGetAttribArrayForVisualConfig( pCreateInfo.visualConfig, &( nsOpenGLPixelFormatAttribArray[2] ) );

		auto * nsOpenGLPixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:nsOpenGLPixelFormatAttribArray];

		auto displaySurface = createSysObject<OSXOpenGLDisplaySurface>( getHandle<OSXOpenGLSystemDriver>() );
		displaySurface->mNativeData.nsPixelFormat = nsOpenGLPixelFormat;

		WindowCreateInfo windowCreateInfo;
		windowCreateInfo.frameGeometry = pCreateInfo.frameGeometry;
		windowCreateInfo.title = "TS3 OpenGL Window";

		platform::osxCreateWindow( displaySurface->mNativeData, nullptr, windowCreateInfo );
		platform::osxCreateWindowDefaultView( displaySurface->mNativeData );
		platform::osxCreateEventListener( displaySurface->mNativeData );
		platform::osxSetInputWindow( displaySurface->mNativeData );

		return displaySurface;
	}

	OpenGLDisplaySurfaceHandle OSXOpenGLSystemDriver::_nativeCreateDisplaySurfaceForCurrentThread()
	{
		return nullptr;
	}

	void OSXOpenGLSystemDriver::_nativeDestroyDisplaySurface( OpenGLDisplaySurface & pDisplaySurface )
	{
	}

	OpenGLRenderContextHandle OSXOpenGLSystemDriver::_nativeCreateRenderContext( OpenGLDisplaySurface & pDisplaySurface,
	                                                                             const OpenGLRenderContextCreateInfo & pCreateInfo )
	{
		auto * osxDisplaySurface = pDisplaySurface.queryInterface<OSXOpenGLDisplaySurface>();
		auto * nsPixelFormat = osxDisplaySurface->mNativeData.nsPixelFormat;

		auto * nsContextHandle = [[NSOSXOpenGLContext alloc] initWithFormat:nsPixelFormat shareContext:nil];

		auto renderContext = createSysObject<OSXOpenGLRenderContext>( getHandle<OSXOpenGLSystemDriver>() );
		renderContext->mNativeData.nsContextHandle = nsContextHandle;

		return renderContext;
	}

	OpenGLRenderContextHandle OSXOpenGLSystemDriver::_nativeCreateRenderContextForCurrentThread()
	{
		return nullptr;
	}

	void OSXOpenGLSystemDriver::_nativeDestroyRenderContext( OpenGLRenderContext & pRenderContext )
	{
	}

	void OSXOpenGLSystemDriver::_nativeResetContextBinding()
	{
	}

	std::vector<EDepthStencilFormat> OSXOpenGLSystemDriver::_nativeQuerySupportedDepthStencilFormats( EColorFormat pColorFormat ) const
	{
		return {};
	}

	std::vector<EMSAAMode> OSXOpenGLSystemDriver::_nativeQuerySupportedMSAAModes( EColorFormat pColorFormat,
	                                                                              EDepthStencilFormat pDepthStencilFormat ) const
	{
		return {};
	}

	bool OSXOpenGLSystemDriver::_nativeIsAPIClassSupported( EOpenGLAPIClass pAPIClass ) const
	{
		if( pAPIClass == EOpenGLAPIClass::OpenGLDesktop )
		{
			return true;
		}
		return false;
	}

	bool OSXOpenGLSystemDriver::_nativeIsRenderContextBound() const
	{
		return false;
	}


	OSXOpenGLDisplaySurface::OSXOpenGLDisplaySurface( OSXOpenGLSystemDriverHandle pGLSystemDriver )
	: OSXNativeObject( std::move( pGLSystemDriver ), &mNativeData )
	{}

	OSXOpenGLDisplaySurface::~OSXOpenGLDisplaySurface() noexcept
	{
	}

	void OSXOpenGLDisplaySurface::_releaseOSXSurfaceState()
	{
	}

	void OSXOpenGLDisplaySurface::_nativeSwapBuffers()
	{
		auto * nsThreadLocalStorage = [NSThread currentThread].threadDictionary;
		auto * nsContextHandle = ( NSOSXOpenGLContext * )nsThreadLocalStorage[@"ts3OpenGLContext"];

		[nsContextHandle flushBuffer];
		[nsContextHandle updateConditional];
	}

	EOpenGLAPIClass OSXOpenGLDisplaySurface::_nativeQuerySupportedAPIClass() const noexcept
	{
		return EOpenGLAPIClass::OpenGLDesktop;
	}

	FrameSize OSXOpenGLDisplaySurface::_nativeQueryRenderAreaSize() const
	{
		return {};
	}

	bool OSXOpenGLDisplaySurface::_nativeSysValidate() const
	{
		return true;
	}

	void OSXOpenGLDisplaySurface::_nativeResize( const FrameSize & pFrameSize, EFrameSizeMode pSizeMode )
	{
	}

	void OSXOpenGLDisplaySurface::_nativeSetFullscreenMode( bool pEnable )
	{
	}

	void OSXOpenGLDisplaySurface::_nativeSetTitle( const std::string & pTitle )
	{
	}

	void OSXOpenGLDisplaySurface::_nativeUpdateGeometry( const FrameGeometry & pFrameGeometry,
	                                                     Bitmask<EFrameGeometryUpdateFlags> pUpdateFlags )
	{
	}

	FrameSize OSXOpenGLDisplaySurface::_nativeGetSize( EFrameSizeMode pSizeMode ) const
	{
		return {};
	}

	bool OSXOpenGLDisplaySurface::_nativeIsFullscreen() const
	{
		return false;
	}


	OSXOpenGLRenderContext::OSXOpenGLRenderContext( OSXOpenGLSystemDriverHandle pGLSystemDriver )
	: OSXNativeObject( std::move( pGLSystemDriver ) )
	{}

	OSXOpenGLRenderContext::~OSXOpenGLRenderContext() noexcept
	{
	}

	void OSXOpenGLRenderContext::_nativeBindForCurrentThread( const OpenGLDisplaySurface & pTargetSurface )
	{
		auto * osxDisplaySurface = pTargetSurface.queryInterface<OSXOpenGLDisplaySurface>();
		auto * nsTargetView = static_cast<NSView *>( osxDisplaySurface->mNativeData.nsView );

		auto * nsThreadLocalStorage = [NSThread currentThread].threadDictionary;
		nsThreadLocalStorage[@"ts3OpenGLContext"] = mNativeData.nsContextHandle;

		[mNativeData.nsContextHandle setView:nsTargetView];
		[mNativeData.nsContextHandle makeCurrentContext];
	}

	bool OSXOpenGLRenderContext::_nativeSysCheckIsCurrent() const
	{
		return false;
	}

	bool OSXOpenGLRenderContext::_nativeSysValidate() const
	{
		return mNativeData.nsContextHandle != nullptr;
	}

	void OSXOpenGLRenderContext::_releaseOSXContextState()
	{
	}

	namespace platform
	{

		NSOpenGLPixelFormatAttribute _osxGetAPIProfileForSurface( const OpenGLDisplaySurfaceCreateInfo & pCreateInfo,
		                                                          NSOpenGLPixelFormatAttribute pNSMaxSupportedVersion )
		{
			NSOpenGLPixelFormatAttribute nsOpenGLProfile = 0;

			if( ( pCreateInfo.targetAPIClass == EOpenGLAPIClass::OpenGLDesktop ) && ( pCreateInfo.minimumAPIVersion <= Version{ 4, 1 } ) )
			{
				if( ( pCreateInfo.minimumAPIVersion > Version{ 3, 2 } ) && ( pNSMaxSupportedVersion == NSOpenGLProfileVersion4_1Core ) )
				{
					nsOpenGLProfile = NSOpenGLProfileVersion4_1Core;
				}
				else if( ( pCreateInfo.minimumAPIVersion >= Version{ 3, 0 } ) && ( pNSMaxSupportedVersion == NSOpenGLProfileVersion3_2Core ) )
				{
					nsOpenGLProfile = NSOpenGLProfileVersion3_2Core;
				}
				else
				{
					nsOpenGLProfile = NSOpenGLProfileVersionLegacy;
				}
			}

			return nsOpenGLProfile;
		}

		void _osxGetAttribArrayForVisualConfig( const VisualConfig & pVisualConfig, NSOpenGLPixelFormatAttribute * pAttribArray )
		{
			int attribIndex = 0;

			ts3OSXOpenGLContextAttribAppend( pAttribArray, attribIndex, NSOpenGLPFAAccelerated );
			ts3OSXOpenGLContextAttribAppend( pAttribArray, attribIndex, NSOpenGLPFAAllowOfflineRenderers );

			ts3OSXOpenGLContextAttribAppend( pAttribArray, attribIndex, NSOpenGLPFAColorSize );
			ts3OSXOpenGLContextAttribAppend( pAttribArray, attribIndex, pVisualConfig.colorDesc.size );

			if( pVisualConfig.flags.isSet( E_VISUAL_ATTRIB_FLAG_TRIPLE_BUFFER_BIT ) )
			{
				ts3OSXOpenGLContextAttribAppend( pAttribArray, attribIndex, NSOpenGLPFATripleBuffer );
			}
			else if( pVisualConfig.flags.isSet( E_VISUAL_ATTRIB_FLAG_DOUBLE_BUFFER_BIT ) )
			{
				ts3OSXOpenGLContextAttribAppend( pAttribArray, attribIndex, NSOpenGLPFADoubleBuffer );
			}

			if( pVisualConfig.flags.isSet( E_VISUAL_ATTRIB_FLAG_STEREO_DISPLAY_BIT ) )
			{
				ts3OSXOpenGLContextAttribAppend( pAttribArray, attribIndex, NSOpenGLPFAStereo );
			}

			if( ( pVisualConfig.msaaDesc.bufferCount != 0 ) && ( pVisualConfig.msaaDesc.quality != 0 ) )
			{
				ts3OSXOpenGLContextAttribAppend( pAttribArray, attribIndex, NSOpenGLPFASampleBuffers );
				ts3OSXOpenGLContextAttribAppend( pAttribArray, attribIndex, pVisualConfig.msaaDesc.bufferCount );
				ts3OSXOpenGLContextAttribAppend( pAttribArray, attribIndex, NSOpenGLPFASamples );
				ts3OSXOpenGLContextAttribAppend( pAttribArray, attribIndex, pVisualConfig.msaaDesc.quality );
				ts3OSXOpenGLContextAttribAppend( pAttribArray, attribIndex, NSOpenGLPFANoRecovery );
			}

			if( pVisualConfig.depthStencilDesc.depthBufferSize != 0 )
			{
				ts3OSXOpenGLContextAttribAppend( pAttribArray, attribIndex, NSOpenGLPFADepthSize );
				ts3OSXOpenGLContextAttribAppend( pAttribArray, attribIndex, pVisualConfig.depthStencilDesc.depthBufferSize );
			}

			if( pVisualConfig.depthStencilDesc.stencilBufferSize != 0 )
			{
				ts3OSXOpenGLContextAttribAppend( pAttribArray, attribIndex, NSOpenGLPFAStencilSize );
				ts3OSXOpenGLContextAttribAppend( pAttribArray, attribIndex, pVisualConfig.depthStencilDesc.stencilBufferSize );
			}

			ts3OSXOpenGLContextAttribAppend( pAttribArray, attribIndex, 0 );
		}

	}

} // namespace ts3::system

#ifdef __clang__
#  pragma clang diagnostic pop
#endif
