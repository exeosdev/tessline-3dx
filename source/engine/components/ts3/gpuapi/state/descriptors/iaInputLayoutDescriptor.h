
#pragma once

#ifndef __TS3_GPUAPI_VERTEX_STREAM_DESCRIPTORS_H__
#define __TS3_GPUAPI_VERTEX_STREAM_DESCRIPTORS_H__

#include "../inputAssemblerCommon.h"

namespace ts3::gpuapi
{

	/// @brief A definition of a vertex attribute. Associates IAVertexAttributeInfo with a specific attribute index.
	/// Used to create IAVertexInputFormat via createIAVertexInputFormat() function.
	struct IAVertexAttributeIndexBindingDesc
	{
		/// Attribute index this definition refers to.
		input_assembler_index_t attributeIndex{ cxdefs::IA_VERTEX_ATTRIBUTE_INDEX_UNDEFINED };

		/// Attribute definition.
		IAVertexAttributeInfo attributeInfo;
	};

	class IAInputLayoutDescriptor
	{
	public:
		IAInputLayoutDescriptor( const IAInputLayoutDescriptor & ) = default;
		IAInputLayoutDescriptor & operator=( const IAInputLayoutDescriptor & ) = default;

		IAInputLayoutDescriptor() = default;

		IAInputLayoutDescriptor( const IAVertexAttributeInfoArray & pAttributeArray );

		IAInputLayoutDescriptor & operator=( const IAVertexAttributeInfoArray & pAttributeArray );

		TS3_ATTR_NO_DISCARD const IAVertexAttributeInfoArray & getAttributeArray() const noexcept;

		TS3_ATTR_NO_DISCARD bool isAttributeActive( uint32 pAttributeIndex ) const noexcept;

		void configureAttributeMask( Bitmask<EIAVertexAttributeFlags> pAttachmentMask );

		IAVertexAttributeInfo & configureAttributeActive( uint32 pAttributeIndex, bool pActive );

		IAVertexAttributeInfo & configureAttributeInfo( uint32 pAttributeIndex );

		void configureAttributeInfo( uint32 pAttributeIndex, const IAVertexAttributeInfo & pAttributeInfo );

		void setAttributeInfoArray( const IAVertexAttributeInfoArray & pAttributeArray );

		void reset();

	private:
		IAVertexAttributeInfoArray _attributeArray;
	};

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_VERTEX_STREAM_DESCRIPTORS_H__
