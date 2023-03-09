
#pragma once

#ifndef __TS3DRIVER_GPUAPI_GLCOMMON_INPUT_ASSEMBLER_H__
#define __TS3DRIVER_GPUAPI_GLCOMMON_INPUT_ASSEMBLER_H__

#include "../GL_prerequisites.h"
#include <ts3/gpuapi/state/inputAssemblerImmutableStates.h>

namespace ts3::gpuapi
{

	ts3GLDeclareOpenGLObjectHandle( GLVertexArrayObject );

	enum class EGLVertexBufferBindingType : uint32
	{
		Undefined,
		Interleaved,
		Separate
	};

	struct GLIAVertexAttributeInfo
	{
		uint16 streamIndex;
		uint8 normalized;
		uint8 componentsNum;
		GLenum baseType;
		uint32 byteSize;
		uint32 relativeOffset;
		uint32 instanceRate;
	};

	using GLIAVertexAttributeInfoArray = std::array<GLIAVertexAttributeInfo, cxdefs::IA_MAX_VERTEX_ATTRIBUTES_NUM>;

	struct GLIAInputLayoutDefinition
	{
		Bitmask<EIAVertexAttributeFlags> activeAttributesMask;
		GLIAVertexAttributeInfoArray attributeArray;
		GLenum primitiveTopology;
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

		/// Separate VB bindings (a struct with per-property arrays).
		struct SeparateBindings
		{
			/// Array of GL-specific handles. Zero at index N means the binding for stream N is not active.
			GLuint handleArray[cxdefs::IA_MAX_VERTEX_BUFFER_BINDINGS_NUM];
			/// Array of offsets, in bytes, from the beginning of each buffer storage. Undefined for inactive bindings.
			GLintptr offsetArray[cxdefs::IA_MAX_VERTEX_BUFFER_BINDINGS_NUM];
			/// Array of data strides, in bytes, of the data in each buffer. Undefined for inactive bindings.
			GLsizei strideArray[cxdefs::IA_MAX_VERTEX_BUFFER_BINDINGS_NUM];
		};

		/// Interleaved VB bindings (an array of structs).
		using InterleavedBindingsArray = std::array<InterleavedBinding, cxdefs::IA_MAX_VERTEX_BUFFER_BINDINGS_NUM>;

		union
		{
			InterleavedBindingsArray interleavedBindings;

			SeparateBindings separateBindings;
		};

		EGLVertexBufferBindingType bindingType = EGLVertexBufferBindingType::Undefined;
	};

	/// @brief
	struct GLIAVertexStreamDefinition
	{
		Bitmask<EIAVertexStreamBindingFlags> activeBindingsMask;
		GLIAVertexBuffersBindings vertexBufferBindings;
		GLIAIndexBufferBinding indexBufferBinding;
	};

	struct GLDrawTopologyProperties
	{
		GLuint indexBufferBaseOffset = 0;
		GLenum indexBufferDataType = 0;
		GLuint indexBufferElementByteSize = 0;
		GLenum primitiveTopology = 0;

		void reset()
		{
			indexBufferBaseOffset = 0;
			indexBufferDataType = 0;
			indexBufferElementByteSize = 0;
			primitiveTopology = 0;
		}
	};

	///
	class GLIAInputLayoutImmutableState : public IAInputLayoutImmutableState
	{
	public:
		GLIAInputLayoutDefinition const mGLInputLayoutDefinition;
		GLVertexArrayObjectHandle const mVertexArrayObject;

	public:
		GLIAInputLayoutImmutableState(
				GLGPUDevice & pGPUDevice,
				const IAInputLayoutStateCommonProperties & pCommonProperties,
				const GLIAInputLayoutDefinition & pGLInputLayoutDefinition,
				GLVertexArrayObjectHandle pVertexArrayObject );

		virtual ~GLIAInputLayoutImmutableState();

		static GpaHandle<GLIAInputLayoutImmutableState> createInstance(
				GLGPUDevice & pGPUDevice,
				const IAInputLayoutDefinition & pInputLayoutDefinition );
	};

	///
	class GLIAVertexStreamImmutableState : public IAVertexStreamImmutableState
	{
	public:
		GLIAVertexStreamDefinition const mGLVertexStreamDefinition;

	public:
		GLIAVertexStreamImmutableState(
				GLGPUDevice & pGPUDevice,
				const IAVertexStreamStateCommonProperties & pCommonProperties,
				const GLIAVertexStreamDefinition & pGLVertexStreamDefinition );

		virtual ~GLIAVertexStreamImmutableState();

		static GpaHandle<GLIAVertexStreamImmutableState> createInstance(
				GLGPUDevice & pGPUDevice,
				const IAVertexStreamDefinition & pVertexStreamDefinition );
	};

	namespace smutil
	{

		TS3_ATTR_NO_DISCARD GLIAVertexAttributeInfo translateIAVertexAttributeInfo(
				const IAVertexAttributeInfo & pAttributeInfo );

		TS3_ATTR_NO_DISCARD GLIAInputLayoutDefinition translateIAInputLayoutDefinition(
				const IAInputLayoutDefinition & pDefinition );

		TS3_ATTR_NO_DISCARD GLIAVertexStreamDefinition translateIAVertexStreamDefinition(
				const IAVertexStreamDefinition & pDefinition );

		TS3_ATTR_NO_DISCARD uint32 translateVertexBufferReferences(
				const IAVertexBufferReferenceArray & pVBReferences,
				Bitmask<EIAVertexStreamBindingFlags> pBindingMask,
				GLIAVertexBuffersBindings & pOutGLBindings);

		TS3_ATTR_NO_DISCARD bool translateIndexBufferReference(
				const IAIndexBufferReference & pIBReference,
				GLIAIndexBufferBinding & pOutGLBinding );

		TS3_ATTR_NO_DISCARD GLIAVertexBuffersBindings translateVertexBufferReferences(
				const IAVertexBufferReferenceArray & pVBReferences,
				Bitmask<EIAVertexStreamBindingFlags> pBindingMask );

		TS3_ATTR_NO_DISCARD GLIAIndexBufferBinding translateIndexBufferReference(
				const IAIndexBufferReference & pIBReference );

		TS3_ATTR_NO_DISCARD GLVertexArrayObjectHandle createGLVertexArrayObjectFromLayoutDefinition(
				const GLIAInputLayoutDefinition & pGLLayoutDefinition ) noexcept;

		TS3_ATTR_NO_DISCARD GLVertexArrayObjectHandle createGLVertexArrayObjectFromLayoutDefinition(
				const GLIAInputLayoutDefinition & pGLLayoutDefinition ) noexcept;

	}

} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_GLCOMMON_INPUT_ASSEMBLER_H__
