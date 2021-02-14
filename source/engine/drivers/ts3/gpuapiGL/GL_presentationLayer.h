
#pragma once

#ifndef __TS3DRIVER_GPUAPI_GLCOMMON_PRESENTATION_LAYER_H__
#define __TS3DRIVER_GPUAPI_GLCOMMON_PRESENTATION_LAYER_H__

#include "GL_prerequisites.h"
#include <ts3/gpuapi/presentationLayer.h>

namespace ts3::gpuapi
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
		SysGLSurfaceHandle const mSysGLSurface;

	public:
		GLPresentationLayer( GLGPUDevice & pDevice, SysGLSurfaceHandle pSysGLSurface );
		virtual ~GLPresentationLayer();

		virtual SysEventSource * querySysEventSourceObject() const noexcept override;
	};

	class GLScreenPresentationLayer final : public GLPresentationLayer
	{
	public:
		GLScreenPresentationLayer( GLGPUDevice & pDevice, SysGLSurfaceHandle pSysGLSurface );
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

}

#endif // __TS3DRIVER_GPUAPI_GLCOMMON_PRESENTATION_LAYER_H__
