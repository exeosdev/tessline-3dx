
#pragma once

#ifndef __TS3DRIVER_GPUAPI_GLCOMMON_PRESENTATION_LAYER_H__
#define __TS3DRIVER_GPUAPI_GLCOMMON_PRESENTATION_LAYER_H__

#include "GL_prerequisites.h"
#include <ts3/gpuapi/presentationLayer.h>

namespace ts3
{
namespace gpuapi
{

	ts3DeclareClassHandle( GLPresentationLayer );
	ts3DeclareClassHandle( GLScreenPresentationLayer );

	struct GLPresentationLayerCreateInfo : public PresentationLayerCreateInfo
	{
	};

	/// @brief
	class TS3GX_GL_CLASS GLPresentationLayer : public PresentationLayer
	{
	public:
		system::OpenGLDisplaySurfaceHandle const mSysGLDisplaySurface;

	public:
	    GLPresentationLayer( GLGPUDevice & pDevice, system::OpenGLDisplaySurfaceHandle pSysGLDisplaySurface );
		virtual ~GLPresentationLayer();

		virtual system::EventSource * getInternalSystemEventSource() const noexcept override;
	};

	class GLScreenPresentationLayer final : public GLPresentationLayer
	{
	public:
	    GLScreenPresentationLayer( GLGPUDevice & pDevice, system::OpenGLDisplaySurfaceHandle pSysGLDisplaySurface );
		virtual ~GLScreenPresentationLayer();

		virtual void bindRenderTarget( CommandContext * pCmdContext ) override;

		virtual void invalidateRenderTarget( CommandContext * pCmdContext ) override;

		virtual void present() override;

		virtual void resize( uint32 pWidth, uint32 pHeight ) override;

		virtual void setFullscreenMode( bool pEnable ) override;

		virtual ts3::math::Vec2u32 queryRenderTargetSize() const override;

		/// @brief Creates new swap chain using provided create params.
		static GLScreenPresentationLayerHandle create( GLGPUDevice & pDevice, const GLPresentationLayerCreateInfo & pCreateInfo );
	};

} /* namespace ts3 */
} /* namespace gpuapi */

#endif // __TS3DRIVER_GPUAPI_GLCOMMON_PRESENTATION_LAYER_H__
