
#include "GL_presentationLayer.h"
#include "GL_commandList.h"
#include "GL_gpuDevice.h"

namespace ts3::gpuapi
{

	static system::GLDisplaySurfaceHandle createSysGLSurface( system::GLSystemDriverHandle pSysGLDriver,
                                                              const GLPresentationLayerCreateInfo & pPLCreateInfo )
	{
		try
		{
		    system::GLDisplaySurfaceCreateInfo surfaceCreateInfo;
			surfaceCreateInfo.windowGeometry.position = pPLCreateInfo.screenRect.offset;
			surfaceCreateInfo.windowGeometry.size = pPLCreateInfo.screenRect.size;
			surfaceCreateInfo.windowGeometry.frameStyle = system::EWindowFrameStyle::Default;
			surfaceCreateInfo.visualConfig = pPLCreateInfo.visualConfig;
			surfaceCreateInfo.flags = 0u;

			if( pPLCreateInfo.displayConfigFlags.isSet( E_DISPLAY_CONFIGURATION_FLAG_FULLSCREEN_BIT ) )
			{
				surfaceCreateInfo.flags.set( system::E_GL_DISPLAY_SURFACE_CREATE_FLAG_FULLSCREEN_BIT );
			}

			if( pPLCreateInfo.displayConfigFlags.isSet( E_DISPLAY_CONFIGURATION_FLAG_SYNC_MODE_ADAPTIVE_BIT ) )
			{
				surfaceCreateInfo.flags.set( system::E_GL_DISPLAY_SURFACE_CREATE_FLAG_SYNC_ADAPTIVE_BIT );
			}
			else if( pPLCreateInfo.displayConfigFlags.isSet( E_DISPLAY_CONFIGURATION_FLAG_SYNC_MODE_VERTICAL_BIT ) )
			{
				surfaceCreateInfo.flags.set( system::E_GL_DISPLAY_SURFACE_CREATE_FLAG_SYNC_VERTICAL_BIT );
			}

			auto sysGLSurface = pSysGLDriver->createDisplaySurface( surfaceCreateInfo );

			return sysGLSurface;
		}
		catch ( ... )
		{
			ts3DebugInterrupt();
		}

		return nullptr;
	}

	GLPresentationLayer::GLPresentationLayer( GLGPUDevice & pDevice, system::GLDisplaySurfaceHandle pSysGLDisplaySurface )
	: PresentationLayer( pDevice )
	, mSysGLDisplaySurface( pSysGLDisplaySurface )
	{ }

	GLPresentationLayer::~GLPresentationLayer() = default;

	system::EventSource * GLPresentationLayer::getInternalSystemEventSource() const noexcept
	{
		return mSysGLDisplaySurface.get();
	}


	GLScreenPresentationLayer::GLScreenPresentationLayer( GLGPUDevice & pDevice, system::GLDisplaySurfaceHandle pSysGLDisplaySurface )
	: GLPresentationLayer( pDevice, pSysGLDisplaySurface )
	{ }

	GLScreenPresentationLayer::~GLScreenPresentationLayer() = default;

	GLScreenPresentationLayerHandle GLScreenPresentationLayer::create( GLGPUDevice & pDevice, const GLPresentationLayerCreateInfo & pCreateInfo )
	{
		auto sysGLSurface = createSysGLSurface( pDevice.mSysGLDriver, pCreateInfo );
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
		mSysGLDisplaySurface->swapBuffers();
	}

	void GLScreenPresentationLayer::resize( uint32 pWidth, uint32 pHeight )
	{
	}

	void GLScreenPresentationLayer::setFullscreenMode( bool pEnable )
	{
	}

	ts3::math::Vec2u32 GLScreenPresentationLayer::queryRenderTargetSize() const
	{
		return mSysGLDisplaySurface->queryRenderAreaSize();
	}

}
