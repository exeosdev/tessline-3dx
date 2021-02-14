
#pragma once

#ifndef __TS3_GPUAPI_GPU_RESOURCE_H__
#define __TS3_GPUAPI_GPU_RESOURCE_H__

#include "commonGPUResourceDefs.h"

namespace ts3::gpuapi
{

	struct GPUResourceProperties
	{
		Bitmask<gpu_resource_flags_value_t> resourceFlags;
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

		bool isMapped() const;
		bool isMapped( const MemoryRegion & pRegion ) const;

		const ResourceMappedMemory & getMappedMemory() const;

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

}

#endif // __TS3_GPUAPI_GPU_RESOURCE_H__
