
#include "shadowRenderer.h"
#include "../shaderLibrary.h"

#include <ts3/gpuapi/commandContext.h>
#include <ts3/gpuapi/gpuDevice.h>
#include <ts3/gpuapi/resources/renderTargetTexture.h>
#include <ts3/gpuapi/resources/texture.h>
#include <ts3/gpuapi/state/pipelineStateObject.h>
#include <ts3/gpuapi/state/sampler.h>

namespace ts3
{

	ShadowRenderer::ShadowRenderer( ShaderLibrary & pShaderLibrary, const ShadowConfig & pShadowConfig )
	: _gpuDevice( pShaderLibrary.gpuDevice() )
	, _shaderLibrary( pShaderLibrary )
	, _shadowConfig( pShadowConfig )
	{
		setCSLightPosition( { -2.0f, 3.0f, -2.0f } );
		setCSLightTarget( { 0.0f, 0.0f,  5.0f } );
	}

	ShadowRenderer::~ShadowRenderer() = default;

	void ShadowRenderer::createRendererResources()
	{
		initializeResources();
		initializeRenderPassStates();
		initializePipelineStateObjects();
	}

	void ShadowRenderer::setCSLightPosition( math::Vec3f pLightPosition )
	{
		_currentState.vLightPosition = pLightPosition;
	}

	void ShadowRenderer::setCSLightTarget( math::Vec3f pLightTarget )
	{
		_currentState.vLightTarget = pLightTarget;
	}

	void ShadowRenderer::setCSProjectionMatrixLightOrthoDefault()
	{
		_currentState.mLightProjection = math::orthoOffCenterLH( -32.0f, 32.0f, -32.0f, 32.0f, 1.0f, 64.0f );
	}

	void ShadowRenderer::setCSProjectionMatrixLightPerspectiveDefault()
	{
		_currentState.mLightProjection = math::perspectiveAspectLH( math::constants::cxFloatRad90Degree, 1.0f, 1.0f, 24.0f );
	}

	void ShadowRenderer::updateMatricesForLightPass( gpuapi::CommandContext & pCommandContext )
	{
		using namespace ts3::gpuapi;

		ts3DebugAssert( pCommandContext.checkFeatureSupport( E_COMMAND_OBJECT_PROPERTY_MASK_CONTEXT_FAMILY_DIRECT_GRAPHICS ) );
		auto * directGraphicsContext = pCommandContext.queryInterface<CommandContextDirectGraphics>();

		_currentState.mLightView = math::lookAtLH( _currentState.vLightPosition, _currentState.vLightTarget, { 0.0f, 1.0f, 0.0f } );
		_currentState.mLightSpace = math::mul( _currentState.mLightProjection, _currentState.mLightView );

		CBShadowData cbShadowData;
		cbShadowData.m4fLightSpaceMatrix = _currentState.mLightSpace;
		cbShadowData.v4fShadowProperties.x = _shadowConfig.shadowMapSize.width;
		cbShadowData.v4fShadowProperties.y = _shadowConfig.shadowMapSize.height;
		cbShadowData.v4fShadowProperties.z = _shadowConfig.screenSize.width;
		cbShadowData.v4fShadowProperties.w = _shadowConfig.screenSize.height;

		directGraphicsContext->updateBufferDataUpload( *_resources.constantBuffer, cbShadowData );
	}

	void ShadowRenderer::updateMatricesForShadowPass( gpuapi::CommandContext & pCommandContext )
	{
		using namespace ts3::gpuapi;

		ts3DebugAssert( pCommandContext.checkFeatureSupport( E_COMMAND_OBJECT_PROPERTY_MASK_CONTEXT_FAMILY_DIRECT_GRAPHICS ) );
		auto * directGraphicsContext = pCommandContext.queryInterface<CommandContextDirectGraphics>();

		CBShadowData cbShadowData;
		cbShadowData.m4fLightSpaceMatrix = _currentState.mLightSpace;
		cbShadowData.v4fShadowProperties.x = _shadowConfig.shadowMapSize.width;
		cbShadowData.v4fShadowProperties.y = _shadowConfig.shadowMapSize.height;
		cbShadowData.v4fShadowProperties.z = _shadowConfig.screenSize.width;
		cbShadowData.v4fShadowProperties.w = _shadowConfig.screenSize.height;

		directGraphicsContext->updateBufferDataUpload( *_resources.constantBuffer, cbShadowData );
	}

