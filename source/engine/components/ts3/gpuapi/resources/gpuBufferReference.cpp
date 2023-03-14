
#include "gpuBufferReference.h"
#include "gpuBuffer.h"

namespace ts3::gpuapi
{

	GPUBufferReference::GPUBufferReference( GPUBufferHandle pGPUBuffer )
	: _refBufferObject( pGPUBuffer )
	, _refSubRegion{ pGPUBuffer->getWholeBufferRegion() }
	{}

	GPUBufferReference::GPUBufferReference( GPUBufferHandle pGPUBuffer, const GPUMemoryRegion & pReferencedSubRegion )
	{
		_setRefBufferInternal( pGPUBuffer, rcutil::validateGPUBufferRegion( pGPUBuffer, pReferencedSubRegion ) );
	}

	GPUBufferReference::GPUBufferReference( GPUBufferHandle pGPUBuffer, gpu_memory_size_t pOffset, gpu_memory_size_t pSize )
	{
		_setRefBufferInternal( pGPUBuffer, rcutil::validateGPUBufferRegion( pGPUBuffer, pOffset, pSize ) );
	}

	bool GPUBufferReference::setRefBuffer( GPUBufferHandle pGPUBuffer )
	{
		return _setRefBufferInternal( pGPUBuffer, pGPUBuffer->getWholeBufferRegion() );
	}

	bool GPUBufferReference::setRefBuffer( GPUBufferHandle pGPUBuffer, const GPUMemoryRegion & pReferencedSubRegion )
	{
		return _setRefBufferInternal( pGPUBuffer, rcutil::validateGPUBufferRegion( pGPUBuffer, pReferencedSubRegion ) );
	}

	bool GPUBufferReference::setRefBuffer( GPUBufferHandle pGPUBuffer, gpu_memory_size_t pOffset, gpu_memory_size_t pSize )
	{
		return _setRefBufferInternal( pGPUBuffer, rcutil::validateGPUBufferRegion( pGPUBuffer, pOffset, pSize ) );
	}

	bool GPUBufferReference::reset()
	{
		return _setRefBufferInternal( nullptr );
	}

	bool GPUBufferReference::_setRefBufferInternal( GPUBufferHandle pGPUBuffer, const GPUMemoryRegion & pReferencedSubRegion )
	{
		bool referenceUpdated = false;

		// If an empty buffer handle is specified, reset this reference object: the buffer gets cleared (and its
		// active use counter decremented by one), referenced region is reset to default.
		if( !pGPUBuffer )
		{
			// Of course, assuming there is something to reset in the first place.
			if( _refBufferObject )
			{
				_refBufferObject->releaseActiveRef();
				_refBufferObject = nullptr;
				_refSubRegion.reset();

				referenceUpdated = true;
			}
		}
		// Otherwise, check if there is anything to update: either the buffer is a
		// different buffer or the referenced subregion is different from the current one.
		else if( ( pGPUBuffer != _refBufferObject ) || ( pReferencedSubRegion != _refSubRegion ) )
		{
			// Validate if the specified subregion is valid for the given buffer object.
			if( rcutil::checkGPUBufferRegion( pGPUBuffer, pReferencedSubRegion ) )
			{
				// Update the buffer only if necessary.
				if( pGPUBuffer != _refBufferObject )
				{
					// This can be null (empty reference).
					if( _refBufferObject )
					{
						_refBufferObject->releaseActiveRef();
						_refBufferObject = nullptr;
					}

					_refBufferObject = pGPUBuffer;
					_refBufferObject->addActiveRef();
				}

				// Update the referenced subregion.
				_refSubRegion = pReferencedSubRegion;

				referenceUpdated = true;
			}
		}

		return referenceUpdated;
	}

}
