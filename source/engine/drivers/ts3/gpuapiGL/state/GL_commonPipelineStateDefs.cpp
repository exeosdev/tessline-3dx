
#include "GL_commonPipelineStateDefs.h"
#include <ts3/gpuapiGL/GL_coreAPIProxy.h>

namespace ts3::gpuapi
{

	namespace smutil
	{

		GLBlendConfig translateBlendConfig( const BlendConfig & pConfig )
		{
			GLBlendConfig openglBlendConfig{};

			const auto blendActiveAttachmentsNum = popCount( pConfig.attachmentsMask );
			const auto commonBlendSettings = smutil::translateRTColorAttachmentBlendSettings( pConfig.attachments[0] );

			uint32 currentAttachmentsNum = 0;

			for( uint32 attachmentIndex = 0; attachmentIndex < cxdefs::RT_MAX_COLOR_ATTACHMENTS_NUM; ++attachmentIndex )
			{
				const auto attachmentBit = cxdefs::makeRTAttachmentFlag( attachmentIndex );
				if( pConfig.attachmentsMask.isSet( attachmentBit ) )
				{
					auto & openglAttachmentSettings = openglBlendConfig.attachments[attachmentIndex];

					if( pConfig.flags.isSet( E_BLEND_CONFIG_FLAG_ENABLE_MRT_INDEPENDENT_BLENDING_BIT ) )
					{
						const auto & inputAttachmentSettings = pConfig.attachments[attachmentIndex];
						openglAttachmentSettings = smutil::translateRTColorAttachmentBlendSettings( pConfig.attachments[attachmentIndex] );
					}
					else
					{
						openglAttachmentSettings = commonBlendSettings;
					}

					++currentAttachmentsNum;

					if( currentAttachmentsNum == blendActiveAttachmentsNum )
					{
						break;
					}
				}
			}

			return openglBlendConfig;
		}

		GLDepthStencilConfig translateDepthStencilConfig( const DepthStencilConfig & pConfig )
		{
			GLDepthStencilConfig openglDepthStencilConfig{};

			auto & depthSettings = pConfig.depthTestSettings;
			openglDepthStencilConfig.depthSettings.depthCompFunc = GLCoreAPIProxy::translateGLCompFunc( depthSettings.depthCompFunc );
			openglDepthStencilConfig.depthSettings.writeMask = ( depthSettings.depthWriteMask == EDepthWriteMask::All ) ? GL_TRUE : GL_FALSE;

			auto & stencilBackFaceDesc = pConfig.stencilTestSettings.backFace;
			openglDepthStencilConfig.stencilSettings.backFace.compFunc = GLCoreAPIProxy::translateGLCompFunc( stencilBackFaceDesc.compFunc );
			openglDepthStencilConfig.stencilSettings.backFace.opFail = GLCoreAPIProxy::translateGLStencilOp( stencilBackFaceDesc.opFail );
			openglDepthStencilConfig.stencilSettings.backFace.opPassDepthFail = GLCoreAPIProxy::translateGLStencilOp( stencilBackFaceDesc.opPassDepthFail );
			openglDepthStencilConfig.stencilSettings.backFace.opPassDepthPass = GLCoreAPIProxy::translateGLStencilOp( stencilBackFaceDesc.opPassDepthPass );
			openglDepthStencilConfig.stencilSettings.backFace.readMask = pConfig.stencilTestSettings.readMask;
			openglDepthStencilConfig.stencilSettings.backFace.refValue = pConfig.stencilTestSettings.refValue;
			openglDepthStencilConfig.stencilSettings.backFace.writeMask = pConfig.stencilTestSettings.writeMask;

			auto & stencilFrontFaceDesc = pConfig.stencilTestSettings.frontFace;
			openglDepthStencilConfig.stencilSettings.frontFace.compFunc = GLCoreAPIProxy::translateGLCompFunc( stencilFrontFaceDesc.compFunc );
			openglDepthStencilConfig.stencilSettings.frontFace.opFail = GLCoreAPIProxy::translateGLStencilOp( stencilFrontFaceDesc.opFail );
			openglDepthStencilConfig.stencilSettings.frontFace.opPassDepthFail = GLCoreAPIProxy::translateGLStencilOp( stencilFrontFaceDesc.opPassDepthFail );
			openglDepthStencilConfig.stencilSettings.frontFace.opPassDepthPass = GLCoreAPIProxy::translateGLStencilOp( stencilFrontFaceDesc.opPassDepthPass );
			openglDepthStencilConfig.stencilSettings.frontFace.readMask = pConfig.stencilTestSettings.readMask;
			openglDepthStencilConfig.stencilSettings.frontFace.refValue = pConfig.stencilTestSettings.refValue;
			openglDepthStencilConfig.stencilSettings.frontFace.writeMask = pConfig.stencilTestSettings.writeMask;

			return openglDepthStencilConfig;
		}

