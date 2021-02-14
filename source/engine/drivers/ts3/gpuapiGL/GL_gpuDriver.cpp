
#include <ts3/gpuapiGL/GL_gpuDriver.h>
#include <ts3/gpuapiGL/GL_gpuDevice.h>
#include <ts3/gpuapiGL/GL_presentationLayer.h>
#include <ts3/system/displayManager.h>

namespace ts3::gpuapi
{

	GLGPUDriver::GLGPUDriver( SysGLSubsystemHandle pSysGLSubsystem )
	: GPUDriver( pSysGLSubsystem->mSysContext )
	, mSysGLSubsystem( std::move( pSysGLSubsystem ) )
	{ }

	GLGPUDriver::~GLGPUDriver() = default;

	SysGLSubsystemHandle GLGPUDriver::initializeSysGLSubsystem( SysContextHandle pSysContext )
	{
		try
		{
			auto sysDisplayManager = SysDisplayManager::create( pSysContext );
			auto sysGLSubsystem = SysGLSubsystem::create( sysDisplayManager );

			sysGLSubsystem->initializePlatform();

			return sysGLSubsystem;
		}
		catch ( ... )
		{}

		return nullptr;
	}

}
