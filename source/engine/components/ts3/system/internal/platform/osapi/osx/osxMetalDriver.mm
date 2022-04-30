
#include "osxMetalDriver.h"
#include "osxDisplaySystem.h"
#include <ts3/system/internal/platform/shared/mtl/mtlCommon.h>

namespace ts3::system
{

	OSXMetalSystemDriver::OSXMetalSystemDriver( OSXDisplayManagerHandle pDisplayManager )
	: OSXNativeObject( std::move( pDisplayManager ) )
	{}

	OSXMetalSystemDriver::~OSXMetalSystemDriver() noexcept = default;

	MetalDisplaySurfaceHandle OSXMetalSystemDriver::_nativeCreateDisplaySurface( const MetalDisplaySurfaceCreateInfo & pCreateInfo )
	{
        auto displaySurface = createSysObject<OSXMetalDisplaySurface>( getHandle<OSXMetalSystemDriver>() );

        WindowCreateInfo windowCreateInfo;
        windowCreateInfo.frameGeometry = pCreateInfo.frameGeometry;
        windowCreateInfo.title = "TS3 Metal Window";

        platform::osxCreateWindow( displaySurface->mNativeData, nullptr, windowCreateInfo );
        platform::osxCreateSurfaceMetalView( displaySurface->mNativeData, pCreateInfo );
        platform::osxCreateEventListener( displaySurface->mNativeData );
        platform::osxSetInputWindow( displaySurface->mNativeData );

        return displaySurface;
	}

	void OSXMetalSystemDriver::_nativeDestroyDisplaySurface( MetalDisplaySurface & pDisplaySurface )
	{
    }


	OSXMetalDisplaySurface::OSXMetalDisplaySurface( OSXMetalSystemDriverHandle pMTLSystemDriver )
	: OSXNativeObject( std::move( pMTLSystemDriver ), &mNativeData )
	{}

	OSXMetalDisplaySurface::~OSXMetalDisplaySurface() noexcept
	{
	}
	
    FrameSize OSXMetalDisplaySurface::_nativeQueryRenderAreaSize() const
    {
        const auto & drawableSize = [mNativeData.caMetalLayer drawableSize];

        FrameSize result;
        result.x = static_cast<uint32>( drawableSize.width );
        result.y = static_cast<uint32>( drawableSize.height );

        return result;
    }

    bool OSXMetalDisplaySurface::_nativeSysValidate() const
    {
        return mNativeData.caMetalLayer != nil;
    }

    void OSXMetalDisplaySurface::_nativeResize( const FrameSize & pFrameSize, EFrameSizeMode pSizeMode )
    {
    }

    void OSXMetalDisplaySurface::_nativeSetFullscreenMode( bool pEnable )
    {
    }

    void OSXMetalDisplaySurface::_nativeSetTitle( const std::string & pTitle )
    {
        platform::osxSetFrameTitle( mNativeData.nsWindow, pTitle );
    }

    void OSXMetalDisplaySurface::_nativeUpdateGeometry( const FrameGeometry & pFrameGeometry,
                                                        Bitmask<EFrameGeometryUpdateFlags> pUpdateFlags )
    {
    }

    FrameSize OSXMetalDisplaySurface::_nativeGetSize( EFrameSizeMode pSizeMode ) const
    {
        return platform::osxGetFrameSize( mNativeData.nsWindow, pSizeMode );
    }

    bool OSXMetalDisplaySurface::_nativeIsFullscreen() const
    {
        return false;
    }

    namespace platform
    {

        void osxCreateSurfaceMetalView( OSXMetalDisplaySurfaceNativeData & pSurfaceNativeData,
		                                const MetalDisplaySurfaceCreateInfo & pCreateInfo )
        {
        @autoreleasepool
        {
            if( ![( id )pSurfaceNativeData.nsWindow isKindOfClass:[NSOSXWindow class]] )
            {
                return;
            }

            auto * nsWindow = static_cast<NSOSXWindow *>( pSurfaceNativeData.nsWindow );

            @try
            {
                auto * nsMetalView = [[NSOSXMetalView alloc] initForWindow:nsWindow];
				auto * caMetalLayer = nsMetalView->mMetalLayer;

				auto mtlPixelFormat = platform::mtlChoosePixelFormatForVisualConfig( pCreateInfo.visualConfig );

				[caMetalLayer setPixelFormat:mtlPixelFormat];

                pSurfaceNativeData.nsView = nsMetalView;
                pSurfaceNativeData.caMetalLayer = caMetalLayer;
            }
            @catch( NSException * pException )
            {
                const auto message = [[pException reason] UTF8String];
                ts3DebugInterrupt();
            }
        }
        }

    }
	
}
