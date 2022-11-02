
#include "inputAssemblerVertexStream.h"

namespace ts3::GpuAPI
{

	IAVertexStreamDescriptor::IAVertexStreamDescriptor(
			GPUDevice & pGPUDevice,
			PipelineDescriptorID pDescriptorID,
			const IAVertexStreamConfiguration & pStreamConfiguration )
	: GraphicsPipelineDescriptor( pGPUDevice, pDescriptorID, EGraphicsPipelineDescriptorType::IAVertexStream )
	, mActiveBindingsMask( pStreamConfiguration.activeBindingsMask )
	, mActiveVertexBufferBindingsNum( trunc_numeric_cast<uint32>( pStreamConfiguration.vertexBufferBindings.size() ) )
	{}

	IAVertexStreamDescriptor::~IAVertexStreamDescriptor() = default;


	namespace StateMgmt
	{

		Bitmask<EIAVertexStreamBindingFlags> getIAVertexStreamActiveBindingsMask(
				const IAVertexBufferBindingArray & pVertexBufferBindings,
				const IAIndexBufferBinding & pIndexBufferBinding ) noexcept
		{
			Bitmask<EIAVertexStreamBindingFlags> bindingsMask = 0;

			for( uint32 vertexBufferIndex = 0; vertexBufferIndex < E_GPU_SYSTEM_METRIC_IA_MAX_VERTEX_BUFFER_BINDINGS_NUM; ++vertexBufferIndex )
			{
				if( pVertexBufferBindings[vertexBufferIndex].active() )
				{
					bindingsMask.set( CxDefs::makeIAVertexBufferFlag( vertexBufferIndex ) );
				}
			}

			if( pIndexBufferBinding.active() )
			{
				bindingsMask.set( E_IA_VERTEX_STREAM_BINDING_FLAG_INDEX_BUFFER_BIT );
			}

			return bindingsMask;
		}

		uint32 getIAVertexBufferActiveBindingsNum( const IAVertexBufferBindingArray & pVertexBufferBindings ) noexcept
		{
			uint32 bindingsNum = 0;

			for( uint32 vertexBufferIndex = 0; vertexBufferIndex < E_GPU_SYSTEM_METRIC_IA_MAX_VERTEX_BUFFER_BINDINGS_NUM; ++vertexBufferIndex )
			{
				if( pVertexBufferBindings[vertexBufferIndex].active() )
				{
					++bindingsNum;
				}
			}

			return bindingsNum;
		}

		IAVertexStreamConfiguration createIAVertexStreamConfiguration(
				const ArrayView<IAVertexBufferBindingDesc> & pVertexBufferBindingDefinitions,
				const IAIndexBufferBinding & pIndexBufferBindingDefinition ) noexcept
		{
			IAVertexStreamConfiguration vertexStreamConfig{};

			for( const auto & vertexBufferBindingDesc : pVertexBufferBindingDefinitions )
			{
				if( !CxDefs::isIAVertexBufferIndexValid( vertexBufferBindingDesc.streamIndex ) && vertexBufferBindingDesc.vbBinding.active() )
				{
					ts3DebugOutputFmt( "Invalid IA Stream Index: %u", static_cast<uint32>( vertexBufferBindingDesc.streamIndex ) );
					continue;
				}

				vertexStreamConfig.vertexBufferBindings[vertexBufferBindingDesc.streamIndex] = vertexBufferBindingDesc.vbBinding;
			}

			if( pIndexBufferBindingDefinition.active() )
			{
				vertexStreamConfig.indexBufferBinding = pIndexBufferBindingDefinition;
			}

			vertexStreamConfig.activeBindingsMask =
				getIAVertexStreamActiveBindingsMask( vertexStreamConfig.vertexBufferBindings, vertexStreamConfig.indexBufferBinding );

			vertexStreamConfig.activeVertexBufferBindingsNum =
				getIAVertexBufferActiveBindingsNum( vertexStreamConfig.vertexBufferBindings );

			return vertexStreamConfig;
		}

