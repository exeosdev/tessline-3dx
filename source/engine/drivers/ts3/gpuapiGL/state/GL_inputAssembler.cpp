
#include "GL_inputAssembler.h"
#include <ts3/gpuapiGL/GL_coreAPIProxy.h>
#include <ts3/gpuapiGL/GL_gpuDevice.h>
#include <ts3/gpuapiGL/objects/GL_vertexArrayObject.h>
#include <ts3/gpuapiGL/resources/GL_gpuBuffer.h>

namespace ts3::gpuapi
{

	GLIAInputLayoutImmutableState::GLIAInputLayoutImmutableState(
			GLGPUDevice & pGPUDevice,
			const IAInputLayoutStateCommonProperties & pCommonProperties,
			const GLIAInputLayoutDefinition & pGLInputLayoutDefinition,
			GLVertexArrayObjectHandle pVertexArrayObject )
	: IAInputLayoutImmutableState( pGPUDevice, pCommonProperties )
	, mGLInputLayoutDefinition( pGLInputLayoutDefinition )
	, mVertexArrayObject( std::move( pVertexArrayObject ) )
	{}

	GLIAInputLayoutImmutableState::~GLIAInputLayoutImmutableState() = default;

	GpaHandle<GLIAInputLayoutImmutableState> GLIAInputLayoutImmutableState::createInstance(
			GLGPUDevice & pGPUDevice,
			const IAInputLayoutDefinition & pInputLayoutDefinition )
	{
		const auto inputLayoutCommonProperties = smutil::getIAInputLayoutStateCommonProperties( pInputLayoutDefinition );
		const auto glcInputLayoutDefinition = smutil::translateIAInputLayoutDefinition( pInputLayoutDefinition );

		auto vertexArrayObject = smutil::createGLVertexArrayObjectFromLayoutDefinition( glcInputLayoutDefinition );
		if( !vertexArrayObject )
		{
			return nullptr;
		}

		auto immutableState = createGPUAPIObject<GLIAInputLayoutImmutableState>(
				pGPUDevice,
				inputLayoutCommonProperties,
				glcInputLayoutDefinition,
				std::move( vertexArrayObject ) );

		return immutableState;
	}


	GLIAVertexStreamImmutableState::GLIAVertexStreamImmutableState(
			GLGPUDevice & pGPUDevice,
			const IAVertexStreamStateCommonProperties & pCommonProperties,
			const GLIAVertexStreamDefinition & pGLVertexStreamDefinition )
	: IAVertexStreamImmutableState( pGPUDevice, pCommonProperties )
	, mGLVertexStreamDefinition( pGLVertexStreamDefinition )
	{}

	GLIAVertexStreamImmutableState::~GLIAVertexStreamImmutableState() = default;

	GpaHandle<GLIAVertexStreamImmutableState> GLIAVertexStreamImmutableState::createInstance(
			GLGPUDevice & pGPUDevice,
			const IAVertexStreamDefinition & pVertexStreamDefinition )
	{
		const auto vertexStreamCommonProperties = smutil::getIAVertexStreamStateCommonProperties( pVertexStreamDefinition );
		const auto glcVertexStreamDefinition = smutil::translateIAVertexStreamDefinition( pVertexStreamDefinition );

		auto immutableState = createGPUAPIObject<GLIAVertexStreamImmutableState>(
				pGPUDevice,
				vertexStreamCommonProperties,
				glcVertexStreamDefinition );

		return immutableState;
	}


	namespace smutil
	{

		GLIAVertexAttributeInfo translateIAVertexAttributeInfo(
				const IAVertexAttributeInfo & pAttributeInfo )
		{
			GLIAVertexAttributeInfo glcAttributeInfo{};

			glcAttributeInfo.streamIndex = static_cast<GLuint>( pAttributeInfo.streamIndex );
			glcAttributeInfo.instanceRate = pAttributeInfo.instanceRate;
			glcAttributeInfo.relativeOffset = static_cast<uint32>( pAttributeInfo.relativeOffset );
			glcAttributeInfo.byteSize = cxdefs::getVertexAttribFormatByteSize( pAttributeInfo.format );

			const auto attributeFormatBaseType = cxdefs::getVertexAttribFormatBaseDataType( pAttributeInfo.format );
			glcAttributeInfo.baseType = GLCoreAPIProxy::translateGLBaseDataType( attributeFormatBaseType );

			const auto attributeFormatLength = cxdefs::getVertexAttribFormatLength( pAttributeInfo.format );
			glcAttributeInfo.componentsNum = static_cast<uint32>( attributeFormatLength );

			const auto attributeFormatFlags = cxdefs::getVertexAttribFormatFlags( pAttributeInfo.format );
			glcAttributeInfo.normalized = attributeFormatFlags.isSet( E_GPU_DATA_FORMAT_FLAG_NORMALIZED_BIT ) ? GL_TRUE : GL_FALSE;

			return glcAttributeInfo;
		}

