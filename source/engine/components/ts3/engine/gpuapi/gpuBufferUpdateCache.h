
#ifndef __TS3_ENGINE_GPU_BUFFER_UPDATE_CACHE_H__
#define __TS3_ENGINE_GPU_BUFFER_UPDATE_CACHE_H__

#include "commonGpuAPIDefs.h"
#include <ts3/stdext/memoryBuffer.h>
#include <deque>

namespace ts3
{

	class GPUBufferUpdateCache
	{
	public:
		struct UpdateDesc
		{
			GPUMemoryRegion bufferRegion;
			void * cacheDataPtr;
		};

		using UpdateDescList = std::deque<UpdateDesc>;

		UpdateDesc & allocateRegion( const GPUMemoryRegion & pUpdateRegion );

		void reset();

		void resize( size_t pCacheSize );

		const UpdateDescList & getUpdateDescList() const;

		void * getInternalPtr() const;

		size_t size() const;

		bool isEmpty() const;

	private:
		ts3::DynamicMemoryBuffer _internalBuffer;
		UpdateDescList _updateDescList;
	};

}

#endif // __TS3_ENGINE_GPU_BUFFER_UPDATE_CACHE_H__
