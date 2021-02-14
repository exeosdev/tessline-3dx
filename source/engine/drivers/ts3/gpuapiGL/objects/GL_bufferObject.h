
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
		GLenum initFlags = 0;
		GPUBufferInitDataDesc initDataDesc;
	};

	class GLBufferObject : public GLObject
	{
	public:
		GLenum const mGLBufferBindTarget = cxGLBindTargetUnknown;
		GLuint const mSize;

		GLBufferObject( GLuint pHandle, const GLBufferCreateInfo & pGLCreateInfo );
		virtual ~GLBufferObject();

		virtual bool release();
		virtual bool validateHandle() const;

		bool map( memory_offset_t pOffset, memory_size_t pLength, GLenum pFlags, GLenum pActiveBindTarget = 0 );

		bool mapPersistent( GLenum pFlags, GLenum pActiveBindTarget = 0 );

		void unmap( GLenum pActiveBindTarget = 0 );

		void flushMappedRegion( memory_offset_t pOffset, memory_size_t pLength, GLenum pActiveBindTarget = 0 );

		void invalidateRegion( memory_offset_t pOffset, memory_size_t pLength, GLenum pActiveBindTarget = 0 );

		void updateCopy( GLBufferObject & pSource, const GPUBufferSubDataCopyDesc & pCopyDesc, GLenum pActiveBindTarget = 0 );

		void updateUpload( const GPUBufferSubDataUploadDesc & pUploadDesc, GLenum pActiveBindTarget = 0 );

		bool queryIsMapped( GLenum pActiveBindTarget = 0 ) const;

		byte * queryMappedPtr( GLenum pActiveBindTarget = 0 ) const;

		MemoryRegion queryMappedRegion( GLenum pActiveBindTarget = 0 ) const;

		memory_size_t querySize( GLenum pActiveBindTarget = 0 ) const;

		static GLBufferObjectHandle create( const GLBufferCreateInfo & pGLCreateInfo );

	private:
		bool initialize( const GLBufferCreateInfo & pGLCreateInfo );

		GLenum checkActiveBindTarget( GLenum pBindTarget ) const;
	};

}

#endif // __TS3DRIVER_GPUAPI_GLCOMMON_BUFFER_OBJECT_H__
