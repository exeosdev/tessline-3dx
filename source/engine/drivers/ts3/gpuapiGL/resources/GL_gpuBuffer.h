
#pragma once

#ifndef __TS3DRIVER_GPUAPI_GLCOMMON_GPU_BUFFER_H__
#define __TS3DRIVER_GPUAPI_GLCOMMON_GPU_BUFFER_H__

#include "../objects/GL_bufferObject.h"
#include <ts3/gpuapi/resources/gpuBuffer.h>

namespace ts3::gpuapi
{

	ts3DeclareClassHandle( GLGPUBuffer );

	class GLGPUBuffer : public GPUBuffer
	{
		friend class GLCommandContext;

	public:
		GLBufferObjectHandle const mGLBufferObject;

	public:
		GLGPUBuffer(
			GLGPUDevice & pGPUDevice,
			const ResourceMemoryInfo & pResourceMemory,
			const GPUBufferProperties & pBufferProperties,
			GLBufferObjectHandle pGLBufferObject );

		virtual ~GLGPUBuffer();

		static GLGPUBufferHandle createInstance( GLGPUDevice & pGPUDevice, const GPUBufferCreateInfo & pCreateInfo );

	protected:
		static bool validateBufferCreateInfo( GPUBufferCreateInfo & pCreateInfo );

		virtual bool mapRegion( void * pCommandObject, const GPUMemoryRegion & pRegion, EGPUMemoryMapMode pMapMode ) override;

		virtual void unmap( void * pCommandObject ) override;

		virtual void flushMappedRegion( void * pCommandObject, const GPUMemoryRegion & pRegion ) override;

		virtual void invalidateRegion( void * pCommandObject, const GPUMemoryRegion & pRegion ) override;

		virtual void updateSubDataCopy( void * pCommandObject, GPUBuffer & pSrcBuffer, const GPUBufferSubDataCopyDesc & pCopyDesc ) override;

		virtual void updateSubDataUpload( void * pCommandObject, const GPUBufferSubDataUploadDesc & pUploadDesc ) override;
		
		virtual bool validateMapRequest( const GPUMemoryRegion & pRegion, const EGPUMemoryMapMode & pMapMode ) override;
	};

} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_GLCOMMON_GPU_BUFFER_H__
