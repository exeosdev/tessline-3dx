
#include "inputAssemblerDynamicStates.h"

namespace ts3::gpuapi
{

	IAVertexStreamDynamicState::IAVertexStreamDynamicState()
	{}

	IAVertexStreamDynamicState::IAVertexStreamDynamicState( const IAVertexStreamDefinition & pDefinition )
	: _vertexStreamDefinition( pDefinition )
	{}

	IAVertexStreamDynamicState::~IAVertexStreamDynamicState() = default;

	bool IAVertexStreamDynamicState::empty() const noexcept
	{
		return _vertexStreamDefinition.activeBindingsMask.empty();
	}

	uint32 IAVertexStreamDynamicState::countActiveVertexBuffers() const noexcept
	{
		return popCount( _vertexStreamDefinition.activeBindingsMask & E_IA_VERTEX_STREAM_BINDING_MASK_VERTEX_BUFFER_ALL_BITS );
	}

	const IAVertexStreamDefinition & IAVertexStreamDynamicState::getVertexStreamDefinition() const noexcept
	{
		return _vertexStreamDefinition;
	}

	void IAVertexStreamDynamicState::assign( const IAVertexStreamDefinition & pDefinition )
	{
		_vertexStreamDefinition = pDefinition;
	}

	IAVertexBufferReference & IAVertexStreamDynamicState::setVertexBufferRef( input_assembler_index_t pIndex )
	{
		ts3DebugAssert( cxdefs::isIAVertexBufferIndexValid( pIndex ) );
		_vertexStreamDefinition.activeBindingsMask.set( cxdefs::makeIAVertexBufferFlag( pIndex ) );
		return _vertexStreamDefinition.vertexBufferReferences[pIndex];

	}

	void IAVertexStreamDynamicState::setVertexBufferRef( input_assembler_index_t pIndex, const IAVertexBufferReference & pVBReference )
	{
		_setVertexBufferRefs( pIndex, 1, &pVBReference );
	}

	void IAVertexStreamDynamicState::setVertexBufferRefs( const IAVertexBufferReferenceArray & pVBReferences )
	{
		_setVertexBufferRefs( 0, pVBReferences.size(), pVBReferences.data() );
	}

	void IAVertexStreamDynamicState::setVertexBufferRefs( uint32 pFirstIndex, uint32 pCount, const IAVertexBufferReference * pVBReferences )
	{
		_setVertexBufferRefs( pFirstIndex, pCount, pVBReferences );
	}

	IAIndexBufferReference & IAVertexStreamDynamicState::setIndexBufferRef()
	{
		_vertexStreamDefinition.activeBindingsMask.set( E_IA_VERTEX_STREAM_BINDING_FLAG_INDEX_BUFFER_BIT );
		return _vertexStreamDefinition.indexBufferReference;
	}

	void IAVertexStreamDynamicState::setIndexBufferRef( const IAIndexBufferReference & pIBReference )
	{
		_vertexStreamDefinition.activeBindingsMask.setOrUnset( E_IA_VERTEX_STREAM_BINDING_FLAG_INDEX_BUFFER_BIT, !pIBReference.empty() );
		_vertexStreamDefinition.indexBufferReference = pIBReference;
	}

	void IAVertexStreamDynamicState::resetVertexBufferRef( input_assembler_index_t pIndex )
	{
		_resetVertexBufferRefs( pIndex, 1 );
	}

	void IAVertexStreamDynamicState::resetVertexBufferRefs( uint32 pFirstIndex, uint32 pCount )
	{
		_resetVertexBufferRefs( pFirstIndex, pCount );
	}

	void IAVertexStreamDynamicState::resetVertexBufferRefs()
	{
		_resetVertexBufferRefs( 0, cxdefs::IA_MAX_VERTEX_BUFFER_BINDINGS_NUM );
	}

	void IAVertexStreamDynamicState::resetIndexBufferRef()
	{
		_vertexStreamDefinition.activeBindingsMask.unset( E_IA_VERTEX_STREAM_BINDING_FLAG_INDEX_BUFFER_BIT );
		_vertexStreamDefinition.indexBufferReference.reset();
	}

	void IAVertexStreamDynamicState::resetAllBufferRefs()
	{
		resetIndexBufferRef();
		resetVertexBufferRefs();
	}

	void IAVertexStreamDynamicState::_setVertexBufferRefs( uint32 pFirstIndex, uint32 pCount, const IAVertexBufferReference * pVBReferences )
	{
		for( uint32 vbIndex = pFirstIndex; ( vbIndex < cxdefs::IA_MAX_VERTEX_BUFFER_BINDINGS_NUM ) && ( pCount != 0 ); ++vbIndex, --pCount )
		{
			const auto & sourceVBReference = pVBReferences[vbIndex - pFirstIndex];

			const auto vertexBufferBit = cxdefs::makeIAVertexBufferFlag( vbIndex );

			_vertexStreamDefinition.vertexBufferReferences[vbIndex] = sourceVBReference;
			_vertexStreamDefinition.activeBindingsMask.setOrUnset( vertexBufferBit, !sourceVBReference.empty() );
		}
	}

	void IAVertexStreamDynamicState::_resetVertexBufferRefs( uint32 pFirstIndex, uint32 pCount )
	{
		for( uint32 vbIndex = pFirstIndex; ( vbIndex < cxdefs::IA_MAX_VERTEX_BUFFER_BINDINGS_NUM ) && ( pCount != 0 ); ++vbIndex, --pCount )
		{
			const auto vertexBufferBit = cxdefs::makeIAVertexBufferFlag( vbIndex );

			_vertexStreamDefinition.vertexBufferReferences[vbIndex].reset();
			_vertexStreamDefinition.activeBindingsMask.unset( vertexBufferBit );
		}
	}

}