		VertexBufferIndexArray generateActiveVertexBufferIndices(
				const IAVertexStreamConfiguration & pVertexStreamConfig ) noexcept
		{
			VertexBufferIndexArray vertexBufferActiveIndices;
			vertexBufferActiveIndices.reserve( pVertexStreamConfig.activeVertexBufferBindingsNum );

			for( input_assembler_index_t streamIndex = 0; streamIndex < E_GPU_SYSTEM_METRIC_IA_MAX_VERTEX_BUFFER_BINDINGS_NUM; ++streamIndex )
			{
				if( pVertexStreamConfig.vertexBufferBindings[streamIndex].active() )
				{
					vertexBufferActiveIndices.push_back( streamIndex );
				}
			}

			return vertexBufferActiveIndices;
		}

		VertexBufferRangeArray generateActiveVertexBufferRanges( const IAVertexStreamConfiguration & pVertexStreamConfig ) noexcept
		{
			// Single range is described by two uint16 values which is 4 bytes. More than 90% use cases fall
			// into 1-3 ranges per single binding state. We pre-allocate space for 4 ranges (which is 16 bytes)
			// to prevent from auto growing in typical cases (and few lost bytes per binding in negligible).
			constexpr auto optimalActiveRangesNumPreAllocSize = 4u;

			VertexBufferRangeArray vertexBufferActiveRanges;
			vertexBufferActiveRanges.reserve( optimalActiveRangesNumPreAllocSize );

			IAVertexBufferRange currentVBRange{};
			currentVBRange.firstIndex = CxDefs::IA_VERTEX_BUFFER_BINDING_INDEX_UNDEFINED;

			for( input_assembler_index_t streamIndex = 0; streamIndex < E_GPU_SYSTEM_METRIC_IA_MAX_VERTEX_BUFFER_BINDINGS_NUM; ++streamIndex )
			{
				const auto & currentVBBinding = pVertexStreamConfig.vertexBufferBindings[streamIndex];

				if( currentVBBinding.active() )
				{
					// If the range is not valid, "open" it.
					// Set the current stream as the first stream in the range and range size to 0.
					if( currentVBRange.firstIndex == CxDefs::IA_VERTEX_BUFFER_BINDING_INDEX_UNDEFINED )
					{
						currentVBRange.firstIndex = streamIndex;
						currentVBRange.length = 0;
					}

					// Increase the length of a current range.
					++currentVBRange.length;
				}

				if( !currentVBBinding.bufferObject || ( streamIndex + 1 == E_GPU_SYSTEM_METRIC_IA_MAX_VERTEX_BUFFER_BINDINGS_NUM ) )
				{
					// If the range is not empty, add it to the list of active ranges.
					if( currentVBRange.length > 0 )
					{
						vertexBufferActiveRanges.push_back( currentVBRange );
					}

					// Reset the range by setting the first index to an invalid value and clear the length
					currentVBRange.firstIndex = CxDefs::IA_VERTEX_BUFFER_BINDING_INDEX_UNDEFINED;
					currentVBRange.length = 0;
				}
			}

			// In case we have so many ranges we exceeded our typical pre-allocation space, shrink the vector to
			// ideally fit the space required. This will prevent from potentially more significant waste of memory.
			if( vertexBufferActiveRanges.size() > optimalActiveRangesNumPreAllocSize )
			{
				vertexBufferActiveRanges.shrink_to_fit();
			}

			return vertexBufferActiveRanges;
		}

		bool validateIAVertexStreamConfiguration( const IAVertexStreamConfiguration & pVertexStreamConfig ) noexcept
		{
			if( pVertexStreamConfig.activeVertexBufferBindingsNum > CxDefs::IA_MAX_VERTEX_BUFFER_BINDINGS_NUM )
			{
				return false;
			}

			if( ( pVertexStreamConfig.activeBindingsMask & CxDefs::IA_VERTEX_BUFFER_BINDING_MASK_ALL ) == 0 )
			{
				return false;
			}

			return true;
		}

	}

} //namespace ts3::GpuAPI
