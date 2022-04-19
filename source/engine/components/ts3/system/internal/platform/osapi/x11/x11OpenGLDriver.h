
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_X11_OPENGL_DRIVER_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_X11_OPENGL_DRIVER_H__

#include "x11WindowSystem.h"
#include <ts3/system/openGLDriver.h>
#include <GL/glx.h>

namespace ts3::system
{

	ts3SysDeclareHandle( X11OpenGLSystemDriver );
	ts3SysDeclareHandle( X11OpenGLRenderContext );
	ts3SysDeclareHandle( X11OpenGLRenderContext );

	namespace platform
	{

		using GLXFBConfigArray = std::vector<GLXFBConfig>;

		struct X11OpenGLDisplaySurfaceNativeData : public X11WindowNativeData
		{
			GLXFBConfig fbConfig = nullptr;
			XVisualInfo * visualInfo = nullptr;
		};

		struct X11OpenGLRenderContextNativeData : public X11NativeDataCommon
		{
			GLXContext contextHandle = nullptr;
		};

		struct X11OpenGLSystemDriverNativeData : public X11NativeDataCommon
		{
			struct InitState
			{
				X11OpenGLDisplaySurfaceNativeData surfaceData;
				X11OpenGLRenderContextNativeData contextData;
			};
			std::unique_ptr<InitState> initState = nullptr;
		};

		struct X11OpenGLVisualConfig
		{
			GLXFBConfig fbConfig = nullptr;

			XVisualInfo * xVisualInfo = nullptr;

			explicit operator bool() const
			{
				return fbConfig && xVisualInfo;
			}
		};

	}

	/// @brief X11-specific implementation of the OpenGLSystemDriver class.
	class X11OpenGLSystemDriver : public X11NativeObject<OpenGLSystemDriver, platform::X11OpenGLSystemDriverNativeData>
	{
	public:
		X11OpenGLSystemDriver( X11DisplayManagerHandle pDisplayManager );
		virtual ~X11OpenGLSystemDriver() noexcept;

	private:
		void _releaseX11DriverState();

		/// @override OpenGLSystemDriver::_nativeInitializePlatform
		virtual void _nativeInitializePlatform() override final;

		/// @override OpenGLSystemDriver::_nativeReleaseInitState
		virtual void _nativeReleaseInitState() noexcept override final;

		/// @override OpenGLSystemDriver::_nativeCreateDisplaySurface
		virtual OpenGLDisplaySurfaceHandle _nativeCreateDisplaySurface( const GLDisplaySurfaceCreateInfo & pCreateInfo ) override final;

		/// @override OpenGLSystemDriver::_nativeCreateDisplaySurfaceForCurrentThread
		virtual OpenGLDisplaySurfaceHandle _nativeCreateDisplaySurfaceForCurrentThread() override final;

		/// @override OpenGLSystemDriver::_nativeDestroyDisplaySurface
		virtual void _nativeDestroyDisplaySurface( OpenGLDisplaySurface & pDisplaySurface ) override final;

		/// @override OpenGLSystemDriver::_nativeCreateRenderContext
		virtual OpenGLRenderContextHandle _nativeCreateRenderContext( OpenGLDisplaySurface & pDisplaySurface,
		                                                              const GLRenderContextCreateInfo & pCreateInfo ) override final;

		/// @override OpenGLSystemDriver::_nativeCreateRenderContextForCurrentThread
		virtual OpenGLRenderContextHandle _nativeCreateRenderContextForCurrentThread() override final;

		/// @override OpenGLSystemDriver::_nativeDestroyDisplaySurface
		virtual void _nativeDestroyRenderContext( OpenGLRenderContext & pRenderContext ) override final;

		/// @override OpenGLSystemDriver::_nativeResetContextBinding
		virtual void _nativeResetContextBinding() override final;

		/// @override OpenGLSystemDriver::_nativeQuerySupportedDepthStencilFormats
		virtual std::vector<EDepthStencilFormat> _nativeQuerySupportedDepthStencilFormats( EColorFormat pColorFormat ) const override final;

		/// @override OpenGLSystemDriver::_nativeQuerySupportedMSAAModes
		virtual std::vector<EMSAAMode> _nativeQuerySupportedMSAAModes( EColorFormat pColorFormat,
                                                                       EDepthStencilFormat pDepthStencilFormat ) const override final;

		/// @override OpenGLSystemDriver::_nativeIssGLAPIProfileSupported
		virtual bool _nativeIsGLAPIProfileSupported( EGLAPIProfile pGLAPIProfile ) const override final;;

		/// @override OpenGLSystemDriver::_nativeIsRenderContextBound
		virtual bool _nativeIsRenderContextBound() const override final;
	};

	/// @brief X11-specific implementation of the OpenGLDisplaySurface class.
	class X11OpenGLDisplaySurface : public X11NativeObject<OpenGLDisplaySurface, platform::X11OpenGLDisplaySurfaceNativeData>
	{
	public:
		explicit X11OpenGLDisplaySurface( X11OpenGLSystemDriverHandle pGLSystemDriver );
		virtual ~X11OpenGLDisplaySurface() noexcept;

	private:
		void _releaseX11SurfaceState();

		/// @override OpenGLDisplaySurface::_nativeSwapBuffers
		virtual void _nativeSwapBuffers() override final;

		/// @override OpenGLDisplaySurface::_nativeQueryRenderAreaSize
		virtual FrameSize _nativeQueryRenderAreaSize() const override final;

		/// @override OpenGLDisplaySurface::_nativeSysValidate
		virtual bool _nativeSysValidate() const override final;

		/// @override OpenGLDisplaySurface::_nativeResize
		virtual void _nativeResize( const FrameSize & pFrameSize, EFrameSizeMode pSizeMode ) override final;

		/// @override OpenGLDisplaySurface::_nativeSetFullscreenMode
		virtual void _nativeSetFullscreenMode( bool pEnable ) override final;

		/// @override OpenGLDisplaySurface::_nativeSetTitle
		virtual void _nativeSetTitle( const std::string & pTitle ) override final;

		/// @override OpenGLDisplaySurface::_nativeUpdateGeometry
		virtual void _nativeUpdateGeometry( const FrameGeometry & pFrameGeometry,
		                                    Bitmask<EFrameGeometryUpdateFlags> pUpdateFlags ) override final;

		/// @override OpenGLDisplaySurface::_nativeGetSize
		virtual FrameSize _nativeGetSize( EFrameSizeMode pSizeMode ) const override final;

		/// @override OpenGLDisplaySurface::_nativeIsFullscreen
		virtual bool _nativeIsFullscreen() const override final;
	};

	/// @brief X11-specific implementation of the OpenGLRenderContext class.
	class X11OpenGLRenderContext : public X11NativeObject<OpenGLRenderContext, platform::X11OpenGLRenderContextNativeData>
	{
	public:
		explicit X11OpenGLRenderContext( X11OpenGLSystemDriverHandle pGLSystemDriver );
		virtual ~X11OpenGLRenderContext() noexcept;

	private:
		void _releaseX11ContextState();

		/// @override OpenGLRenderContext::_nativeBindForCurrentThread
		virtual void _nativeBindForCurrentThread( const OpenGLDisplaySurface & pTargetSurface ) override final;

		/// @override OpenGLRenderContext::_nativeIsCurrent
		virtual bool _nativeSysCheckIsCurrent() const override final;

		/// @override OpenGLRenderContext::_nativeSysValidate
		virtual bool _nativeSysValidate() const override final;
	};

} // namespace ts3::system

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_X11_OPENGL_DRIVER_H__
