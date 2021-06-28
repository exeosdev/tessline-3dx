
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
		GLBufferObjectHandle const mGLBufferObject = nullptr;
		void * const mPersistentMapPtr = nullptr;

	public:
		GLGPUBuffer( GLGPUDevice & pGLGPUDevice,
		             const ResourceMemoryInfo & pResourceMemory,
		             const GPUBufferProperties & pBufferProperties,
		             GLBufferObjectHandle pGLBufferObject,
		             void * pPersistentMapPtr );

		virtual ~GLGPUBuffer();

		static GLGPUBufferHandle create( GLGPUDevice & pGLGPUDevice, const GPUBufferCreateInfo & pCreateInfo );

	private:
		virtual bool mapRegion( void * pCommandObject, const MemoryRegion & pRegion, EGPUMemoryMapMode pMapMode ) override;

		virtual void unmap( void * pCommandObject ) override;

		virtual void flushMappedRegion( void * pCommandObject, const MemoryRegion & pRegion ) override;

		virtual void invalidateRegion( void * pCommandObject, const MemoryRegion & pRegion ) override;

		virtual void updateSubDataCopy( void * pCommandObject, GPUBuffer & pSource, const GPUBufferSubDataCopyDesc & pCopyDesc ) override;

		virtual void updateSubDataUpload( void * pCommandObject, const GPUBufferSubDataUploadDesc & pUploadDesc ) override;
		
		virtual bool validateMapRequest( const MemoryRegion & pRegion, const EGPUMemoryMapMode & pMapMode ) override;

		static bool validateBufferCreateInfo( GPUBufferCreateInfo & pCreateInfo );
	};

}

#endif // __TS3DRIVER_GPUAPI_GLCOMMON_GPU_BUFFER_H__
