
#pragma once

#ifndef __TS3DRIVER_GPUAPI_MTLCOMMON_GPU_BUFFER_H__
#define __TS3DRIVER_GPUAPI_MTLCOMMON_GPU_BUFFER_H__

#include "../MTL_prerequisites.h"
#include <ts3/gpuapi/resources/gpuBuffer.h>

namespace ts3::gpuapi
{

	ts3DeclareClassHandle( MetalGPUBuffer );

	class MetalGPUBuffer : public GPUBuffer
	{
	public:
		id<MTLDevice> const mMTLDevice;
		id<MTLBuffer> const mMTLBuffer;

	public:
		MetalGPUBuffer(
			MetalGPUDevice & pGPUDevice,
			const ResourceMemoryInfo & pResourceMemory,
			const GPUBufferProperties & pBufferProperties,
			id<MTLBuffer> pMTLBuffer );

		virtual ~MetalGPUBuffer();

		static MetalGPUBufferHandle create( MetalGPUDevice & pGPUDevice, const GPUBufferCreateInfo & pCreateInfo );

	private:
	    virtual bool mapRegion( void * pCommandObject, const GPUMemoryRegion & pRegion, EGPUMemoryMapMode pMapMode ) override;

		virtual void unmap( void * pCommandObject ) override;

		virtual void flushMappedRegion( void * pCommandObject, const GPUMemoryRegion & pRegion ) override;

		virtual void invalidateRegion( void * pCommandObject, const GPUMemoryRegion & pRegion ) override;

		virtual void updateSubDataCopy( void * pCommandObject, GPUBuffer & pSrcBuffer, const GPUBufferSubDataCopyDesc & pCopyDesc ) override;

		virtual void updateSubDataUpload( void * pCommandObject, const GPUBufferSubDataUploadDesc & pUploadDesc ) override;
		
		virtual bool validateMapRequest( const GPUMemoryRegion & pRegion, const EGPUMemoryMapMode & pMapMode ) override;

		static id<MTLCommandBuffer> getMTLCommandBuffer( void * pCommandObject );

		static bool checkCommandListTransfer( void * pCommandObject );
	};

} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_MTLCOMMON_GPU_BUFFER_H__
