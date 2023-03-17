
#pragma once

#ifndef __TS3DRIVER_GPUAPI_GLCOMMON_BUFFER_OBJECT_H__
#define __TS3DRIVER_GPUAPI_GLCOMMON_BUFFER_OBJECT_H__

#include "../GL_prerequisites.h"
#include <ts3/gpuapi/resources/gpuBufferCommon.h>

namespace ts3::gpuapi
{

	ts3GLDeclareOpenGLObjectHandle( GLBufferObject );

	struct GLBufferCreateInfo
	{
		GLenum bindTarget = 0;
		GLuint size = 0;
		Bitmask<resource_flags_value_t> resourceFlags = 0;
		Bitmask<EGPUMemoryFlags> memoryFlags = 0;
		GPUBufferInitDataDesc initDataDesc;
	};

	class GLBufferObject : public GLObject
	{
	public:
		GLenum const mGLBufferBindTarget = CX_GL_BIND_TARGET_UNKNOWN;
		GLuint const mSize;

		GLBufferObject( GLuint pHandle, const GLBufferCreateInfo & pGLCreateInfo );
		virtual ~GLBufferObject();

		virtual bool release();
		virtual bool validateHandle() const;

		TS3_ATTR_NO_DISCARD bool queryIsMapped( GLenum pActiveBindTarget = 0 ) const;

		TS3_ATTR_NO_DISCARD byte * queryMappedPtr( GLenum pActiveBindTarget = 0 ) const;

		TS3_ATTR_NO_DISCARD MemoryRegion queryMappedRegion( GLenum pActiveBindTarget = 0 ) const;

		TS3_ATTR_NO_DISCARD memory_size_t querySize( GLenum pActiveBindTarget = 0 ) const;

		TS3_ATTR_NO_DISCARD bool isMappedPersistent() const;

		TS3_ATTR_NO_DISCARD void * getPersistentMapPtr() const;

		bool map( gpu_memory_size_t pOffset, gpu_memory_size_t pLength, GLenum pFlags, GLenum pActiveBindTarget = 0 );
		bool mapPersistent( GLenum pFlags, GLenum pActiveBindTarget = 0 );
		void unmap( GLenum pActiveBindTarget = 0 );

		void flushMappedRegion( gpu_memory_size_t pOffset, gpu_memory_size_t pLength, GLenum pActiveBindTarget = 0 );
		void invalidateRegion( gpu_memory_size_t pOffset, gpu_memory_size_t pLength, GLenum pActiveBindTarget = 0 );

		void updateCopyInvalidate( GLBufferObject & pSrcBuffer, const GPUBufferSubDataCopyDesc & pCopyDesc, GLenum pActiveBindTarget = 0 );
		void updateCopyOrphan( GLBufferObject & pSrcBuffer, const GPUBufferSubDataCopyDesc & pCopyDesc, GLenum pActiveBindTarget = 0 );

		void updateUploadInvalidate( const GPUBufferSubDataUploadDesc & pUploadDesc, GLenum pActiveBindTarget = 0 );
		void updateUploadOrphan( const GPUBufferSubDataUploadDesc & pUploadDesc, GLenum pActiveBindTarget = 0 );

		static GLBufferObjectHandle createCore( const GLBufferCreateInfo & pGLCreateInfo );
		static GLBufferObjectHandle createCompat( const GLBufferCreateInfo & pGLCreateInfo );

	private:
		static GLBufferObjectHandle createBase( const GLBufferCreateInfo & pGLCreateInfo );

		GLenum checkActiveBindTarget( GLenum pBindTarget ) const;

		bool initializeCore( const GLBufferCreateInfo & pGLCreateInfo );

		bool initializeCompat( const GLBufferCreateInfo & pGLCreateInfo );

	private:
		void * _persistentMapPointer = nullptr;
	};

} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_GLCOMMON_BUFFER_OBJECT_H__
