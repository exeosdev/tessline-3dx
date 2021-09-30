
#include "GL_bufferObject.h"

namespace ts3::gpuapi
{

	GLBufferObject::GLBufferObject( GLuint pHandle, const GLBufferCreateInfo & pGLCreateInfo )
	: GLObject( GLObjectBaseType::Buffer, pHandle )
	, mGLBufferBindTarget( pGLCreateInfo.bindTarget )
	, mSize( pGLCreateInfo.size )
	{}

	GLBufferObject::~GLBufferObject() = default;

	GLBufferObjectHandle GLBufferObject::create( const GLBufferCreateInfo & pGLCreateInfo )
	{
		GLuint bufferHandle = 0;

		glGenBuffers( 1, &bufferHandle );
		ts3GLHandleLastError();

		glBindBuffer( pGLCreateInfo.bindTarget, bufferHandle );
		ts3GLHandleLastError();

		GLBufferObjectHandle openglBufferObject{ new GLBufferObject( bufferHandle, pGLCreateInfo ) };
		if( !openglBufferObject->initialize( pGLCreateInfo ) )
		{
			return nullptr;
		}

		return openglBufferObject;
	}

	bool GLBufferObject::release()
	{
		glDeleteBuffers( 1, &mGLHandle );
		ts3GLHandleLastError();

		return true;
	}

	bool GLBufferObject::validateHandle() const
	{
		auto isBuffer = glIsBuffer( mGLHandle );
		ts3GLHandleLastError();

		return isBuffer != GL_FALSE;
	}

	bool GLBufferObject::map( gpu_memory_size_t pOffset, memory_size_t pLength, GLenum pFlags, GLenum pActiveBindTarget )
	{
		auto bufferBindTarget = checkActiveBindTarget( pActiveBindTarget );

		void * mapPointer = nullptr;
		glGetBufferPointerv( bufferBindTarget, GL_BUFFER_MAP_POINTER, &mapPointer );
		ts3GLHandleLastError();

		if( mapPointer != nullptr )
		{
			ts3DebugInterrupt();
		}

		glMapBufferRange( bufferBindTarget, pOffset, pLength, pFlags );
		ts3GLHandleLastError();

		return queryIsMapped();
	}

	bool GLBufferObject::mapPersistent( GLenum pFlags, GLenum pActiveBindTarget )
	{
		// Unset all flags which may be redundant and/or invalid (like INVALIDATE_BUFFER_RANGE or UNSYNCHRONIZED).
		pFlags &= ( GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_COHERENT_BIT | GL_MAP_FLUSH_EXPLICIT_BIT );
		// Make sure a persistent version of the mapping will be requested.
		pFlags |= GL_MAP_PERSISTENT_BIT;

		return map( 0, mSize, pFlags );
	}

	void GLBufferObject::unmap( GLenum pActiveBindTarget )
	{
		auto bufferBindTarget = checkActiveBindTarget( pActiveBindTarget );

		glUnmapBuffer( bufferBindTarget );
		ts3GLHandleLastError();
	}

	void GLBufferObject::flushMappedRegion( gpu_memory_size_t pOffset, memory_size_t pLength, GLenum pActiveBindTarget )
	{
		auto bufferBindTarget = checkActiveBindTarget( pActiveBindTarget );

		void * mapPointer = nullptr;
		glGetBufferPointerv( bufferBindTarget, GL_BUFFER_MAP_POINTER, &mapPointer );
		ts3GLHandleLastError();

		ts3DebugAssert( mapPointer );

		glFlushMappedBufferRange( bufferBindTarget, pOffset, pLength );
		ts3GLHandleLastError();
	}

	void GLBufferObject::invalidateRegion( gpu_memory_size_t pOffset, memory_size_t pLength, GLenum pActiveBindTarget )
	{
	#if( TS3GX_GL_PLATFORM_TYPE == TS3GX_GL_PLATFORM_TYPE_ES )
		auto bufferBindTarget = checkActiveBindTarget( pActiveBindTarget );

		glBufferSubData( bufferBindTarget, pOffset, pLength, nullptr );
		ts3GLHandleLastError();
	#else
		glInvalidateBufferSubData( mGLHandle, pOffset, pLength );
		ts3GLHandleLastError();
	#endif
	}

	void GLBufferObject::updateCopy( GLBufferObject & pSource, const GPUBufferSubDataCopyDesc & pCopyDesc, GLenum pActiveBindTarget )
	{
		auto bufferBindTarget = checkActiveBindTarget( pActiveBindTarget );

		// If we copy data into the whole buffer, give driver a hint to invalidate the storage.
		// For ES, use storage orphaning since glInvalidateBuffer() is not available there.
		if( ( pCopyDesc.sourceBufferRegion.size == mSize ) || pCopyDesc.flags.isSet( E_GPU_BUFFER_DATA_COPY_FLAG_MODE_INVALIDATE_BIT ) )
		{
		#if( TS3GX_GL_PLATFORM_TYPE == TS3GX_GL_PLATFORM_TYPE_ES )
			GLint bufferUsage = 0;
			glGetBufferParameteriv( bufferBindTarget, GL_BUFFER_USAGE, &bufferUsage );
			ts3GLHandleLastError();

			glBufferData( bufferBindTarget, mSize, nullptr, bufferUsage );
			ts3GLHandleLastError();
		#else
			glInvalidateBufferData( mGLHandle );
			ts3GLHandleLastError();
		#endif
		}

		glBindBuffer( GL_COPY_READ_BUFFER, pSource.mGLHandle );
		ts3GLHandleLastError();

		glCopyBufferSubData( GL_COPY_READ_BUFFER,
		                     bufferBindTarget,
		                     pCopyDesc.sourceBufferRegion.offset,
		                     pCopyDesc.targetBufferOffset,
		                     pCopyDesc.sourceBufferRegion.size );
		ts3GLHandleLastError();
	}

