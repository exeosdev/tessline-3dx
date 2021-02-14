
#pragma once

#ifndef __TS3_GPUAPI_PRESENTATION_LAYER_H__
#define __TS3_GPUAPI_PRESENTATION_LAYER_H__

#include "displayCommon.h"
#include <ts3/system/displayCommon.h>
#include <ts3/system/eventDefs.h>
#include <ts3/system/windowDefs.h>

namespace ts3::gpuapi
{

	struct PresentationLayerCreateInfo
	{
		SysDisplayManagerHandle sysDisplayManager = nullptr;
		SysWindowManagerHandle sysWindowManager = nullptr;
		ScreenRect screenRect;
		ts3::SysVisualConfig visualConfig;
		Bitmask<EDisplayConfigurationFlags> displayConfigFlags = 0u;
	};

	/// @brief
	class TS3_GPUAPI_CLASS PresentationLayer : public GPUDriverChildObject
	{
	public:
		GPUDevice & mGPUDevice;

		explicit PresentationLayer( GPUDevice & pGPUDevice );
		virtual ~PresentationLayer();

		virtual SysEventSource * querySysEventSourceObject() const noexcept;

		virtual void bindRenderTarget( CommandContext * pCmdContext ) = 0;

		virtual void invalidateRenderTarget( CommandContext * pCmdContext ) = 0;

		virtual void present() = 0;

		virtual void resize( uint32 pWidth, uint32 pHeight ) = 0;

		virtual void setFullscreenMode( bool pEnable ) = 0;

		virtual ts3::math::Vec2u32 queryRenderTargetSize() const = 0;
	};

}

#endif // __TS3_GPUAPI_PRESENTATION_LAYER_H__
