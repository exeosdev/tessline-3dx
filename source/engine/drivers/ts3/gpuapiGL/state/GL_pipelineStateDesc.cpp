
#include "GL_pipelineStateDesc.h"
#include <ts3/gpuapiGL/GL_coreAPIProxy.h>
#include <ts3/gpuapiGL/objects/GL_bufferObject.h>
#include <ts3/gpuapiGL/resources/GL_gpuBuffer.h>
#include <ts3/stdext/memory.h>

namespace ts3::gpuapi
{

	GLVertexDataSourceBinding createGLVertexDataSourceBinding( const VertexDataSourceBinding & pCommonBinding )
	{
		GLVertexDataSourceBinding openglVertexDataSourceBinding;
		openglVertexDataSourceBinding.vertexBufferActiveBindingsNum = 0;
		memZero( openglVertexDataSourceBinding.indexBufferBinding );
		memZero( openglVertexDataSourceBinding.vertexBufferBinding );

		if( pCommonBinding.indexBufferBinding.bufferObject )
		{
			auto * openglIndexBuffer = pCommonBinding.indexBufferBinding.bufferObject->queryInterface<GLGPUBuffer>();
			auto openglIBDataFormat = GLCoreAPIProxy::translateGLBaseDataType( static_cast<EBaseDataType>( pCommonBinding.indexBufferBinding.format ) );
			auto openglIBElementByteSize = ecGetBaseDataTypeByteSize( static_cast<EBaseDataType>( pCommonBinding.indexBufferBinding.format ) );
			openglVertexDataSourceBinding.indexBufferBinding.active = GL_TRUE;
			openglVertexDataSourceBinding.indexBufferBinding.buffer = openglIndexBuffer->mGLBufferObject->mGLHandle;
			openglVertexDataSourceBinding.indexBufferBinding.offset = trunc_numeric_cast<GLintptr>( pCommonBinding.indexBufferBinding.dataOffset );
			openglVertexDataSourceBinding.indexBufferBinding.format = openglIBDataFormat;
			openglVertexDataSourceBinding.indexBufferBinding.elementByteSize = openglIBElementByteSize;
		}

		for( vertex_stream_index_t vertexInputStreamIndex = 0; vertexInputStreamIndex < GPU_SYSTEM_METRIC_IA_MAX_VERTEX_INPUT_STREAMS_NUM; ++vertexInputStreamIndex )
		{
			const auto & vbBindingDesc = pCommonBinding.vertexBufferBindingArray[vertexInputStreamIndex];
			if( vbBindingDesc.bufferObject )
			{
				auto * openglVertexBuffer = vbBindingDesc.bufferObject->queryInterface<GLGPUBuffer>();
			#if( TS3GX_GL_PLATFORM_TYPE == TS3GX_GL_PLATFORM_TYPE_ES )
				openglVertexDataSourceBinding.vertexBufferBinding[vertexInputStreamIndex].active = GL_TRUE;
				openglVertexDataSourceBinding.vertexBufferBinding[vertexInputStreamIndex].buffer = openglVertexBuffer->mGLBufferObject->mGLHandle;
				openglVertexDataSourceBinding.vertexBufferBinding[vertexInputStreamIndex].offset = vbBindingDesc.dataOffset;
				openglVertexDataSourceBinding.vertexBufferBinding[vertexInputStreamIndex].stride = static_cast<GLsizei>( vbBindingDesc.dataStride );
				openglVertexDataSourceBinding.vertexBufferActiveBindingsNum += 1;
			#else
				openglVertexDataSourceBinding.vertexBufferBinding.activeArray[vertexInputStreamIndex] = GL_TRUE;
				openglVertexDataSourceBinding.vertexBufferBinding.bufferArray[vertexInputStreamIndex] = openglVertexBuffer->mGLBufferObject->mGLHandle;
				openglVertexDataSourceBinding.vertexBufferBinding.offsetArray[vertexInputStreamIndex] = trunc_numeric_cast<GLintptr>( vbBindingDesc.dataOffset );
				openglVertexDataSourceBinding.vertexBufferBinding.strideArray[vertexInputStreamIndex] = trunc_numeric_cast<GLsizei>( vbBindingDesc.dataStride );
				openglVertexDataSourceBinding.vertexBufferActiveBindingsNum += 1;
			#endif
			}
		}

	#if( TS3GX_GL_PLATFORM_TYPE == TS3GX_GL_PLATFORM_TYPE_ES )
		openglVertexDataSourceBinding.vertexStreamActiveIndexArray = pCommonBinding.vertexStreamActiveIndexArray;
	#else
		openglVertexDataSourceBinding.vertexStreamActiveRangeList = pCommonBinding.vertexStreamActiveRangeList;
	#endif

		return openglVertexDataSourceBinding;
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
		openglRSDescriptor.configDesc.primitiveFillMode = GLCoreAPIProxy::translateGLPrimitiveFillMode( pConfigDesc.primitiveFillMode );
		openglRSDescriptor.configDesc.triangleFrontFaceOrder = GLCoreAPIProxy::translateGLTriangleVerticesOrder( pConfigDesc.triangleFrontFaceOrder );
		openglRSDescriptor.configDesc.scissorTestState = pConfigDesc.scissorTestState;
		
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
		for( uint32 attributeIndex = 0; attributeIndex < GPU_SYSTEM_METRIC_IA_MAX_VERTEX_ATTRIBUTES_NUM; ++attributeIndex )
		{
			if( const auto & vertexAttributeDesc = pInputFormatDesc.vertexAttributeArray[attributeIndex] )
			{
				auto attributeRelativeOffset = vertexAttributeDesc.relativeOffset;
				if( attributeRelativeOffset == cxVertexAttributeOffsetPackedAppend )
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

		return openglVIFDescriptor;
	}

} // namespace ts3::gpuapi
