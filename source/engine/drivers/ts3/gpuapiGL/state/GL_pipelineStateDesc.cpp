
#include "GL_pipelineStateDesc.h"
#include <ts3/gpuapiGL/GL_coreAPIProxy.h>
#include <ts3/gpuapiGL/objects/GL_bufferObject.h>
#include <ts3/gpuapiGL/objects/GL_vertexArrayObject.h>
#include <ts3/gpuapiGL/resources/GL_gpuBuffer.h>
#include <ts3/stdext/memory.h>

namespace ts3::gpuapi
{

	GLVertexStreamBindingDescriptor createGLVertexStreamBindingDescriptor( const VertexDataSourceBinding & pCommonBinding )
	{
		GLVertexStreamBindingDescriptor openglVSBDescriptor;
		openglVSBDescriptor.vertexBufferActiveBindingsNum = 0;

		if( pCommonBinding.indexBufferBinding.bufferObject )
		{
			auto * openglIndexBuffer = pCommonBinding.indexBufferBinding.bufferObject->queryInterface<GLGPUBuffer>();
			auto iboDataFormat = GLCoreAPIProxy::translateGLBaseDataType( static_cast<EBaseDataType>( pCommonBinding.indexBufferBinding.format ) );
			auto iboElementByteSize = ecGetBaseDataTypeByteSize( static_cast<EBaseDataType>( pCommonBinding.indexBufferBinding.format ) );
			openglVSBDescriptor.indexBufferBinding.handle = openglIndexBuffer->mGLBufferObject->mGLHandle;
			openglVSBDescriptor.indexBufferBinding.offset = trunc_numeric_cast<GLintptr>( pCommonBinding.indexBufferBinding.dataOffset );
			openglVSBDescriptor.indexBufferBinding.format = iboDataFormat;
			openglVSBDescriptor.indexBufferBinding.elementByteSize = iboElementByteSize;
		}
		else
		{
			openglVSBDescriptor.indexBufferBinding.handle = 0u;
			openglVSBDescriptor.indexBufferBinding.offset = 0u;
			openglVSBDescriptor.indexBufferBinding.format = 0u;
			openglVSBDescriptor.indexBufferBinding.elementByteSize = 0u;
		}

		for( vertex_stream_index_t vertexInputStreamIndex = 0; vertexInputStreamIndex < E_GPU_SYSTEM_METRIC_IA_MAX_VERTEX_INPUT_STREAMS_NUM; ++vertexInputStreamIndex )
		{
			const auto & vbBindingDesc = pCommonBinding.vertexBufferBindingArray[vertexInputStreamIndex];
			if( vbBindingDesc.bufferObject )
			{
				auto * openglVertexBuffer = vbBindingDesc.bufferObject->queryInterface<GLGPUBuffer>();
				const auto vboHandle = openglVertexBuffer->mGLBufferObject->mGLHandle;

			#if( TS3GX_GL_PLATFORM_TYPE == TS3GX_GL_PLATFORM_TYPE_ES )
				openglVSBDescriptor.vbBindingArray[vertexInputStreamIndex].handle = vboHandle;
				openglVSBDescriptor.vbBindingArray[vertexInputStreamIndex].offset = vbBindingDesc.dataOffset;
				openglVSBDescriptor.vbBindingArray[vertexInputStreamIndex].stride = static_cast<GLsizei>( vbBindingDesc.dataStride );
				openglVSBDescriptor.vbBindingArrayindingsNum += 1;
			#else
				openglVSBDescriptor.vertexBuffersBinding.vbHandleArray[vertexInputStreamIndex] = vboHandle;
				openglVSBDescriptor.vertexBuffersBinding.vbOffsetArray[vertexInputStreamIndex] = trunc_numeric_cast<GLintptr>( vbBindingDesc.dataOffset );
				openglVSBDescriptor.vertexBuffersBinding.vbStrideArray[vertexInputStreamIndex] = trunc_numeric_cast<GLsizei>( vbBindingDesc.dataStride );
				openglVSBDescriptor.vertexBufferActiveBindingsNum += 1;
			#endif
			}
			else
			{
			#if( TS3GX_GL_PLATFORM_TYPE == TS3GX_GL_PLATFORM_TYPE_ES )
				openglVSBDescriptor.vbBindingArray[vertexInputStreamIndex].handle = 0u;
				openglVSBDescriptor.vbBindingArray[vertexInputStreamIndex].offset = 0u;
				openglVSBDescriptor.vbBindingArray[vertexInputStreamIndex].stride = 0u;
			#else
				openglVSBDescriptor.vertexBuffersBinding.vbHandleArray[vertexInputStreamIndex] = 0u;
				openglVSBDescriptor.vertexBuffersBinding.vbOffsetArray[vertexInputStreamIndex] = 0u;
				openglVSBDescriptor.vertexBuffersBinding.vbStrideArray[vertexInputStreamIndex] = 0u;
			#endif
			}
		}

		return openglVSBDescriptor;
	}

