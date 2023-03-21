
#include "GL_commonGraphicsConfig.h"
#include <ts3/gpuapiGL/GL_apiTranslationLayer.h>
#include <ts3/gpuapiGL/GL_gpuDevice.h>

namespace ts3::gpuapi
{

	GLBlendImmutableState::GLBlendImmutableState(
			GLGPUDevice & pGPUDevice,
			Bitmask<EBlendConfigFlags> pBlendFlags,
			const GLBlendConfig & pGLBlendConfig )
	: BlendImmutableState( pGPUDevice, pBlendFlags )
	, mGLBlendConfig( pGLBlendConfig )
	{}

	GLBlendImmutableState::~GLBlendImmutableState() = default;

	GpaHandle<GLBlendImmutableState> GLBlendImmutableState::createInstance(
			GLGPUDevice & pGPUDevice,
			const BlendConfig & pBlendConfig )
	{
		const auto glcBlendConfig = smutil::translateBlendConfigGL( pBlendConfig );

		auto immutableState = createGPUAPIObject<GLBlendImmutableState>(
				pGPUDevice,
				pBlendConfig.flags,
				glcBlendConfig );

		return immutableState;
	}


	GLDepthStencilImmutableState::GLDepthStencilImmutableState(
			GLGPUDevice & pGPUDevice,
			Bitmask<EDepthStencilConfigFlags> pDepthStencilFlags,
			const GLDepthStencilConfig & pGLDepthStencilConfig  )
	: DepthStencilImmutableState( pGPUDevice, pDepthStencilFlags )
	, mGLDepthStencilConfig( pGLDepthStencilConfig )
	{}

	GLDepthStencilImmutableState::~GLDepthStencilImmutableState() = default;

	GpaHandle<GLDepthStencilImmutableState> GLDepthStencilImmutableState::createInstance(
			GLGPUDevice & pGPUDevice,
			const DepthStencilConfig & pDepthStencilConfig  )
	{
		const auto glcDepthStencilConfig = smutil::translateDepthStencilConfigGL( pDepthStencilConfig );

		auto immutableState = createGPUAPIObject<GLDepthStencilImmutableState>(
				pGPUDevice,
				pDepthStencilConfig.commonFlags,
				glcDepthStencilConfig );

		return immutableState;
	}


	GLRasterizerImmutableState::GLRasterizerImmutableState(
			GLGPUDevice & pGPUDevice,
			Bitmask<ERasterizerConfigFlags> pRasterizerFlags,
			const GLRasterizerConfig & pGLRasterizerConfig )
	: RasterizerImmutableState( pGPUDevice, pRasterizerFlags )
	, mGLRasterizerConfig( pGLRasterizerConfig )
	{}

	GLRasterizerImmutableState::~GLRasterizerImmutableState() = default;

	GpaHandle<GLRasterizerImmutableState> GLRasterizerImmutableState::createInstance(
			GLGPUDevice & pGPUDevice,
			const RasterizerConfig & pRasterizerConfig )
	{
		const auto glcRasterizerConfig = smutil::translateRasterizerConfigGL( pRasterizerConfig );

		auto immutableState = createGPUAPIObject<GLRasterizerImmutableState>(
				pGPUDevice,
				pRasterizerConfig.flags,
				glcRasterizerConfig );

		return immutableState;
	}


	namespace smutil
	{

		GLBlendConfig translateBlendConfigGL( const BlendConfig & pConfig )
		{
			GLBlendConfig openglBlendConfig{};
			openglBlendConfig.attachmentsMask = pConfig.attachmentsMask;
			openglBlendConfig.flags = pConfig.flags;

			if( pConfig.attachmentsMask != 0 )
			{
				if( !pConfig.flags.isSet( E_BLEND_CONFIG_FLAG_ENABLE_MRT_INDEPENDENT_BLENDING_BIT ) )
				{
					openglBlendConfig.attachments[0] = smutil::translateRTColorAttachmentBlendSettingsGL( pConfig.attachments[0] );
				}
				else
				{
					foreachRTAttachmentIndex( pConfig.attachmentsMask,
						[&]( native_uint pIndex, ERTAttachmentFlags pAttachmentBit )
						{
							auto & openglAttachmentSettings = openglBlendConfig.attachments[pIndex];
							openglAttachmentSettings = smutil::translateRTColorAttachmentBlendSettingsGL( pConfig.attachments[pIndex] );
							openglAttachmentSettings.blendActive = true;
							return true;
						} );
				}

				openglBlendConfig.constantColor = pConfig.constantColor;
			}

			return openglBlendConfig;
		}