		GLIAInputLayoutDefinition translateIAInputLayoutDefinition( const IAInputLayoutDefinition & pDefinition )
		{
			GLIAInputLayoutDefinition openglLayoutDefinition{};

			const auto definedVertexAttributesNum = popCount( pDefinition.activeAttributesMask );

			uint32 currentVertexAttributesNum = 0;
			uint64 currentAttributePackedRelativeOffset = 0;

			for( uint32 attributeIndex = 0; attributeIndex < cxdefs::GPU_SYSTEM_METRIC_IA_MAX_VERTEX_ATTRIBUTES_NUM; ++attributeIndex )
			{
				const auto attributeBit = cxdefs::makeIAVertexAttributeFlag( attributeIndex );
				if( pDefinition.activeAttributesMask.isSet( attributeBit ) )
				{
					const auto & inputAttributeInfo = pDefinition.attributeArray[attributeIndex];
					auto & openglAttributeInfo = openglLayoutDefinition.attributeArray[attributeIndex];

					// Translate the attribute data. This includes the relative offset.
					openglAttributeInfo = translateIAVertexAttributeInfo( inputAttributeInfo );

					if( inputAttributeInfo.relativeOffset == cxdefs::VERTEX_ATTRIBUTE_OFFSET_APPEND )
					{
						// If the offset is APPEND, update it with the current packed offset calculated.
						openglAttributeInfo.relativeOffset = trunc_numeric_cast<uint32>( currentAttributePackedRelativeOffset );
					}

					// Update the current packed offset.
					currentAttributePackedRelativeOffset = openglAttributeInfo.relativeOffset + openglAttributeInfo.byteSize;

					++currentVertexAttributesNum;

					if( currentVertexAttributesNum == definedVertexAttributesNum )
					{
						break;
					}
				}
			}

			openglLayoutDefinition.activeAttributesMask = pDefinition.activeAttributesMask;
			openglLayoutDefinition.primitiveTopology = GLCoreAPIProxy::translateGLPrimitiveTopology( pDefinition.primitiveTopology );

			return openglLayoutDefinition;
		}

		GLIAVertexAttributeInfo translateIAVertexAttributeInfo( const IAVertexAttributeInfo & pAttributeInfo )
		{
			GLIAVertexAttributeInfo openglAttributeInfo{};

			openglAttributeInfo.streamIndex = static_cast<GLuint>( pAttributeInfo.streamIndex );
			openglAttributeInfo.instanceRate = pAttributeInfo.instanceRate;
			openglAttributeInfo.relativeOffset = static_cast<uint32>( pAttributeInfo.relativeOffset );
			openglAttributeInfo.byteSize = cxdefs::getVertexAttribFormatByteSize( pAttributeInfo.format );

			const auto attributeFormatBaseType = cxdefs::getVertexAttribFormatBaseDataType( pAttributeInfo.format );
			openglAttributeInfo.baseType = GLCoreAPIProxy::translateGLBaseDataType( attributeFormatBaseType );

			const auto attributeFormatLength = cxdefs::getVertexAttribFormatLength( pAttributeInfo.format );
			openglAttributeInfo.componentsNum = static_cast<uint32>( attributeFormatLength );

			const auto attributeFormatFlags = cxdefs::getVertexAttribFormatFlags( pAttributeInfo.format );
			openglAttributeInfo.normalized = attributeFormatFlags.isSet( E_GPU_DATA_FORMAT_FLAG_NORMALIZED_BIT ) ? GL_TRUE : GL_FALSE;

			return openglAttributeInfo;
		}

		GLRasterizerConfig translateRasterizerConfig( const RasterizerConfig & pConfig )
		{
			GLRasterizerConfig rasterizerConfig{};

			rasterizerConfig.cullMode = GLCoreAPIProxy::translateGLCullMode( pConfig.cullMode );
			rasterizerConfig.frontFaceVerticesOrder = GLCoreAPIProxy::translateGLTriangleVerticesOrder( pConfig.frontFaceVerticesOrder );
			rasterizerConfig.scissorTestState = pConfig.flags.isSet( E_RASTERIZER_FLAG_ENABLE_SCISSOR_TEST_BIT );
		#if( TS3GX_GL_FEATURE_SUPPORT_PRIMITIVE_FILL_MODE )
			rasterizerConfig.primitiveFillMode = GLCoreAPIProxy::translateGLPrimitiveFillMode( pConfig.primitiveFillMode );
		#endif

			return rasterizerConfig;
		}

		GLRTColorAttachmentBlendSettings translateRTColorAttachmentBlendSettings( const RTColorAttachmentBlendSettings & pSettings )
		{
			GLRTColorAttachmentBlendSettings openglBlendSettings{};

			openglBlendSettings.constantFactor = pSettings.constantFactor;
			openglBlendSettings.colorEquation = GLCoreAPIProxy::translateGLBlendOp( pSettings.opColor );
			openglBlendSettings.alphaEquation = GLCoreAPIProxy::translateGLBlendOp( pSettings.opAlpha );
			openglBlendSettings.srcColorFactor = GLCoreAPIProxy::translateGLBlendFactor( pSettings.factorSrcColor );
			openglBlendSettings.dstColorFactor = GLCoreAPIProxy::translateGLBlendFactor( pSettings.factorDstColor );
			openglBlendSettings.srcAlphaFactor = GLCoreAPIProxy::translateGLBlendFactor( pSettings.factorSrcAlpha );
			openglBlendSettings.dstAlphaFactor = GLCoreAPIProxy::translateGLBlendFactor( pSettings.factorDstAlpha );

			return openglBlendSettings;
		}

	}

}
