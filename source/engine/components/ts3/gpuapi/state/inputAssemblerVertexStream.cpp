
#include "inputAssemblerVertexStream.h"

namespace ts3::gpuapi
{

	IAVertexStreamDescriptor::IAVertexStreamDescriptor( GPUDevice & pGPUDevice,
	                                                    pipeline_descriptor_id_t pDescriptorID,
	                                                    const IAVertexStreamBufferBinding & pBufferBinding )
	: GraphicsPipelineDescriptor( pGPUDevice, pDescriptorID, EGraphicsPipelineDescriptorType::IAVertexStream )
	, mActiveBindingsMask( pBufferBinding.activeBindingsMask )
	, mActiveVertexBufferBindingsNum( trunc_numeric_cast<uint16>( pBufferBinding.activeVertexBufferBindingsNum ) )
	, mActiveVertexBufferRangesNum( trunc_numeric_cast<uint16>( pBufferBinding.activeVertexBufferRangesNum ) )
	{}

	IAVertexStreamDescriptor::~IAVertexStreamDescriptor() = default;

	bool IAVertexStreamDescriptor::isValid() const noexcept
	{
		return ( mActiveVertexBufferBindingsNum > 0 ) && ( mActiveVertexBufferBindingsNum <= E_IA_CONSTANT_MAX_VERTEX_ATTRIBUTES_NUM );
	}


