
#pragma once

#ifndef __TS3_GPUAPI_INPUT_ASSEMBLER_DYNAMIC_STATES_H__
#define __TS3_GPUAPI_INPUT_ASSEMBLER_DYNAMIC_STATES_H__

#include "inputAssemblerCommon.h"

namespace ts3::gpuapi
{

	/// @brief
	class TS3_GPUAPI_CLASS IAVertexStreamDynamicState
	{
	public:
		IAVertexStreamDynamicState( const IAVertexStreamDynamicState & ) = default;
		IAVertexStreamDynamicState & operator=( const IAVertexStreamDynamicState & ) = default;

		IAVertexStreamDynamicState();
		explicit IAVertexStreamDynamicState( const IAVertexStreamDefinition & pDefinition );

		~IAVertexStreamDynamicState();

		TS3_ATTR_NO_DISCARD bool empty() const noexcept;

		TS3_ATTR_NO_DISCARD uint32 countActiveVertexBuffers() const noexcept;

		TS3_ATTR_NO_DISCARD const IAVertexStreamDefinition & getVertexStreamDefinition() const noexcept;

		void assign( const IAVertexStreamDefinition & pDefinition );

		IAVertexBufferReference & setVertexBufferRef( input_assembler_index_t pIndex );

		void setVertexBufferRef( input_assembler_index_t pIndex, const IAVertexBufferReference & pVBReference );
		void setVertexBufferRefs( const IAVertexBufferReferenceArray & pVBReferences );
		void setVertexBufferRefs( uint32 pFirstIndex, uint32 pCount, const IAVertexBufferReference * pVBReferences );

		IAIndexBufferReference & setIndexBufferRef();

		void setIndexBufferRef( const IAIndexBufferReference & pIBReference );

		void resetVertexBufferRef( input_assembler_index_t pIndex );
		void resetVertexBufferRefs( uint32 pFirstIndex, uint32 pCount );
		void resetVertexBufferRefs();

		void resetIndexBufferRef();

		void resetAllBufferRefs();

	private:
		void _setVertexBufferRefs( uint32 pFirstIndex, uint32 pCount, const IAVertexBufferReference * pVBReferences );
		void _resetVertexBufferRefs( uint32 pFirstIndex, uint32 pCount );

	private:
		IAVertexStreamDefinition _vertexStreamDefinition;
	};

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_INPUT_ASSEMBLER_DYNAMIC_STATES_H__