		GLIAInputLayoutDefinition translateIAInputLayoutDefinition(
				const IAInputLayoutDefinition & pDefinition )
		{
			GLIAInputLayoutDefinition glcInputLayoutDefinition{};

			const auto definedVertexAttributesNum = popCount( pDefinition.activeAttributesMask );

			uint32 currentVertexAttributesNum = 0;
			uint64 currentAttributePackedRelativeOffset = 0;

			for( uint32 attributeIndex = 0; attributeIndex < cxdefs::GPU_SYSTEM_METRIC_IA_MAX_VERTEX_ATTRIBUTES_NUM; ++attributeIndex )
			{
				const auto attributeBit = cxdefs::makeIAVertexAttributeFlag( attributeIndex );
				if( pDefinition.activeAttributesMask.isSet( attributeBit ) )
				{
					const auto & inputAttributeInfo = pDefinition.attributeArray[attributeIndex];
					auto & glcAttributeInfo = glcInputLayoutDefinition.attributeArray[attributeIndex];

					// Translate the attribute data. This includes the relative offset.
					glcAttributeInfo = translateIAVertexAttributeInfo( inputAttributeInfo );

					if( inputAttributeInfo.relativeOffset == cxdefs::VERTEX_ATTRIBUTE_OFFSET_APPEND )
					{
						// If the offset is APPEND, update it with the current packed offset calculated.
						glcAttributeInfo.relativeOffset = numeric_cast<uint32>( currentAttributePackedRelativeOffset );
					}

					// Update the current packed offset.
					currentAttributePackedRelativeOffset = glcAttributeInfo.relativeOffset + glcAttributeInfo.byteSize;

					++currentVertexAttributesNum;

					if( currentVertexAttributesNum == definedVertexAttributesNum )
					{
						break;
					}
				}
			}

			glcInputLayoutDefinition.activeAttributesMask = pDefinition.activeAttributesMask;
			glcInputLayoutDefinition.primitiveTopology = GLCoreAPIProxy::translateGLPrimitiveTopology( pDefinition.primitiveTopology );

			return glcInputLayoutDefinition;
		}

		GLIAVertexStreamDefinition translateIAVertexStreamDefinition(
				const IAVertexStreamDefinition & pDefinition )
		{
			GLIAVertexStreamDefinition glcVertexStreamDefinition{};

			if( pDefinition.activeBindingsMask.isSetAnyOf( E_IA_VERTEX_STREAM_BINDING_MASK_VERTEX_BUFFER_ALL_BITS ) )
			{
				translateVertexBufferReferences(
						pDefinition.vertexBufferReferences,
						pDefinition.activeBindingsMask,
						glcVertexStreamDefinition.vertexBufferBindings );
			}

			if( pDefinition.activeBindingsMask.isSet( E_IA_VERTEX_STREAM_BINDING_FLAG_INDEX_BUFFER_BIT ) )
			{
				translateIndexBufferReference(
						pDefinition.indexBufferReference,
						glcVertexStreamDefinition.indexBufferBinding );
			}

			glcVertexStreamDefinition.activeBindingsMask = ( pDefinition.activeBindingsMask & E_IA_VERTEX_STREAM_BINDING_MASK_ALL );

			return glcVertexStreamDefinition;
		}

		uint32 translateVertexBufferReferences(
				const IAVertexBufferReferenceArray & pVBReferences,
				Bitmask<EIAVertexStreamBindingFlags> pBindingMask,
				GLIAVertexBuffersBindings & pOutGLBindings)
		{
			uint32 activeBindingsNum = 0;

		#if( TS3GX_GL_PLATFORM_TYPE == TS3GX_GL_PLATFORM_TYPE_ES )
			pOutGLBindings.interleavedBindings = {};
		#else
			pOutGLBindings.separateBindings = {};
		#endif

			for( input_assembler_index_t streamIndex = 0; streamIndex < cxdefs::IA_MAX_VERTEX_BUFFER_BINDINGS_NUM; ++streamIndex )
			{
				const auto & inputVertexBufferRef = pVBReferences[streamIndex];
				const auto vbBindingFlag = cxdefs::makeIAVertexBufferFlag( streamIndex );

				if( inputVertexBufferRef && pBindingMask.isSet( vbBindingFlag ) )
				{
					const auto * glcVertexBuffer = inputVertexBufferRef.sourceBuffer->queryInterface<GLGPUBuffer>();

				#if( TS3GX_GL_PLATFORM_TYPE == TS3GX_GL_PLATFORM_TYPE_ES )
					pOutGLBindings.interleavedBindings[streamIndex].handle = glcVertexBuffer->mGLBufferObject->mGLHandle;
					pOutGLBindings.interleavedBindings[streamIndex].offset = numeric_cast<GLintptr>( inputVertexBufferRef.dataOffset() );
					pOutGLBindings.interleavedBindings[streamIndex].stride = numeric_cast<GLsizei>( inputVertexBufferRef.vertexStride );
				#else
					pOutGLBindings.separateBindings.handleArray[streamIndex] = glcVertexBuffer->mGLBufferObject->mGLHandle;
					pOutGLBindings.separateBindings.offsetArray[streamIndex] = numeric_cast<GLintptr>( inputVertexBufferRef.dataOffset() );
					pOutGLBindings.separateBindings.strideArray[streamIndex] = numeric_cast<GLsizei>( inputVertexBufferRef.vertexStride );
				#endif

					++activeBindingsNum;
				}
				else
				{
				#if( TS3GX_GL_PLATFORM_TYPE == TS3GX_GL_PLATFORM_TYPE_ES )
					pOutGLBindings.interleavedBindings[streamIndex].handle = 0u;
					pOutGLBindings.interleavedBindings[streamIndex].offset = 0u;
					pOutGLBindings.interleavedBindings[streamIndex].stride = 0u;
				#else
					pOutGLBindings.separateBindings.handleArray[streamIndex] = 0u;
					pOutGLBindings.separateBindings.offsetArray[streamIndex] = 0u;
					pOutGLBindings.separateBindings.strideArray[streamIndex] = 0u;
				#endif
				}
			}

			return activeBindingsNum;
		}

