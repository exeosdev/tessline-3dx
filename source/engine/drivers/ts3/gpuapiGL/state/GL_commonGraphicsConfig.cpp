
#include "GL_commonGraphicsConfig.h"
#include <ts3/gpuapiGL/GL_apiTranslationLayer.h>
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

			const auto commonBlendSettings = smutil::translateRTColorAttachmentBlendSettings( pConfig.attachments[0] );

			foreachRTAttachmentIndex( pConfig.attachmentsMask,
				[&]( native_uint pIndex, ERTAttachmentFlags pAttachmentBit )
				{
					auto & openglAttachmentSettings = openglBlendConfig.attachments[pIndex];
					if( pConfig.flags.isSet( E_BLEND_CONFIG_FLAG_ENABLE_MRT_INDEPENDENT_BLENDING_BIT ) )
					{
						const auto & inputAttachmentSettings = pConfig.attachments[pIndex];
						openglAttachmentSettings = smutil::translateRTColorAttachmentBlendSettings( inputAttachmentSettings );
					}
					else
					{
						openglAttachmentSettings = commonBlendSettings;
					}
					openglAttachmentSettings.blendActive = 1;
					return true;
				} );

			openglBlendConfig.constantColor = pConfig.constantColor;
			openglBlendConfig.flags = pConfig.flags;

			return openglBlendConfig;
		}

		GLDepthStencilConfig translateDepthStencilConfig( const DepthStencilConfig & pConfig )
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

		GLRasterizerConfig translateRasterizerConfig( const RasterizerConfig & pConfig )
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

		GLRTColorAttachmentBlendSettings translateRTColorAttachmentBlendSettings( const RTColorAttachmentBlendSettings & pSettings )
		{
			GLRTColorAttachmentBlendSettings openglBlendSettings{};

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