		GLDepthStencilConfig translateDepthStencilConfigGL( const DepthStencilConfig & pConfig )
		{
			GLDepthStencilConfig openglDepthStencilConfig{};

			auto & depthSettings = pConfig.depthTestSettings;
			openglDepthStencilConfig.depthTestActive = pConfig.commonFlags.isSet( E_DEPTH_STENCIL_CONFIG_FLAG_ENABLE_DEPTH_TEST_BIT );
			openglDepthStencilConfig.depthSettings.depthCompFunc = atl::translateGLCompFunc( depthSettings.depthCompFunc );
			openglDepthStencilConfig.depthSettings.writeMask = ( depthSettings.depthWriteMask == EDepthWriteMask::All ) ? GL_TRUE : GL_FALSE;

			openglDepthStencilConfig.stencilTestActive = pConfig.commonFlags.isSet( E_DEPTH_STENCIL_CONFIG_FLAG_ENABLE_STENCIL_TEST_BIT );

			auto & stencilBackFaceDesc = pConfig.stencilTestSettings.backFace;
			openglDepthStencilConfig.stencilSettings.backFace.compFunc = atl::translateGLCompFunc( stencilBackFaceDesc.compFunc );
			openglDepthStencilConfig.stencilSettings.backFace.opFail = atl::translateGLStencilOp( stencilBackFaceDesc.opFail );
			openglDepthStencilConfig.stencilSettings.backFace.opPassDepthFail = atl::translateGLStencilOp( stencilBackFaceDesc.opPassDepthFail );
			openglDepthStencilConfig.stencilSettings.backFace.opPassDepthPass = atl::translateGLStencilOp( stencilBackFaceDesc.opPassDepthPass );
			openglDepthStencilConfig.stencilSettings.backFace.readMask = pConfig.stencilTestSettings.readMask;
			openglDepthStencilConfig.stencilSettings.backFace.refValue = pConfig.stencilTestSettings.refValue;
			openglDepthStencilConfig.stencilSettings.backFace.writeMask = pConfig.stencilTestSettings.writeMask;

			auto & stencilFrontFaceDesc = pConfig.stencilTestSettings.frontFace;
			openglDepthStencilConfig.stencilSettings.frontFace.compFunc = atl::translateGLCompFunc( stencilFrontFaceDesc.compFunc );
			openglDepthStencilConfig.stencilSettings.frontFace.opFail = atl::translateGLStencilOp( stencilFrontFaceDesc.opFail );
			openglDepthStencilConfig.stencilSettings.frontFace.opPassDepthFail = atl::translateGLStencilOp( stencilFrontFaceDesc.opPassDepthFail );
			openglDepthStencilConfig.stencilSettings.frontFace.opPassDepthPass = atl::translateGLStencilOp( stencilFrontFaceDesc.opPassDepthPass );
			openglDepthStencilConfig.stencilSettings.frontFace.readMask = pConfig.stencilTestSettings.readMask;
			openglDepthStencilConfig.stencilSettings.frontFace.refValue = pConfig.stencilTestSettings.refValue;
			openglDepthStencilConfig.stencilSettings.frontFace.writeMask = pConfig.stencilTestSettings.writeMask;

			return openglDepthStencilConfig;
		}

		GLRasterizerConfig translateRasterizerConfigGL( const RasterizerConfig & pConfig )
		{
			GLRasterizerConfig rasterizerConfig{};

			rasterizerConfig.scissorTestActive = pConfig.flags.isSet( E_RASTERIZER_CONFIG_FLAG_ENABLE_SCISSOR_TEST_BIT );
			rasterizerConfig.cullMode = atl::translateGLCullMode( pConfig.cullMode );
			rasterizerConfig.frontFaceVerticesOrder = atl::translateGLTriangleVerticesOrder( pConfig.frontFaceVerticesOrder );
		#if( TS3GX_GL_FEATURE_SUPPORT_PRIMITIVE_FILL_MODE )
			rasterizerConfig.primitiveFillMode = atl::translateGLPrimitiveFillMode( pConfig.primitiveFillMode );
		#endif

			return rasterizerConfig;
		}

		GLRTColorAttachmentBlendSettings translateRTColorAttachmentBlendSettingsGL( const RTColorAttachmentBlendSettings & pSettings )
		{
			GLRTColorAttachmentBlendSettings openglBlendSettings;
			openglBlendSettings.equation.rgb = atl::translateGLBlendOp( pSettings.opColor );
			openglBlendSettings.equation.alpha = atl::translateGLBlendOp( pSettings.opAlpha );
			openglBlendSettings.factor.rgbSrc = atl::translateGLBlendFactor( pSettings.factorSrcColor );
			openglBlendSettings.factor.rgbDst = atl::translateGLBlendFactor( pSettings.factorDstColor );
			openglBlendSettings.factor.alphaSrc = atl::translateGLBlendFactor( pSettings.factorSrcAlpha );
			openglBlendSettings.factor.alphaDst = atl::translateGLBlendFactor( pSettings.factorDstAlpha );

			return openglBlendSettings;
		}

	}

}
