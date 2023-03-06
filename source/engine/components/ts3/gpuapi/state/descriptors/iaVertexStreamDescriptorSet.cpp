
#include "iaVertexStreamDescriptorSet.h"

namespace ts3::gpuapi
{

	IAVertexStreamDescriptorSet::IAVertexStreamDescriptorSet( GPUDevice & pGPUDevice )
	: GPUDeviceChildObject( pGPUDevice )
	, _stateFlags( 0 )
	{}

	IAVertexStreamDescriptorSet::~IAVertexStreamDescriptorSet() = default;

	void IAVertexStreamDescriptorSet::assign( const IAVertexStreamDescriptorSet & pOther )
	{
		_vertexBufferDescriptors = pOther._vertexBufferDescriptors;
		_indexBufferDescriptor = pOther._indexBufferDescriptor;
		_activeBindingsMask = pOther._activeBindingsMask;
		_activeVertexBufferBindingsNum = pOther._activeVertexBufferBindingsNum;
	}

	IAVertexBufferDescriptor & IAVertexStreamDescriptorSet::getWritableIAVertexBufferDescriptor( input_assembler_index_t pIndex )
	{
		auto & descriptor = _vertexBufferDescriptors.at( pIndex );
		_stateFlags.set( E_VERTEX_STREAM_BUFFER_DESCRIPTOR_SET_STATE_FLAGS_UPDATE_MANUAL );
		return descriptor;
	}

	IAIndexBufferDescriptor & IAVertexStreamDescriptorSet::getWritableIAIndexBufferDescriptor()
	{
		_stateFlags.set( E_VERTEX_STREAM_BUFFER_DESCRIPTOR_SET_STATE_FLAGS_UPDATE_MANUAL );
		return _indexBufferDescriptor;
	}

	void IAVertexStreamDescriptorSet::setVertexBuffer( input_assembler_index_t pIndex, const IAVertexBufferDescriptor & pDescriptor )
	{
		ts3DebugAssert( cxdefs::isIAVertexBufferIndexValid( pIndex ) );
		_setIAVertexBufferDescriptor( pIndex, pDescriptor );
	}

	void IAVertexStreamDescriptorSet::setVertexBuffer( const IAVertexBufferDescriptorBindingDesc & pBinding )
	{
		ts3DebugAssert( cxdefs::isIAVertexBufferIndexValid( pBinding.streamIndex ) );
		_setIAVertexBufferDescriptor( pBinding.streamIndex, pBinding.vertexBufferDescriptor );
	}

	void IAVertexStreamDescriptorSet::setVertexBuffers( input_assembler_index_t pFirstIndex, const ArrayView<IAVertexBufferDescriptor> & pDescriptors )
	{
		const auto firstIndex = getMinOf( pFirstIndex, cxdefs::IA_MAX_VERTEX_BUFFER_BINDINGS_NUM );
		const auto inputDescriptorsNum = pDescriptors.size();

		for( auto arrayIndex = 0; arrayIndex < inputDescriptorsNum; ++arrayIndex )
		{
			if( !cxdefs::isIAVertexBufferIndexValid( firstIndex + arrayIndex ) )
			{
				break;
			}
			_setIAVertexBufferDescriptor( firstIndex + arrayIndex, pDescriptors[arrayIndex] );
		}
	}

	void IAVertexStreamDescriptorSet::setVertexBuffers( const ArrayView<IAVertexBufferDescriptorBindingDesc> & pBindings )
	{
		for( const auto & descriptorBinding : pBindings )
		{
			if( !cxdefs::isIAVertexBufferIndexValid( descriptorBinding.streamIndex ) )
			{
				continue;
			}
			_setIAVertexBufferDescriptor( descriptorBinding.streamIndex, descriptorBinding.vertexBufferDescriptor );
		}
	}

	void IAVertexStreamDescriptorSet::setVertexBuffers( const IAVertexBufferDescriptorArray & pDescriptors, uint32 pFirstIndex, uint32 pCount )
	{
		const auto firstIndex = getMinOf( pFirstIndex, cxdefs::IA_MAX_VERTEX_BUFFER_BINDINGS_NUM );
		const auto maxDescriptorsNum = cxdefs::IA_MAX_VERTEX_BUFFER_BINDINGS_NUM - firstIndex;
		const auto inputDescriptorsNum = getMinOf( pCount, maxDescriptorsNum );

		for( auto arrayIndex = firstIndex; arrayIndex < inputDescriptorsNum; ++arrayIndex )
		{
			_setIAVertexBufferDescriptor( firstIndex + arrayIndex, pDescriptors[firstIndex + arrayIndex] );
		}
	}

