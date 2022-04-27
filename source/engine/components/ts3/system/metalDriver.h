
#ifndef __TS3_SYSTEM_METAL_DRIVER_H__
#define __TS3_SYSTEM_METAL_DRIVER_H__

#include "metalCommon.h"

namespace ts3::system
{

	ts3SysDeclareHandle( DisplayManager );
	ts3SysDeclareHandle( MetalDisplaySurface );
	ts3SysDeclareHandle( MetalSystemDriver );

	/// @brief
	struct MetalDisplaySurfaceCreateInfo : public WindowCreateInfo
	{
	};

	class MetalSystemDriver : public SysObject
	{
	public:
		DisplayManagerHandle const mDisplayManager;

	public:
		explicit MetalSystemDriver( DisplayManagerHandle pDisplayManager );
		virtual ~MetalSystemDriver() noexcept;
		
		/// @brief
		TS3_FUNC_NO_DISCARD MetalDisplaySurfaceHandle createDisplaySurface( const MetalDisplaySurfaceCreateInfo & pCreateInfo );
		
	private:
		virtual MetalDisplaySurfaceHandle _nativeCreateDisplaySurface( const MetalDisplaySurfaceCreateInfo & pCreateInfo ) = 0;

		virtual void _nativeDestroyDisplaySurface( MetalDisplaySurface & pDisplaySurface ) = 0;

	};

	class MetalDisplaySurface : public Frame
	{
		friend class MetalSystemDriver;

	public:
		MetalSystemDriverHandle const mGLSystemDriver;

	public:
		explicit MetalDisplaySurface( MetalSystemDriverHandle pGLSystemDriver, void * pNativeData );
		virtual ~MetalDisplaySurface() noexcept;

		void clearColorBuffer();

		/// @copybrief Frame::resizeClientArea
		virtual void resizeClientArea( const FrameSize & pSize ) override final;

		/// @copybrief Frame::resizeFrame
		virtual void resizeFrame( const FrameSize & pSize ) override final;

		/// @copybrief Frame::setFullscreenMode
		virtual void setFullscreenMode( bool pEnable ) override final;

		/// @copybrief Frame::setTitle
		virtual void setTitle( const std::string & pTitleText ) override final;

		/// @copybrief Frame::updateGeometry
		virtual void updateGeometry( const FrameGeometry & pFrameGeometry,
		                             Bitmask<EFrameGeometryUpdateFlags> pUpdateFlags ) override final;

		/// @copybrief Frame::getClientAreaSize
		TS3_FUNC_NO_DISCARD virtual FrameSize getClientAreaSize() const override final;

		/// @copybrief Frame::getSize
		TS3_FUNC_NO_DISCARD virtual FrameSize getFrameSize() const override final;

		/// @copybrief Frame::isFullscreen
		TS3_FUNC_NO_DISCARD virtual bool isFullscreen() const override final;
		
	private:
		virtual FrameSize _nativeQueryRenderAreaSize() const = 0;

		virtual bool _nativeSysValidate() const = 0;

		virtual void _nativeResize( const FrameSize & pFrameSize, EFrameSizeMode pSizeMode ) = 0;

		virtual void _nativeSetFullscreenMode( bool pEnable ) = 0;

		virtual void _nativeSetTitle( const std::string & pTitle ) = 0;

		virtual void _nativeUpdateGeometry( const FrameGeometry & pFrameGeometry,
		                                    Bitmask<EFrameGeometryUpdateFlags> pUpdateFlags ) = 0;

		virtual FrameSize _nativeGetSize( EFrameSizeMode pSizeMode ) const = 0;

		virtual bool _nativeIsFullscreen() const = 0;
	};

}

#endif // __TS3_SYSTEM_METAL_DRIVER_H__
