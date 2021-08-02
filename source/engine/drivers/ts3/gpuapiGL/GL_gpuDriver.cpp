
#include <ts3/gpuapiGL/GL_gpuDriver.h>
#include <ts3/gpuapiGL/GL_gpuDevice.h>
#include <ts3/gpuapiGL/GL_presentationLayer.h>
#include <ts3/system/displayManager.h>

namespace ts3::gpuapi
{

	GLGPUDriver::GLGPUDriver( SysGLDriverHandle pSysGLDriver )
	: GPUDriver( pSysGLDriver->mSysContext )
	, mSysGLDriver( std::move( pSysGLDriver ) )
	{ }

	GLGPUDriver::~GLGPUDriver() = default;

	SysGLDriverHandle GLGPUDriver::initializeSysGLDriver( SysContextHandle pSysContext )
	{
		try
		{
			auto sysDisplayManager = SysDisplayManager::create( pSysContext );
			auto sysGLDriver = SysGLDriver::create( sysDisplayManager );

			sysGLDriver->initializePlatform();

			return sysGLDriver;
		}
		catch ( ... )
		{}

		return nullptr;
	}

}