	void IAVertexStreamDescriptorSet::setIndexBuffer( const IAIndexBufferDescriptor & pDescriptor )
	{
		bool descriptorUpdated = false;
		const auto previousBindingStatus = !_indexBufferDescriptor.empty();

		_indexBufferDescriptor = pDescriptor;

		if( !pDescriptor.empty() && !previousBindingStatus )
		{
			ts3DebugAssert( !_activeBindingsMask.isSet( E_IA_VERTEX_STREAM_BINDING_FLAG_INDEX_BUFFER_BIT ) );
			_activeBindingsMask.set( E_IA_VERTEX_STREAM_BINDING_FLAG_INDEX_BUFFER_BIT );
			descriptorUpdated = true;
		}
		else if( pDescriptor.empty() && previousBindingStatus )
		{
			ts3DebugAssert( _activeBindingsMask.isSet( E_IA_VERTEX_STREAM_BINDING_FLAG_INDEX_BUFFER_BIT ) );
			_activeBindingsMask.unset( E_IA_VERTEX_STREAM_BINDING_FLAG_INDEX_BUFFER_BIT );
			descriptorUpdated = true;
		}

		if( descriptorUpdated )
		{
			_stateFlags.set( E_VERTEX_STREAM_BUFFER_DESCRIPTOR_SET_STATE_FLAG_CHANGE_PENDING_BIT );
		}
	}

	void IAVertexStreamDescriptorSet::resetVertexBuffer( input_assembler_index_t pIndex )
	{
		if( !cxdefs::isIAVertexBufferIndexValid( pIndex ) )
		{
			return;
		}
		_resetVertexBuffers( pIndex, 1 );
	}

	void IAVertexStreamDescriptorSet::resetVertexBuffers()
	{
		_resetVertexBuffers( 0, cxdefs::IA_MAX_VERTEX_BUFFER_BINDINGS_NUM );
	}

	void IAVertexStreamDescriptorSet::resetVertexBuffers( uint32 pFirstIndex, uint32 pCount )
	{
		const auto firstIndex = getMinOf( pFirstIndex, cxdefs::IA_MAX_VERTEX_BUFFER_BINDINGS_NUM );
		const auto maxDescriptorsNum = cxdefs::IA_MAX_VERTEX_BUFFER_BINDINGS_NUM - firstIndex;
		const auto resetDescriptorsNum = getMinOf( pCount, maxDescriptorsNum );

		foreachIn( bindArrayView( &( _vertexBufferDescriptors[firstIndex] ), resetDescriptorsNum ), &IAVertexBufferDescriptor::reset );

		const auto bindingMask = makeLSFBitmask<uint32>( resetDescriptorsNum ) << firstIndex;
		const auto activeBindingsNum = popCount( _activeBindingsMask & bindingMask );

		_activeBindingsMask.unset( bindingMask );
		_activeVertexBufferBindingsNum -= activeBindingsNum;
		_stateFlags.set( E_VERTEX_STREAM_BUFFER_DESCRIPTOR_SET_STATE_FLAGS_UPDATE_AUTO );
	}

	void IAVertexStreamDescriptorSet::resetIndexBuffer()
	{
		_indexBufferDescriptor.reset();
		_activeBindingsMask.unset( E_IA_VERTEX_STREAM_BINDING_FLAG_INDEX_BUFFER_BIT );
		_stateFlags.set( E_VERTEX_STREAM_BUFFER_DESCRIPTOR_SET_STATE_FLAG_CHANGE_PENDING_BIT );
	}

	bool IAVertexStreamDescriptorSet::applyDescriptorBindings()
	{
		if( _stateFlags.isSet( E_VERTEX_STREAM_BUFFER_DESCRIPTOR_SET_STATE_FLAG_CHANGE_PENDING_BIT ) )
		{
			if( _stateFlags.isSet( E_VERTEX_STREAM_BUFFER_DESCRIPTOR_SET_STATE_FLAG_DIRTY_BIT ) )
			{
				_processDirtyChange();
			}

			_stateFlags.unset( E_VERTEX_STREAM_BUFFER_DESCRIPTOR_SET_STATE_MASK_ALL_BITS );

			return true;
		}

		return false;
	}

	void IAVertexStreamDescriptorSet::_processDirtyChange()
	{
		ts3DebugAssert( _stateFlags.isSet( E_VERTEX_STREAM_BUFFER_DESCRIPTOR_SET_STATE_FLAGS_UPDATE_MANUAL ) );

		Bitmask<EIAVertexStreamBindingFlags> activeBindingsMask = 0;
		uint32 activeVertexBufferBindingsNum = 0;

		for( input_assembler_index_t vbDescriptorIndex = 0; vbDescriptorIndex < cxdefs::IA_MAX_VERTEX_BUFFER_BINDINGS_NUM; ++vbDescriptorIndex )
		{
			const auto & vertexBufferDescriptor = _vertexBufferDescriptors[vbDescriptorIndex];
			if( vertexBufferDescriptor.valid() )
			{
				const auto vertexBufferBindingFlag = cxdefs::makeIAVertexBufferFlag( vbDescriptorIndex );
				activeBindingsMask.set( vertexBufferBindingFlag );
				activeVertexBufferBindingsNum += 1;
			}
		}

		if( _indexBufferDescriptor.valid() )
		{
			activeBindingsMask.set( E_IA_VERTEX_STREAM_BINDING_FLAG_INDEX_BUFFER_BIT );
		}

		_activeBindingsMask = activeBindingsMask;
		_activeVertexBufferBindingsNum = activeVertexBufferBindingsNum;
	}

