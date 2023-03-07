
#pragma once

#ifndef __TS3_GPUAPI_GPU_DEVICE_H__
#define __TS3_GPUAPI_GPU_DEVICE_H__

#include "commonCommandDefs.h"
#include "displayCommon.h"
#include "resources/textureCommon.h"
#include "state/graphicsShaderState.h"
#include "state/renderPassCommon.h"

namespace ts3::gpuapi
{

	class IAIndexBufferDescriptor;
	class IAVertexBufferDescriptor;

	class PipelineImmutableStateCache;
	class PipelineImmutableStateFactory;

	struct PresentationLayerCreateInfo;
	struct RenderTargetTextureCreateInfo;

	struct IAVertexBufferDescriptorBindingDesc;

	enum EGPUDeviceCreateFlags : uint32
	{
		E_GPU_DEVICE_CREATE_FLAG_INIT_DEFAULT_PRESENT_QUEUE_BIT = 0x0004,
		E_GPU_DEVICE_CREATE_FLAGS_DEFAULT = E_GPU_DEVICE_CREATE_FLAG_INIT_DEFAULT_PRESENT_QUEUE_BIT
	};

	enum EGPUDeviceStateFlags : uint32
	{
	};

	struct GPUDeviceCreateInfo
	{
		// DisplayManager * displayManager = nullptr;
		// display_system_id_t adapterOutputID = cvDisplaySystemIDDefault;
		display_system_id_t adapterID = cvDisplaySystemIDDefault;
		Bitmask<EGPUDeviceCreateFlags> flags = E_GPU_DEVICE_CREATE_FLAGS_DEFAULT;
	};

	class TS3_GPUAPI_CLASS GPUDevice : public GPUDriverChildObject
	{
		friend class GPUResource;

	public:
		EGPUDriverID const mGPUDriverID;
		system::SysContextHandle const mSysContext;

		explicit GPUDevice( GPUDriver & pDriver );
		virtual ~GPUDevice();

		TS3_ATTR_NO_DISCARD bool isDebugDevice() const noexcept;

		TS3_ATTR_NO_DISCARD bool isMultiThreadAccessSupported() const noexcept;

		TS3_ATTR_NO_DISCARD bool isResourceActiveRefsTrackingEnabled() const noexcept;

		TS3_ATTR_NO_DISCARD CommandSystem * getCommandSystem() const noexcept;

		TS3_ATTR_NO_DISCARD PresentationLayer * getPresentationLayer() const noexcept;

		TS3_ATTR_NO_DISCARD const math::RGBAColorU8 & getDefaultClearColor() const noexcept;

		TS3_ATTR_NO_DISCARD const RenderTargetAttachmentClearConfig & getDefaultClearConfig() const noexcept;

		GPUBufferHandle createGPUBuffer( const GPUBufferCreateInfo & pCreateInfo );
		SamplerHandle createSampler( const SamplerCreateInfo & pCreateInfo );
		ShaderHandle createShader( const ShaderCreateInfo & pCreateInfo );
		TextureHandle createTexture( const TextureCreateInfo & pCreateInfo );

		/// @brief Creates an RTT using the provided CIS.
		/// This function will automatically create a required resource, depending on the specified layout and usage.
		/// It can either be an explicit texture object which can be retrieved later or an implicit render buffer
		/// (e.g. if the RTT is supposed to be only a depth/stencil attachment used for depth and/or stencil testing).
		RenderTargetTextureHandle createRenderTargetTexture( const RenderTargetTextureCreateInfo & pCreateInfo );

		/// @brief
		GraphicsPipelineStateObjectHandle createGraphicsPipelineStateObject( const GraphicsPipelineStateObjectCreateInfo & pCreateInfo );

		BlendImmutableStateHandle createBlendImmutableState( const BlendConfig & pConfig );
		DepthStencilImmutableStateHandle createDepthStencilImmutableState( const DepthStencilConfig & pConfig );
		GraphicsShaderLinkageImmutableStateHandle createGraphicsShaderLinkageImmutableState( const GraphicsShaderSet & pShaderSet );
		IAInputLayoutImmutableStateHandle createIAInputLayoutImmutableState( const IAInputLayoutDefinition & pDefinition );
		IAVertexStreamImmutableStateHandle createIAVertexStreamImmutableState( const IAVertexStreamDefinition & pDefinition );
		RasterizerImmutableStateHandle createRasterizerImmutableState( const RasterizerConfig & pConfig );
		RenderTargetBindingImmutableStateHandle createRenderTargetBindingImmutableState( const RenderTargetBindingDefinition & pDefinition );
		RenderPassImmutableStateHandle createRenderPassImmutableState( const RenderPassConfiguration & pConfiguration );

