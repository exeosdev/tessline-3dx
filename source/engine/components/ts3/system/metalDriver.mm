
#include "metalDriver.h"
#include "displaySystem.h"

namespace ts3::system
{

	MetalSystemDriver::MetalSystemDriver( DisplayManagerHandle pDisplayManager )
	: SysObject( pDisplayManager->mSysContext )
	, mDisplayManager( std::move( pDisplayManager ) )
	{}

	MetalSystemDriver::~MetalSystemDriver() noexcept = default;

	MetalDisplaySurfaceHandle MetalSystemDriver::createDisplaySurface( const MetalDisplaySurfaceCreateInfo & pCreateInfo )
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

		auto displaySurface = _nativeCreateDisplaySurface( surfaceCreateInfo );

		return displaySurface;
	}


	MetalDisplaySurface::MetalDisplaySurface( MetalSystemDriverHandle pGLSystemDriver, void * pNativeData )
	: Frame( pGLSystemDriver->mSysContext )
	, mGLSystemDriver( std::move( pGLSystemDriver ) )
	{
		setEventSourceNativeData( pNativeData );
	}

	MetalDisplaySurface::~MetalDisplaySurface() noexcept
	{
		resetEventSourceNativeData();
	}

	void MetalDisplaySurface::resizeClientArea( const FrameSize & pSize )
	{
		FrameGeometry newFrameGeometry{};
		newFrameGeometry.position = CX_FRAME_POS_AUTO;
		newFrameGeometry.size = pSize;
		newFrameGeometry.style = EFrameStyle::Unspecified;

		newFrameGeometry = mGLSystemDriver->mDisplayManager->validateFrameGeometry( newFrameGeometry );

		const auto updateFlags = E_FRAME_GEOMETRY_UPDATE_FLAG_POSITION_BIT | E_FRAME_GEOMETRY_UPDATE_FLAG_SIZE_CLIENT_AREA_BIT;
		_nativeUpdateGeometry( newFrameGeometry, updateFlags );
	}

	void MetalDisplaySurface::resizeFrame( const FrameSize & pSize )
	{
		FrameGeometry newFrameGeometry{};
		newFrameGeometry.position = CX_FRAME_POS_AUTO;
		newFrameGeometry.size = pSize;
		newFrameGeometry.style = EFrameStyle::Unspecified;

		newFrameGeometry = mGLSystemDriver->mDisplayManager->validateFrameGeometry( newFrameGeometry );

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
