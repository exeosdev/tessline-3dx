
#pragma once

#ifndef __TS3DRIVER_GPUAPI_GLCOMMON_IA_VERTEX_STREAM_DESCRIPTOR_SET_H__
#define __TS3DRIVER_GPUAPI_GLCOMMON_IA_VERTEX_STREAM_DESCRIPTOR_SET_H__

#include "../GL_prerequisites.h"
#include <ts3/gpuapi/state/descriptors/iaVertexStreamDescriptors.h>
#include <ts3/gpuapi/state/descriptors/iaVertexStreamDescriptorSet.h>

namespace ts3::gpuapi
{

	enum class EGLVertexBufferBindingType : enum_default_value_t
	{
		Undefined,
		Interleaved,
		Separate,
	};

	struct GLIAIndexBufferBinding
	{
		GLuint handle;
		GLintptr offset;
		GLenum format;
		GLuint elementByteSize;
	};

	struct GLIAVertexBuffersBindings
	{
		struct InterleavedBinding
		{
			/// GL-specific handle of the buffer object. Zero means the binding is not active.
			GLuint handle;
			/// Offset, in bytes, from the beginning of the buffer storage.
			GLintptr offset;
			/// Stride, in bytes, of the data in the buffer.
			GLsizei stride;
		};

		struct SeparateBindings
		{
			/// Array of GL-specific handles. Zero at index N means the binding for stream N is not active.
			GLuint handleArray[cxdefs::IA_MAX_VERTEX_BUFFER_BINDINGS_NUM];
			/// Array of offsets, in bytes, from the beginning of each buffer storage. Undefined for inactive bindings.
			GLintptr offsetArray[cxdefs::IA_MAX_VERTEX_BUFFER_BINDINGS_NUM];
			/// Array of data strides, in bytes, of the data in each buffer. Undefined for inactive bindings.
			GLsizei strideArray[cxdefs::IA_MAX_VERTEX_BUFFER_BINDINGS_NUM];
		};

		union
		{
			/// Interleaved VB bindings (an array of structs).
			InterleavedBinding interleavedBindingArray[cxdefs::IA_MAX_VERTEX_BUFFER_BINDINGS_NUM];
			/// Separate VB bindings (a struct with per-property arrays).
			SeparateBindings separateBindings;
		};

		EGLVertexBufferBindingType bindingType = EGLVertexBufferBindingType::Undefined;
	};

	class GLIAVertexStreamDescriptorSet : public IAVertexStreamDescriptorSet
	{
	public:
		GLIAVertexStreamDescriptorSet( GPUDevice & pGPUDevice );
		virtual ~GLIAVertexStreamDescriptorSet();

		virtual bool applyDescriptorBindings() override;

	private:
		GLIAIndexBufferBinding _indexBufferBinding;
		GLIAVertexBuffersBindings _vertexBuffersBindings;
		IAVertexBufferRangeList _vertexBufferActiveRanges;
	};

	namespace smutil
	{

		GLIAIndexBufferBinding translateIndexBufferDescriptor( const IAIndexBufferDescriptor & pDescriptor );

		GLIAVertexBuffersBindings translateVertexBufferDescriptorArray(
				const IAVertexBufferDescriptorArray & pDescriptors,
				Bitmask<EIAVertexStreamBindingFlags> pBindingMask );

	}

} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_GLCOMMON_IA_VERTEX_STREAM_DESCRIPTOR_SET_H__
