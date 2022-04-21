
#pragma once

#ifndef __TS3DRIVER_GPUAPI_VKCOMMON_PRESENTATION_LAYER_H__
#define __TS3DRIVER_GPUAPI_VKCOMMON_PRESENTATION_LAYER_H__

#include "VK_prerequisites.h"
#include <ts3/gpuapi/presentationLayer.h>

namespace ts3::gpuapi
{

	ts3DeclareClassHandle( VKPresentationLayer );

	class TS3GX_VKCOMMON_CLASS VKPresentationLayer : public PresentationLayer
	{
	public:
		VKPresentationLayer( GPUDevice & pDevice );
		virtual ~VKPresentationLayer();
	};

	class TS3GX_VKCOMMON_CLASS VKScreenPresentationLayer : public VKPresentationLayer
	{
	public:
		VKScreenPresentationLayer( GPUDevice & pDevice );
		virtual ~VKScreenPresentationLayer();

		virtual void bindRenderTarget( CommandContext * pCmdContext ) override;

		virtual void invalidateRenderTarget( CommandContext * pCmdContext ) override;

		virtual void present() override;

		virtual void resize( uint32 pWidth, uint32 pHeight ) override;

		virtual void setFullscreenMode( bool pEnable ) override;

		virtual ts3::math::Vec2u32 queryRenderTargetSize() const override;
	};

} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_VKCOMMON_PRESENTATION_LAYER_H__
