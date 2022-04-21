
#include <ts3/gpuapiGL/GL_gpuDriver.h>
#include <ts3/gpuapiGL/GL_gpuDevice.h>
#include <ts3/gpuapiGL/GL_presentationLayer.h>
#include <ts3/system/displaySystem.h>

namespace ts3::gpuapi
{

	GLGPUDriver::GLGPUDriver( system::OpenGLSystemDriverHandle pSysGLDriver )
	: GPUDriver( pSysGLDriver->mSysContext )
	, mSysGLDriver( std::move( pSysGLDriver ) )
	{ }

	GLGPUDriver::~GLGPUDriver() = default;

	system::OpenGLSystemDriverHandle GLGPUDriver::initializeSysGLDriver( system::SysContextHandle pSysContext )
	{
		try
		{
		    auto sysDisplayManager = pSysContext->createDisplayManager();
		    auto sysGLDriver = pSysContext->createOpenGLSystemDriver( sysDisplayManager );

			sysGLDriver->initializePlatform();

			return sysGLDriver;
		}
		catch ( ... )
		{}

		return nullptr;
	}

} // namespace ts3::gpuapi
