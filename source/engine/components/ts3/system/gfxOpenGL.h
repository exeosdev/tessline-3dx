
#ifndef __TS3_SYSTEM_GFX_OPENGL_H__
#define __TS3_SYSTEM_GFX_OPENGL_H__

#include "gfxOpenGLCommon.h"

namespace ts3
{
namespace system
{

	/// @brief Contains parameters used to specify how a GL display surface should be created.
	struct GfxGLSurfaceCreateInfo
	{
		//
		GfxVisualConfig visualConfig;
		//
		WmWindowGeometry windowGeometry;
		//
		Bitmask<EGfxGLSurfaceCreateFlags> flags = 0;
	};

	/// @brief
	struct GfxGLRenderContextCreateInfo
	{
		//
        GfxGLRenderContextHandle shareContext = nullptr;
		// Target API version (minimum) a context must support.
		Version requiredAPIVersion;
		// Target API profile which will be used by the app.sysCreateGLDriverInstance
		EGfxGLAPIProfile targetAPIProfile;
		//
		Bitmask<EGfxGLRenderContextCreateFlags> flags = 0;
	};

	/// @brief
	TS3_SYSTEM_API_NODISCARD GfxGLDriverHandle gfxCreateGLDriver();

	/// @brief Initializes core OpenGL state and system-level interfaces.
	/// This method also creates any additionally required
	TS3_SYSTEM_API void gfxGLDriverInitializePlatform( GfxGLDriverHandle pGLDriver );

	/// @brief Releases temporary init state created automatically by the library. Context must be a valid GL context.
	/// Call this method after you have created actual display surface and context. It's not mandatory, (everything
	/// will be released at shutdown anyway), but on some platforms this could free some extra memory and release
	/// couple system-level interfaces.
	TS3_SYSTEM_API void gfxGLDriverReleaseInitState( GfxGLRenderContext & pGLRenderContext );

	/// @brief
	TS3_SYSTEM_API_NODISCARD GfxGLSurfaceHandle gfxGLDriverCreateDisplaySurface( GfxGLDriverHandle pGLDriver,
                                                                       const GfxGLSurfaceCreateInfo & pCreateInfo );

	/// @brief
	TS3_SYSTEM_API_NODISCARD GfxGLSurfaceHandle gfxGLDriverCreateDisplaySurfaceForCurrentThread( GfxGLDriverHandle pGLDriver );

	/// @brief
	TS3_SYSTEM_API_NODISCARD GfxGLRenderContextHandle gfxGLDriverCreateRenderContext( GfxGLDriverHandle pGLDriver,
                                                                            GfxGLSurface & pSurface,
                                                                            const GfxGLRenderContextCreateInfo & pCreateInfo );

	/// @brief
	TS3_SYSTEM_API_NODISCARD GfxGLRenderContextHandle gfxGLDriverCreateRenderContextForCurrentThread( GfxGLDriverHandle pGLDriver );

	/// @brief
	TS3_SYSTEM_API_NODISCARD std::vector<GfxDepthStencilFormat> gfxGLDriverQuerySupportedDepthStencilFormats( GfxColorFormat pColorFormat );

	/// @brief
	TS3_SYSTEM_API_NODISCARD std::vector<GfxMSAAMode> gfxGLDriverQuerySupportedMSAAModes( GfxColorFormat pColorFormat,
                                                                                GfxDepthStencilFormat pDepthStencilFormat );

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/// @brief
	TS3_SYSTEM_API void gfxGLSurfaceSwapBuffers( GfxGLSurfaceHandle pGLSurface );

	/// @brief
	TS3_SYSTEM_API_NODISCARD WmWindowSize gfxGLSurfaceQueryRenderAreaSize( GfxGLSurfaceHandle pGLSurface );

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/// @brief
	TS3_SYSTEM_API void sysGLRenderContextBindForCurrentThread( GfxGLRenderContextHandle pGLRenderContext );

	/// @brief
	TS3_SYSTEM_API_NODISCARD bool sysGLRenderContextValidateCurrentBinding( GfxGLRenderContextHandle pGLRenderContext );

	/// @brief
	TS3_SYSTEM_API_NODISCARD GfxGLSystemVersionInfo sysGLRenderContextQuerySystemVersionInfo( GfxGLRenderContextHandle pGLRenderContext );

} // namespace system
} // namespace ts3

#endif // __TS3_SYSTEM_GFX_OPENGL_H__
