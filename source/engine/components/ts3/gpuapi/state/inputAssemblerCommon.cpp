
#include "inputAssemblerCommon.h"

namespace ts3::gpuapi
{
	
	namespace smutil
	{

		IAVertexBufferRangeList generateActiveVertexBufferRanges( const IAVertexBufferReferenceArray & pVBReferences ) noexcept
		{
			// Single range is described by two uint16 values which is 4 bytes. More than 90% use cases fall
			// into 1-3 ranges per single binding state. We pre-allocate space for 4 ranges (which is 16 bytes)
			// to prevent from auto growing in typical cases (and few lost bytes per binding in negligible).
			constexpr auto optimalActiveRangesNumPreAllocSize = 4u;

			IAVertexBufferRangeList vertexBufferActiveRanges;
			vertexBufferActiveRanges.reserve( optimalActiveRangesNumPreAllocSize );

			IAVertexBufferRange currentVBRange{};
			currentVBRange.firstIndex = cxdefs::IA_VERTEX_BUFFER_BINDING_INDEX_UNDEFINED;

			for( input_assembler_index_t streamIndex = 0; streamIndex < cxdefs::GPU_SYSTEM_METRIC_IA_MAX_VERTEX_BUFFER_BINDINGS_NUM; ++streamIndex )
			{
				const auto & currentVBReference = pVBReferences[streamIndex];

				if( !currentVBReference.empty() )
				{
					// If the range is not valid, "open" it.
					// Set the current stream as the first stream in the range and range size to 0.
					if( currentVBRange.firstIndex == cxdefs::IA_VERTEX_BUFFER_BINDING_INDEX_UNDEFINED )
					{
						currentVBRange.firstIndex = streamIndex;
						currentVBRange.length = 0;
					}

					// Increase the length of a current range.
					++currentVBRange.length;
				}

				if( currentVBReference.empty() || ( streamIndex + 1 == cxdefs::GPU_SYSTEM_METRIC_IA_MAX_VERTEX_BUFFER_BINDINGS_NUM ) )
				{
					// If the range is not empty, add it to the list of active ranges.
					if( currentVBRange.length > 0 )
					{
						vertexBufferActiveRanges.push_back( currentVBRange );
					}

					// Reset the range by setting the first index to an invalid value and clear the length
					currentVBRange.firstIndex = cxdefs::IA_VERTEX_BUFFER_BINDING_INDEX_UNDEFINED;
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
		
	}
	
}