	IAVertexStreamBufferBinding createIAVertexStreamBufferBinding( const ArrayView<IAVertexBufferBindingDesc> & pVertexBufferBindings,
	                                                               const IAIndexBufferBinding & pIndexBufferBinding )
	{
		IAVertexStreamBufferBinding bufferBindingState{};

		for( const auto & vbBindingDesc : pVertexBufferBindings )
		{
			TS3_DEBUG_CODE({
				if( !ecIsIAVertexStreamVBIndexValid( vbBindingDesc.streamIndex ) )
				{
					ts3DebugOutputFmt( "Invalid IA Stream Index: %u", static_cast<uint32>( vbBindingDesc.streamIndex ) );
					continue;
				}
			})

			if( ecIsIAVertexStreamVBIndexValid( vbBindingDesc.streamIndex ) && vbBindingDesc.vbBinding.isActive() )
			{
				const auto bindingIndex = static_cast<input_assembler_index_t>( vbBindingDesc.streamIndex );
				const auto bindingFlag = ecMakeIAVertexStreamVBFlag( vbBindingDesc.streamIndex );

				bufferBindingState.vertexBufferBindings[bindingIndex] = vbBindingDesc.vbBinding;

				// Check if the specified stream index has been already set. It is possible to have more
				// than one binding for a given slot. In this case, we simply overwrite them in the given
				// order. Just make sure we don't increment the counter multiple times.
				if( !bufferBindingState.activeBindingsMask.isSet( bindingFlag ) )
				{
					bufferBindingState.activeBindingsMask.set( bindingFlag );
					bufferBindingState.activeVertexBufferBindingsNum += 1;
				}
			}
		}

		// If there is no vertex buffer, the binding is considered invalid.
		// We can skip the rest of the setup in this case, it won't matter anyway.
		if( bufferBindingState.activeVertexBufferBindingsNum == 0 )
		{
			return {};
		}

		if( pIndexBufferBinding.isActive() )
		{
			bufferBindingState.indexBufferBinding = pIndexBufferBinding;
			bufferBindingState.activeBindingsMask.set( E_IA_VERTEX_STREAM_BINDING_FLAG_INDEX_BUFFER );
		}

		// At this point, bufferBindingState.vertexBufferBindings[N] contains the definition of a vertex buffer binding
		// for a vertex stream index N. This array may have gaps which is perfectly fine (arbitrary configuration of
		// vertex buffers are allowed for better flexibility).
		// To avoid looping through all streams during setting the descriptor, we create two helper data structs:
		// - VertexBufferIndexArray: an array (vector) of all active streams (i.e. those with a vertex buffer bound),
		//   represented by their indices. This list is used by the older drivers (like OpenGL ES), where only a single
		//   buffer can be bound at once (example: glBindBuffer/glBindVertexBuffer).
		// - VertexBufferRangeArray: an array (vector) with all active vertex stream ranges (range is expressed as
		//   {startIndex, rangeLength} pair). This list is used by the new drivers which allows us to bind multiple
		//   vertex buffers via single call (example: glBindVertexBuffers/IASetVertexBuffers).
		//   For example, if user specifies three VBs bound to stream 0, 1 and 4, two ranges will be produced:
		//   - {0,2} - a range of two active bindings starting at index 0 (i.e. streams 0 and 1),
		//   - {4,1} - a range of one active binding starting at index 4 (i.e. stream 4).
		// Concrete divers are free to use this information or do their own preparation of the binding state. The purpose
		// of IAVertexStreamBufferBinding is just to provide an intermediate format, pre-processed for further usage.

		// Single range is described by two uint16 values which is 4 bytes. More than 90% use cases fall
		// into 1-3 ranges per single binding state. We pre-allocate space for 4 ranges (which is 16 bytes)
		// to prevent from auto growing in typical cases (and few lost bytes per binding in negligible).
		constexpr auto optimalActiveRangesNumPreAllocSize = 4u;

		IAVertexStreamBufferBinding::VertexBufferRangeArray vertexBufferActiveRanges;
		vertexBufferActiveRanges.reserve( optimalActiveRangesNumPreAllocSize );

		IAVertexStreamBufferBinding::VertexBufferIndexArray vertexBufferActiveBindings;
		vertexBufferActiveBindings.reserve( bufferBindingState.activeVertexBufferBindingsNum );

		IAVertexStreamBufferRange currentVBRange{};
		currentVBRange.firstIndex = E_IA_CONSTANT_VERTEX_STREAM_INDEX_UNDEFINED;

		for( input_assembler_index_t streamIndex = 0; streamIndex < E_GPU_SYSTEM_METRIC_IA_MAX_VERTEX_STREAMS_NUM; ++streamIndex )
		{
			const auto & currentVBBinding = bufferBindingState.vertexBufferBindings[streamIndex];

			if( currentVBBinding.isActive() )
			{
				vertexBufferActiveBindings.push_back( streamIndex );

				// If the range is not valid, "open" it.
				// Set the current stream as the first stream in the range and range size to 0.
				if( currentVBRange.firstIndex == E_IA_CONSTANT_VERTEX_STREAM_INDEX_UNDEFINED )
				{
					currentVBRange.firstIndex = streamIndex;
					currentVBRange.length = 0;
				}

				// Increase the length of a current range.
				++currentVBRange.length;
			}

			if( !currentVBBinding.bufferObject || ( streamIndex + 1 == E_GPU_SYSTEM_METRIC_IA_MAX_VERTEX_STREAMS_NUM ) )
			{
				// If the range is not empty, add it to the list of active ranges.
				if( currentVBRange.length > 0 )
				{
					vertexBufferActiveRanges.push_back( currentVBRange );
				}

				// Reset the range by setting the first index to an invalid value and clear the length
				currentVBRange.firstIndex = E_IA_CONSTANT_VERTEX_STREAM_INDEX_UNDEFINED;
				currentVBRange.length = 0;
			}
		}

		// In case we have so many ranges we exceeded our typical pre-allocation space, shrink the vector to
		// ideally fit the space required. This will prevent from potentially more significant waste of memory.
		if( vertexBufferActiveRanges.size() > optimalActiveRangesNumPreAllocSize )
		{
			vertexBufferActiveRanges.shrink_to_fit();
		}

		bufferBindingState.vertexBufferActiveRanges = std::move( vertexBufferActiveRanges );
		bufferBindingState.vertexBufferActiveBindings = std::move( vertexBufferActiveBindings );

		return bufferBindingState;
	}

} //namespace ts3::gpuapi
