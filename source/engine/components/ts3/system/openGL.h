
#ifndef __TS3_SYSTEM_GFX_OPENGL_H__
#define __TS3_SYSTEM_GFX_OPENGL_H__

#include "openGLDefs.h"

namespace ts3
{

	/// @brief Contains parameters used to specify how a GL display surface should be created.
	struct SysGLSurfaceCreateInfo
	{
		//
		SysVisualConfig visualConfig;
		//
		SysWindowGeometry windowGeometry;
		//
		Bitmask<ESysGLSurfaceCreateFlags> flags = 0;
	};

	/// @brief
	struct SysGLRenderContextCreateInfo
	{
		//
        SysGLRenderContextHandle shareContext = nullptr;
		// Target API version (minimum) a context must support.
		Version requiredAPIVersion;
		// Target API profile which will be used by the app.
		ESysGLAPIProfile targetAPIProfile;
		//
		Bitmask<ESysGLRenderContextCreateFlags> flags = 0;
	};

	/// @brief
	class SysGLSurface : public SysBaseObject, public SysEventSource
	{
		friend class SysGLCoreDevice;

	public:
		SysGLSurfaceNativeData mNativeData;
		SysGLCoreDeviceHandle const mGLCoreDevice;

		explicit SysGLSurface( SysGLCoreDeviceHandle pGLCoreDevice ) noexcept;
		virtual ~SysGLSurface() noexcept;

		void swapBuffers();

		TS3_PCL_ATTR_NO_DISCARD SysWindowSize queryCurrentSize() const;

	private:
		void _sysDestroy() noexcept;
		void _sysSwapBuffers();
		void _sysQueryCurrentSize( SysWindowSize & pSize ) const;
	};

	/// @brief
	class SysGLRenderContext : public SysBaseObject
	{
		friend class SysGLCoreDevice;

	public:
		SysGLRenderContextNativeData mNativeData;
		SysGLCoreDeviceHandle const mGLCoreDevice;

		explicit SysGLRenderContext( SysGLCoreDeviceHandle pGLCoreDevice ) noexcept;
		virtual ~SysGLRenderContext() noexcept;

		void bindForCurrentThread( SysGLSurface & pTargetSurface );

		TS3_PCL_ATTR_NO_DISCARD bool validateCurrentBinding() const;

		TS3_PCL_ATTR_NO_DISCARD SysGLSystemVersionInfo querySystemVersionInfo() const;

	private:
		void _sysDestroy() noexcept;
		void _sysBindForCurrentThread( SysGLSurface & pTargetSurface );
		bool _sysValidateCurrentBinding() const;
	};

	/// @brief
	class SysGLCoreDevice : public SysBaseObject
	{
	public:
		SysGLCoreDeviceNativeData mNativeData;
		SysDisplayManagerHandle const mDisplayManager;

		explicit SysGLCoreDevice( SysDisplayManagerHandle pDisplayManager ) noexcept;
		virtual ~SysGLCoreDevice() noexcept;

		static SysGLCoreDeviceHandle create( SysDisplayManagerHandle pDisplayManager );

		/// @brief Initializes core OpenGL state and system-level interfaces.
		///
		/// This method also creates a temporary context if necessary (Win32). Use releaseInitState() to free that.
		void initializePlatform();

		/// @brief Releases temporary init state created automatically by the subsystem. Context must be a valid GL context.
		///
		/// Call this method after you have created actual display surface and context. It's not mandatory, (everything
		/// will be released at shutdown anyway), but on some platforms this could free some extra memory and release
		/// couple system-level interfaces. Context is required to ensure another valid context has been already created.
		void releaseInitState( SysGLRenderContext & pGLRenderContext );

		SysGLSurfaceHandle createDisplaySurface( const SysGLSurfaceCreateInfo & pCreateInfo );
		SysGLSurfaceHandle createDisplaySurfaceForCurrentThread();
		SysGLRenderContextHandle createRenderContext( SysGLSurface & pSurface, const SysGLRenderContextCreateInfo & pCreateInfo );
		SysGLRenderContextHandle createRenderContextForCurrentThread();

		void setPrimaryContext( SysGLRenderContext & pPrimaryContext );

		void resetPrimaryContext();

		std::vector<SysDepthStencilFormat> querySupportedDepthStencilFormats( SysColorFormat pColorFormat );

		std::vector<SysMSAAMode> querySupportedMSAAModes( SysColorFormat pColorFormat, SysDepthStencilFormat pDepthStencilFormat );

		SysGLRenderContext * getPrimaryContext() const;

	private:
		void _sysInitializePlatform();
		void _sysReleaseInitState();
		void _sysCreateDisplaySurface( SysGLSurface & pGLSurface, const SysGLSurfaceCreateInfo & pCreateInfo );
		void _sysCreateDisplaySurfaceForCurrentThread( SysGLSurface & pGLSurface );
		void _sysCreateRenderContext( SysGLRenderContext & pGLRenderContext, SysGLSurface & pGLSurface, const SysGLRenderContextCreateInfo & pCreateInfo );
		void _sysCreateRenderContextForCurrentThread( SysGLRenderContext & pGLRenderContext );

	private:
		SysGLRenderContext * _primaryContext;
	};

}

#endif // __TS3_SYSTEM_GFX_OPENGL_H__
