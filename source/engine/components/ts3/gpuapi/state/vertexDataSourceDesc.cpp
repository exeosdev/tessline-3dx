
#include "vertexDataSourceDesc.h"

namespace ts3::gpuapi
{

	VertexDataSourceBinding createVertexDataSourceBinding( const VertexDataSourceBindingDesc & pBindingDesc )
	{
		VertexDataSourceBinding dataSourceBinding;
		dataSourceBinding.vertexBufferActiveBindingsNum = 0;
		dataSourceBinding.vertexBufferActiveRangesNum = 0;

		for( const auto & vbBindingDesc : pBindingDesc.vertexBufferBindingDescArray )
		{
			if( vbBindingDesc.bufferObject && ( vbBindingDesc.streamIndex != cxInvalidVertexStreamIndex ) )
			{
				dataSourceBinding.vertexBufferBindingArray[vbBindingDesc.streamIndex].bufferObject = vbBindingDesc.bufferObject;
				dataSourceBinding.vertexBufferBindingArray[vbBindingDesc.streamIndex].dataOffset = vbBindingDesc.dataOffset;
				dataSourceBinding.vertexBufferBindingArray[vbBindingDesc.streamIndex].dataStride = vbBindingDesc.dataStride;
				dataSourceBinding.vertexBufferActiveBindingsNum += 1;
			}
		}

		if( dataSourceBinding.vertexBufferActiveBindingsNum == 0 )
		{
			return CX_INIT_EMPTY;
		}

		dataSourceBinding.vertexStreamActiveIndexArray.reserve( dataSourceBinding.vertexBufferActiveBindingsNum );

		dataSourceBinding.indexBufferBinding.bufferObject = pBindingDesc.indexBufferBindingDesc.bufferObject;
		dataSourceBinding.indexBufferBinding.dataOffset = pBindingDesc.indexBufferBindingDesc.dataOffset;
		dataSourceBinding.indexBufferBinding.format = pBindingDesc.indexBufferBindingDesc.format;

		// At this point, dataSourceBinding.vertexBufferBindingArray[N] contains property of
		// a binding (offset, stride and the buffer itself) for a stream index N. That array may have gaps which is
		// perfectly fine - this is allowed to enable better flexibility and ability to replace IB/VB bindings without
		// switching other heavy pipeline state.
		// To avoid looping through all streams during setting the descriptor, we create two helper data structs:
		// - openglVDSDescriptor.dataSourceDesc.vertexStreamActiveIndexArray: an array (vector) of all active streams
		//   (i.e. those with vertex buffer bound), represented by their indices. This list is used by the older API
		//   and the ES driver, where GL is only able to bound single buffer at once (via glBindVertexBuffer function).
		// - openglVDSDescriptor.dataSourceDesc.vertexStreamActiveRangeList: a list (an std::list) with all active
		//   stream ranges (range expressed as {startIndex, rangeLength} pair). This list is used by the new API
		//   (desktop driver) which allows us to bind multiple vertex buffers via single glBindVertexBuffers call.
		//   For example, if user specifies three VBOs bound to streams 0, 1 and 4, two ranges will be produced:
		//   - {0,2} - a range of two active streams starting at index 0 (i.e. streams 0 and 1)
		//   - {4,1} - a range of one active stream starting at index 4 (i.e. stream 4)
		// Note, that usually gaps in the VBO binding array are not present. In such case, for any number of bindings,
		// only a single range will get produced, reducing the number of required GL API calls for binding from N to 1.

		VertexDataSourceBinding::VertexStreamRange streamRange;
		streamRange.firstIndex = cxInvalidVertexStreamIndex; // cxInvalidVertexStreamIndex means the range is not valid
		streamRange.length = 0;

		for( vertex_stream_index_t vertexInputStreamIndex = 0; vertexInputStreamIndex < GPU_SYSTEM_METRIC_IA_MAX_VERTEX_INPUT_STREAMS_NUM; ++vertexInputStreamIndex )
		{
			const auto & vbBinding =  dataSourceBinding.vertexBufferBindingArray[vertexInputStreamIndex];
			if( vbBinding.bufferObject )
			{
				dataSourceBinding.vertexStreamActiveIndexArray.push_back( vertexInputStreamIndex );
				// If the range is not valid, "open" it.
				// Set the current stream as the first stream in the range and range size to 0.
				if( streamRange.firstIndex == cxInvalidVertexStreamIndex )
				{
					streamRange.firstIndex = vertexInputStreamIndex;
					streamRange.length = 0;
				}
				// Increase the length of a current range.
				++streamRange.length;
			}
			// This not-so-obvious if allows us to also handle the last range and avoid doing checks outside the loop.
			if( !vbBinding.bufferObject || ( vertexInputStreamIndex + 1 == GPU_SYSTEM_METRIC_IA_MAX_VERTEX_INPUT_STREAMS_NUM ) )
			{
				// If the range is not empty, add it to the list of active ranges.
				if( streamRange.length > 0 )
				{
					dataSourceBinding.vertexStreamActiveRangeList.push_back( streamRange );
					dataSourceBinding.vertexBufferActiveRangesNum += 1;
				}
				// Reset the range by setting the first index to an invalid value.
				streamRange.firstIndex = cxInvalidVertexStreamIndex;
				streamRange.length = 0;
			}
		}

		return dataSourceBinding;
	}

} // namespace ts3::gpuapi
