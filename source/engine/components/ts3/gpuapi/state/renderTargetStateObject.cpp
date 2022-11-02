
#include "renderTargetStateObject.h"
#include <ts3/gpuapi/resources/renderBuffer.h>
#include <ts3/gpuapi/resources/texture.h>

namespace ts3::GpuAPI
{

	RenderTargetStateObject::RenderTargetStateObject( GPUDevice & pGPUDevice,
	                                                  const RenderTargetLayout & pRTLayout,
	                                                  const RenderTargetResourceBinding & pRTResourceBinding )
	: GPUStateObject( pGPUDevice )
	, mRTLayout( pRTLayout )
	, mRTResourceBinding( pRTResourceBinding )
	{}

	RenderTargetStateObject::~RenderTargetStateObject() = default;

} // namespace ts3::GpuAPI
