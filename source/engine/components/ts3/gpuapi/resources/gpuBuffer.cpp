
#include "gpuBuffer.h"

namespace ts3::gpuapi
{

	EGPUBufferTarget getBufferTargetFromResourceFlags( const Bitmask<resource_flags_value_t> & pBufferResourceFlags );

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

	bool GPUBuffer::checkBufferTargetSupport( EGPUBufferTarget pBufferTarget ) const
	{
		return mBufferProperties.resourceFlags.isSet( pBufferTarget );
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

		if( !GpuMem::checkMemoryMapAccess( pMapMode, mResourceMemory.memoryFlags ) )
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
		if( pCreateInfo.initialTarget != EGPUBufferTarget::Unknown )
		{
			// If the user has specified explicit initial target for the buffer, we need to verify
			// if the buffer description allows for that target in the first place.
			if( !pCreateInfo.resourceFlags.isSet( pCreateInfo.initialTarget ) )
			{
				pCreateInfo.resourceFlags.set( pCreateInfo.initialTarget );
			}
		}
		else
		{
			auto initialTarget = getBufferTargetFromResourceFlags( pCreateInfo.resourceFlags );
			if( initialTarget == EGPUBufferTarget::Unknown )
			{
				return false;
			}
			pCreateInfo.initialTarget = initialTarget;
		}

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


	EGPUBufferTarget getBufferTargetFromResourceFlags( const Bitmask<resource_flags_value_t> & pBufferResourceFlags )
	{
		static const EGPUBufferTarget bufferTargetArray[] =
		{
			EGPUBufferTarget::ConstantBuffer,
			EGPUBufferTarget::VertexBuffer,
			EGPUBufferTarget::IndexBuffer,
			EGPUBufferTarget::StreamOutputBuffer,
			EGPUBufferTarget::ShaderInputBuffer,
			EGPUBufferTarget::ShaderUAVBuffer,
			EGPUBufferTarget::IndirectDispatchBuffer,
			EGPUBufferTarget::IndirectDrawBuffer,
			EGPUBufferTarget::TransferSourceBuffer,
			EGPUBufferTarget::TransferTargetBuffer,
		};

		for( auto bufferTarget : bufferTargetArray )
		{
			auto bufferTargetResourceFlags = static_cast<resource_flags_value_t>( bufferTarget );
			if( pBufferResourceFlags.isSet( bufferTargetResourceFlags ) )
			{
				return bufferTarget;
			}
		}

		return EGPUBufferTarget::Unknown;
	}

} // namespace ts3::gpuapi