	void ShadowRenderer::beginRenderPass1Light( gpuapi::CommandContext & pCommandContext )
	{
		using namespace ts3::gpuapi;

		ts3DebugAssert( pCommandContext.checkFeatureSupport( E_COMMAND_OBJECT_PROPERTY_MASK_CONTEXT_FAMILY_DIRECT_GRAPHICS ) );
		auto * directGraphicsContext = pCommandContext.queryInterface<CommandContextDirectGraphics>();

		gpuapi::ViewportDesc viewportDescLight;
		viewportDescLight.origin.x = 0;
		viewportDescLight.origin.y = 0;
		viewportDescLight.size.x = _shadowConfig.shadowMapSize.width;
		viewportDescLight.size.y = _shadowConfig.shadowMapSize.height;
		viewportDescLight.depthRange.zNear = 0.0f;
		viewportDescLight.depthRange.zFar = 1.0f;

		directGraphicsContext->setRenderTargetBindingState( _gpuAPIState.rtBindingPass1Light );
		directGraphicsContext->setGraphicsPipelineStateObject( *_gpuAPIState.psoPass1Light );

		directGraphicsContext->beginRenderPass( *_gpuAPIState.renderPass1Light );

		directGraphicsContext->cmdSetViewport( viewportDescLight );
		directGraphicsContext->cmdSetShaderConstantBuffer( 17, *_resources.constantBuffer );
	}

	void ShadowRenderer::beginRenderPass2Shadow( gpuapi::CommandContext & pCommandContext )
	{
		using namespace ts3::gpuapi;

		ts3DebugAssert( pCommandContext.checkFeatureSupport( E_COMMAND_OBJECT_PROPERTY_MASK_CONTEXT_FAMILY_DIRECT_GRAPHICS ) );
		auto * directGraphicsContext = pCommandContext.queryInterface<CommandContextDirectGraphics>();

		gpuapi::ViewportDesc viewportDescLight;
		viewportDescLight.origin.x = 0;
		viewportDescLight.origin.y = 0;
		viewportDescLight.size.x = _shadowConfig.screenSize.width;
		viewportDescLight.size.y = _shadowConfig.screenSize.height;
		viewportDescLight.depthRange.zNear = 0.0f;
		viewportDescLight.depthRange.zFar = 1.0f;

		directGraphicsContext->setGraphicsPipelineStateObject( *_gpuAPIState.psoPass2Shadow );

		directGraphicsContext->beginRenderPass( *_gpuAPIState.renderPass2Shadow );

		directGraphicsContext->cmdSetShaderConstantBuffer( 17, *_resources.constantBuffer );
		directGraphicsContext->cmdSetShaderTextureImage( 27, *_resources.shadowMapTexture );
		directGraphicsContext->cmdSetShaderTextureSampler( 77, *_gpuAPIState.samplerPass2Shadow );
	}

	void ShadowRenderer::endRenderPass( gpuapi::CommandContext & pCommandContext )
	{
		using namespace ts3::gpuapi;

		ts3DebugAssert( pCommandContext.checkFeatureSupport( E_COMMAND_OBJECT_PROPERTY_MASK_CONTEXT_FAMILY_DIRECT_GRAPHICS ) );
		auto * directGraphicsContext = pCommandContext.queryInterface<CommandContextDirectGraphics>();

		directGraphicsContext->endRenderPass();
	}

