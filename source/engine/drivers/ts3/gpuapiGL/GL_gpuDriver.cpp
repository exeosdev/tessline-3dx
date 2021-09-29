
#include <ts3/gpuapiGL/GL_gpuDriver.h>
#include <ts3/gpuapiGL/GL_gpuDevice.h>
#include <ts3/gpuapiGL/GL_presentationLayer.h>
#include <ts3/system/display.h>

namespace ts3::gpuapi
{

	GLGPUDriver::GLGPUDriver( system::GLSystemDriverHandle pSysGLDriver )
	: GPUDriver( pSysGLDriver->mSysContext )
	, mSysGLDriver( std::move( pSysGLDriver ) )
	{ }

	GLGPUDriver::~GLGPUDriver() = default;

	system::GLSystemDriverHandle GLGPUDriver::initializeSysGLDriver( system::SysContextHandle pSysContext )
	{
		try
		{
		    auto sysDisplayManager = system::createSysObject<system::DisplayManager>( pSysContext );
		    auto sysGLDriver = system::createSysObject<system::GLSystemDriver>( sysDisplayManager );

			sysGLDriver->initializePlatform();

			return sysGLDriver;
		}
		catch ( ... )
		{}

		return nullptr;
	}

}
