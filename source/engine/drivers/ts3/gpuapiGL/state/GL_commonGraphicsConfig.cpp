
#include "GL_commonGraphicsConfig.h"
#include <ts3/gpuapiGL/GL_coreAPIProxy.h>
#include <ts3/gpuapiGL/GL_gpuDevice.h>

namespace ts3::gpuapi
{

	GLBlendImmutableState::GLBlendImmutableState(
			GLGPUDevice & pGPUDevice,
			const GLBlendConfig & pGLBlendConfig,
			Bitmask<EBlendConfigFlags> pBlendFlags )
	: BlendImmutableState( pGPUDevice, pBlendFlags )
	, mGLBlendConfig( pGLBlendConfig )
	{}

	GLBlendImmutableState::~GLBlendImmutableState() = default;

	GpaHandle<GLBlendImmutableState> GLBlendImmutableState::createInstance(
			GLGPUDevice & pGPUDevice,
			const BlendConfig & pBlendConfig )
	{
		const auto glcBlendConfig = smutil::translateBlendConfig( pBlendConfig );
		auto immutableState = createGPUAPIObject<GLBlendImmutableState>(
				pGPUDevice,
				glcBlendConfig,
				pBlendConfig.flags & E_BLEND_CONFIG_MASK_ALL );
		return immutableState;
	}


	GLDepthStencilImmutableState::GLDepthStencilImmutableState(
			GLGPUDevice & pGPUDevice,
			const GLDepthStencilConfig & pGLDepthStencilConfig,
			Bitmask<EDepthStencilConfigFlags> pDepthStencilFlags  )
	: DepthStencilImmutableState( pGPUDevice, pDepthStencilFlags )
	, mGLDepthStencilConfig( pGLDepthStencilConfig )
	{}

	GLDepthStencilImmutableState::~GLDepthStencilImmutableState() = default;

	GpaHandle<GLDepthStencilImmutableState> GLDepthStencilImmutableState::createInstance(
			GLGPUDevice & pGPUDevice,
			const DepthStencilConfig & pDepthStencilConfig  )
	{
		const auto glcDepthStencilConfig = smutil::translateDepthStencilConfig( pDepthStencilConfig );
		auto immutableState = createGPUAPIObject<GLDepthStencilImmutableState>(
				pGPUDevice,
				glcDepthStencilConfig,
				pDepthStencilConfig.commonFlags & E_DEPTH_STENCIL_CONFIG_MASK_ALL );
		return immutableState;
	}


	GLRasterizerImmutableState::GLRasterizerImmutableState(
			GLGPUDevice & pGPUDevice,
			const GLRasterizerConfig & pGLRasterizerConfig,
			Bitmask<ERasterizerConfigFlags> pRasterizerFlags )
	: RasterizerImmutableState( pGPUDevice, pRasterizerFlags )
	, mGLRasterizerConfig( pGLRasterizerConfig )
	{}

	GLRasterizerImmutableState::~GLRasterizerImmutableState() = default;

	GpaHandle<GLRasterizerImmutableState> GLRasterizerImmutableState::createInstance(
			GLGPUDevice & pGPUDevice,
			const RasterizerConfig & pRasterizerConfig )
	{
		const auto glcRasterizerConfig = smutil::translateRasterizerConfig( pRasterizerConfig );
		auto immutableState = createGPUAPIObject<GLRasterizerImmutableState>(
				pGPUDevice,
				glcRasterizerConfig,
				pRasterizerConfig.flags & E_RASTERIZER_CONFIG_MASK_ALL );
		return immutableState;
	}


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

		GLRasterizerConfig translateRasterizerConfig( const RasterizerConfig & pConfig )
		{
			GLRasterizerConfig rasterizerConfig{};

			rasterizerConfig.flags = ( pConfig.flags & E_RASTERIZER_CONFIG_MASK_ALL );
			rasterizerConfig.cullMode = GLCoreAPIProxy::translateGLCullMode( pConfig.cullMode );
			rasterizerConfig.frontFaceVerticesOrder = GLCoreAPIProxy::translateGLTriangleVerticesOrder( pConfig.frontFaceVerticesOrder );
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
