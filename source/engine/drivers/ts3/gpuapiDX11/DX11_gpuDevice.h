
#pragma once

#ifndef __TS3DRIVER_GPUAPI_DX11_GPU_DEVICE_H__
#define __TS3DRIVER_GPUAPI_DX11_GPU_DEVICE_H__

#include "DX11_prerequisites.h"
#include "state/DX11_pipelineImmutableStateFactory.h"
#include <ts3/gpuapiDX/DX_gpuDevice.h>
#include <ts3/gpuapi/state/pipelineImmutableStateCache.h>

namespace ts3::gpuapi
{

	struct DX11GPUDeviceCreateInfo : public GPUDeviceCreateInfo
	{
	};

	/// @brief
	class TS3GX_DX11_CLASS DX11GPUDevice : public DXGPUDevice
	{
		friend class DX11CommandList;
		friend class DX11GraphicsPipelineStateObject;

	public:
		ComPtr<ID3D11Device1> const mD3D11Device1;
		ComPtr<ID3D11Debug> const mD3D11DebugInterface;

		explicit DX11GPUDevice(
				DX11GPUDriver & pDriver,
				ComPtr<ID3D11Device1> pD3D11Device1,
				ComPtr<ID3D11Debug> pD3D11Debug );

		virtual ~DX11GPUDevice();

		void waitForCommandSync( CommandSync & pCommandSync ) override final;

		static DX11GPUDeviceHandle create( DX11GPUDriver & pDX11Driver, const DX11GPUDeviceCreateInfo & pCreateInfo );

	private:
		virtual void initializeCommandSystem() override;

		virtual GPUBufferHandle _drvCreateGPUBuffer( const GPUBufferCreateInfo & pCreateInfo ) override final;
		virtual SamplerHandle _drvCreateSampler( const SamplerCreateInfo & pCreateInfo ) override final;
		virtual ShaderHandle _drvCreateShader( const ShaderCreateInfo & pCreateInfo ) override final;
		virtual TextureHandle _drvCreateTexture( const TextureCreateInfo & pCreateInfo ) override final;

		virtual RenderTargetTextureHandle _drvCreateRenderTargetTexture(
				const RenderTargetTextureCreateInfo & pCreateInfo ) override final;

		virtual GraphicsPipelineStateObjectHandle _drvCreateGraphicsPipelineStateObject(
				const GraphicsPipelineStateObjectCreateInfo & pCreateInfo ) override final;

	private:
		DX11PipelineImmutableStateFactory _immutableStateFactoryDX11;
		PipelineImmutableStateCache _immutableStateCache;
	};

} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_DX11_GPU_DEVICE_H__
