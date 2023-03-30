
#pragma once

#ifndef __TS3_GPUAPI_GPU_DEVICE_H__
#define __TS3_GPUAPI_GPU_DEVICE_H__

#include "commonCommandDefs.h"
#include "displayCommon.h"
#include "resources/textureCommon.h"
#include "state/graphicsShaderState.h"
#include "state/renderPassCommon.h"
#include "state/pipelineImmutableStateCache.h"

namespace ts3::gpuapi
{

	struct PresentationLayerCreateInfo;
	struct RenderTargetTextureCreateInfo;

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
		friend class GPUDriver;
		friend class GPUResource;

	public:
		EGPUDriverID const mGPUDriverID;
		system::SysContextHandle const mSysContext;

		explicit GPUDevice( GPUDriver & pDriver );
		virtual ~GPUDevice();

		TS3_ATTR_NO_DISCARD virtual bool isNullDevice() const noexcept;

		TS3_ATTR_NO_DISCARD bool isDebugDevice() const noexcept;

		TS3_ATTR_NO_DISCARD bool isMultiThreadAccessSupported() const noexcept;

		TS3_ATTR_NO_DISCARD bool isResourceActiveRefsTrackingEnabled() const noexcept;

		TS3_ATTR_NO_DISCARD CommandSystem & getCommandSystem() const noexcept;

		TS3_ATTR_NO_DISCARD PipelineImmutableStateFactory & getPipelineStateFactory() const noexcept;

		TS3_ATTR_NO_DISCARD PresentationLayer * getPresentationLayer() const noexcept;

		TS3_ATTR_NO_DISCARD const math::RGBAColorU8 & getDefaultClearColor() const noexcept;

		TS3_ATTR_NO_DISCARD const RenderTargetAttachmentClearConfig & getDefaultClearConfig() const noexcept;

		template <typename TState>
		TS3_ATTR_NO_DISCARD GpaHandle<TState> getCachedImmutableState( UniqueGPUObjectID pUniqueID ) const noexcept;

		template <typename TState>
		TS3_ATTR_NO_DISCARD GpaHandle<TState> getCachedImmutableState( const UniqueGPUObjectName & pStateObjectName ) const noexcept;

		TS3_ATTR_NO_DISCARD GPUBufferHandle createGPUBuffer( const GPUBufferCreateInfo & pCreateInfo );
		TS3_ATTR_NO_DISCARD SamplerHandle createSampler( const SamplerCreateInfo & pCreateInfo );
		TS3_ATTR_NO_DISCARD ShaderHandle createShader( const ShaderCreateInfo & pCreateInfo );
		TS3_ATTR_NO_DISCARD TextureHandle createTexture( const TextureCreateInfo & pCreateInfo );

		/// @brief Creates an RTT using the provided CIS.
		/// This function will automatically create a required resource, depending on the specified layout and usage.
		/// It can either be an explicit texture object which can be retrieved later or an implicit render buffer
		/// (e.g. if the RTT is supposed to be only a depth/stencil attachment used for depth and/or stencil testing).
		TS3_ATTR_NO_DISCARD RenderTargetTextureHandle createRenderTargetTexture( const RenderTargetTextureCreateInfo & pCreateInfo );

		/// @brief
		TS3_ATTR_NO_DISCARD GraphicsPipelineStateObjectHandle createGraphicsPipelineStateObject( const GraphicsPipelineStateObjectCreateInfo & pCreateInfo );

		TS3_ATTR_NO_DISCARD BlendImmutableStateHandle createBlendImmutableState( const BlendConfig & pConfig );
		TS3_ATTR_NO_DISCARD DepthStencilImmutableStateHandle createDepthStencilImmutableState( const DepthStencilConfig & pConfig );
		TS3_ATTR_NO_DISCARD GraphicsShaderLinkageImmutableStateHandle createGraphicsShaderLinkageImmutableState( const GraphicsShaderSet & pShaderSet );
		TS3_ATTR_NO_DISCARD IAInputLayoutImmutableStateHandle createIAInputLayoutImmutableState( const IAInputLayoutDefinition & pDefinition, Shader & pVertexShaderWithBinary );
		TS3_ATTR_NO_DISCARD IAVertexStreamImmutableStateHandle createIAVertexStreamImmutableState( const IAVertexStreamDefinition & pDefinition );
		TS3_ATTR_NO_DISCARD RasterizerImmutableStateHandle createRasterizerImmutableState( const RasterizerConfig & pConfig );
		TS3_ATTR_NO_DISCARD RenderTargetBindingImmutableStateHandle createRenderTargetBindingImmutableState( const RenderTargetBindingDefinition & pDefinition );
		TS3_ATTR_NO_DISCARD RenderPassConfigurationImmutableStateHandle createRenderPassConfigurationImmutableState( const RenderPassConfiguration & pConfiguration );