		bool translateIndexBufferReference(
				const IAIndexBufferReference & pIBReference,
				GLIAIndexBufferBinding & pOutGLBinding )
		{
			if( pIBReference )
			{
				const auto * glcIndexBuffer = pIBReference.sourceBuffer->queryInterface<GLGPUBuffer>();

				pOutGLBinding.handle = glcIndexBuffer->mGLBufferObject->mGLHandle;
				pOutGLBinding.offset = pIBReference.dataOffset();
				pOutGLBinding.format = GLCoreAPIProxy::translateGLIndexDataFormat( pIBReference.indexFormat );
				pOutGLBinding.elementByteSize = cxdefs::getIndexDataFormatByteSize( pIBReference.indexFormat );

				return true;
			}
			else
			{
				pOutGLBinding.handle = 0;
				pOutGLBinding.offset = 0;
				pOutGLBinding.format = 0;
				pOutGLBinding.elementByteSize = 0;

				return false;
			}
		}

		GLIAVertexBuffersBindings translateVertexBufferReferences(
				const IAVertexBufferReferenceArray & pVBReferences,
				Bitmask<EIAVertexStreamBindingFlags> pBindingMask )
		{
			GLIAVertexBuffersBindings glcVertexBufferBindings{};
			translateVertexBufferReferences( pVBReferences, pBindingMask, glcVertexBufferBindings );
			return glcVertexBufferBindings;
		}

		GLIAIndexBufferBinding translateIndexBufferReference(
				const IAIndexBufferReference & pIBReference )
		{
			GLIAIndexBufferBinding glcIndexBufferBinding{};
			translateIndexBufferReference( pIBReference, glcIndexBufferBinding );
			return glcIndexBufferBinding;
		}

		GLVertexArrayObjectHandle createGLVertexArrayObjectFromLayoutDefinition(
				const GLIAInputLayoutDefinition & pGLLayoutDefinition ) noexcept
		{
			if( !pGLLayoutDefinition.activeAttributesMask.isSetAnyOf( E_IA_VERTEX_ATTRIBUTE_MASK_ALL ) )
			{
				return nullptr;
			}

			auto vertexArrayObject = GLVertexArrayObject::create();

			glBindVertexArray( vertexArrayObject->mGLHandle );
			ts3OpenGLHandleLastError();

			for( uint32 attributeIndex = 0; attributeIndex < cxdefs::IA_MAX_VERTEX_ATTRIBUTES_NUM; ++attributeIndex )
			{
				const auto attributeBit = cxdefs::makeIAVertexAttributeFlag( attributeIndex );
				if( pGLLayoutDefinition.activeAttributesMask.isSet( attributeBit ) )
				{
					const auto & glcAttribute = pGLLayoutDefinition.attributeArray[attributeIndex];

					glEnableVertexAttribArray( attributeIndex );
					ts3OpenGLHandleLastError();

					glVertexAttribFormat(
							attributeIndex,
							glcAttribute.componentsNum,
							glcAttribute.baseType,
							glcAttribute.normalized,
							glcAttribute.relativeOffset );
					ts3OpenGLHandleLastError();

					// NOTE: glVertexAttribDivisor modifies the binding between attribute index and its vertex stream slot.
					// Internally, it does the equivalent of:
					// 1. glVertexBindingDivisor( attributeIndex, instanceRate );
					// 2. glVertexAttribBinding( attributeIndex, attributeIndex );
					// glVertexAttribDivisor( attributeIndex, glcVertexAttribute.instanceRate );
					// For this reason, we use glVertexBindingDivisor() instead.

					glVertexBindingDivisor( attributeIndex, glcAttribute.instanceRate );
					ts3OpenGLHandleLastError();

					// This call has to be executed after any call that implicitly modifies vertex attribute binding.
					// One of the example is glVertexAttribDivisor() above (the actual reason of the old reported crash).
					glVertexAttribBinding( attributeIndex, glcAttribute.streamIndex );
					ts3OpenGLHandleLastError();
				}
			}

			glBindVertexArray( 0 );
			ts3OpenGLHandleLastError();

			return vertexArrayObject;
		}

	}

}