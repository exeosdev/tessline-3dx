
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_OSX_OPENGL_DRIVER_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_OSX_OPENGL_DRIVER_H__

#include "osxWindowSystem.h"
#include <ts3/system/openGLDriver.h>

#ifdef __clang__
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif

#import <AppKit/NSOpenGL.h>
#import <Foundation/NSThread.h>
#import <OpenGL/OpenGL.h>

namespace ts3::system
{

	ts3SysDeclareHandle( OSXOpenGLSystemDriver );
	ts3SysDeclareHandle( OSXOpenGLDisplaySurface );
	ts3SysDeclareHandle( OSXOpenGLRenderContext );

	namespace platform
	{

		struct OSXOpenGLDisplaySurfaceNativeData : public OSXWindowNativeData
		{
			NSOpenGLPixelFormat * nsPixelFormat = nil;
		};

		struct OSXOpenGLRenderContextNativeData : public OSXNativeDataCommon
		{
			NSOpenGLContext * nsContextHandle = nil;
		};

		struct OSXOpenGLSystemDriverNativeData : public OSXNativeDataCommon
		{
		};

		constexpr size_t CX_OSX_MAX_NSGL_FBCONFIG_ATTRIBUTES_NUM = 32u;

	}

	/// @brief OSX-specific implementation of the OpenGLSystemDriver class.
	class OSXOpenGLSystemDriver : public OSXNativeObject<OpenGLSystemDriver, platform::OSXOpenGLSystemDriverNativeData>
	{
	public:
		OSXOpenGLSystemDriver( OSXDisplayManagerHandle pDisplayManager );
		virtual ~OSXOpenGLSystemDriver() noexcept;

	private:
		void _initializeOSXDriverState();
		void _releaseOSXDriverState();

		/// @copybrief OpenGLSystemDriver::_nativeInitializePlatform
		virtual void _nativeInitializePlatform() override final;

		/// @copybrief OpenGLSystemDriver::_nativeReleaseInitState
		virtual void _nativeReleaseInitState() noexcept override final;

		/// @copybrief OpenGLSystemDriver::_nativeQueryVersionSupportInfo
		virtual OpenGLVersionSupportInfo _nativeQueryVersionSupportInfo() const noexcept override final;

		/// @copybrief OpenGLSystemDriver::_nativeCreateDisplaySurface
		virtual OpenGLDisplaySurfaceHandle _nativeCreateDisplaySurface( const OpenGLDisplaySurfaceCreateInfo & pCreateInfo ) override final;

		/// @copybrief OpenGLSystemDriver::_nativeCreateDisplaySurfaceForCurrentThread
		virtual OpenGLDisplaySurfaceHandle _nativeCreateDisplaySurfaceForCurrentThread() override final;

		/// @copybrief OpenGLSystemDriver::_nativeDestroyDisplaySurface
		virtual void _nativeDestroyDisplaySurface( OpenGLDisplaySurface & pDisplaySurface ) override final;

		/// @copybrief OpenGLSystemDriver::_nativeCreateRenderContext
		virtual OpenGLRenderContextHandle _nativeCreateRenderContext( OpenGLDisplaySurface & pDisplaySurface,
		                                                              const OpenGLRenderContextCreateInfo & pCreateInfo ) override final;

		/// @copybrief OpenGLSystemDriver::_nativeCreateRenderContextForCurrentThread
		virtual OpenGLRenderContextHandle _nativeCreateRenderContextForCurrentThread() override final;

		/// @copybrief OpenGLSystemDriver::_nativeDestroyDisplaySurface
		virtual void _nativeDestroyRenderContext( OpenGLRenderContext & pRenderContext ) override final;

		/// @copybrief OpenGLSystemDriver::_nativeResetContextBinding
		virtual void _nativeResetContextBinding() override final;

		/// @copybrief OpenGLSystemDriver::_nativeQuerySupportedDepthStencilFormats
		virtual std::vector<EDepthStencilFormat> _nativeQuerySupportedDepthStencilFormats( EColorFormat pColorFormat ) const override final;

