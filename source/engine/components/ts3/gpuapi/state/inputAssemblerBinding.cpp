
#include "inputAssemblerBinding.h"

namespace ts3::gpuapi
{

	VertexStreamDescriptor::VertexStreamDescriptor( Bitmask<EVertexStreamBindingFlags> pActiveBindingsMask,
	                                                VertexBufferIndexArray pVertexBufferActiveIndices,
	                                                VertexBufferRangeArray pVertexBufferActiveRanges )
	: mActiveBindingsMask( pActiveBindingsMask )
	, mVertexBufferActiveBindingsNum( trunc_numeric_cast<uint32>( pVertexBufferActiveIndices.size() ) )
	, mVertexBufferActiveRangesNum( trunc_numeric_cast<uint32>( pVertexBufferActiveRanges.size() ) )
	, mVertexBufferActiveIndices( std::move( pVertexBufferActiveIndices ) )
	, mVertexBufferActiveRanges( std::move( pVertexBufferActiveRanges ) )
	{}

	VertexStreamDescriptor::InternalBindingDesc VertexStreamDescriptor::createInternalBindingDesc( const VertexStreamBufferBindingDesc & pBindingDesc )
	{
		InternalBindingDesc internalBindingDesc;
		internalBindingDesc.activeBindingsMask = 0;

		uint32 activeVertexBuffersNum = 0;

		for( const auto & inputVBBindingDesc : pBindingDesc.vertexBufferBindings )
		{
			if( inputVBBindingDesc.bufferObject && ( inputVBBindingDesc.streamIndex != CX_VERTEX_STREAM_INDEX_INVALID ) )
			{
				auto & currentVBBinding = internalBindingDesc.vertexBufferOrderedBindings[inputVBBindingDesc.streamIndex];

				if( !currentVBBinding.bufferObject && inputVBBindingDesc.bufferObject )
				{
					activeVertexBuffersNum += 1;
					internalBindingDesc.activeBindingsMask.set( 1 << inputVBBindingDesc.streamIndex );
				}

				if( currentVBBinding.bufferObject && !inputVBBindingDesc.bufferObject )
				{
					activeVertexBuffersNum -= 1;
					internalBindingDesc.activeBindingsMask.unset( 1 << inputVBBindingDesc.streamIndex );
				}

				currentVBBinding.bufferObject = inputVBBindingDesc.bufferObject;
				currentVBBinding.dataOffset = inputVBBindingDesc.dataOffset;
				currentVBBinding.dataStride = inputVBBindingDesc.dataStride;
			}
		}

		if( pBindingDesc.indexBufferBinding.bufferObject )
		{
			auto & ibBinding = internalBindingDesc.indexBufferBinding;

			ibBinding.bufferObject = pBindingDesc.indexBufferBinding.bufferObject;
			ibBinding.dataOffset = pBindingDesc.indexBufferBinding.dataOffset;
			ibBinding.format = pBindingDesc.indexBufferBinding.format;

			internalBindingDesc.activeBindingsMask.set( E_VERTEX_STREAM_BINDING_FLAG_INDEX_BUFFER_BIT );
		}

		if( activeVertexBuffersNum == 0 )
		{
			return nullptr;
		}

		// At this point, internalBindingDesc.vertexBufferOrderedBindings[N] contains the definition of a binding
		// (offset, stride and the buffer itself) for a stream index N. That array may have some gaps which is
		// perfectly fine - this is allowed to enable better flexibility.
		// To avoid looping through all streams during setting the descriptor, we create two helper data structs:
		// - activeVertexBufferIndices: an array (std::vector) of all active streams (i.e. those with a vertex buffer),
		//   represented by their indices. This list is used by the older APIs limited to binding only single buffer
		//   at once (like glBindVertexBuffer()).
		// - activeVertexBufferRanges: an array (std::vector) with all active stream ranges (range expressed as
		//   {startIndex, rangeLength} pair). This list is used by the new API which allows us to bind multiple
		//   vertex buffers (like glBindVertexBuffers()).

		// Single range is described by two uint16 values which is 4 bytes. More than 90% use cases fall
		// into 1-3 ranges per single binding descriptor. We pre-allocate space for 4 ranges (which is 16 bytes)
		// to prevent from auto growing in typical cases (and few lost bytes per binding in negligible).
		constexpr auto optimalActiveRangesNumPreAllocSize = 4u;

		internalBindingDesc.vertexBufferActiveBindings.reserve( activeVertexBuffersNum );
		internalBindingDesc.vertexBufferActiveRanges.reserve( optimalActiveRangesNumPreAllocSize );

		VertexStreamBufferRange currentVBRange{};
		currentVBRange.firstIndex = CX_VERTEX_STREAM_INDEX_INVALID;

		for( vertex_stream_index_t streamIndex = 0; streamIndex < E_GPU_SYSTEM_METRIC_IA_MAX_VERTEX_INPUT_STREAMS_NUM; ++streamIndex )
		{
			const auto & currentVBBinding =  internalBindingDesc.vertexBufferOrderedBindings[streamIndex];

			if( currentVBBinding.bufferObject )
			{
				internalBindingDesc.vertexBufferActiveBindings.push_back( streamIndex );

				// If the range is not valid, "open" it.
				// Set the current stream as the first stream in the range and range size to 0.
				if( currentVBRange.firstIndex == CX_VERTEX_STREAM_INDEX_INVALID )
				{
					currentVBRange.firstIndex = streamIndex;
					currentVBRange.length = 0;
				}

				// Increase the length of a current range.
				++currentVBRange.length;
			}

			if( !currentVBBinding.bufferObject || ( streamIndex + 1 == E_GPU_SYSTEM_METRIC_IA_MAX_VERTEX_INPUT_STREAMS_NUM ) )
			{
				// If the range is not empty, add it to the list of active ranges.
				if( currentVBRange.length > 0 )
				{
					internalBindingDesc.vertexBufferActiveRanges.push_back( currentVBRange );
				}

				// Reset the range by setting the first index to an invalid value and clear the length
				currentVBRange.firstIndex = CX_VERTEX_STREAM_INDEX_INVALID;
				currentVBRange.length = 0;
			}
		}

		// In case we have so many ranges we exceeded our typical pre-allocation space, shrink the vector to
		// ideally fit the space required. This will prevent from potentially more significant waste of memory.
		if( internalBindingDesc.vertexBufferActiveRanges.size() > optimalActiveRangesNumPreAllocSize )
		{
			internalBindingDesc.vertexBufferActiveRanges.shrink_to_fit();
		}

		return internalBindingDesc;
	}

}
