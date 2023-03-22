
#include "gpuBuffer.h"

namespace ts3::gpuapi
{

	GPUBuffer::GPUBuffer(
			GPUDevice & pGPUDevice,
			const ResourceMemoryInfo & pResourceMemory,
			const GPUBufferProperties & pBufferProperties )
	: GPUResource( pGPUDevice, EGPUResourceBaseType::Buffer, pResourceMemory )
	, mBufferProperties( pBufferProperties )
	{}

	GPUBuffer::~GPUBuffer() = default;

	const GPUResourceProperties & GPUBuffer::getProperties() const
	{
		return mBufferProperties;
	}

	GPUMemoryRegion GPUBuffer::getWholeBufferRegion() const
	{
		return GPUMemoryRegion{ 0, mBufferProperties.byteSize };
	}

	bool GPUBuffer::validateMapRequest( const GPUMemoryRegion & pRegion, const EGPUMemoryMapMode & pMapMode )
	{
		if( isMapped() )
		{
			return false;
		}

		if( !memutil::checkMemoryMapAccess( pMapMode, mResourceMemory.memoryFlags ) )
		{
			return false;
		}

		const GPUMemoryRegion bufferDataRegion{ 0, mBufferProperties.byteSize };
		if( !ts3::checkRangeSubRange( bufferDataRegion.asRange(), pRegion.asRange() ) )
		{
			return false;
		}

		return true;
	}

	bool GPUBuffer::validateBufferCreateInfo( GPUBufferCreateInfo & pCreateInfo )
	{
		if( pCreateInfo.memoryBaseAlignment == 0 )
		{
		    pCreateInfo.memoryBaseAlignment = ts3::cxdefs::MEMORY_DEFAULT_ALIGNMENT;
		}

		if( ( pCreateInfo.bufferSize == 0 ) && pCreateInfo.initDataDesc )
		{
			pCreateInfo.bufferSize = pCreateInfo.initDataDesc.size;
		}

		return true;
	}

} // namespace ts3::gpuapi