		/// @copybrief OpenGLSystemDriver::_nativeQuerySupportedMSAAModes
		virtual std::vector<EMSAAMode> _nativeQuerySupportedMSAAModes( EColorFormat pColorFormat,
		                                                               EDepthStencilFormat pDepthStencilFormat ) const override final;

		/// @copybrief OpenGLSystemDriver::_nativeIssAPIClassSupported
		virtual bool _nativeIsAPIClassSupported( EOpenGLAPIClass pAPIClass ) const override final;;

		/// @copybrief OpenGLSystemDriver::_nativeIsRenderContextBound
		virtual bool _nativeIsRenderContextBound() const override final;

	private:
		NSOpenGLPixelFormatAttribute _nsSupportedOpenGLVersion;
	};

	/// @brief OSX-specific implementation of the OpenGLDisplaySurface class.
	class OSXOpenGLDisplaySurface : public OSXNativeObject<OpenGLDisplaySurface, platform::OSXOpenGLDisplaySurfaceNativeData>
	{
	public:
		explicit OSXOpenGLDisplaySurface( OSXOpenGLSystemDriverHandle pGLSystemDriver );
		virtual ~OSXOpenGLDisplaySurface() noexcept;

	private:
		void _releaseOSXSurfaceState();

		/// @copybrief OpenGLDisplaySurface::_nativeSwapBuffers
		virtual void _nativeSwapBuffers() override final;

		/// @copybrief OpenGLDisplaySurface::_nativeQuerySupportedAPIClass
		virtual EOpenGLAPIClass _nativeQuerySupportedAPIClass() const noexcept override final;

		virtual VisualConfig _nativeQueryVisualConfig() const override final;

		/// @copybrief OpenGLDisplaySurface::_nativeQueryRenderAreaSize
		virtual FrameSize _nativeQueryRenderAreaSize() const override final;

		/// @copybrief OpenGLDisplaySurface::_nativeSysValidate
		virtual bool _nativeSysValidate() const override final;

		/// @copybrief OpenGLDisplaySurface::_nativeResize
		virtual void _nativeResize( const FrameSize & pFrameSize, EFrameSizeMode pSizeMode ) override final;

		/// @copybrief OpenGLDisplaySurface::_nativeSetFullscreenMode
		virtual void _nativeSetFullscreenMode( bool pEnable ) override final;

		/// @copybrief OpenGLDisplaySurface::_nativeSetTitle
		virtual void _nativeSetTitle( const std::string & pTitle ) override final;

		/// @copybrief OpenGLDisplaySurface::_nativeUpdateGeometry
		virtual void _nativeUpdateGeometry( const FrameGeometry & pFrameGeometry,
		                                    Bitmask<EFrameGeometryUpdateFlags> pUpdateFlags ) override final;

		/// @copybrief OpenGLDisplaySurface::_nativeGetSize
		virtual FrameSize _nativeGetSize( EFrameSizeMode pSizeMode ) const override final;

		/// @copybrief OpenGLDisplaySurface::_nativeIsFullscreen
		virtual bool _nativeIsFullscreen() const override final;
	};

	/// @brief OSX-specific implementation of the OpenGLRenderContext class.
	class OSXOpenGLRenderContext : public OSXNativeObject<OpenGLRenderContext, platform::OSXOpenGLRenderContextNativeData>
	{
	public:
		explicit OSXOpenGLRenderContext( OSXOpenGLSystemDriverHandle pGLSystemDriver );
		virtual ~OSXOpenGLRenderContext() noexcept;

	private:
		void _releaseOSXContextState();

		/// @copybrief OpenGLRenderContext::_nativeBindForCurrentThread
		virtual void _nativeBindForCurrentThread( const OpenGLDisplaySurface & pTargetSurface ) override final;

		/// @copybrief OpenGLRenderContext::_nativeIsCurrent
		virtual bool _nativeSysCheckIsCurrent() const override final;

		/// @copybrief OpenGLRenderContext::_nativeSysValidate
		virtual bool _nativeSysValidate() const override final;
	};
	
} // namespace ts3::system

#ifdef __clang__
#  pragma clang diagnostic pop
#endif

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_OSX_OPENGL_DRIVER_H__
