
#pragma once

#ifndef __TS3_GPUAPI_GPU_BUFFER_H__
#define __TS3_GPUAPI_GPU_BUFFER_H__

#include "gpuResource.h"
#include "gpuBufferCommon.h"

namespace ts3::GpuAPI
{

	struct GPUBufferProperties : public GPUResourceProperties
	{
		gpu_memory_size_t byteSize;
	};

	class GPUBuffer : public GPUResource
	{
		friend class CommandList;

	public:
		GPUBufferProperties const mBufferProperties;

	public:
		GPUBuffer(
			GPUDevice & pGPUDevice,
			const ResourceMemoryInfo & pResourceMemory,
			const GPUBufferProperties & pBufferProperties );

		virtual ~GPUBuffer();

		TS3_ATTR_NO_DISCARD virtual const GPUResourceProperties & getProperties() const override final;

		TS3_ATTR_NO_DISCARD bool checkBufferTargetSupport( EGPUBufferTarget pBufferTarget ) const;

	protected:
		virtual bool mapRegion( void * pCommandObject, const GPUMemoryRegion & pRegion, EGPUMemoryMapMode pMapMode ) = 0;

		virtual void unmap( void * pCommandObject ) = 0;

		virtual void flushMappedRegion( void * pCommandObject, const GPUMemoryRegion & pRegion ) = 0;

		virtual void invalidateRegion( void * pCommandObject, const GPUMemoryRegion & pRegion ) = 0;

		virtual void updateSubDataCopy( void * pCommandObject, GPUBuffer & pSource, const GPUBufferSubDataCopyDesc & pCopyDesc ) = 0;

		virtual void updateSubDataUpload( void * pCommandObject, const GPUBufferSubDataUploadDesc & pUploadDesc ) = 0;

		virtual bool validateMapRequest( const GPUMemoryRegion & pRegion, const EGPUMemoryMapMode & pMapMode );

		static bool validateBufferCreateInfo( GPUBufferCreateInfo & pCreateInfo );
	};

} // namespace ts3::GpuAPI

#endif // __TS3_GPUAPI_GPU_BUFFER_H__
