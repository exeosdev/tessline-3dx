
#ifndef __TS3_SYSTEM_GFX_OPENGL_H__
#define __TS3_SYSTEM_GFX_OPENGL_H__

#include "eventCore.h"
#include "visual.h"
#include "windowDefs.h"
#include <GL/glew.h>

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_ANDROID )
#  include "internal/platform/osapi/android/androidOpenGL.h"
#elif( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_WIN32 )
#  include "internal/platform/osapi/win32/win32OpenGL.h"
#elif( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_X11 )
#  include "internal/platform/osapi/x11/x11OpenGL.h"
#endif

namespace ts3
{

	ts3DeclareSysHandle( SysDisplayManager );
	ts3DeclareSysHandle( SysGLSubsystem );
	ts3DeclareSysHandle( SysGLSurface );
	ts3DeclareSysHandle( SysGLContext );

	/// @brief
	enum ESysGLSurfaceCreateFlags : uint32
	{
		// Specifies a full-screen surface, covering the whole screen.
		// Ignored for mobile (iOS and Android), where fullscreen is always used.
		E_SYS_GFX_GL_DISPLAY_SURFACE_CREATE_FLAG_FULLSCREEN_BIT = 0x1000,
		E_SYS_GFX_GL_DISPLAY_SURFACE_CREATE_FLAG_SYNC_ADAPTIVE_BIT = 0x2000,
		E_SYS_GFX_GL_DISPLAY_SURFACE_CREATE_FLAG_SYNC_VERTICAL_BIT = 0x4000,
	};

	/// @brief
	enum ESysGLContextCreateFlags : uint32
	{
		E_SYS_GFX_GL_RENDER_CONTEXT_CREATE_FLAG_ENABLE_DEBUG_BIT = 0x1000,
		E_SYS_GFX_GL_RENDER_CONTEXT_CREATE_FLAG_FORWARD_COMPATIBLE_BIT = 0x2000,
		E_SYS_GFX_GL_RENDER_CONTEXT_CREATE_FLAG_ENABLE_SHARING_BIT = 0x0010,
		E_SYS_GFX_GL_RENDER_CONTEXT_CREATE_FLAG_SHARE_WITH_CURRENT_BIT = 0x0020
	};

	/// @brief
	enum class ESysGLAPIProfile : enum_default_value_t
	{
		Auto,
		Core,
		GLES,
		Legacy
	};

	/// @brief Represents combined info about the current OpenGL subsystem version.
	/// Basically, this struct stores the output from all version-related GL queries.
	struct SysGLSystemVersionInfo
	{
	public:
		// Numeric version of the GL (GL_VERSION_MAJOR.GL_VERSION_MINOR)
		Version apiVersion;
		// Text version of the GL (GL_VERSION)
		std::string apiVersionStr;
		// Text version of the GLSL (GL_SHADING_LANGUAGE_VERSION)
		std::string glslVersionStr;
		// Name of the renderer (GL_RENDERER_NAME)
		std::string rendererName;
		// Name of the vendor (GL_VENDOR_NAME)
		std::string vendorName;

	public:
		std::string toString() const;
	};

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
	struct SysGLContextCreateInfo
	{
		//
		// SysGLSurface * displaySurface = nullptr;
		//
		SysGLContext * shareContext = nullptr;
		// Target API version (minimum) a context must support.
		Version requiredAPIVersion;
		// Target API profile which will be used by the app.
		ESysGLAPIProfile targetAPIProfile;
		//
		Bitmask<ESysGLContextCreateFlags> flags = 0;
	};

	/// @brief
	class SysGLSurface : public SysBaseObject, public SysEventSource
	{
		friend class SysGLSubsystem;

	public:
		SysGLSurfaceNativeData mNativeData;
		SysGLSubsystemHandle const mGLSubsystem;

		explicit SysGLSurface( SysGLSubsystemHandle pGLSubsystem ) noexcept;
		virtual ~SysGLSurface() noexcept;

		void swapBuffers();

		TS3_PCL_ATTR_NO_DISCARD SysWindowSize queryCurrentSize() const;

	private:
		void _sysDestroy() noexcept;
		void _sysSwapBuffers();
		void _sysQueryCurrentSize( SysWindowSize & pSize ) const;
	};

	/// @brief
	class SysGLContext : public SysBaseObject
	{
		friend class SysGLSubsystem;

	public:
		SysGLContextNativeData mNativeData;
		SysGLSubsystemHandle const mGLSubsystem;

		explicit SysGLContext( SysGLSubsystemHandle pGLSubsystem ) noexcept;
		virtual ~SysGLContext() noexcept;

		void bindForCurrentThread( SysGLSurface & pTargetSurface );

		TS3_PCL_ATTR_NO_DISCARD bool validateCurrentBinding() const;

		TS3_PCL_ATTR_NO_DISCARD SysGLSystemVersionInfo querySystemVersionInfo() const;

	private:
		void _sysDestroy() noexcept;
		void _sysBindForCurrentThread( SysGLSurface & pTargetSurface );
		bool _sysValidateCurrentBinding() const;
	};

	/// @brief
	class SysGLSubsystem : public SysBaseObject
	{
	public:
		SysGLSubsystemNativeData mNativeData;
		SysDisplayManagerHandle const mDisplayManager;

		explicit SysGLSubsystem( SysDisplayManagerHandle pDisplayManager ) noexcept;
		virtual ~SysGLSubsystem() noexcept;

		static SysGLSubsystemHandle create( SysDisplayManagerHandle pDisplayManager );

		/// @brief Initializes core OpenGL state and system-level interfaces.
		///
		/// This method also creates a temporary context if necessary (Win32). Use releaseInitState() to free that.
		void initializePlatform();

		/// @brief Releases temporary init state created automatically by the subsystem. Context must be a valid GL context.
		///
		/// Call this method after you have created actual display surface and context. It's not mandatory, (everything
		/// will be released at shutdown anyway), but on some platforms this could free some extra memory and release
		/// couple system-level interfaces. Context is required to ensure another valid context has been already created.
		void releaseInitState( SysGLContext & pGLContext );

		SysGLSurfaceHandle createDisplaySurface( const SysGLSurfaceCreateInfo & pCreateInfo );
		SysGLSurfaceHandle createDisplaySurfaceForCurrentThread();
		SysGLContextHandle createRenderContext( SysGLSurface & pSurface, const SysGLContextCreateInfo & pCreateInfo );
		SysGLContextHandle createRenderContextForCurrentThread();

		void setPrimaryContext( SysGLContext & pPrimaryContext );

		void resetPrimaryContext();

		std::vector<SysDepthStencilFormat> querySupportedDepthStencilFormats( SysColorFormat pColorFormat );

		std::vector<SysMSAAMode> querySupportedMSAAModes( SysColorFormat pColorFormat, SysDepthStencilFormat pDepthStencilFormat );

		SysGLContext * getPrimaryContext() const;

	private:
		void _sysInitializePlatform();
		void _sysReleaseInitState();
		void _sysCreateDisplaySurface( SysGLSurface & pGLSurface, const SysGLSurfaceCreateInfo & pCreateInfo );
		void _sysCreateDisplaySurfaceForCurrentThread( SysGLSurface & pGLSurface );
		void _sysCreateRenderContext( SysGLContext & pGLContext, SysGLSurface & pGLSurface, const SysGLContextCreateInfo & pCreateInfo );
		void _sysCreateRenderContextForCurrentThread( SysGLContext & pGLContext );

	private:
		SysGLContext * _primaryContext;
	};

}

#endif // __TS3_SYSTEM_GFX_OPENGL_H__
