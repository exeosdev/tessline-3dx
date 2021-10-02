
#include "GL_gpuBuffer.h"
#include "../GL_coreAPIProxy.h"
#include "../GL_gpuDevice.h"

namespace ts3::gpuapi
{

#if( TS3GX_GL_PLATFORM_TYPE == TS3GX_GL_PLATFORM_TYPE_ES )
	// OpenGL ES 3.1 exposes neither the immutable storage API (glBufferStorage), nor the persistent mapping
	// (as a natural consequence of the former). Thus, explicit coherency/persistence flags are not used.
	static constexpr uint32 sSupportedEGPUMemoryFlags = E_GPU_MEMORY_ACCESS_MASK_CPU_READ_WRITE | E_GPU_MEMORY_ACCESS_MASK_GPU_READ_WRITE;
#else
	// Core supports full set of features, including immutable storage, persistent mapping and explicit flushes.
	static constexpr uint32 sSupportedEGPUMemoryFlags = E_GPU_MEMORY_ACCESS_MASK_CPU_READ_WRITE |
	                                                E_GPU_MEMORY_ACCESS_MASK_GPU_READ_WRITE |
	                                                E_GPU_MEMORY_HEAP_PROPERTY_FLAG_CPU_COHERENT_BIT |
	                                                E_GPU_MEMORY_HEAP_PROPERTY_FLAG_GPU_COHERENT_BIT |
	                                                E_GPU_MEMORY_HEAP_PROPERTY_FLAG_PERSISTENT_MAP_BIT;
#endif

	GLGPUBuffer::GLGPUBuffer( GLGPUDevice & pGLGPUDevice,
	                          const ResourceMemoryInfo & pResourceMemory,
	                          const GPUBufferProperties & pBufferProperties,
	                          GLBufferObjectHandle pGLBufferObject,
	                          void * pPersistentMapPtr )
	: GPUBuffer( pGLGPUDevice, pResourceMemory, pBufferProperties )
	, mGLBufferObject( std::move( pGLBufferObject ) )
	, mPersistentMapPtr( pPersistentMapPtr )
	{ }

	GLGPUBuffer::~GLGPUBuffer() = default;

	GLGPUBufferHandle GLGPUBuffer::create( GLGPUDevice & pGLGPUDevice, const GPUBufferCreateInfo & pCreateInfo )
	{
		auto createInfo = pCreateInfo;
		if( !validateBufferCreateInfo( createInfo ) )
		{
			return nullptr;
		}

		GLBufferCreateInfo openglCreateInfo;
		openglCreateInfo.bindTarget = GLCoreAPIProxy::translateGLBufferBindTarget( createInfo.initialTarget );
		openglCreateInfo.size = static_cast<GLuint>( createInfo.bufferSize );
		openglCreateInfo.initFlags = GLCoreAPIProxy::chooseGLBufferInitFlagsCoreES( openglCreateInfo.bindTarget, createInfo.resourceFlags, createInfo.memoryFlags );
		openglCreateInfo.initDataDesc = pCreateInfo.initDataDesc;

		auto openglBufferObject = GLBufferObject::create( openglCreateInfo );
		ts3DebugAssert( openglBufferObject );

		void * persistentMapPtr = nullptr;
		// Since flags are ANDed with the mask of supported bits, this should never get executed for the ES.
		if( createInfo.memoryFlags.isSet( E_GPU_MEMORY_HEAP_PROPERTY_FLAG_PERSISTENT_MAP_BIT ) )
		{
			// Map with the access specified for the buffer storage.
			auto mapMode = static_cast<EGPUMemoryMapMode>( static_cast<uint32>( createInfo.memoryFlags & E_GPU_MEMORY_ACCESS_MASK_CPU_ALL ) );
			auto openglMapFlags = GLCoreAPIProxy::translateGLBufferMapFlags( mapMode, createInfo.memoryFlags );
			if( !openglBufferObject->mapPersistent( openglMapFlags ) )
			{
				return nullptr;
			}
			persistentMapPtr = openglBufferObject->queryMappedPtr();
		}

		GPUBufferProperties bufferProperties;
		bufferProperties.byteSize = openglBufferObject->querySize();
		bufferProperties.resourceFlags = createInfo.resourceFlags;

		ResourceMemoryInfo bufferMemoryInfo;
		bufferMemoryInfo.sourceHeapRegion.offset = 0;
		bufferMemoryInfo.sourceHeapRegion.size = bufferProperties.byteSize;
		bufferMemoryInfo.baseAlignment = createInfo.memoryBaseAlignment;
		bufferMemoryInfo.memoryFlags = createInfo.memoryFlags;

		auto openglBuffer = createDynamicInterfaceObject<GLGPUBuffer>( pGLGPUDevice,
		                                                               bufferMemoryInfo,
		                                                               bufferProperties,
		                                                               std::move( openglBufferObject ),
		                                                               persistentMapPtr );

		return openglBuffer;
	}

