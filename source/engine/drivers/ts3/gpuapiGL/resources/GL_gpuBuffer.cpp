
#include "GL_gpuBuffer.h"
#include "../GL_apiTranslationLayer.h"
#include "../GL_gpuDevice.h"

namespace ts3::gpuapi
{

#if( TS3GX_GL_PLATFORM_TYPE == TS3GX_GL_PLATFORM_TYPE_ES )
	// OpenGL ES 3.1 exposes neither the immutable storage API (glBufferStorage), nor the persistent mapping
	// (as a natural consequence of the former). Thus, explicit coherency/persistence flags are not used.
	static constexpr uint32 sSupportedEGPUMemoryFlags = E_GPU_MEMORY_ACCESS_MASK_CPU_READ_WRITE | E_GPU_MEMORY_ACCESS_MASK_GPU_READ_WRITE;
#else
	// Core supports full set of features, including immutable storage, persistent mapping and explicit flushes.
	static constexpr uint32 sSupportedEGPUMemoryFlags =
			E_GPU_MEMORY_ACCESS_MASK_CPU_READ_WRITE |
			E_GPU_MEMORY_ACCESS_MASK_GPU_READ_WRITE |
			E_GPU_MEMORY_HEAP_PROPERTY_FLAG_CPU_COHERENT_BIT |
			E_GPU_MEMORY_HEAP_PROPERTY_FLAG_GPU_COHERENT_BIT |
			E_GPU_MEMORY_HEAP_PROPERTY_FLAG_PERSISTENT_MAP_BIT;
#endif


	GLGPUBuffer::GLGPUBuffer(
			GLGPUDevice & pGPUDevice,
			const ResourceMemoryInfo & pResourceMemory,
			const GPUBufferProperties & pBufferProperties,
			GLBufferObjectHandle pGLBufferObject )
	: GPUBuffer( pGPUDevice, pResourceMemory, pBufferProperties )
	, mGLBufferObject( std::move( pGLBufferObject ) )
	{}

	GLGPUBuffer::~GLGPUBuffer() = default;

	GLGPUBufferHandle GLGPUBuffer::createInstance( GLGPUDevice & pGPUDevice, const GPUBufferCreateInfo & pCreateInfo )
	{
		auto createInfo = pCreateInfo;
		if( !validateBufferCreateInfo( createInfo ) )
		{
			return nullptr;
		}

		GLBufferCreateInfo openglCreateInfo;
		openglCreateInfo.bindTarget = atl::translateGLBufferBindTarget( createInfo.initialTarget );
		openglCreateInfo.size = static_cast<GLuint>( createInfo.bufferSize );
		openglCreateInfo.resourceFlags = createInfo.resourceFlags;
		openglCreateInfo.memoryFlags = createInfo.memoryFlags;
		openglCreateInfo.initDataDesc = pCreateInfo.initDataDesc;

		GLBufferObjectHandle openglBufferObject = nullptr;
		if( pGPUDevice.isCompatibilityDevice() )
		{
			openglBufferObject = GLBufferObject::createCompat( openglCreateInfo );
			ts3DebugAssert( openglBufferObject );
		}
		else
		{
			openglBufferObject = GLBufferObject::createCore( openglCreateInfo );
			ts3DebugAssert( openglBufferObject );
		}

		GPUBufferProperties bufferProperties;
		bufferProperties.byteSize = openglBufferObject->querySize();
		bufferProperties.resourceFlags = createInfo.resourceFlags;

		ResourceMemoryInfo bufferMemoryInfo;
		bufferMemoryInfo.sourceHeapRegion.offset = 0;
		bufferMemoryInfo.sourceHeapRegion.size = bufferProperties.byteSize;
		bufferMemoryInfo.baseAlignment = createInfo.memoryBaseAlignment;
		bufferMemoryInfo.memoryFlags = createInfo.memoryFlags;

		auto openglBuffer = createDynamicInterfaceObject<GLGPUBuffer>(
				pGPUDevice,
				bufferMemoryInfo,
				bufferProperties,
				std::move( openglBufferObject ) );

		return openglBuffer;
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

	bool GLGPUBuffer::mapRegion( void * , const GPUMemoryRegion & pRegion, EGPUMemoryMapMode pMapMode )
	{
		void * mappedMemoryPtr = nullptr;

		if( mGLBufferObject->isMappedPersistent() )
		{
			if( !mResourceMemory.memoryFlags.isSet( E_GPU_MEMORY_HEAP_PROPERTY_FLAG_CPU_COHERENT_BIT ) )
			{
				glMemoryBarrier( GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT );
				ts3OpenGLHandleLastError();
			}
			mappedMemoryPtr = mGLBufferObject->getPersistentMapPtr();
		}
		else
		{
			const auto openglMapFlags = atl::translateGLBufferMapFlags( pMapMode, mResourceMemory.memoryFlags );
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

	void GLGPUBuffer::unmap( void * )
	{
		if( const auto & mappedMemory = getMappedMemory() )
		{
			if( mGLBufferObject->isMappedPersistent() )
			{
				if( !mResourceMemory.memoryFlags.isSet( E_GPU_MEMORY_HEAP_PROPERTY_FLAG_CPU_COHERENT_BIT ) )
				{
					mGLBufferObject->flushMappedRegion( mappedMemory.mappedRegion.offset, mappedMemory.mappedRegion.size );
				}
			}
			else
			{
				mGLBufferObject->unmap();
			}
			resetMappedMemory();
		}
	}

	void GLGPUBuffer::flushMappedRegion( void * , const GPUMemoryRegion & pRegion )
	{
		mGLBufferObject->flushMappedRegion( pRegion.offset, pRegion.size );
	}

	void GLGPUBuffer::invalidateRegion( void * , const GPUMemoryRegion & pRegion )
	{
		mGLBufferObject->invalidateRegion( pRegion.offset, pRegion.size );
	}

	void GLGPUBuffer::updateSubDataCopy( void * , GPUBuffer & pSrcBuffer, const GPUBufferSubDataCopyDesc & pCopyDesc )
	{
		auto * openglSourceBuffer = pSrcBuffer.queryInterface<GLGPUBuffer>();
		if( !mGPUDevice.queryInterface<GLGPUDevice>()->isCompatibilityDevice() )
		{
			mGLBufferObject->updateCopyInvalidate( *openglSourceBuffer->mGLBufferObject, pCopyDesc );
		}
		else
		{
			mGLBufferObject->updateCopyOrphan( *openglSourceBuffer->mGLBufferObject, pCopyDesc );
		}
	}

	void GLGPUBuffer::updateSubDataUpload( void * , const GPUBufferSubDataUploadDesc & pUploadDesc )
	{
		if( !mGPUDevice.queryInterface<GLGPUDevice>()->isCompatibilityDevice() )
		{
			mGLBufferObject->updateUploadInvalidate( pUploadDesc );
		}
		else
		{
			mGLBufferObject->updateUploadOrphan( pUploadDesc );
		}
	}

	bool GLGPUBuffer::validateMapRequest( const GPUMemoryRegion & pRegion, const EGPUMemoryMapMode & pMapMode )
	{
		return GPUBuffer::validateMapRequest( pRegion, pMapMode );
	}

} // namespace ts3::gpuapi