		BlendImmutableStateHandle createBlendImmutableStateCached(
				const UniqueGPUObjectName & pUniqueName,
				const BlendConfig & pConfig );

		DepthStencilImmutableStateHandle createDepthStencilImmutableStateCached(
				const UniqueGPUObjectName & pUniqueName,
				const DepthStencilConfig & pConfig );

		GraphicsShaderLinkageImmutableStateHandle createGraphicsShaderLinkageImmutableStateCached(
				const UniqueGPUObjectName & pUniqueName,
				const GraphicsShaderSet & pShaderSet );

		IAInputLayoutImmutableStateHandle createIAInputLayoutImmutableStateCached(
				const UniqueGPUObjectName & pUniqueName,
				const IAInputLayoutDefinition & pDefinition );

		IAVertexStreamImmutableStateHandle createIAVertexStreamImmutableStateCached(
				const UniqueGPUObjectName & pUniqueName,
				const IAVertexStreamDefinition & pDefinition );

		RasterizerImmutableStateHandle createRasterizerImmutableStateCached(
				const UniqueGPUObjectName & pUniqueName,
				const RasterizerConfig & pConfig );

		RenderTargetBindingImmutableStateHandle createRenderTargetBindingImmutableStateCached(
				const UniqueGPUObjectName & pUniqueName,
				const RenderTargetBindingDefinition & pDefinition );

		RenderPassImmutableStateHandle createRenderPassImmutableStateCached(
				const UniqueGPUObjectName & pUniqueName,
				const RenderPassConfiguration & pConfiguration );

		void resetImmutableStateCache( Bitmask<EPipelineImmutableStateTypeFlags> pResetMask = E_PIPELINE_IMMUTABLE_STATE_TYPE_MASK_ALL );

		virtual void waitForCommandSync( CommandSync & pCommandSync ) = 0;

		void setPresentationLayer( PresentationLayerHandle pPresentationLayer );

	protected:
        /// @brief API-level initialization of the command system. Called by the parent driver when a device is created.
		virtual void initializeCommandSystem() = 0;

		/// @brief API-level initialization of the state factory. Called by the parent driver when a device is created.
		virtual void initializeImmutableStateFactory() = 0;

		virtual bool onGPUResourceActiveRefsZero( GPUResource & pGPUResource );

		void setImmutableStateFactory( std::unique_ptr<PipelineImmutableStateFactory> pFactory );

	ts3driverApi( private ):
		virtual bool _drvOnSetPresentationLayer( PresentationLayerHandle pPresentationLayer );
		virtual GPUBufferHandle _drvCreateGPUBuffer( const GPUBufferCreateInfo & pCreateInfo );
		virtual SamplerHandle _drvCreateSampler( const SamplerCreateInfo & pCreateInfo );
		virtual ShaderHandle _drvCreateShader( const ShaderCreateInfo & pCreateInfo );
		virtual TextureHandle _drvCreateTexture( const TextureCreateInfo & pCreateInfo );
		virtual RenderTargetTextureHandle _drvCreateRenderTargetTexture( const RenderTargetTextureCreateInfo & pCreateInfo );
		virtual GraphicsPipelineStateObjectHandle _drvCreateGraphicsPipelineStateObject( const GraphicsPipelineStateObjectCreateInfo & pCreateInfo );

	protected:
		CommandSystemHandle _commandSystem;
		PresentationLayerHandle _presentationLayer;

		/// Factory used to create immutable states. Set by the actual driver class during initialization.
		/// This decouples the state creation from the GPUDevice class so it's easier to manage and extend.
		std::unique_ptr<PipelineImmutableStateFactory> _immutableStateFactory;

		/// Immutable state cache. Holds created states and enables re-using them across all APIs.
		/// Requires PipelineImmutableStateFactory to be specified when created.
		std::unique_ptr<PipelineImmutableStateCache> _immutableStateCache;

		Bitmask<uint32> _internalStateFlags;
	};

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_GPU_DEVICE_H__
