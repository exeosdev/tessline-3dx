
#pragma once

#ifndef __TS3DRIVER_GPUAPI_MTLCOMMON_PRESENTATION_LAYER_H__
#define __TS3DRIVER_GPUAPI_MTLCOMMON_PRESENTATION_LAYER_H__

#include "MTL_prerequisites.h"
#include <ts3/gpuapi/presentationLayer.h>

namespace ts3::gpuapi
{

	ts3DeclareClassHandle( MetalPresentationLayer );
	ts3DeclareClassHandle( MetalScreenPresentationLayer );

	struct MetalPresentationLayerCreateInfo : public PresentationLayerCreateInfo
	{
	};

	/// @brief
	class MetalPresentationLayer : public PresentationLayer
	{
	public:
		system::MetalDisplaySurfaceHandle const mSysMetalDisplaySurface;

	public:
		MetalPresentationLayer( MetalGPUDevice & pGPUDevice, system::MetalDisplaySurfaceHandle pSysMetalDisplaySurface );
		virtual ~MetalPresentationLayer();

		virtual system::EventSource * getInternalSystemEventSource() const noexcept override;
	};

	class GLScreenPresentationLayer final : public MetalPresentationLayer
	{
	public:
		GLScreenPresentationLayer( MetalGPUDevice & pGPUDevice, system::MetalDisplaySurfaceHandle pSysMetalDisplaySurface );
		virtual ~GLScreenPresentationLayer();

		virtual void bindRenderTarget( CommandContext * pCmdContext ) override;

		virtual void invalidateRenderTarget( CommandContext * pCmdContext ) override;

		virtual void present() override;

		virtual void resize( uint32 pWidth, uint32 pHeight ) override;

		virtual void setFullscreenMode( bool pEnable ) override;

		virtual ts3::math::Vec2u32 queryRenderTargetSize() const override;

		/// @brief Creates new swap chain using provided create params.
		static GLScreenPresentationLayerHandle create( GLGPUDevice & pDevice, const MetalPresentationLayerCreateInfo & pCreateInfo );
	};

} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_MTLCOMMON_PRESENTATION_LAYER_H__