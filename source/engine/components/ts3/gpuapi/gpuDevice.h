
#pragma once

#ifndef __TS3_GPUAPI_GPU_DEVICE_H__
#define __TS3_GPUAPI_GPU_DEVICE_H__

#include "commonCommandDefs.h"
#include "displayCommon.h"
#include "resources/commonGPUResourceDefs.h"
#include "state/graphicsShaderState.h"

namespace ts3::gpuapi
{

	ts3GpaDeclareClassHandle( IAVertexStreamDescriptorSet );

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

		GPUBufferHandle createGPUBuffer( const GPUBufferCreateInfo & pCreateInfo );
		SamplerHandle createSampler( const SamplerCreateInfo & pCreateInfo );
		ShaderHandle createShader( const ShaderCreateInfo & pCreateInfo );
		TextureHandle createTexture( const TextureCreateInfo & pCreateInfo );

		BlendImmutableState * createBlendImmutableState( UniqueGPUObjectName pUniqueName, const BlendDescriptor & pDescriptor );
		DepthStencilImmutableState * createDepthStencilImmutableState( UniqueGPUObjectName pUniqueName, const DepthStencilDescriptor & pDescriptor );
		GraphicsShaderLinkageImmutableState * createGraphicsShaderLinkageImmutableState( UniqueGPUObjectName pUniqueName, const GraphicsShaderSet & pShaderSet );
		RasterizerImmutableState * createRasterizerImmutableState( UniqueGPUObjectName pUniqueName, const RasterizerDescriptor & pDescriptor );
		VertexInputLayoutImmutableState * createVertexInputLayoutImmutableState( UniqueGPUObjectName pUniqueName, const VertexInputLayoutDescriptor & pDescriptor );

		/// @brief Creates an RTT using the provided CIS.
		/// This function will automatically create a required resource, depending on the specified layout and usage.
		/// It can either be an explicit texture object which can be retrieved later or an implicit render buffer
		/// (e.g. if the RTT is supposed to be only a depth/stencil attachment used for depth and/or stencil testing).
		TextureHandle createRenderTargetTexture( const RenderTargetTextureCreateInfo & pCreateInfo );

		/// @brief Creates an RTT using the specified texture object and RT binding.
		/// @param pTexture A texture object to use as a target texture. If null, the function will fail.
		/// @param pBindFlags Bind flags for the created RTT. If texture does not support them, the function will fail.
		/// @return A handle to the created RTT or null if the function failed.
		/// This function will use the texture object as the referenced texture for the created RTT. Bind flags specify
		/// how this RTT will be used for rendering and must be compatible with the texture object.
		TextureHandle createRenderTargetTexture( TextureHandle pTexture, Bitmask<ETextureBindFlags> pBindFlags );

		IAVertexStreamDescriptorSetHandle createIAVertexStreamDescriptorSet(
				const ArrayView<IAVertexBufferDescriptorBindingDesc> & pIAVertexBufferDescriptorBindings,
				const IAIndexBufferDescriptor & pIAIndexBufferDescriptor );

		virtual GraphicsPipelineStateObjectHandle createGraphicsPipelineStateObject(
				const GraphicsPipelineStateObjectCreateInfo & pCreateInfo ) { return nullptr; } // = 0;

		virtual RenderTargetStateObjectHandle createRenderTargetStateObject(
				const RenderTargetStateObjectCreateInfo & pCreateInfo ) { return nullptr; } // = 0;

		virtual void waitForCommandSync( CommandSync & pCommandSync ) = 0;

		const math::RGBAColorU8 & getDefaultClearColor() const;
		const RTAttachmentClearValue & getDefaultRTAttachmentClearValue() const;

		void setPresentationLayer( PresentationLayerHandle pPresentationLayer );

		TS3_ATTR_NO_DISCARD CommandSystem * getCommandSystem() const;

		TS3_ATTR_NO_DISCARD PresentationLayer * getPresentationLayer() const;

		TS3_ATTR_NO_DISCARD bool isDebugDevice() const noexcept;

		TS3_ATTR_NO_DISCARD bool isMultiThreadAccessSupported() const noexcept;

		TS3_ATTR_NO_DISCARD bool isResourceActiveRefsTrackingEnabled() const noexcept;

	protected:
        /// @brief API-level initialization of the command system. Called by the parent driver when a device is created.
		virtual void initializeCommandSystem() = 0;

		/// @brief API-level initialization of the state factory. Called by the parent driver when a device is created.
		virtual void initializeImmutableStateFactory() = 0;

		virtual bool onGPUResourceActiveRefsZero( GPUResource & pGPUResource );

		void setImmutableStateFactory( std::unique_ptr<PipelineImmutableStateFactory> pFactory );

	ts3driverApi( private ):
		virtual bool _drvOnSetPresentationLayer( PresentationLayerHandle pPresentationLayer );
		virtual GPUBufferHandle _drvCreateGPUBuffer( const GPUBufferCreateInfo & pCreateInfo ) { return nullptr; }
		virtual SamplerHandle _drvCreateSampler( const SamplerCreateInfo & pCreateInfo ) { return nullptr; }
		virtual ShaderHandle _drvCreateShader( const ShaderCreateInfo & pCreateInfo ) { return nullptr; }
		virtual TextureHandle _drvCreateTexture( const TextureCreateInfo & pCreateInfo ) { return nullptr; }
		virtual TextureHandle _drvCreateRenderTargetTexture( const RenderTargetTextureCreateInfo & pCreateInfo ) { return nullptr; }

	protected:
		enum InternalStateFlags : uint32
		{
			E_INTERNAL_STATE_FLAG_DEBUG_DEVICE_BIT = 0x0001,
			E_INTERNAL_STATE_FLAG_MULTI_THREAD_ACCESS_BIT = 0x0002,
			E_INTERNAL_STATE_FLAG_ENABLE_RESOURCE_ACTIVE_REFS_TRACKING_BIT = 0x0008
		};

		CommandSystemHandle _commandSystem;
		PresentationLayerHandle _presentationLayer;

		/// Factory used to create immutable states. Set by the actual driver class during initialization.
		/// This decouples the state creation from the GPUDevice class so it's easier to manage and extend.
		std::unique_ptr<PipelineImmutableStateFactory> _immutableStateFactory;

		/// Immutable state cache. Holds created states and enables re-using them across all APIs.
		/// Requires PipelineImmutableStateFactory to be specified when created.
		std::unique_ptr<PipelineImmutableStateCache> _immutableStateCache;

		Bitmask<InternalStateFlags> _internalStateFlags;
	};

	inline bool GPUDevice::isDebugDevice() const noexcept
	{
		return _internalStateFlags.isSet( E_INTERNAL_STATE_FLAG_DEBUG_DEVICE_BIT );
	}

	inline bool GPUDevice::isMultiThreadAccessSupported() const noexcept
	{
		return _internalStateFlags.isSet( E_INTERNAL_STATE_FLAG_MULTI_THREAD_ACCESS_BIT );
	}

	inline bool GPUDevice::isResourceActiveRefsTrackingEnabled() const noexcept
	{
		return _internalStateFlags.isSet( E_INTERNAL_STATE_FLAG_ENABLE_RESOURCE_ACTIVE_REFS_TRACKING_BIT );
	}

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_GPU_DEVICE_H__