	bool GLGPUBuffer::mapRegion( void * pCommandObject, const GPUMemoryRegion & pRegion, EGPUMemoryMapMode pMapMode )
	{
		void * mappedMemoryPtr = nullptr;

	#if( TS3GX_GL_FEATURE_SUPPORT_BUFFER_PERSISTENT_MAP )
		if( mPersistentMapPtr )
		{
			if( !mResourceMemory.memoryFlags.isSet( E_GPU_MEMORY_HEAP_PROPERTY_FLAG_CPU_COHERENT_BIT ) )
			{
				glMemoryBarrier( GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT );
				ts3GLHandleLastError();
			}
			mappedMemoryPtr = mPersistentMapPtr;
		}
		else
    #endif
		{
			auto openglMapFlags = GLCoreAPIProxy::translateGLBufferMapFlags( pMapMode, mResourceMemory.memoryFlags );
			if( mGLBufferObject->map( pRegion.offset, pRegion.size, openglMapFlags ) )
			{
				mappedMemoryPtr = mGLBufferObject->queryMappedPtr();
			}
		}

		if( !mappedMemoryPtr )
		{
			return false;
		}

		ResourceMappedMemory mappedMemoryInfo;
		mappedMemoryInfo.pointer = mappedMemoryPtr;
		mappedMemoryInfo.mappedRegion = pRegion;
		mappedMemoryInfo.memoryMapFlags = static_cast<EGPUMemoryMapFlags>( pMapMode );
		setMappedMemory( mappedMemoryInfo );

		return true;
	}

	void GLGPUBuffer::unmap( void * pCommandObject )
	{
		if( !mPersistentMapPtr )
		{
			mGLBufferObject->unmap();
		}
		else if( !mResourceMemory.memoryFlags.isSet( E_GPU_MEMORY_HEAP_PROPERTY_FLAG_CPU_COHERENT_BIT ) )
		{
			const auto & mappedMemory = getMappedMemory();
			mGLBufferObject->flushMappedRegion( mappedMemory.mappedRegion.offset, mappedMemory.mappedRegion.size );
		}

		resetMappedMemory();
	}

	void GLGPUBuffer::flushMappedRegion( void * pCommandObject, const GPUMemoryRegion & pRegion )
	{
		mGLBufferObject->flushMappedRegion( pRegion.offset, pRegion.size );
	}

	void GLGPUBuffer::invalidateRegion( void * pCommandObject, const GPUMemoryRegion & pRegion )
	{
		mGLBufferObject->invalidateRegion( pRegion.offset, pRegion.size );
	}

	void GLGPUBuffer::updateSubDataCopy( void * pCommandObject, GPUBuffer & pSource, const GPUBufferSubDataCopyDesc & pCopyDesc )
	{
		auto * openglSourceBufferStorage = pSource.queryInterface<GLGPUBuffer>();
		mGLBufferObject->updateCopy( *( openglSourceBufferStorage->mGLBufferObject ), pCopyDesc );
	}

	void GLGPUBuffer::updateSubDataUpload( void * pCommandObject, const GPUBufferSubDataUploadDesc & pUploadDesc )
	{
		mGLBufferObject->updateUpload( pUploadDesc );
	}

	bool GLGPUBuffer::validateMapRequest( const GPUMemoryRegion & pRegion, const EGPUMemoryMapMode & pMapMode )
	{
		return GPUBuffer::validateMapRequest( pRegion, pMapMode );
	}

	bool GLGPUBuffer::validateBufferCreateInfo( GPUBufferCreateInfo & pCreateInfo )
	{
		if( !GPUBuffer::validateBufferCreateInfo( pCreateInfo ) )
		{
			return false;
		}

		// Unset all flags which are not supported by the current platform.
		pCreateInfo.memoryFlags = pCreateInfo.memoryFlags & sSupportedEGPUMemoryFlags;

		return true;
	}
	
}
