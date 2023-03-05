
#pragma once

#ifndef __TS3_GPUAPI_VERTEX_STREAM_DESCRIPTOR_SET_H__
#define __TS3_GPUAPI_VERTEX_STREAM_DESCRIPTOR_SET_H__

#include "iaVertexStreamDescriptors.h"

namespace ts3::gpuapi
{

	/// @brief Describes a single, continuous range of vertex buffer binding points in the vertex stream.
	/// Used by drivers with support for range-based binding (D3D11, GL4) to reduce the number of API calls.
	struct IAVertexBufferRange
	{
		/// First index of the vertex buffer binding this range defines.
		input_assembler_index_t firstIndex = cxdefs::IA_VERTEX_BUFFER_BINDING_INDEX_UNDEFINED;

		/// Length of this range, i.e. number of vertex buffers within the range.
		uint16 length = 0;
	};

	using IAVertexBufferDescriptorArray = std::array<IAVertexBufferDescriptor, cxdefs::IA_MAX_VERTEX_BUFFER_BINDINGS_NUM>;

	/// @brief A definition of a vertex buffer binding. Associates IAVertexBufferBinding with a specific VB binding point.
	struct IAVertexBufferDescriptorBindingDesc
	{
		/// VB binding point index this binding refers to.
		input_assembler_index_t streamIndex = cxdefs::IA_VERTEX_BUFFER_BINDING_INDEX_UNDEFINED;

		/// Binding definition.
		IAVertexBufferDescriptor vertexBufferDescriptor;
	};

	class IAVertexStreamDescriptorSet : public GPUDeviceChildObject
	{
	public:
		IAVertexStreamDescriptorSet( GPUDevice & pGPUDevice );
		virtual ~IAVertexStreamDescriptorSet();

		TS3_ATTR_NO_DISCARD const IAVertexBufferDescriptor & getIAVertexBufferDescriptor( input_assembler_index_t pIndex ) const noexcept;

		TS3_ATTR_NO_DISCARD const IAIndexBufferDescriptor & getIAIndexBufferDescriptor() const noexcept;

		TS3_ATTR_NO_DISCARD Bitmask<EIAVertexStreamBindingFlags> getActiveBindingsMask() const noexcept;

		TS3_ATTR_NO_DISCARD uint32 getActiveVertexBufferBindingsNum() const noexcept;

		TS3_ATTR_NO_DISCARD bool checkIAVertexBufferDescriptor( input_assembler_index_t pIndex ) const noexcept;

		TS3_ATTR_NO_DISCARD bool checkIAIndexBufferDescriptor() const noexcept;

		void assign( const IAVertexStreamDescriptorSet & pOther );

		IAVertexBufferDescriptor & getWritableIAVertexBufferDescriptor( input_assembler_index_t pIndex );

		IAIndexBufferDescriptor & getWritableIAIndexBufferDescriptor();

		void setVertexBuffer( input_assembler_index_t pIndex, const IAVertexBufferDescriptor & pDescriptor );

		void setVertexBuffers( input_assembler_index_t pFirstIndex, const ArrayView<IAVertexBufferDescriptor> & pDescriptors );

		void setVertexBuffer( const IAVertexBufferDescriptorBindingDesc & pBinding );

		void setVertexBuffers( const ArrayView<IAVertexBufferDescriptorBindingDesc> & pBindings );

		void setVertexBuffers( const IAVertexBufferDescriptorArray & pDescriptors );

		void setVertexBuffers( const IAVertexBufferDescriptorArray & pDescriptors, uint32 pFirstIndex, uint32 pCount );

		void setIndexBuffer( const IAIndexBufferDescriptor & pDescriptor );

		void resetVertexBuffer( input_assembler_index_t pIndex );

		void resetVertexBuffers( uint32 pFirstIndex, uint32 pCount );

		void resetVertexBuffers();

		void resetIndexBuffer();

		virtual bool applyDescriptorBindings();

	private:
		enum EStateFlags : uint32
		{
			E_VERTEX_STREAM_BUFFER_DESCRIPTOR_SET_STATE_FLAG_CHANGE_PENDING_BIT = 0x01,

			E_VERTEX_STREAM_BUFFER_DESCRIPTOR_SET_STATE_FLAG_DIRTY_BIT = 0x08,

			E_VERTEX_STREAM_BUFFER_DESCRIPTOR_SET_STATE_MASK_ALL_BITS = 0x0F,

			E_VERTEX_STREAM_BUFFER_DESCRIPTOR_SET_STATE_FLAGS_UPDATE_AUTO =
				E_VERTEX_STREAM_BUFFER_DESCRIPTOR_SET_STATE_FLAG_CHANGE_PENDING_BIT,