	GLBlendStateDescriptor GLGraphicsPSDCacheTraits::createBlendDescriptor( const BlendConfigDesc & pConfigDesc,
	                                                                        pipeline_input_desc_hash_t pDescHash )
	{
		GLBlendStateDescriptor openglBSDescriptor;

		openglBSDescriptor.configDesc.blendState = pConfigDesc.blendState;
		if( openglBSDescriptor.configDesc.blendState == EActiveState::Enabled )
		{
			openglBSDescriptor.configDesc.constantFactor = pConfigDesc.constantFactor;
			openglBSDescriptor.configDesc.equation.color = GLCoreAPIProxy::translateGLBlendOp( pConfigDesc.opColor );
			openglBSDescriptor.configDesc.equation.alpha = GLCoreAPIProxy::translateGLBlendOp( pConfigDesc.opAlpha );
			openglBSDescriptor.configDesc.factor.srcColor = GLCoreAPIProxy::translateGLBlendFactor( pConfigDesc.factorSrcColor );
			openglBSDescriptor.configDesc.factor.dstColor = GLCoreAPIProxy::translateGLBlendFactor( pConfigDesc.factorDstColor );
			openglBSDescriptor.configDesc.factor.srcAlpha = GLCoreAPIProxy::translateGLBlendFactor( pConfigDesc.factorSrcAlpha );
			openglBSDescriptor.configDesc.factor.dstAlpha = GLCoreAPIProxy::translateGLBlendFactor( pConfigDesc.factorDstAlpha );
		}

		openglBSDescriptor.inputDescHash = pDescHash;
		openglBSDescriptor.descriptorID = computePipelineStateDescriptorID( openglBSDescriptor.configDesc );

		return openglBSDescriptor;
	}

