
#include "presentationLayer.h"
#include "gpuDevice.h"

namespace ts3
{
namespace gpuapi
{

	PresentationLayer::PresentationLayer( GPUDevice & pGPUDevice )
	: GPUDriverChildObject( pGPUDevice.mGPUDriver )
	, mGPUDevice( pGPUDevice )
	{}

	PresentationLayer::~PresentationLayer() = default;

	system::EventSource * PresentationLayer::getInternalSystemEventSource() const noexcept
	{
		return nullptr;
	}

} /* namespace ts3 */
} /* namespace gpuapi */