		template <typename TState, typename TInputDesc, typename... TArgs>
		GpaHandle<TState> createCachedImmutableState( UniqueGPUObjectID pUniqueID, const TInputDesc & pInputDesc, TArgs && ...pArgs );

		template <typename TState, typename TInputDesc, typename... TArgs>
		GpaHandle<TState> createCachedImmutableState( const UniqueGPUObjectName & pUniqueName, const TInputDesc & pInputDesc, TArgs && ...pArgs );

		void resetImmutableStateCache( Bitmask<EPipelineImmutableStateTypeFlags> pResetMask = E_PIPELINE_IMMUTABLE_STATE_TYPE_MASK_ALL );

		void setPresentationLayer( PresentationLayerHandle pPresentationLayer );

		virtual void waitForCommandSync( CommandSync & pCommandSync ) = 0;

		TS3_ATTR_NO_DISCARD static GPUDevice & nullDevice();

	protected:
		virtual bool onGPUResourceActiveRefsZero( GPUResource & pGPUResource );

		void setImmutableStateCache( PipelineImmutableStateCache & pStateCache );

	private:
		/// @brief API-level initialization of the command system. Called by the parent driver when a device is created.
		virtual void initializeCommandSystem() = 0;

	ts3driverApi( private ):
		virtual bool _drvOnSetPresentationLayer( PresentationLayerHandle pPresentationLayer );

		virtual GPUBufferHandle _drvCreateGPUBuffer( const GPUBufferCreateInfo & pCreateInfo );
		virtual SamplerHandle _drvCreateSampler( const SamplerCreateInfo & pCreateInfo );
		virtual ShaderHandle _drvCreateShader( const ShaderCreateInfo & pCreateInfo );
		virtual TextureHandle _drvCreateTexture( const TextureCreateInfo & pCreateInfo );
		virtual RenderTargetTextureHandle _drvCreateRenderTargetTexture( const RenderTargetTextureCreateInfo & pCreateInfo );

		virtual GraphicsPipelineStateObjectHandle _drvCreateGraphicsPipelineStateObject(
				const GraphicsPipelineStateObjectCreateInfo & pCreateInfo );

	protected:
		CommandSystemHandle _commandSystem;
		PresentationLayerHandle _presentationLayer;

		/// Factory used to create immutable states. Set by the actual driver class during initialization.
		/// This decouples the state creation from the GPUDevice class so it's easier to manage and extend.
		PipelineImmutableStateFactory * _immutableStateFactoryBase = nullptr;

		/// Immutable state cache. Holds created states and enables re-using them across all APIs.
		/// Requires PipelineImmutableStateFactory to be specified when created.
		PipelineImmutableStateCache * _immutableStateCachePtr = nullptr;

		Bitmask<uint32> _internalStateFlags;
	};

	template <typename TState>
	inline GpaHandle<TState> GPUDevice::getCachedImmutableState( UniqueGPUObjectID pUniqueID ) const noexcept
	{
		return _immutableStateCachePtr->template getState<TState>( pUniqueID );
	}

	template <typename TState>
	inline GpaHandle<TState> GPUDevice::getCachedImmutableState( const UniqueGPUObjectName & pUniqueName ) const noexcept
	{
		return _immutableStateCachePtr->template getState<TState>( pUniqueName );
	}

	template <typename TState, typename TInputDesc, typename... TArgs>
	inline GpaHandle<TState> GPUDevice::createCachedImmutableState( UniqueGPUObjectID pUniqueID, const TInputDesc & pInputDesc, TArgs && ...pArgs )
	{
		return _immutableStateCachePtr->template createState<TState>( pUniqueID, pInputDesc, std::forward<TArgs>( pArgs )... );
	}

	template <typename TState, typename TInputDesc, typename... TArgs>
	inline GpaHandle<TState> GPUDevice::createCachedImmutableState( const UniqueGPUObjectName & pUniqueName, const TInputDesc & pInputDesc, TArgs && ...pArgs )
	{
		return _immutableStateCachePtr->template createState<TState>( pUniqueName, pInputDesc, std::forward<TArgs>( pArgs )... );
	}

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_GPU_DEVICE_H__
