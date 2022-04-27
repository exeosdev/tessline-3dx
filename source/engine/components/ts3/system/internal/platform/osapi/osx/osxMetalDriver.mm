
#include "osxMetalDriver.h"
#include "osxDisplaySystem.h"

namespace ts3::system
{

	OSXMetalSystemDriver::OSXMetalSystemDriver( OSXDisplayManagerHandle pDisplayManager )
	: OSXNativeObject( std::move( pDisplayManager ) )
	{}

	OSXMetalSystemDriver::~OSXMetalSystemDriver() noexcept = default;

	MetalDisplaySurfaceHandle OSXMetalSystemDriver::_nativeCreateDisplaySurface( const MetalDisplaySurfaceCreateInfo & pCreateInfo )
	{
	}

	void OSXMetalSystemDriver::_nativeDestroyDisplaySurface( MetalDisplaySurface & pDisplaySurface )
	{
	}
	
}
