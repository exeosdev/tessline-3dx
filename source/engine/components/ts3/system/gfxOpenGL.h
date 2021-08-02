
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
		// Target API profile which will be used by the app.sysCreateGLDriverInstance
		ESysGLAPIProfile targetAPIProfile;
		//
		Bitmask<ESysGLRenderContextCreateFlags> flags = 0;
	};

	/// @brief
	TS3_SYSTEM_API SysGLDriverHandle sysGfxCreateGLDriver();

	/// @brief Initializes core OpenGL state and system-level interfaces.
	/// This method also creates any additionally required
	TS3_SYSTEM_API void sysGfxGLDriverInitializePlatform( SysGLDriverHandle pGLDriver );

	/// @brief Releases temporary init state created automatically by the library. Context must be a valid GL context.
	/// Call this method after you have created actual display surface and context. It's not mandatory, (everything
	/// will be released at shutdown anyway), but on some platforms this could free some extra memory and release
	/// couple system-level interfaces.
	TS3_SYSTEM_API void sysGfxGLDriverReleaseInitState( SysGLRenderContext & pGLRenderContext );

	/// @brief
	TS3_SYSTEM_API SysGLSurfaceHandle sysGfxGLDriverCreateDisplaySurface( SysGLDriverHandle pGLDriver,
                                                                       const SysGLSurfaceCreateInfo & pCreateInfo );

	/// @brief
	TS3_SYSTEM_API SysGLSurfaceHandle sysGfxGLDriverCreateDisplaySurfaceForCurrentThread( SysGLDriverHandle pGLDriver );

	/// @brief
	TS3_SYSTEM_API SysGLRenderContextHandle sysGfxGLDriverCreateRenderContext( SysGLDriverHandle pGLDriver,
                                                                            SysGLSurface & pSurface,
                                                                            const SysGLRenderContextCreateInfo & pCreateInfo );

	/// @brief
	TS3_SYSTEM_API SysGLRenderContextHandle sysGfxGLDriverCreateRenderContextForCurrentThread( SysGLDriverHandle pGLDriver );

	/// @brief
	TS3_SYSTEM_API std::vector<SysDepthStencilFormat> sysGfxGLDriverQuerySupportedDepthStencilFormats( SysColorFormat pColorFormat );

	/// @brief
	TS3_SYSTEM_API std::vector<SysMSAAMode> sysGfxGLDriverQuerySupportedMSAAModes( SysColorFormat pColorFormat,
                                                                                SysDepthStencilFormat pDepthStencilFormat );

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/// @brief
	TS3_SYSTEM_API void sysGfxGLSurfaceSwapBuffers( SysGLSurfaceHandle pGLSurface );

	/// @brief
	TS3_SYSTEM_API TS3_PCL_ATTR_NO_DISCARD SysWindowSize sysGfxGLSurfaceQueryRenderAreaSize( SysGLSurfaceHandle pGLSurface );

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/// @brief
	TS3_SYSTEM_API void sysGLRenderContextBindForCurrentThread( SysGLRenderContextHandle pGLRenderContext );

	/// @brief
	TS3_SYSTEM_API bool sysGLRenderContextValidateCurrentBinding( SysGLRenderContextHandle pGLRenderContext );

	/// @brief
	TS3_SYSTEM_API SysGLSystemVersionInfo sysGLRenderContextQuerySystemVersionInfo( SysGLRenderContextHandle pGLRenderContext );

}

#endif // __TS3_SYSTEM_GFX_OPENGL_H__