	void ShadowRenderer::initializeResources()
	{
		using namespace ts3::gpuapi;

		{
			ts3::gpuapi::GPUBufferCreateInfo constantBufferCreateInfo;
			constantBufferCreateInfo.memoryFlags = ts3::gpuapi::E_GPU_MEMORY_ACCESS_FLAG_GPU_READ_BIT;
			constantBufferCreateInfo.resourceFlags = ts3::gpuapi::E_GPU_RESOURCE_CONTENT_FLAG_STATIC_BIT;
			constantBufferCreateInfo.resourceFlags |= ts3::gpuapi::E_GPU_BUFFER_BIND_FLAG_CONSTANT_BUFFER_BIT;
			constantBufferCreateInfo.bufferSize = sizeof( CBShadowData );

			_resources.constantBuffer = _gpuDevice.createGPUBuffer( constantBufferCreateInfo );
		}

		{
			TextureCreateInfo shadowMapTextureCreateInfo;
			shadowMapTextureCreateInfo.texClass = ETextureClass::T2D;
			shadowMapTextureCreateInfo.dimensions.width = _shadowConfig.shadowMapSize.width;
			shadowMapTextureCreateInfo.dimensions.height = _shadowConfig.shadowMapSize.height;
			shadowMapTextureCreateInfo.memoryFlags = E_GPU_MEMORY_ACCESS_FLAG_GPU_READ_BIT;
			shadowMapTextureCreateInfo.resourceFlags =
					E_GPU_RESOURCE_USAGE_FLAG_RENDER_TARGET_DEPTH_BIT | E_GPU_RESOURCE_USAGE_FLAG_SHADER_INPUT_BIT;
			shadowMapTextureCreateInfo.internalFormat = ETextureFormat::D32F;

			_resources.shadowMapTexture = _gpuDevice.createTexture( shadowMapTextureCreateInfo );

			ts3::gpuapi::RenderTargetTextureCreateInfo shadowMapRTTCreateInfo;
			shadowMapRTTCreateInfo.targetTexture = _resources.shadowMapTexture;
			shadowMapRTTCreateInfo.bindFlags =
				E_TEXTURE_BIND_FLAG_RENDER_TARGET_DEPTH_ATTACHMENT_BIT | E_TEXTURE_BIND_FLAG_SHADER_INPUT_SAMPLED_IMAGE_BIT;

			_resources.shadowMapRTT = _gpuDevice.createRenderTargetTexture( shadowMapRTTCreateInfo );

			auto & depthStencilAttachment = _gpuAPIState.rtBindingPass1Light.setDepthStencilAttachmentBinding();
			depthStencilAttachment.attachmentTexture = _resources.shadowMapRTT;
		}

		{
			gpuapi::SamplerCreateInfo samplerPass2ShadowCreateInfo;
			samplerPass2ShadowCreateInfo.samplerConfig = gpuapi::cvSamplerConfigDefault;
			samplerPass2ShadowCreateInfo.samplerConfig.addressModeConfig.coordU = ETextureAddressMode::ClampToColor;
			samplerPass2ShadowCreateInfo.samplerConfig.addressModeConfig.coordV = ETextureAddressMode::ClampToColor;
			samplerPass2ShadowCreateInfo.samplerConfig.addressModeConfig.coordW = ETextureAddressMode::ClampToColor;
			samplerPass2ShadowCreateInfo.samplerConfig.textureCompareMode = ETextureCompareMode::RefToTexture;
			samplerPass2ShadowCreateInfo.samplerConfig.textureCompareFunc = ECompFunc::LessEqual;
			samplerPass2ShadowCreateInfo.samplerConfig.borderColor = math::RGBAColorR32Norm{ 1.0f, 0.0f, 0.0f, 0.0f };

			_gpuAPIState.samplerPass2Shadow = _gpuDevice.createSampler( samplerPass2ShadowCreateInfo );
		}
	}

	void ShadowRenderer::initializeRenderPassStates()
	{
		using namespace ts3::gpuapi;

		{
			RenderPassConfiguration renderPass1LightConfig;
			renderPass1LightConfig.activeAttachmentsMask = E_RT_ATTACHMENT_MASK_DEFAULT_DS_ONLY;
			renderPass1LightConfig.attachmentsActionClearMask = E_RT_ATTACHMENT_MASK_DEFAULT_DS_ONLY;
			renderPass1LightConfig.depthStencilAttachment.renderPassLoadAction = ERenderPassAttachmentLoadAction::Clear;
			renderPass1LightConfig.depthStencilAttachment.renderPassStoreAction = ERenderPassAttachmentStoreAction::Keep;
			renderPass1LightConfig.depthStencilAttachment.clearConfig.depthValue = 1.0f;
			renderPass1LightConfig.depthStencilAttachment.clearConfig.stencilValue = 0;

			_gpuAPIState.renderPass1Light = _gpuDevice.createRenderPassConfigurationImmutableState( renderPass1LightConfig );
		}

		{
			RenderPassConfiguration renderPass2ShadowConfig;
			renderPass2ShadowConfig.activeAttachmentsMask = E_RT_ATTACHMENT_MASK_DEFAULT_C0_DS;
			renderPass2ShadowConfig.attachmentsActionClearMask = E_RT_ATTACHMENT_MASK_DEFAULT_C0_DS;
			renderPass2ShadowConfig.colorAttachments[0].renderPassLoadAction = ERenderPassAttachmentLoadAction::Clear;
			renderPass2ShadowConfig.colorAttachments[0].renderPassStoreAction = ERenderPassAttachmentStoreAction::Keep;
			renderPass2ShadowConfig.colorAttachments[0].clearConfig.colorValue = { 0.0f, 0.0f, 0.0f, 1.0f };
			renderPass2ShadowConfig.depthStencilAttachment.renderPassLoadAction = ERenderPassAttachmentLoadAction::Clear;
			renderPass2ShadowConfig.depthStencilAttachment.renderPassStoreAction = ERenderPassAttachmentStoreAction::Keep;
			renderPass2ShadowConfig.depthStencilAttachment.clearConfig.depthValue = 1.0f;
			renderPass2ShadowConfig.depthStencilAttachment.clearConfig.stencilValue = 0;

			_gpuAPIState.renderPass2Shadow = _gpuDevice.createRenderPassConfigurationImmutableState( renderPass2ShadowConfig );
		}
	}

