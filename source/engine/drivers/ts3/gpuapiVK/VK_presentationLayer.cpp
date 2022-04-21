
#include "VK_presentationLayer.h"
#include <ts3/gpuapiVK/VK_gpuDevice.h>
#include <ts3/gpuapiVK/VK_commandList.h>
#include <ts3/gpuapiVK/VK_commandSystem.h>

namespace ts3::gpuapi
{

	VKPresentationLayer::VKPresentationLayer( GPUDevice & pDevice )
	: PresentationLayer( pDevice )
	{}

	VKPresentationLayer::~VKPresentationLayer() = default;


	VKScreenPresentationLayer::VKScreenPresentationLayer( GPUDevice & pDevice )
	: VKPresentationLayer( pDevice )
	{}

	VKScreenPresentationLayer::~VKScreenPresentationLayer() = default;

	void VKScreenPresentationLayer::bindRenderTarget( CommandContext * pCmdContext )
	{
	}

	void VKScreenPresentationLayer::invalidateRenderTarget( CommandContext * pCmdContext )
	{
	}

	void VKScreenPresentationLayer::present()
	{
	}

	void VKScreenPresentationLayer::resize( uint32 pWidth, uint32 pHeight )
	{
	}

	void VKScreenPresentationLayer::setFullscreenMode( bool pEnable )
	{
	}

	ts3::math::Vec2u32 VKScreenPresentationLayer::queryRenderTargetSize() const
	{
		return {};
	}
	
} // namespace ts3::gpuapi
