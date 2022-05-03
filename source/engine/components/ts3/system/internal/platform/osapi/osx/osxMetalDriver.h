
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_OSX_Metal_DRIVER_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_OSX_Metal_DRIVER_H__

#include "osxWindowSystem.h"
#include "nsOSXMetalSupport.h"
#include <ts3/system/metalNative.h>

#import <Metal/MTLDevice.h>

namespace ts3::system
{

	ts3SysDeclareHandle( OSXMetalSystemDriver );
	ts3SysDeclareHandle( OSXMetalRenderContext );

	namespace platform
	{

		struct OSXMetalDisplaySurfaceNativeData : public OSXWindowNativeData
		{
            CAMetalLayer * caMetalLayer = nil;
		};

		struct OSXMetalRenderContextNativeData : public OSXNativeDataCommon
		{
		};

		struct OSXMetalSystemDriverNativeData : public OSXNativeDataCommon
		{
		};

        void osxCreateSurfaceMetalView( OSXMetalDisplaySurfaceNativeData & pSurfaceNativeData,
		                                const MetalDisplaySurfaceCreateInfo & pCreateInfo );

	}

	/// @brief OSX-specific implementation of the MetalSystemDriver class.
	class OSXMetalSystemDriver : public OSXNativeObject<MetalSystemDriver, platform::OSXMetalSystemDriverNativeData>
	{
	public:
		OSXMetalSystemDriver( OSXDisplayManagerHandle pDisplayManager, MetalDeviceHandle pMetalDevice );
		virtual ~OSXMetalSystemDriver() noexcept;
		
	private:
		virtual MetalDisplaySurfaceHandle _nativeCreateDisplaySurface( const MetalDisplaySurfaceCreateInfo & pCreateInfo ) override final;

		virtual void _nativeDestroyDisplaySurface( MetalDisplaySurface & pDisplaySurface ) override final;
	};

    /// @brief OSX-specific implementation of the MetalDisplaySurface class.
    class OSXMetalDisplaySurface : public OSXNativeObject<MetalDisplaySurface, platform::OSXMetalDisplaySurfaceNativeData>
    {
    public:
        explicit OSXMetalDisplaySurface( OSXMetalSystemDriverHandle pGLSystemDriver );
        virtual ~OSXMetalDisplaySurface() noexcept;

    private:
	    /// @copybrief MetalDisplaySurface::_nativeQueryRenderAreaSize
        virtual FrameSize _nativeQueryRenderAreaSize() const override final;

        /// @copybrief MetalDisplaySurface::_nativeSysValidate
        virtual bool _nativeSysValidate() const override final;

        /// @copybrief MetalDisplaySurface::_nativeResize
        virtual void _nativeResize( const FrameSize & pFrameSize, EFrameSizeMode pSizeMode ) override final;

        /// @copybrief MetalDisplaySurface::_nativeSetFullscreenMode
        virtual void _nativeSetFullscreenMode( bool pEnable ) override final;

        /// @copybrief MetalDisplaySurface::_nativeSetTitle
        virtual void _nativeSetTitle( const std::string & pTitle ) override final;

        /// @copybrief MetalDisplaySurface::_nativeUpdateGeometry
        virtual void _nativeUpdateGeometry( const FrameGeometry & pFrameGeometry,
                                            Bitmask<EFrameGeometryUpdateFlags> pUpdateFlags ) override final;

        /// @copybrief MetalDisplaySurface::_nativeGetSize
        virtual FrameSize _nativeGetSize( EFrameSizeMode pSizeMode ) const override final;

        /// @copybrief MetalDisplaySurface::_nativeIsFullscreen
        virtual bool _nativeIsFullscreen() const override final;
    };

} // namespace ts3::system

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_OSX_Metal_DRIVER_H__