	void ShadowRenderer::initializePipelineStateObjects()
	{
		using namespace ts3::gpuapi;

		{
			auto vertexShaderPass1 = _shaderLibrary.getShader( "SID_SHADOW_0_PASS1_LIGHT_VS" );
			auto pixelShaderPass1 = _shaderLibrary.getShader( "SID_SHADOW_0_PASS1_LIGHT_PS" );

			gpuapi::GraphicsPipelineStateObjectCreateInfo psoPass1LightCreateInfo;
			psoPass1LightCreateInfo.renderTargetLayout.activeAttachmentsMask = E_RT_ATTACHMENT_MASK_DEFAULT_DS_ONLY;
			psoPass1LightCreateInfo.renderTargetLayout.depthStencilAttachment.format = ETextureFormat::D24UNS8U;
			psoPass1LightCreateInfo.blendConfig = defaults::cvPipelineBlendConfigDefault;
			psoPass1LightCreateInfo.depthStencilConfig = defaults::cvPipelineDepthStencilConfigEnableDepthTest;
			psoPass1LightCreateInfo.rasterizerConfig = defaults::cvPipelineRasterizerConfigDefault;
			psoPass1LightCreateInfo.rasterizerConfig.cullMode = ECullMode::Front;
			psoPass1LightCreateInfo.rasterizerConfig.primitiveFillMode = EPrimitiveFillMode::Solid;
			psoPass1LightCreateInfo.rasterizerConfig.frontFaceVerticesOrder = ETriangleVerticesOrder::CounterClockwise;
			psoPass1LightCreateInfo.inputLayoutDefinition.primitiveTopology = EPrimitiveTopology::TriangleList;
			psoPass1LightCreateInfo.inputLayoutDefinition.activeAttributesMask =
				E_IA_VERTEX_ATTRIBUTE_FLAG_ATTR_0_BIT | E_IA_VERTEX_ATTRIBUTE_FLAG_ATTR_1_BIT | E_IA_VERTEX_ATTRIBUTE_FLAG_ATTR_2_BIT | E_IA_VERTEX_ATTRIBUTE_FLAG_ATTR_3_BIT;
			psoPass1LightCreateInfo.inputLayoutDefinition.attributeArray[0] = { 0, "POSITION", 0, ts3::gpuapi::EVertexAttribFormat::Vec3F32, 0 };
			psoPass1LightCreateInfo.inputLayoutDefinition.attributeArray[1] = { 0, "COLOR", 0, ts3::gpuapi::EVertexAttribFormat::Vec4F32, ts3::gpuapi::cxdefs::VERTEX_ATTRIBUTE_OFFSET_APPEND };
			psoPass1LightCreateInfo.inputLayoutDefinition.attributeArray[2] = { 0, "NORMAL", 0, ts3::gpuapi::EVertexAttribFormat::Vec3F32, ts3::gpuapi::cxdefs::VERTEX_ATTRIBUTE_OFFSET_APPEND };
			psoPass1LightCreateInfo.inputLayoutDefinition.attributeArray[3] = { 0, "TEXCOORD", 0, ts3::gpuapi::EVertexAttribFormat::Vec2F32, ts3::gpuapi::cxdefs::VERTEX_ATTRIBUTE_OFFSET_APPEND };
			psoPass1LightCreateInfo.shaderSet.addShader( vertexShaderPass1 );
			psoPass1LightCreateInfo.shaderSet.addShader( pixelShaderPass1 );
			psoPass1LightCreateInfo.shaderInputSignatureDesc.activeShaderStagesMask = E_SHADER_STAGE_FLAG_GRAPHICS_VERTEX_BIT;
			psoPass1LightCreateInfo.shaderInputSignatureDesc.descriptorSetsNum = 1;
			{
				psoPass1LightCreateInfo.shaderInputSignatureDesc.descriptorSetArray[0].descriptorType = EShaderInputDescriptorType::Resource;
				psoPass1LightCreateInfo.shaderInputSignatureDesc.descriptorSetArray[0].descriptorsNum = 2;

				psoPass1LightCreateInfo.shaderInputSignatureDesc.descriptorSetArray[0].descriptorList[0] = {
					10, EShaderInputDescriptorType::Resource, E_SHADER_STAGE_FLAG_GRAPHICS_VERTEX_BIT
				};
				psoPass1LightCreateInfo.shaderInputSignatureDesc.descriptorSetArray[0].descriptorList[0].uResourceDesc = {
					EShaderInputResourceType::CBVConstantBuffer, 0, 1
				};
				psoPass1LightCreateInfo.shaderInputSignatureDesc.descriptorSetArray[0].descriptorList[1] = {
					17, EShaderInputDescriptorType::Resource, E_SHADER_STAGE_FLAG_GRAPHICS_VERTEX_BIT
				};
				psoPass1LightCreateInfo.shaderInputSignatureDesc.descriptorSetArray[0].descriptorList[1].uResourceDesc = {
					EShaderInputResourceType::CBVConstantBuffer, 7, 1
				};
			}

			_gpuAPIState.psoPass1Light = _gpuDevice.createGraphicsPipelineStateObject( psoPass1LightCreateInfo );
		}

		{
			auto vertexShaderPass2 = _shaderLibrary.getShader( "SID_SHADOW_0_PASS2_SHADOW_VS" );
			auto pixelShaderPass2 = _shaderLibrary.getShader( "SID_SHADOW_0_PASS2_SHADOW_PS" );

			gpuapi::GraphicsPipelineStateObjectCreateInfo psoPass2ShadowCreateInfo;
			psoPass2ShadowCreateInfo.renderTargetLayout.activeAttachmentsMask = E_RT_ATTACHMENT_MASK_DEFAULT_C0_DS;
			psoPass2ShadowCreateInfo.renderTargetLayout.colorAttachments[0].format = ETextureFormat::BGRA8UN;
			psoPass2ShadowCreateInfo.renderTargetLayout.depthStencilAttachment.format = ETextureFormat::D24UNS8U;
			psoPass2ShadowCreateInfo.blendConfig = defaults::cvPipelineBlendConfigDefault;
			psoPass2ShadowCreateInfo.depthStencilConfig = defaults::cvPipelineDepthStencilConfigEnableDepthTest;
			psoPass2ShadowCreateInfo.rasterizerConfig = defaults::cvPipelineRasterizerConfigDefault;
			psoPass2ShadowCreateInfo.rasterizerConfig.cullMode = ECullMode::Back;
			psoPass2ShadowCreateInfo.rasterizerConfig.primitiveFillMode = EPrimitiveFillMode::Solid;
			psoPass2ShadowCreateInfo.rasterizerConfig.frontFaceVerticesOrder = ETriangleVerticesOrder::CounterClockwise;
			psoPass2ShadowCreateInfo.inputLayoutDefinition.primitiveTopology = EPrimitiveTopology::TriangleList;
			psoPass2ShadowCreateInfo.inputLayoutDefinition.activeAttributesMask =
				E_IA_VERTEX_ATTRIBUTE_FLAG_ATTR_0_BIT | E_IA_VERTEX_ATTRIBUTE_FLAG_ATTR_1_BIT | E_IA_VERTEX_ATTRIBUTE_FLAG_ATTR_2_BIT | E_IA_VERTEX_ATTRIBUTE_FLAG_ATTR_3_BIT;
			psoPass2ShadowCreateInfo.inputLayoutDefinition.attributeArray[0] = { 0, "POSITION", 0, ts3::gpuapi::EVertexAttribFormat::Vec3F32, 0 };
			psoPass2ShadowCreateInfo.inputLayoutDefinition.attributeArray[1] = { 0, "COLOR", 0, ts3::gpuapi::EVertexAttribFormat::Vec4F32, ts3::gpuapi::cxdefs::VERTEX_ATTRIBUTE_OFFSET_APPEND };
			psoPass2ShadowCreateInfo.inputLayoutDefinition.attributeArray[2] = { 0, "NORMAL", 0, ts3::gpuapi::EVertexAttribFormat::Vec3F32, ts3::gpuapi::cxdefs::VERTEX_ATTRIBUTE_OFFSET_APPEND };
			psoPass2ShadowCreateInfo.inputLayoutDefinition.attributeArray[3] = { 0, "TEXCOORD", 0, ts3::gpuapi::EVertexAttribFormat::Vec2F32, ts3::gpuapi::cxdefs::VERTEX_ATTRIBUTE_OFFSET_APPEND };
			psoPass2ShadowCreateInfo.shaderSet.addShader( vertexShaderPass2 );
			psoPass2ShadowCreateInfo.shaderSet.addShader( pixelShaderPass2 );
			psoPass2ShadowCreateInfo.shaderInputSignatureDesc.activeShaderStagesMask = E_SHADER_STAGE_MASK_GRAPHICS_VS_PS;
			psoPass2ShadowCreateInfo.shaderInputSignatureDesc.descriptorSetsNum = 2;
			{
				psoPass2ShadowCreateInfo.shaderInputSignatureDesc.descriptorSetArray[0].descriptorType = EShaderInputDescriptorType::Resource;
				psoPass2ShadowCreateInfo.shaderInputSignatureDesc.descriptorSetArray[0].descriptorsNum = 4;

				psoPass2ShadowCreateInfo.shaderInputSignatureDesc.descriptorSetArray[0].descriptorList[0] = {
					10, EShaderInputDescriptorType::Resource, E_SHADER_STAGE_FLAG_GRAPHICS_VERTEX_BIT
				};
				psoPass2ShadowCreateInfo.shaderInputSignatureDesc.descriptorSetArray[0].descriptorList[0].uResourceDesc = {
					EShaderInputResourceType::CBVConstantBuffer, 0, 1
				};
				psoPass2ShadowCreateInfo.shaderInputSignatureDesc.descriptorSetArray[0].descriptorList[1] = {
					17, EShaderInputDescriptorType::Resource, E_SHADER_STAGE_FLAG_GRAPHICS_VERTEX_BIT
				};
				psoPass2ShadowCreateInfo.shaderInputSignatureDesc.descriptorSetArray[0].descriptorList[1].uResourceDesc = {
					EShaderInputResourceType::CBVConstantBuffer, 7, 1
				};
				psoPass2ShadowCreateInfo.shaderInputSignatureDesc.descriptorSetArray[0].descriptorList[2] = {
					20, EShaderInputDescriptorType::Resource, E_SHADER_STAGE_FLAG_GRAPHICS_PIXEL_BIT
				};
				psoPass2ShadowCreateInfo.shaderInputSignatureDesc.descriptorSetArray[0].descriptorList[2].uResourceDesc = {
					EShaderInputResourceType::SRVTextureImage, 0, 1
				};
				psoPass2ShadowCreateInfo.shaderInputSignatureDesc.descriptorSetArray[0].descriptorList[3] = {
					27, EShaderInputDescriptorType::Resource, E_SHADER_STAGE_FLAG_GRAPHICS_PIXEL_BIT
				};
				psoPass2ShadowCreateInfo.shaderInputSignatureDesc.descriptorSetArray[0].descriptorList[3].uResourceDesc = {
					EShaderInputResourceType::SRVTextureImage, 7, 1
				};
			}
			{
				psoPass2ShadowCreateInfo.shaderInputSignatureDesc.descriptorSetArray[1].descriptorType = EShaderInputDescriptorType::Sampler;
				psoPass2ShadowCreateInfo.shaderInputSignatureDesc.descriptorSetArray[1].descriptorsNum = 2;

				psoPass2ShadowCreateInfo.shaderInputSignatureDesc.descriptorSetArray[1].descriptorList[0] =
						{ 70, EShaderInputDescriptorType::Sampler, E_SHADER_STAGE_FLAG_GRAPHICS_PIXEL_BIT };
				psoPass2ShadowCreateInfo.shaderInputSignatureDesc.descriptorSetArray[1].descriptorList[0].uSamplerConfig = { 0 };

				psoPass2ShadowCreateInfo.shaderInputSignatureDesc.descriptorSetArray[1].descriptorList[1] =
						{ 77, EShaderInputDescriptorType::Sampler, E_SHADER_STAGE_FLAG_GRAPHICS_PIXEL_BIT };
				psoPass2ShadowCreateInfo.shaderInputSignatureDesc.descriptorSetArray[1].descriptorList[1].uSamplerConfig = { 7 };
			}

			_gpuAPIState.psoPass2Shadow = _gpuDevice.createGraphicsPipelineStateObject( psoPass2ShadowCreateInfo );
		}
	}

}
