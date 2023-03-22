
#include "metalNative.h"
#include "displaySystem.h"

#include <Metal/MTLRenderPass.h>

namespace ts3::system
{

	MetalDevice::MetalDevice( SysContextHandle pSysContext )
	: SysObject( std::move( pSysContext ) )
	, mDeviceData( std::make_unique<MetalDeviceData>() )
	{}

	MetalDevice::~MetalDevice() = default;

	MetalDeviceHandle MetalDevice::createDefault( SysContextHandle pSysContext )
	{
	@autoreleasepool
	{
		id<MTLDevice> defaultMTLDevice = MTLCreateSystemDefaultDevice();
		if( defaultMTLDevice == nil )
		{
			ts3DebugInterrupt();
			return nullptr;
		}

		id<MTLCommandQueue> mainMTLCommandQueue = [defaultMTLDevice newCommandQueue];
		if( mainMTLCommandQueue == nil )
		{
			ts3DebugInterrupt();
			return nullptr;
		}

		auto metalDevice = createSysObject<MetalDevice>( std::move( pSysContext ) );
		metalDevice->mDeviceData->mtlDevice = defaultMTLDevice;
		metalDevice->mDeviceData->mtlMainCmdQueue = mainMTLCommandQueue;

		return metalDevice;
	}
	}


	MetalSystemDriver::MetalSystemDriver( DisplayManagerHandle pDisplayManager )
	: SysObject( pDisplayManager->mSysContext )
	, mDriverData( std::make_unique<MetalSystemDriverData>() )
	, mDisplayManager( std::move( pDisplayManager ) )
	{}

	MetalSystemDriver::~MetalSystemDriver() noexcept = default;

	MetalDevice & MetalSystemDriver::initializeDefaultDevice()
	{
		ts3DebugAssert( !_defaultMetalDevice );
		_defaultMetalDevice = MetalDevice::createDefault( mSysContext );
		return *_defaultMetalDevice;
	}

	MetalDevice & MetalSystemDriver::getDefaultDevice() noexcept
	{
		if( !_defaultMetalDevice )
		{
			_defaultMetalDevice = MetalDevice::createDefault( mSysContext );
		}

		return *_defaultMetalDevice;
	}

	MetalDevice & MetalSystemDriver::getDefaultDevice() const
	{
		if( !_defaultMetalDevice )
		{
			ts3Throw( 0 );
		}
		return *_defaultMetalDevice;
	}

	MetalDisplaySurfaceHandle MetalSystemDriver::createDisplaySurface(
			MetalDevice & pMetalDevice,
			const MetalDisplaySurfaceCreateInfo & pCreateInfo )
	{
		MetalDisplaySurfaceCreateInfo surfaceCreateInfo = pCreateInfo;

		if( pCreateInfo.flags.isSet( E_METAL_DISPLAY_SURFACE_CREATE_FLAG_FULLSCREEN_BIT ) )
		{
			surfaceCreateInfo.frameGeometry.size = CX_FRAME_SIZE_MAX;
			surfaceCreateInfo.frameGeometry.style = EFrameStyle::Overlay;
		}
		else
		{
			surfaceCreateInfo.frameGeometry.position = pCreateInfo.frameGeometry.position;
			surfaceCreateInfo.frameGeometry.size = pCreateInfo.frameGeometry.size;
			surfaceCreateInfo.frameGeometry.style = pCreateInfo.frameGeometry.style;
		}

		surfaceCreateInfo.frameGeometry = mDisplayManager->validateFrameGeometry( surfaceCreateInfo.frameGeometry );

		auto displaySurface = _nativeCreateDisplaySurface( pMetalDevice, surfaceCreateInfo );

		auto * caMetalLayer = displaySurface->mSurfaceData->caMetalLayer;
		ts3DebugAssert( caMetalLayer != nil );

		auto mtlDevice = pMetalDevice.mDeviceData->mtlDevice;
		[caMetalLayer setDevice:mtlDevice];

		return displaySurface;
	}