	void GLBufferObject::updateUpload( const GPUBufferSubDataUploadDesc & pUploadDesc, GLenum pActiveBindTarget )
	{
		auto bufferBindTarget = checkActiveBindTarget( pActiveBindTarget );

		if( ( pUploadDesc.bufferRegion.size == mSize ) || pUploadDesc.flags.isSet( E_GPU_BUFFER_DATA_COPY_FLAG_MODE_INVALIDATE_BIT ) )
		{
		#if( TS3GX_GL_PLATFORM_TYPE == TS3GX_GL_PLATFORM_TYPE_ES )
			GLint bufferUsage = 0;
			glGetBufferParameteriv( bufferBindTarget, GL_BUFFER_USAGE, &bufferUsage );
			ts3GLHandleLastError();

			glBufferData( bufferBindTarget, mSize, nullptr, bufferUsage );
			ts3GLHandleLastError();
		#else
			glInvalidateBufferData( mGLHandle );
			ts3GLHandleLastError();
		#endif
		}

		glBufferSubData( bufferBindTarget,
		                 pUploadDesc.bufferRegion.offset,
		                 pUploadDesc.bufferRegion.size,
		                 pUploadDesc.inputDataDesc.pointer );
		ts3GLHandleLastError();
	}

	bool GLBufferObject::queryIsMapped( GLenum pActiveBindTarget ) const
	{
		auto bufferBindTarget = checkActiveBindTarget( pActiveBindTarget );

		GLint64 mappedState = 0;
		glGetBufferParameteri64v( bufferBindTarget, GL_BUFFER_MAPPED, &mappedState );
		ts3GLHandleLastError();

		return mappedState != GL_FALSE;
	}

	byte * GLBufferObject::queryMappedPtr( GLenum pActiveBindTarget ) const
	{
		auto bufferBindTarget = checkActiveBindTarget( pActiveBindTarget );

		void * mappedMemoryPtr = nullptr;
		glGetBufferPointerv( bufferBindTarget, GL_BUFFER_MAP_POINTER, &mappedMemoryPtr );
		ts3GLHandleLastError();

		return reinterpret_cast<byte *>( mappedMemoryPtr );
	}

	MemoryRegion GLBufferObject::queryMappedRegion( GLenum pActiveBindTarget ) const
	{
		auto bufferBindTarget = checkActiveBindTarget( pActiveBindTarget );

		GLint64 mapOffset = 0;
		glGetBufferParameteri64v( bufferBindTarget, GL_BUFFER_MAP_OFFSET, &mapOffset );
		ts3GLHandleLastError();

		GLint64 mapLength = 0;
		glGetBufferParameteri64v( bufferBindTarget, GL_BUFFER_MAP_LENGTH, &mapLength );
		ts3GLHandleLastError();

		MemoryRegion mappedRegion;
		mappedRegion.offset = static_cast<memory_size_t>( mapOffset );
		mappedRegion.size = static_cast<memory_size_t>( mapLength );

		return mappedRegion;
	}

	memory_size_t GLBufferObject::querySize( GLenum pActiveBindTarget ) const
	{
		auto bufferBindTarget = checkActiveBindTarget( pActiveBindTarget );

		GLint64 bufferSize = 0;
		glGetBufferParameteri64v( bufferBindTarget, GL_BUFFER_SIZE, &bufferSize );
		ts3GLHandleLastError();

		return static_cast<memory_size_t>( bufferSize );
	}

	bool GLBufferObject::initialize( const GLBufferCreateInfo & pGLCreateInfo )
	{
		bool nonEmptyInitData = pGLCreateInfo.initDataDesc ? true : false;
		bool copyDataOnInit = nonEmptyInitData && ( pGLCreateInfo.initDataDesc.size == pGLCreateInfo.size );

		const void * initDataPtr = copyDataOnInit ? pGLCreateInfo.initDataDesc.pointer : nullptr;

	#if( TS3GX_GL_PLATFORM_TYPE == TS3GX_GL_PLATFORM_TYPE_ES )
		glBufferData( pGLCreateInfo.bindTarget, pGLCreateInfo.size, initDataPtr, pGLCreateInfo.initFlags );
		ts3GLHandleLastError();
	#else
		glBufferStorage( pGLCreateInfo.bindTarget, pGLCreateInfo.size, initDataPtr, pGLCreateInfo.initFlags );
		ts3GLHandleLastError();
	#endif

		if( !copyDataOnInit && nonEmptyInitData )
		{
			auto initDataSize = getMinOf( pGLCreateInfo.size, pGLCreateInfo.initDataDesc.size );
			glBufferSubData( pGLCreateInfo.bindTarget, 0, initDataSize, pGLCreateInfo.initDataDesc.pointer );
			ts3GLHandleLastError();
		}

		return true;
	}

	GLenum GLBufferObject::checkActiveBindTarget( GLenum pBindTarget ) const
	{
		if( pBindTarget == 0 )
		{
			pBindTarget = mGLBufferBindTarget;

			glBindBuffer( pBindTarget, mGLHandle );
			ts3GLHandleLastError();
		}

		return pBindTarget;
	}

}
