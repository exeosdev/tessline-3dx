
#include "GL_presentationLayer.h"
#include "GL_commandList.h"
#include "GL_gpuDevice.h"

namespace ts3::gpuapi
{

	static SysGLSurfaceHandle createSysGLSurface( SysGLCoreDeviceHandle pSysGLCoreDevice, const GLPresentationLayerCreateInfo & pSCCreateInfo )
	{
		try
		{
			SysGLSurfaceCreateInfo surfaceCreateInfo;
			surfaceCreateInfo.windowGeometry.position = pSCCreateInfo.screenRect.offset;
			surfaceCreateInfo.windowGeometry.size = pSCCreateInfo.screenRect.size;
			surfaceCreateInfo.windowGeometry.frameStyle = ts3::SysWindowFrameStyle::Default;
			surfaceCreateInfo.visualConfig = pSCCreateInfo.visualConfig;
			surfaceCreateInfo.flags = 0u;

			if( pSCCreateInfo.displayConfigFlags.isSet( E_DISPLAY_CONFIGURATION_FLAG_FULLSCREEN_BIT ) )
			{
				surfaceCreateInfo.flags.set( E_SYS_GFX_GL_DISPLAY_SURFACE_CREATE_FLAG_FULLSCREEN_BIT );
			}

			if( pSCCreateInfo.displayConfigFlags.isSet( E_DISPLAY_CONFIGURATION_FLAG_SYNC_MODE_ADAPTIVE_BIT ) )
			{
				surfaceCreateInfo.flags.set( E_SYS_GFX_GL_DISPLAY_SURFACE_CREATE_FLAG_SYNC_ADAPTIVE_BIT );
			}
			else if( pSCCreateInfo.displayConfigFlags.isSet( E_DISPLAY_CONFIGURATION_FLAG_SYNC_MODE_VERTICAL_BIT ) )
			{
				surfaceCreateInfo.flags.set( E_SYS_GFX_GL_DISPLAY_SURFACE_CREATE_FLAG_SYNC_VERTICAL_BIT );
			}

			auto sysGLSurface = pSysGLCoreDevice->createDisplaySurface( surfaceCreateInfo );

			return sysGLSurface;
		}
		catch ( ... )
		{
			ts3DebugInterrupt();
		}

		return nullptr;
	}

	GLPresentationLayer::GLPresentationLayer( GLGPUDevice & pDevice, SysGLSurfaceHandle pSysGLSurface )
	: PresentationLayer( pDevice )
	, mSysGLSurface( pSysGLSurface )
	{ }

	GLPresentationLayer::~GLPresentationLayer() = default;

	SysEventSource * GLPresentationLayer::querySysEventSourceObject() const noexcept
	{
		return mSysGLSurface.get();
	}


	GLScreenPresentationLayer::GLScreenPresentationLayer( GLGPUDevice & pDevice, SysGLSurfaceHandle pSysGLSurface )
	: GLPresentationLayer( pDevice, pSysGLSurface )
	{ }

	GLScreenPresentationLayer::~GLScreenPresentationLayer() = default;

	GLScreenPresentationLayerHandle GLScreenPresentationLayer::create( GLGPUDevice & pDevice, const GLPresentationLayerCreateInfo & pCreateInfo )
	{
		auto sysGLSurface = createSysGLSurface( pDevice.mSysGLCoreDevice, pCreateInfo );
		ts3DebugAssert( sysGLSurface );

		auto presentationLayer = createGPUAPIObject<GLScreenPresentationLayer>( pDevice, sysGLSurface );

		return presentationLayer;
	}

	void GLScreenPresentationLayer::bindRenderTarget( CommandContext * pCmdContext )
	{
		// auto * openglCmdContext = pCmdContext->queryInterface<GLCommandContext>();
		// openglCmdContext->mExfGLRenderContext->bindForCurrentThread( mExfGLSurface );
		glBindFramebuffer( GL_FRAMEBUFFER, 0 );
		ts3GLHandleLastError();
	}

	void GLScreenPresentationLayer::invalidateRenderTarget( CommandContext * pCmdContext )
	{
	}

	void GLScreenPresentationLayer::present()
	{
		mSysGLSurface->swapBuffers();
	}

	void GLScreenPresentationLayer::resize( uint32 pWidth, uint32 pHeight )
	{
	}

	void GLScreenPresentationLayer::setFullscreenMode( bool pEnable )
	{
	}

	ts3::math::Vec2u32 GLScreenPresentationLayer::queryRenderTargetSize() const
	{
		return mSysGLSurface->queryCurrentSize();
	}

}
