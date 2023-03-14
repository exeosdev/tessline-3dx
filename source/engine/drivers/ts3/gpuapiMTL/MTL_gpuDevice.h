
#pragma once

#ifndef __TS3DRIVER_GPUAPI_MTLCOMMON_GPU_DEVICE_H__
#define __TS3DRIVER_GPUAPI_MTLCOMMON_GPU_DEVICE_H__

#include "MTL_prerequisites.h"
#include "state/MTL_pipelineImmutableStateFactory.h"
#include <ts3/gpuapi/gpuDevice.h>
#include <ts3/gpuapi/state/pipelineImmutableStateCache.h>

namespace ts3::gpuapi
{

	/// @brief
	class MetalGPUDevice : public GPUDevice
	{
	public:
		system::MetalDeviceHandle const mSysMetalDevice;

		id<MTLDevice> const mMTLDevice;

	public:
		explicit MetalGPUDevice( MetalGPUDriver & pGPUDriver, system::MetalDeviceHandle pSysMetalDevice );
		virtual ~MetalGPUDevice();

		virtual void waitForCommandSync( CommandSync & pCommandSync ) override;

	protected:
		virtual void initializeCommandSystem() override;

	private:
	    virtual bool _drvOnSetPresentationLayer( PresentationLayerHandle pPresentationLayer ) override;

	    virtual GPUBufferHandle _drvCreateGPUBuffer( const GPUBufferCreateInfo & pCreateInfo ) override final;
	    virtual SamplerHandle _drvCreateSampler( const SamplerCreateInfo & pCreateInfo ) override final;
	    virtual ShaderHandle _drvCreateShader( const ShaderCreateInfo & pCreateInfo ) override final;
	    virtual TextureHandle _drvCreateTexture( const TextureCreateInfo & pCreateInfo ) override final;

		virtual RenderTargetTextureHandle _drvCreateRenderTargetTexture(
				const RenderTargetTextureCreateInfo & pCreateInfo ) override final;

		virtual GraphicsPipelineStateObjectHandle _drvCreateGraphicsPipelineStateObject(
				const GraphicsPipelineStateObjectCreateInfo & pCreateInfo ) override final;

	private:
		MetalPipelineImmutableStateFactory _immutableStateFactory;
		PipelineImmutableStateCache _immutableStateCache;
	};

} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_MTLCOMMON_GPU_DEVICE_H__