	GLDepthStencilStateDescriptor GLGraphicsPSDCacheTraits::createDepthStencilDescriptor( const DepthStencilConfigDesc & pConfigDesc,
	                                                                                      pipeline_input_desc_hash_t pDescHash )
	{
		GLDepthStencilStateDescriptor openglDSSDescriptor;
		
		openglDSSDescriptor.configDesc.depthTestState = pConfigDesc.depthTestState;
		if( openglDSSDescriptor.configDesc.depthTestState == EActiveState::Enabled )
		{
			auto & depthSettings = pConfigDesc.depthSettings;
			openglDSSDescriptor.configDesc.depthSettings.depthCompFunc = GLCoreAPIProxy::translateGLCompFunc( depthSettings.depthCompFunc );
			openglDSSDescriptor.configDesc.depthSettings.writeMask = ( depthSettings.depthWriteMask == EDepthWriteMask::All ) ? GL_TRUE : GL_FALSE;
		}
		
		openglDSSDescriptor.configDesc.stencilTestState = pConfigDesc.stencilTestState;
		if( openglDSSDescriptor.configDesc.stencilTestState == EActiveState::Enabled )
		{
			auto & stencilBackFaceDesc = pConfigDesc.stencilSettings.backFace;
			openglDSSDescriptor.configDesc.stencilSettings.backFace.compFunc = GLCoreAPIProxy::translateGLCompFunc( stencilBackFaceDesc.compFunc );
			openglDSSDescriptor.configDesc.stencilSettings.backFace.opFail = GLCoreAPIProxy::translateGLStencilOp( stencilBackFaceDesc.opFail );
			openglDSSDescriptor.configDesc.stencilSettings.backFace.opPassDepthFail = GLCoreAPIProxy::translateGLStencilOp( stencilBackFaceDesc.opPassDepthFail );
			openglDSSDescriptor.configDesc.stencilSettings.backFace.opPassDepthPass = GLCoreAPIProxy::translateGLStencilOp( stencilBackFaceDesc.opPassDepthPass );
			openglDSSDescriptor.configDesc.stencilSettings.backFace.readMask = pConfigDesc.stencilSettings.readMask;
			openglDSSDescriptor.configDesc.stencilSettings.backFace.refValue = pConfigDesc.stencilSettings.refValue;
			openglDSSDescriptor.configDesc.stencilSettings.backFace.writeMask = pConfigDesc.stencilSettings.writeMask;
			auto & stencilFrontFaceDesc = pConfigDesc.stencilSettings.frontFace;
			openglDSSDescriptor.configDesc.stencilSettings.frontFace.compFunc = GLCoreAPIProxy::translateGLCompFunc( stencilFrontFaceDesc.compFunc );
			openglDSSDescriptor.configDesc.stencilSettings.frontFace.opFail = GLCoreAPIProxy::translateGLStencilOp( stencilFrontFaceDesc.opFail );
			openglDSSDescriptor.configDesc.stencilSettings.frontFace.opPassDepthFail = GLCoreAPIProxy::translateGLStencilOp( stencilFrontFaceDesc.opPassDepthFail );
			openglDSSDescriptor.configDesc.stencilSettings.frontFace.opPassDepthPass = GLCoreAPIProxy::translateGLStencilOp( stencilFrontFaceDesc.opPassDepthPass );
			openglDSSDescriptor.configDesc.stencilSettings.frontFace.readMask = pConfigDesc.stencilSettings.readMask;
			openglDSSDescriptor.configDesc.stencilSettings.frontFace.refValue = pConfigDesc.stencilSettings.refValue;
			openglDSSDescriptor.configDesc.stencilSettings.frontFace.writeMask = pConfigDesc.stencilSettings.writeMask;
		}

		openglDSSDescriptor.inputDescHash = pDescHash;
		openglDSSDescriptor.descriptorID = computePipelineStateDescriptorID( openglDSSDescriptor.configDesc );

		return openglDSSDescriptor;
	}

	GLRasterizerStateDescriptor GLGraphicsPSDCacheTraits::createRasterizerDescriptor( const RasterizerConfigDesc & pConfigDesc,
	                                                                                  pipeline_input_desc_hash_t pDescHash )
	{
		GLRasterizerStateDescriptor openglRSDescriptor;
		
		openglRSDescriptor.configDesc.cullMode = GLCoreAPIProxy::translateGLCullMode( pConfigDesc.cullMode );
		openglRSDescriptor.configDesc.triangleFrontFaceOrder = GLCoreAPIProxy::translateGLTriangleVerticesOrder( pConfigDesc.triangleFrontFaceOrder );
		openglRSDescriptor.configDesc.scissorTestState = pConfigDesc.scissorTestState;
	#if( TS3GX_GL_FEATURE_SUPPORT_PRIMITIVE_FILL_MODE )
		openglRSDescriptor.configDesc.primitiveFillMode = GLCoreAPIProxy::translateGLPrimitiveFillMode( pConfigDesc.primitiveFillMode );
	#endif

		openglRSDescriptor.inputDescHash = pDescHash;
		openglRSDescriptor.descriptorID = computePipelineStateDescriptorID( openglRSDescriptor.configDesc );

		return openglRSDescriptor;
	}

