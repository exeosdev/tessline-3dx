
#include "gpuResource.h"

namespace ts3::GpuAPI
{

	GPUResource::GPUResource( GPUDevice & pGPUDevice,
	                          EGPUResourceBaseType pResourceBaseType,
	                          const ResourceMemoryInfo & pResourceMemory )
	: GPUBaseObject( pGPUDevice )
	, mResourceBaseType( pResourceBaseType )
	, mResourceMemory( pResourceMemory )
	{}

	GPUResource::~GPUResource() = default;

	bool GPUResource::isMapped() const
	{
		return ( _mappedMemory.pointer != nullptr ) && !_mappedMemory.mappedRegion.empty();
	}

	bool GPUResource::isMapped( const GPUMemoryRegion & pRegion ) const
	{
		return ts3::checkRangeSubRange( _mappedMemory.mappedRegion.asRange(), pRegion.asRange() );
	}

	void GPUResource::setMappedMemory( const ResourceMappedMemory & pMappedMemory )
	{
		_mappedMemory = pMappedMemory;
		_mappedMemory.sourceMemory = &mResourceMemory;
	}

	void GPUResource::resetMappedMemory()
	{
		_mappedMemory.pointer = nullptr;
		_mappedMemory.memoryMapFlags = 0;
		_mappedMemory.mappedRegion.reset();
	}

} // namespace ts3::GpuAPI
