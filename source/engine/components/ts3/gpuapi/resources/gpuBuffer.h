
#pragma once

#ifndef __TS3_GPUAPI_GPU_BUFFER_H__
#define __TS3_GPUAPI_GPU_BUFFER_H__

#include "gpuResource.h"
#include "gpuBufferCommon.h"

namespace ts3::gpuapi
{

	struct GPUBufferProperties : public GPUResourceProperties
	{
		memory_size_t byteSize;
	};

	class GPUBuffer : public GPUResource
	{
		friend class CommandList;

	public:
		GPUBufferProperties const mBufferProperties;

	public:
		GPUBuffer( GPUDevice & pGPUDevice,
		           const ResourceMemoryInfo & pResourceMemory,
		           const GPUBufferProperties & pBufferProperties );

		virtual ~GPUBuffer();

		bool checkBufferTargetSupport( EGPUBufferTarget pBufferTarget ) const;

	protected:
		virtual bool mapRegion( void * pCommandObject, const MemoryRegion & pRegion, EMemoryMapMode pMapMode ) = 0;

		virtual void unmap( void * pCommandObject ) = 0;

		virtual void flushMappedRegion( void * pCommandObject, const MemoryRegion & pRegion ) = 0;

		virtual void invalidateRegion( void * pCommandObject, const MemoryRegion & pRegion ) = 0;

		virtual void updateSubDataCopy( void * pCommandObject, GPUBuffer & pSource, const GPUBufferSubDataCopyDesc & pCopyDesc ) = 0;

		virtual void updateSubDataUpload( void * pCommandObject, const GPUBufferSubDataUploadDesc & pUploadDesc ) = 0;

		virtual bool validateMapRequest( const MemoryRegion & pRegion, const EMemoryMapMode & pMapMode );

		static bool validateBufferCreateInfo( GPUBufferCreateInfo & pCreateInfo );
	};

}

#endif // __TS3_GPUAPI_GPU_BUFFER_H__
