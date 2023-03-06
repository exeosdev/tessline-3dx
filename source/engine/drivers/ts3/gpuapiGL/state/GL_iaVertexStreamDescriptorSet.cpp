
#include "GL_iaVertexStreamDescriptorSet.h"
#include <ts3/gpuapiGL/GL_coreAPIProxy.h>
#include <ts3/gpuapiGL/objects/GL_bufferObject.h>
#include <ts3/gpuapiGL/resources/GL_gpuBuffer.h>

namespace ts3::gpuapi
{

	GLIAVertexStreamDescriptorSet::GLIAVertexStreamDescriptorSet( GPUDevice & pGPUDevice )
	: IAVertexStreamDescriptorSet( pGPUDevice )
	{}

	GLIAVertexStreamDescriptorSet::~GLIAVertexStreamDescriptorSet() = default;

	bool GLIAVertexStreamDescriptorSet::applyDescriptorBindings()
	{
		if( !IAVertexStreamDescriptorSet::applyDescriptorBindings() )
		{
			return false;
		}

		_indexBufferBinding = smutil::translateIndexBufferDescriptor( _indexBufferDescriptor );
		_vertexBuffersBindings = smutil::translateVertexBufferDescriptorArray( _vertexBufferDescriptors );
		_vertexBufferActiveRanges = smutil::generateActiveVertexBufferRanges( _vertexBufferDescriptors, _activeBindingsMask );

		return true;
	}


	namespace smutil
	{

		GLIAIndexBufferBinding translateIndexBufferDescriptor( const IAIndexBufferDescriptor & pDescriptor )
		{
			GLIAIndexBufferBinding openglBinding{};

			if( pDescriptor.valid() )
			{
				const auto * oglIndexBuffer = pDescriptor.sourceBuffer()->queryInterface<GLGPUBuffer>();
				openglBinding.handle = oglIndexBuffer->mGLBufferObject->mGLHandle;
				openglBinding.offset = trunc_numeric_cast<GLintptr>( pDescriptor.dataOffset() );
				openglBinding.format = GLCoreAPIProxy::translateGLIndexDataFormat( pDescriptor.indexFormat() );
				openglBinding.elementByteSize = cxdefs::getIndexDataFormatByteSize( pDescriptor.indexFormat() );
			}
			else
			{
				openglBinding.handle = 0u;
				openglBinding.offset = 0u;
				openglBinding.format = 0u;
				openglBinding.elementByteSize = 0u;
			}

			return openglBinding;
		}

		GLIAVertexBuffersBindings translateVertexBufferDescriptorArray(
				const IAVertexBufferDescriptorArray & pDescriptors,
				Bitmask<EIAVertexStreamBindingFlags> pBindingMask )
		{
			GLIAVertexBuffersBindings openglBindings{};

			for( input_assembler_index_t streamIndex = 0; streamIndex < cxdefs::IA_MAX_VERTEX_BUFFER_BINDINGS_NUM; ++streamIndex )
			{
				const auto & vbDescriptor = pDescriptors[streamIndex];
				const auto vbBindingFlag = cxdefs::makeIAVertexBufferFlag( streamIndex );

				if( vbDescriptor.valid() && pBindingMask.isSet( vbBindingFlag ) )
				{
				#if( TS3GX_GL_PLATFORM_TYPE == TS3GX_GL_PLATFORM_TYPE_ES )
					const auto * oglVertexBuffer = vbDescriptor.sourceBuffer()->queryInterface<GLGPUBuffer>();
					openglBindings.interleavedBindingArray[streamIndex].handle = oglVertexBuffer->mGLBufferObject->mGLHandle;
					openglBindings.interleavedBindingArray[streamIndex].offset = trunc_numeric_cast<GLintptr>( vbDescriptor.dataOffset() );
					openglBindings.interleavedBindingArray[streamIndex].stride = trunc_numeric_cast<GLsizei>( vbDescriptor.dataOffset() );
				#else
					const auto * oglVertexBuffer = vbDescriptor.sourceBuffer()->queryInterface<GLGPUBuffer>();
					openglBindings.separateBindings.handleArray[streamIndex] = oglVertexBuffer->mGLBufferObject->mGLHandle;
					openglBindings.separateBindings.offsetArray[streamIndex] = trunc_numeric_cast<GLintptr>( vbDescriptor.dataOffset() );
					openglBindings.separateBindings.strideArray[streamIndex] = trunc_numeric_cast<GLsizei>( vbDescriptor.dataOffset() );
				#endif
				}
				else
				{
				#if( TS3GX_GL_PLATFORM_TYPE == TS3GX_GL_PLATFORM_TYPE_ES )
					openglBindings.interleavedBindingArray[streamIndex].handle = 0u;
					openglBindings.interleavedBindingArray[streamIndex].offset = 0u;
					openglBindings.interleavedBindingArray[streamIndex].stride = 0u;
				#else
					openglBindings.separateBindings.handleArray[streamIndex] = 0u;
					openglBindings.separateBindings.offsetArray[streamIndex] = 0u;
					openglBindings.separateBindings.strideArray[streamIndex] = 0u;
				#endif
				}
			}

		#if( TS3GX_GL_PLATFORM_TYPE == TS3GX_GL_PLATFORM_TYPE_ES )
			openglBindings.bindingType = EGLVertexBufferBindingType::Interleaved;
		#else
			openglBindings.bindingType = EGLVertexBufferBindingType::Separate;
		#endif

			return openglBindings;
		}
		}

	}

}
