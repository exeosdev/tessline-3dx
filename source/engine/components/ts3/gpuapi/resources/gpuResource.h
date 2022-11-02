
#pragma once

#ifndef __TS3_GPUAPI_GPU_RESOURCE_H__
#define __TS3_GPUAPI_GPU_RESOURCE_H__

#include "commonGPUResourceDefs.h"

namespace ts3::GpuAPI
{

	struct GPUResourceProperties
	{
		Bitmask<resource_flags_value_t> resourceFlags;
	};

	class GPUResource : public GPUBaseObject
	{
	public:
		EGPUResourceBaseType const mResourceBaseType;
		ResourceMemoryInfo const mResourceMemory;

		GPUResource( GPUDevice & pGPUDevice,
		             EGPUResourceBaseType pResourceBaseType,
		             const ResourceMemoryInfo & pResourceMemory );

		virtual ~GPUResource();

		TS3_ATTR_NO_DISCARD virtual const GPUResourceProperties & getProperties() const = 0;

		TS3_ATTR_NO_DISCARD bool isMapped() const;
		TS3_ATTR_NO_DISCARD bool isMapped( const GPUMemoryRegion & pRegion ) const;

		TS3_ATTR_NO_DISCARD const ResourceMappedMemory & getMappedMemory() const;

	protected:
		void setMappedMemory( const ResourceMappedMemory & pMappedMemory );
		void resetMappedMemory();

	private:
		ResourceMappedMemory _mappedMemory;
	};

	inline const ResourceMappedMemory & GPUResource::getMappedMemory() const
	{
		return _mappedMemory;
	}

} // namespace ts3::GpuAPI

#endif // __TS3_GPUAPI_GPU_RESOURCE_H__