	GLVertexInputFormatStateDescriptor GLGraphicsPSDCacheTraits::createVertexInputFormatDescriptor( const VertexInputFormatDesc & pInputFormatDesc,
	                                                                                                pipeline_input_desc_hash_t pDescHash )
	{
		GLVertexInputFormatStateDescriptor openglVIFDescriptor;

		uint32 activeVertexAttributesNum = 0;
		uint64 currentAttributePackedRelativeOffset = 0;
		for( uint32 attributeIndex = 0; attributeIndex < E_GPU_SYSTEM_METRIC_IA_MAX_VERTEX_ATTRIBUTES_NUM; ++attributeIndex )
		{
			if( const auto & vertexAttributeDesc = pInputFormatDesc.vertexAttributeArray[attributeIndex] )
			{
				auto attributeRelativeOffset = vertexAttributeDesc.relativeOffset;
				if( attributeRelativeOffset == CX_VERTEX_ATTRIBUTE_OFFSET_PACKED_APPEND )
				{
					attributeRelativeOffset = currentAttributePackedRelativeOffset;
				}

				auto & openglVertexAttribute = openglVIFDescriptor.inputFormatDesc.vertexAttributeArray[activeVertexAttributesNum];
				openglVertexAttribute.attributeIndex = static_cast<GLuint>( vertexAttributeDesc.attributeIndex );
				openglVertexAttribute.streamIndex = static_cast<GLuint>( vertexAttributeDesc.streamIndex );
				openglVertexAttribute.instanceRate = vertexAttributeDesc.instanceRate;
				openglVertexAttribute.relativeOffset = static_cast<uint32>( attributeRelativeOffset );
				auto attributeFormatBaseType = ecGetVertexAttribFormatBaseDataType( vertexAttributeDesc.format );
				auto attributeFormatLength = ecGetVertexAttribFormatLength( vertexAttributeDesc.format );
				auto attributeFormatFlags = ecGetVertexAttribFormatFlags( vertexAttributeDesc.format );
				openglVertexAttribute.baseType = GLCoreAPIProxy::translateGLBaseDataType( attributeFormatBaseType );
				openglVertexAttribute.componentsNum = static_cast<uint32>( attributeFormatLength );
				openglVertexAttribute.normalized = attributeFormatFlags.isSet( E_GPU_DATA_FORMAT_FLAG_NORMALIZED_BIT ) ? GL_TRUE : GL_FALSE;

				auto attributeByteSize = ecGetVertexAttribFormatByteSize( vertexAttributeDesc.format );
				currentAttributePackedRelativeOffset = attributeRelativeOffset + attributeByteSize;

				++activeVertexAttributesNum;
			}
		}

		openglVIFDescriptor.inputFormatDesc.primitiveTopology = GLCoreAPIProxy::translateGLPrimitiveTopology( pInputFormatDesc.primitiveTopology );
		openglVIFDescriptor.inputFormatDesc.activeVertexAttributesNum = activeVertexAttributesNum;
		openglVIFDescriptor.inputDescHash = pDescHash;
		openglVIFDescriptor.descriptorID = computePipelineStateDescriptorID( openglVIFDescriptor.inputFormatDesc );

		auto vertexArrayObject = GLVertexArrayObject::create();

		glBindVertexArray( vertexArrayObject->mGLHandle );
		ts3OpenGLHandleLastError();

		for( uint32 attributeCounter = 0; attributeCounter < openglVIFDescriptor.inputFormatDesc.activeVertexAttributesNum; ++attributeCounter )
		{
			const auto & vertexAttribute = openglVIFDescriptor.inputFormatDesc.vertexAttributeArray[attributeCounter];
			ts3DebugAssert( vertexAttribute );

			glEnableVertexAttribArray( vertexAttribute.attributeIndex );
			ts3OpenGLHandleLastError();

			glVertexAttribFormat( vertexAttribute.attributeIndex, vertexAttribute.componentsNum, vertexAttribute.baseType, vertexAttribute.normalized, vertexAttribute.relativeOffset );
			ts3OpenGLHandleLastError();

			glVertexAttribDivisor( vertexAttribute.attributeIndex, vertexAttribute.instanceRate );
			ts3OpenGLHandleLastError();

			// This must be called *AFTER* glEnableVertexAttribArray and glVertexAttribFormat. TODO: documentation?
			// Moving this up causes crash during an actual draw call on at least Quadro T2000 and RX580 (Win 10).
			glVertexAttribBinding( vertexAttribute.attributeIndex, vertexAttribute.streamIndex );
			ts3OpenGLHandleLastError();
		}

		glBindVertexArray( 0 );
		ts3OpenGLHandleLastError();

		return openglVIFDescriptor;
	}

} // namespace ts3::gpuapi
