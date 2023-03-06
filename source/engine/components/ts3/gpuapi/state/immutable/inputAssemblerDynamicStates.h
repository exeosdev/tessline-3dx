
#pragma once

#ifndef __TS3_GPUAPI_INPUT_ASSEMBLER_DYNAMIC_STATES_H__
#define __TS3_GPUAPI_INPUT_ASSEMBLER_DYNAMIC_STATES_H__

#include "../inputAssemblerCommon.h"

namespace ts3::gpuapi
{

	/// @brief
	class IAVertexStreamDynamicState
	{
	public:
		IAVertexStreamDynamicState( const IAVertexStreamDynamicState & ) = default;
		IAVertexStreamDynamicState & operator=( const IAVertexStreamDynamicState & ) = default;

		IAVertexStreamDynamicState();
		explicit IAVertexStreamDynamicState( const IAVertexStreamDefinition & pDefinition );

		void assign( const IAVertexStreamDefinition & pDefinition );

		IAVertexBufferReference & setVertexBufferRef( input_assembler_index_t pIndex );

		void setVertexBufferRef( input_assembler_index_t pIndex, const IAVertexBufferReference & pVBReference );
		void setVertexBufferRefs( input_assembler_index_t pFirstIndex, const ArrayView<IAVertexBufferReference> & pVBReferences );
		void setVertexBufferRefs( const IAVertexBufferReferenceArray & pVBReferences );
		void setVertexBufferRefs( const IAVertexBufferReferenceArray & pVBReferences, uint32 pFirstIndex, uint32 pCount );

		IAIndexBufferReference & setIndexBufferRef();

		void setIndexBufferRef( const IAIndexBufferReference & pIBReference );

		void resetVertexBufferRef( input_assembler_index_t pIndex );
		void resetVertexBufferRefs( uint32 pFirstIndex, uint32 pCount );
		void resetVertexBufferRefs();

		void resetIndexBufferRef();

		void resetAllRefs();

	private:
		IAVertexStreamDefinition _vertexStreamDefinition;
	};

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_INPUT_ASSEMBLER_DYNAMIC_STATES_H__
