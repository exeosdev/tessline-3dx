
#pragma once

#ifndef __TS3_GPUAPI_GPU_DEVICE_H__
#define __TS3_GPUAPI_GPU_DEVICE_H__

#include "commonCommandDefs.h"
#include "displayCommon.h"
#include "resources/commonGPUResourceDefs.h"

namespace ts3::gpuapi
{

	struct PresentationLayerCreateInfo;

	enum EGPUDeviceCreateFlags : uint32
	{
		E_GPU_DEVICE_CREATE_FLAG_INIT_DEFAULT_PRESENT_QUEUE_BIT = 0x0004,
		E_GPU_DEVICE_CREATE_FLAGS_DEFAULT = E_GPU_DEVICE_CREATE_FLAG_INIT_DEFAULT_PRESENT_QUEUE_BIT
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
	public:
		EGPUDriverID const mGPUDriverID;
		system::SysContextHandle const mSysContext;

		explicit GPUDevice( GPUDriver & pDriver );
		virtual ~GPUDevice();

		virtual GPUBufferHandle createGPUBuffer( const GPUBufferCreateInfo & pCreateInfo ) { return nullptr; } // = 0;
		virtual SamplerHandle createSampler( const SamplerCreateInfo & pCreateInfo ) { return nullptr; } // = 0;
		virtual ShaderHandle createShader( const ShaderCreateInfo & pCreateInfo ) { return nullptr; } // = 0;
		virtual TextureHandle createTexture( const TextureCreateInfo & pCreateInfo ) { return nullptr; } // = 0;

		virtual GraphicsPipelineStateObjectHandle createGraphicsPipelineStateObject( const GraphicsPipelineStateObjectCreateInfo & pCreateInfo ) { return nullptr; } // = 0;
		virtual VertexStreamStateObjectHandle createVertexStreamStateObject( const VertexStreamStateObjectCreateInfo & pCreateInfo ) { return nullptr; } // = 0;

		virtual RenderTargetStateObjectHandle createRenderTargetStateObject( const RenderTargetStateObjectCreateInfo & pCreateInfo ) { return nullptr; } // = 0;

		virtual void waitForCommandSync( CommandSync & pCommandSync ) = 0;

		const math::RGBAColorU8 & getDefaultClearColor() const;
		const RenderTargetClearConfig & getDefaultRenderTargetClearConfig() const;

		void setPresentationLayer( PresentationLayerHandle pPresentationLayer );

		CommandSystem * getCommandSystem() const;

		PresentationLayer * getPresentationLayer() const;

		bool isDebugDevice() const;

	protected:
		virtual void initializeCommandSystem() = 0;

		virtual bool onSetPresentationLayer( PresentationLayerHandle pPresentationLayer );

	protected:
		enum InternalStateFlags : uint32
		{
			E_INTERNAL_STATE_FLAG_DEBUG_DEVICE_BIT = 0x0001,
			E_INTERNAL_STATE_FLAG_MULTI_THREAD_ACCESS_BIT = 0x0002
		};

		CommandSystemHandle _commandSystem;
		PresentationLayerHandle _presentationLayer;
		Bitmask<InternalStateFlags> _internalStateFlags;
	};

}

#endif // __TS3_GPUAPI_GPU_DEVICE_H__
