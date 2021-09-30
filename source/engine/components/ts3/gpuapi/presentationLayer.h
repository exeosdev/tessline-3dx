
#pragma once

#ifndef __TS3_GPUAPI_PRESENTATION_LAYER_H__
#define __TS3_GPUAPI_PRESENTATION_LAYER_H__

#include "displayCommon.h"
#include <ts3/system/displayCommon.h>
#include <ts3/system/eventCommon.h>
#include <ts3/system/windowCommon.h>

namespace ts3::gpuapi
{

	struct PresentationLayerCreateInfo
	{
		system::DisplayManagerHandle sysDisplayManager = nullptr;
		system::WindowManagerHandle sysWindowManager = nullptr;
		system::VisualConfig visualConfig = system::vsxGetDefaultVisualConfigForSysWindow();
		ScreenRect screenRect;
		Bitmask<EDisplayConfigurationFlags> displayConfigFlags = 0u;
	};

	/// @brief
	class TS3_GPUAPI_CLASS PresentationLayer : public GPUDriverChildObject
	{
	public:
		GPUDevice & mGPUDevice;

		explicit PresentationLayer( GPUDevice & pGPUDevice );
		virtual ~PresentationLayer();

		virtual system::EventSource * getInternalSystemEventSource() const noexcept;

		virtual void bindRenderTarget( CommandContext * pCmdContext ) = 0;

		virtual void invalidateRenderTarget( CommandContext * pCmdContext ) = 0;

		virtual void present() = 0;

		virtual void resize( uint32 pWidth, uint32 pHeight ) = 0;

		virtual void setFullscreenMode( bool pEnable ) = 0;

		virtual ts3::math::Vec2u32 queryRenderTargetSize() const = 0;
	};

}

#endif // __TS3_GPUAPI_PRESENTATION_LAYER_H__
