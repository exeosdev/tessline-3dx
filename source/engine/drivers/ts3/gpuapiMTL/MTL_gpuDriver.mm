
#include "MTL_gpuDriver.h"

namespace ts3::gpuapi
{

	system::MetalSystemDriverHandle MetalGPUDriver::initializeSysMTLDriver( system::SysContextHandle pSysContext )
	{
		try
		{
			auto sysDisplayManager = pSysContext->createDisplayManager();
			auto sysMetalDriver = pSysContext->createMetalSystemDriver( sysDisplayManager, {} );

			sysMetalDriver->initializeDefaultDevice();

			return sysMetalDriver;
		}
		catch ( ... )
		{}

		return nullptr;
	}

}
