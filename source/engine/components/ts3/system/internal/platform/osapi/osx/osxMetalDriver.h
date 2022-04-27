
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_OSX_OPENGL_DRIVER_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_OSX_OPENGL_DRIVER_H__

#include "osxWindowSystem.h"
#include <ts3/system/metalDriver.h>

#import <Metal/MTLDevice.h>

namespace ts3::system
{

	ts3SysDeclareHandle( OSXMetalSystemDriver );
	ts3SysDeclareHandle( OSXMetalRenderContext );

	namespace platform
	{

		struct OSXMetalDisplaySurfaceNativeData : public OSXWindowNativeData
		{
		};

		struct OSXMetalRenderContextNativeData : public OSXNativeDataCommon
		{
		};

		struct OSXMetalSystemDriverNativeData : public OSXNativeDataCommon
		{
		};

	}

	/// @brief OSX-specific implementation of the MetalSystemDriver class.
	class OSXMetalSystemDriver : public OSXNativeObject<MetalSystemDriver, platform::OSXMetalSystemDriverNativeData>
	{
	public:
		OSXMetalSystemDriver( OSXDisplayManagerHandle pDisplayManager );
		virtual ~OSXMetalSystemDriver() noexcept;
		
	private:
		virtual MetalDisplaySurfaceHandle _nativeCreateDisplaySurface( const MetalDisplaySurfaceCreateInfo & pCreateInfo ) override final;

		virtual void _nativeDestroyDisplaySurface( MetalDisplaySurface & pDisplaySurface ) override final;
	};

} // namespace ts3::system

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_OSX_OPENGL_DRIVER_H__
