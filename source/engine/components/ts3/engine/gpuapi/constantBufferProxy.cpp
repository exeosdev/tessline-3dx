
#include "constantBufferProxy.h"
#include <ts3/gpuapi/resources/gpuBuffer.h>

namespace ts3
{

	ConstantBufferProxy::ConstantBufferProxy( gpuapi::GPUBufferHandle pBuffer )
	: mGPUDevice( pBuffer->mGPUDevice )
	{}

	ConstantBufferProxy::~ConstantBufferProxy() = default;

}