	MetalDisplaySurface::MetalDisplaySurface( MetalSystemDriverHandle pMTLSystemDriver, void * pNativeData )
	: Frame( pMTLSystemDriver->mSysContext )
	, mSurfaceData( std::make_unique<MetalDisplaySurfaceData>() )
	, mMetalDriver( std::move( pMTLSystemDriver ) )
	{
		setEventSourceNativeData( pNativeData );
	}

	MetalDisplaySurface::~MetalDisplaySurface() noexcept
	{
		resetEventSourceNativeData();
	}

	void MetalDisplaySurface::clearColorBuffer()
	{
	@autoreleasepool
	{
		auto mtlDevice = mMetalDevice->mDeviceData->mtlDevice;
		auto mtlCommandQueue = mMetalDevice->mDeviceData->mtlMainCmdQueue;
		auto caMetalLayer = mSurfaceData->caMetalLayer;

		id<CAMetalDrawable> currentDrawable = [caMetalLayer nextDrawable];
		id<MTLTexture> texture = currentDrawable.texture;

		MTLRenderPassDescriptor * clearRPDescriptor = [MTLRenderPassDescriptor new];
		clearRPDescriptor.colorAttachments[0].texture = texture;
		clearRPDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
		clearRPDescriptor.colorAttachments[0].storeAction = MTLStoreActionStore;
		clearRPDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(0.12, 0.42, 0.92, 1.0);

		id<MTLCommandBuffer> commandBuffer = [mtlCommandQueue commandBuffer];
		id<MTLRenderCommandEncoder> commandEncoder = [commandBuffer renderCommandEncoderWithDescriptor:clearRPDescriptor];

		[commandEncoder endEncoding];
		[commandBuffer presentDrawable:currentDrawable];
		[commandBuffer commit];
	}
	}

	void MetalDisplaySurface::resizeClientArea( const FrameSize & pSize )
	{
		FrameGeometry newFrameGeometry{};
		newFrameGeometry.position = CX_FRAME_POS_AUTO;
		newFrameGeometry.size = pSize;
		newFrameGeometry.style = EFrameStyle::Unspecified;

		newFrameGeometry = mMetalDriver->mDisplayManager->validateFrameGeometry( newFrameGeometry );

		const auto updateFlags = E_FRAME_GEOMETRY_UPDATE_FLAG_POSITION_BIT | E_FRAME_GEOMETRY_UPDATE_FLAG_SIZE_CLIENT_AREA_BIT;
		_nativeUpdateGeometry( newFrameGeometry, updateFlags );
	}

	void MetalDisplaySurface::resizeFrame( const FrameSize & pSize )
	{
		FrameGeometry newFrameGeometry{};
		newFrameGeometry.position = CX_FRAME_POS_AUTO;
		newFrameGeometry.size = pSize;
		newFrameGeometry.style = EFrameStyle::Unspecified;

		newFrameGeometry = mMetalDriver->mDisplayManager->validateFrameGeometry( newFrameGeometry );

		const auto updateFlags = E_FRAME_GEOMETRY_UPDATE_FLAG_POSITION_BIT | E_FRAME_GEOMETRY_UPDATE_FLAG_SIZE_OUTER_RECT_BIT;
		_nativeUpdateGeometry( newFrameGeometry, updateFlags );
	}

	void MetalDisplaySurface::setFullscreenMode( bool pEnable )
	{
		_nativeSetFullscreenMode( pEnable );
	}

	void MetalDisplaySurface::setTitle( const std::string & pTitleText )
	{
		_nativeSetTitle( pTitleText );
	}

	void MetalDisplaySurface::updateGeometry( const FrameGeometry & pFrameGeometry,
	                                           Bitmask<EFrameGeometryUpdateFlags> pUpdateFlags )
	{
		_nativeUpdateGeometry( pFrameGeometry, pUpdateFlags );
	}

	FrameSize MetalDisplaySurface::getClientAreaSize() const
	{
		return _nativeGetSize( EFrameSizeMode::ClientArea );
	}

	FrameSize MetalDisplaySurface::getFrameSize() const
	{
		return _nativeGetSize( EFrameSizeMode::OuterRect );
	}

	bool MetalDisplaySurface::isFullscreen() const
	{
		return _nativeIsFullscreen();
	}

}