			E_VERTEX_STREAM_BUFFER_DESCRIPTOR_SET_STATE_FLAGS_UPDATE_MANUAL =
				E_VERTEX_STREAM_BUFFER_DESCRIPTOR_SET_STATE_FLAG_CHANGE_PENDING_BIT |
				E_VERTEX_STREAM_BUFFER_DESCRIPTOR_SET_STATE_FLAG_DIRTY_BIT
		};

		void _processDirtyChange();

		void _setIAVertexBufferDescriptor( input_assembler_index_t pIndex, const IAVertexBufferDescriptor & pDescriptor );

		void _resetVertexBuffers( uint32 pFirstIndex, uint32 pCount );

	protected:
		Bitmask<EStateFlags> _stateFlags;
		IAVertexBufferDescriptorArray _vertexBufferDescriptors;
		IAIndexBufferDescriptor _indexBufferDescriptor;
		Bitmask<EIAVertexStreamBindingFlags> _activeBindingsMask;
		uint32 _activeVertexBufferBindingsNum;
	};

	inline const IAVertexBufferDescriptor & IAVertexStreamDescriptorSet::getIAVertexBufferDescriptor( input_assembler_index_t pIndex ) const noexcept
	{
		ts3DebugAssert( cxdefs::isIAVertexBufferIndexValid( pIndex ) );
		return _vertexBufferDescriptors[pIndex];
	}

	inline const IAIndexBufferDescriptor & IAVertexStreamDescriptorSet::getIAIndexBufferDescriptor() const noexcept
	{
		return _indexBufferDescriptor;
	}

	inline Bitmask<EIAVertexStreamBindingFlags> IAVertexStreamDescriptorSet::getActiveBindingsMask() const noexcept
	{
		return _activeBindingsMask;
	}

	inline uint32 IAVertexStreamDescriptorSet::getActiveVertexBufferBindingsNum() const noexcept
	{
		return _activeVertexBufferBindingsNum;
	}

	// State Management Utilities
	namespace smutil
	{

		/// @brief
		using VertexBufferIndexArray = std::vector<input_assembler_index_t>;

		/// @brief
		using VertexBufferRangeArray = std::vector<IAVertexBufferRange>;

		/// @brief
		TS3_ATTR_NO_DISCARD static std::vector<input_assembler_index_t> generateActiveVertexBufferIndices(
				const IAVertexBufferDescriptorArray & pIAVertexBufferDescriptors );

		/// @brief
		TS3_ATTR_NO_DISCARD static std::vector<IAVertexBufferRange> generateActiveVertexBufferRanges(
				const IAVertexBufferDescriptorArray & pIAVertexBufferDescriptors );

		/// @brief Returns a bitmask representing specified IA buffer bindings state.
		TS3_GPUAPI_API_NO_DISCARD Bitmask<EIAVertexStreamBindingFlags> getIAVertexStreamActiveBindingsMask(
				const IAVertexBufferBindingArray & pVertexBufferBindings,
				const IAIndexBufferBinding & pIndexBufferBinding ) noexcept;

		/// @brief Returns the number of active vertex buffer bindings.
		TS3_GPUAPI_API_NO_DISCARD uint32 getIAVertexBufferActiveBindingsNum(
				const IAVertexBufferBindingArray & pVertexBufferBindings ) noexcept;

		/// @brief
		TS3_GPUAPI_API_NO_DISCARD IAVertexStreamConfiguration createIAVertexStreamConfiguration(
				const ArrayView<IAVertexBufferBindingDesc> & pVertexBufferBindingDefinitions,
				const IAIndexBufferBinding & pIndexBufferBindingDefinition ) noexcept;

		/// @brief
		TS3_GPUAPI_API_NO_DISCARD VertexBufferIndexArray generateActiveVertexBufferIndices(
				const IAVertexStreamConfiguration & pVertexStreamConfig ) noexcept;

		/// @brief
		TS3_GPUAPI_API_NO_DISCARD VertexBufferRangeArray generateActiveVertexBufferRanges(
				const IAVertexStreamConfiguration & pVertexStreamConfig ) noexcept;

		/// @brief
		TS3_GPUAPI_API_NO_DISCARD bool validateIAVertexStreamConfiguration(
				const IAVertexStreamConfiguration & pVertexStreamConfig ) noexcept;

		/// @brief
		TS3_GPUAPI_API_NO_DISCARD PipelineDescriptorHash computePipelineDescriptorHash(
				const IAVertexInputLayoutDescriptorCreateInfo & pDescriptorCreateInfo ) noexcept;

	}

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_VERTEX_STREAM_DESCRIPTOR_SET_H__
