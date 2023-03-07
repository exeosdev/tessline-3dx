
#pragma once

#ifndef __TS3_GPUAPI_GPU_RESOURCE_H__
#define __TS3_GPUAPI_GPU_RESOURCE_H__

#include "commonGPUResourceDefs.h"
#include <ts3/stdext/refCounter.h>

namespace ts3::gpuapi
{

	struct GPUResourceProperties
	{
		Bitmask<resource_flags_value_t> resourceFlags;
	};

	class GPUResource : public GPUDeviceChildObject
	{
	public:
		EGPUResourceBaseType const mResourceBaseType;
		ResourceMemoryInfo const mResourceMemory;

		GPUResource(
			GPUDevice & pGPUDevice,
			EGPUResourceBaseType pResourceBaseType,
			const ResourceMemoryInfo & pResourceMemory );

		virtual ~GPUResource();

		TS3_ATTR_NO_DISCARD ref_counter_value_t getActiveRefsNum() const noexcept;

		TS3_ATTR_NO_DISCARD const ResourceMappedMemory & getMappedMemory() const noexcept;

		TS3_ATTR_NO_DISCARD bool isMapped() const;

		TS3_ATTR_NO_DISCARD bool isMapped( const GPUMemoryRegion & pRegion ) const;

		TS3_ATTR_NO_DISCARD virtual const GPUResourceProperties & getProperties() const = 0;

	protected:
		ref_counter_value_t addActiveRef();

		ref_counter_value_t releaseActiveRef();

		void setMappedMemory( const ResourceMappedMemory & pMappedMemory );

		void resetMappedMemory();

	private:
		AtomicRefCounter _activeRefsCounter;
		ResourceMappedMemory _mappedMemory;
	};

	inline ref_counter_value_t GPUResource::getActiveRefsNum() const noexcept
	{
		return _activeRefsCounter.getValue();
	}

	inline const ResourceMappedMemory & GPUResource::getMappedMemory() const noexcept
	{
		return _mappedMemory;
	}

	class GPUResourceWrapper : public GPUDeviceChildObject
	{
	public:
		EGPUResourceBaseType const mInternalResourceBaseType;

		GPUResourceWrapper(
			GPUDevice & pGPUDevice,
			EGPUResourceBaseType pInternalResourceBaseType );

		virtual ~GPUResourceWrapper();

		TS3_ATTR_NO_DISCARD GPUResource * getInternalResource() const noexcept;

	protected:
		void setInternalResource( GPUResource & pResource );
		void resetInternalResource();

	private:
		GPUResource * _internalResource;
	};

	inline GPUResource * GPUResourceWrapper::getInternalResource() const noexcept
	{
		return _internalResource;
	}

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_GPU_RESOURCE_H__
