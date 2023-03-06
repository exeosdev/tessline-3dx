
#include "GL_graphicsPipelineImmutableState.h"
#include <ts3/gpuapiGL/GL_coreAPIProxy.h>

namespace ts3::gpuapi
{

	namespace smutil
	{

		GLVertexArrayObjectHandle createGLVertexArrayObjectFromLayoutDefinition(
				const GLIAInputLayoutDefinition & pGLLayoutDefinition ) noexcept
		{
			if( !pGLLayoutDefinition.activeAttributesMask.isSetAnyOf( E_IA_VERTEX_ATTRIBUTE_MASK_ALL_BITS ) )
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
					const auto & openglAttribute = pGLLayoutDefinition.attributeArray[attributeIndex];

					glEnableVertexAttribArray( attributeIndex );
					ts3OpenGLHandleLastError();

					glVertexAttribFormat(
							attributeIndex,
							openglAttribute.componentsNum,
							openglAttribute.baseType,
							openglAttribute.normalized,
							openglAttribute.relativeOffset );
					ts3OpenGLHandleLastError();

					// NOTE: glVertexAttribDivisor modifies the binding between attribute index and its vertex stream slot.
					// Internally, it does the equivalent of:
					// 1. glVertexBindingDivisor( attributeIndex, instanceRate );
					// 2. glVertexAttribBinding( attributeIndex, attributeIndex );
					// glVertexAttribDivisor( attributeIndex, oglVertexAttribute.instanceRate );
					// For this reason, we use glVertexBindingDivisor() instead.

					glVertexBindingDivisor( attributeIndex, openglAttribute.instanceRate );
					ts3OpenGLHandleLastError();

					// This call has to be executed after any call that implicitly modifies vertex attribute binding.
					// One of the example is glVertexAttribDivisor() above (the actual reason of the old reported crash).
					glVertexAttribBinding( attributeIndex, openglAttribute.streamIndex );
					ts3OpenGLHandleLastError();
				}
			}

			glBindVertexArray( 0 );
			ts3OpenGLHandleLastError();

			return vertexArrayObject;
		}

	}
	
}