	void IAVertexStreamDescriptorSet::_setIAVertexBufferDescriptor( input_assembler_index_t pIndex, const IAVertexBufferDescriptor & pDescriptor )
	{
		bool descriptorUpdated = false;
		auto & descriptorRef = _vertexBufferDescriptors[pIndex];

		const auto previousBindingStatus = !descriptorRef.empty();
		const auto vertexBufferBindingFlag = cxdefs::makeIAVertexBufferFlag( pIndex );

		descriptorRef = pDescriptor;

		if( pDescriptor.valid() && !previousBindingStatus )
		{
			ts3DebugAssert( !_activeBindingsMask.isSet( vertexBufferBindingFlag ) );
			_activeBindingsMask.set( vertexBufferBindingFlag );
			_activeVertexBufferBindingsNum += 1;
			descriptorUpdated = true;
		}
		else if( pDescriptor.empty() && previousBindingStatus )
		{
			ts3DebugAssert( _activeBindingsMask.isSet( vertexBufferBindingFlag ) && ( _activeVertexBufferBindingsNum > 0 ) );
			_activeBindingsMask.unset( vertexBufferBindingFlag );
			_activeVertexBufferBindingsNum -= 1;
			descriptorUpdated = true;
		}

		if( descriptorUpdated )
		{
			_stateFlags.set( E_VERTEX_STREAM_BUFFER_DESCRIPTOR_SET_STATE_FLAGS_UPDATE_AUTO );
		}
	}

	void IAVertexStreamDescriptorSet::_resetVertexBuffers( uint32 pFirstIndex, uint32 pCount )
	{
		const auto descriptorRegion = getValidRegion( pFirstIndex, pCount, cxdefs::IA_MAX_VERTEX_BUFFER_BINDINGS_NUM );

		foreachIn( bindArrayView( _vertexBufferDescriptors, descriptorRegion ), &IAVertexBufferDescriptor::reset );

		const auto bindingMask = makeLSFBitmask<uint32>( descriptorRegion.size ) << descriptorRegion.offset;
		const auto resetBindingsNum = popCount( _activeBindingsMask & bindingMask );

		_activeBindingsMask.unset( bindingMask );
		_activeVertexBufferBindingsNum -= resetBindingsNum;
		_stateFlags.set( E_VERTEX_STREAM_BUFFER_DESCRIPTOR_SET_STATE_FLAGS_UPDATE_AUTO );
	}

	namespace smutil
	{

		IAVertexBufferBindingIndexList generateActiveVertexBufferIndices(
				const IAVertexBufferDescriptorArray & pVertexBufferDescriptors,
				Bitmask<EIAVertexStreamBindingFlags> pBindingMask )
		{
			constexpr auto optimalActiveIndicesNumPreAllocSize = 8u;

			IAVertexBufferBindingIndexList vertexBufferActiveIndices;
			vertexBufferActiveIndices.reserve( optimalActiveIndicesNumPreAllocSize );

			for( input_assembler_index_t streamIndex = 0; streamIndex < cxdefs::GPU_SYSTEM_METRIC_IA_MAX_VERTEX_BUFFER_BINDINGS_NUM; ++streamIndex )
			{
				const auto & vbDescriptor = pVertexBufferDescriptors[streamIndex];
				const auto vbBindingFlag = cxdefs::makeIAVertexBufferFlag( streamIndex );
				const auto vbBindingActive = pBindingMask.isSet( vbBindingFlag ) && vbDescriptor.valid();

				if( vbBindingActive )
				{
					vertexBufferActiveIndices.push_back( streamIndex );
				}
			}

			return vertexBufferActiveIndices;
		}

		IAVertexBufferRangeList generateActiveVertexBufferRanges(
				const IAVertexBufferDescriptorArray & pVertexBufferDescriptors,
				Bitmask<EIAVertexStreamBindingFlags> pBindingMask )
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
				const auto & vbDescriptor = pVertexBufferDescriptors[streamIndex];
				const auto vbBindingFlag = cxdefs::makeIAVertexBufferFlag( streamIndex );
				const auto vbBindingActive = pBindingMask.isSet( vbBindingFlag ) && vbDescriptor.valid();

				if( vbBindingActive )
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

				if( !vbBindingActive || ( streamIndex + 1 == cxdefs::GPU_SYSTEM_METRIC_IA_MAX_VERTEX_BUFFER_BINDINGS_NUM ) )
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

} //namespace ts3::gpuapi
