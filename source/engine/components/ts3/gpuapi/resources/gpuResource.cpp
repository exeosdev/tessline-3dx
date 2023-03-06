
#include "gpuResource.h"
#include "../gpuDevice.h"

namespace ts3::gpuapi
{

	GPUResource::GPUResource( GPUDevice & pGPUDevice,
	                          EGPUResourceBaseType pResourceBaseType,
	                          const ResourceMemoryInfo & pResourceMemory )
	: GPUDeviceChildObject( pGPUDevice )
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

	ref_counter_value_t GPUResource::addActiveRef()
	{
		_activeRefsCounter.increment();
	}

	ref_counter_value_t GPUResource::releaseActiveRef()
	{
		const auto activeRefNum = _activeRefsCounter.decrement();

		if( mGPUDevice.isResourceActiveRefsTrackingEnabled() && ( activeRefNum == 0 ) )
		{
			mGPUDevice.onGPUResourceActiveRefsZero( *this );
		}
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

} // namespace ts3::gpuapi
