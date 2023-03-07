
#include "gpuResource.h"
#include "../gpuDevice.h"

namespace ts3::gpuapi
{

	GPUResource::GPUResource(
			GPUDevice & pGPUDevice,
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
		return _activeRefsCounter.increment();
	}

	ref_counter_value_t GPUResource::releaseActiveRef()
	{
		const auto activeRefNum = _activeRefsCounter.decrement();

		if( mGPUDevice.isResourceActiveRefsTrackingEnabled() && ( activeRefNum == 0 ) )
		{
			mGPUDevice.onGPUResourceActiveRefsZero( *this );
		}

		return activeRefNum;
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


	GPUResourceWrapper::GPUResourceWrapper(
			GPUDevice & pGPUDevice,
			EGPUResourceBaseType pInternalResourceBaseType )
	: GPUDeviceChildObject( pGPUDevice )
	, mInternalResourceBaseType( pInternalResourceBaseType )
	, _internalResource( nullptr )
	{}

	GPUResourceWrapper::~GPUResourceWrapper() = default;

	void GPUResourceWrapper::setInternalResource( GPUResource & pResource )
	{
		_internalResource = &pResource;
	}

	void GPUResourceWrapper::resetInternalResource()
	{
		_internalResource = nullptr;
	}

} // namespace ts3::gpuapi
