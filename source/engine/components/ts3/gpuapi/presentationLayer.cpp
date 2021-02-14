
#include "presentationLayer.h"
#include "gpuDevice.h"

namespace ts3::gpuapi
{

	PresentationLayer::PresentationLayer( GPUDevice & pGPUDevice )
	: GPUDriverChildObject( pGPUDevice.mGPUDriver )
	, mGPUDevice( pGPUDevice )
	{}

	PresentationLayer::~PresentationLayer() = default;

	SysEventSource * PresentationLayer::querySysEventSourceObject() const noexcept
	{
		return nullptr;
	}

}
