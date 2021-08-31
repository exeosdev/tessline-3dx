
#include "GL_commandSystem.h"
#include "GL_commandList.h"
#include "GL_gpuDevice.h"
#include <ts3/gpuapi/commandContext.h>

namespace ts3::gpuapi
{

	GLCommandSystem::GLCommandSystem( GLGPUDevice & pGLGPUDevice )
	: CommandSystem( pGLGPUDevice )
	, _targetSysGLSurface( nullptr )
	{}

	GLCommandSystem::~GLCommandSystem() = default;

	std::unique_ptr<CommandContext> GLCommandSystem::acquireCommandContext( ECommandContextType pContextType )
	{
		std::unique_ptr<CommandContext> commandContext;

		auto contextExecutionMode = ecGetCommandContextExecutionMode( pContextType );
		if( auto * commandList = acquireCommandList( contextExecutionMode ) )
		{
			ts3DebugAssert( contextExecutionMode == ECommandExecutionMode::Direct );
			commandContext = std::make_unique<CommandContextDirectGraphics>( *this, *commandList );
			commandList->mSysGLRenderContext->bindForCurrentThread( *_targetSysGLSurface );
		}

		if( commandContext )
		{
			auto * openglGPUDevice = mGPUDevice.queryInterface<GLGPUDevice>();
			auto * openglDebugOutput = openglGPUDevice->getDebugOutputInterface();
			openglDebugOutput->enableDebugOutput( true );
			openglDebugOutput->enableBreakOnEvent( true );
			openglDebugOutput->enableSync( true );
		}

		return commandContext;
	}

	CommandSync GLCommandSystem::submitContext( CommandContextDirect & pContext, const CommandContextSubmitInfo & pSubmitInfo )
	{
		CommandSync cmdSyncObject;

		if( pSubmitInfo.syncMode == ECommandSubmitSyncMode::Default )
		{
			auto * openglCommandSyncData = new GLCommandSyncData();
			cmdSyncObject.syncData = openglCommandSyncData;
			cmdSyncObject.syncDataReleaseFunc = releaseGLCommandSyncData;

			auto syncFence = glFenceSync( GL_SYNC_GPU_COMMANDS_COMPLETE, 0 );
			ts3GLCheckLastResult();

			glFlush();
			ts3GLCheckLastResult();

			openglCommandSyncData->openglSyncFence = syncFence;
		}

		return cmdSyncObject;
	}

	void GLCommandSystem::setTargetGLSurface( SysGLDisplaySurfaceHandle pSysGLSurface )
	{
		// Surface is required to create a GL context. Since CommandSystem is bound to a device
		// and PresentationLayer is created when device is already available, there is no way
		// we can know the surface in advance. Hence, we set it manually (eh...) and use to
		// create a GL context when a new CommandList is instantiated.
		_targetSysGLSurface = pSysGLSurface;
	}

	GLCommandList * GLCommandSystem::acquireCommandList( ECommandExecutionMode pCommandExecutionMode )
	{
		GLCommandList * commandList = nullptr;

		if( pCommandExecutionMode == ECommandExecutionMode::Direct )
		{
			if( !_mainCommandList )
			{
				initializeMainCommandList();
			}
			if( _mainCommandList && _mainCommandList->acquireList() )
			{
				commandList = _mainCommandList.get();
			}
		}

		return commandList;
	}

	bool GLCommandSystem::initializeMainCommandList()
	{
		ts3DebugAssert( !_mainCommandList );

		auto * openglGPUDevice = mGPUDevice.queryInterface<GLGPUDevice>();
		ts3DebugAssert( openglGPUDevice );

		auto sysGLRenderContext = GLCommandSystem::createSysGLRenderContext( *openglGPUDevice, _targetSysGLSurface );
		ts3DebugAssert( sysGLRenderContext );

		_mainCommandList = createGPUAPIObject<GLCommandList>( *this, ECommandListType::DirectGraphics, sysGLRenderContext );

		return true;
	}

	SysGLRenderContextHandle GLCommandSystem::createSysGLRenderContext( GLGPUDevice & pGLGPUDevice, SysGLDisplaySurfaceHandle pSysGLSurface )
	{
		SysGLRenderContextHandle sysGLRenderContext = nullptr;

		try
		{
			SysGLRenderContextCreateInfo contextCreateInfo;
			contextCreateInfo.shareContext = nullptr;
			contextCreateInfo.flags.set( E_SYS_GL_RENDER_CONTEXT_CREATE_FLAG_FORWARD_COMPATIBLE_BIT );

		#if( TS3GX_GL_TARGET == TS3GX_GL_TARGET_GL43 )
			contextCreateInfo.requiredAPIVersion.major = 4;
			contextCreateInfo.requiredAPIVersion.minor = 3;
			contextCreateInfo.targetAPIProfile = ESysGLAPIProfile::Core;
			contextCreateInfo.flags.set( E_SYS_GL_RENDER_CONTEXT_CREATE_FLAG_ENABLE_SHARING_BIT );
		#elif( TS3GX_GL_TARGET == TS3GX_GL_TARGET_ES31 )
			contextCreateInfo.requiredAPIVersion.major = 3;
			contextCreateInfo.requiredAPIVersion.minor = 1;
			contextCreateInfo.targetAPIProfile = ESysGLAPIProfile::GLES;
			contextCreateInfo.flags.set( E_SYS_GL_RENDER_CONTEXT_CREATE_FLAG_FORWARD_COMPATIBLE_BIT );
		#endif

			if( pGLGPUDevice.isDebugDevice() )
			{
				contextCreateInfo.flags.set( E_SYS_GL_RENDER_CONTEXT_CREATE_FLAG_ENABLE_DEBUG_BIT );
			}

			auto sysGLRenderContext = pSysGLSurface->mGLDriver->createRenderContext( *pSysGLSurface, contextCreateInfo );

			return sysGLRenderContext;
		}
		catch ( ... )
		{
			ts3DebugInterrupt();
		}

		return nullptr;
	}


	void releaseGLCommandSyncData( void * pSyncData )
	{
		if( pSyncData )
		{
			auto * openglCommandSyncData = static_cast<GLCommandSyncData *>( pSyncData );
			openglCommandSyncData->openglSyncFence = nullptr;
			delete openglCommandSyncData;
		}
	}

}
