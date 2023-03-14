
#pragma once

#ifndef __TS3DRIVER_GPUAPI_DX11_GPU_DEVICE_H__
#define __TS3DRIVER_GPUAPI_DX11_GPU_DEVICE_H__

#include "DX11_prerequisites.h"
#include "state/DX11_pipelineStateDesc.h"
#include <ts3/gpuapiDX/DX_gpuDevice.h>
#include <ts3/gpuapi/state/graphicsPipelineDescriptorCache.h>

namespace ts3::gpuapi
{

	struct DX11GPUDeviceCreateInfo : public GPUDeviceCreateInfo
	{
	};

	/// @brief
	class TS3GX_DX11_CLASS DX11GPUDevice : public DXGPUDevice
	{
		friend class DX11CommandContext;
		friend class DX11CommandList;
		friend class DX11GraphicsPipelineStateObject;
		friend class DX11VertexStreamStateObject;

	public:
		ComPtr<ID3D11Device1> const mD3D11Device1;
		ComPtr<ID3D11Debug> const mD3D11DebugInterface;

		explicit DX11GPUDevice( DX11GPUDriver & pDriver,
		                        ComPtr<ID3D11Device1> pD3D11Device1,
		                        ComPtr<ID3D11Debug> pD3D11Debug );

		virtual ~DX11GPUDevice();

		virtual GraphicsPipelineStateObjectHandle createGraphicsPipelineStateObject( const GraphicsPipelineStateObjectCreateInfo & pCreateInfo ) override;
		virtual VertexStreamStateObjectHandle createVertexStreamStateObject( const VertexStreamStateObjectCreateInfo & pCreateInfo ) override;
		virtual RenderTargetStateObjectHandle createRenderTargetStateObject( const RenderTargetStateObjectCreateInfo & pCreateInfo ) override;

		virtual void waitForCommandSync( CommandSync & pCommandSync ) override;

		const DX11BlendStateDescriptor & getBlendDescriptor( pipeline_state_descriptor_id_t pDescriptorID ) const;
		const DX11DepthStencilStateDescriptor & getDepthStencilDescriptor( pipeline_state_descriptor_id_t pDescriptorID ) const;
		const DX11RasterizerStateDescriptor & getRasterizerDescriptor( pipeline_state_descriptor_id_t pDescriptorID ) const;
		const DX11VertexInputFormatStateDescriptor & getVertexInputFormatDescriptor( pipeline_state_descriptor_id_t pDescriptorID ) const;

		static DX11GPUDeviceHandle create( DX11GPUDriver & pDX11Driver, const DX11GPUDeviceCreateInfo & pCreateInfo );

	friendapi:
		pipeline_state_descriptor_id_t createBlendDescriptor( const BlendConfigDesc & pConfigDesc );
		pipeline_state_descriptor_id_t createDepthStencilDescriptor( const DepthStencilConfigDesc & pConfigDesc );
		pipeline_state_descriptor_id_t createRasterizerDescriptor( const RasterizerConfigDesc & pConfigDesc );
		pipeline_state_descriptor_id_t createVertexInputFormatDescriptor( const VertexInputFormatDesc & pInputFormatDesc, Shader & pVertexShader );

		DX11GraphicsPipelineStateDescriptorCache & getDescriptorCache();

	private:
		virtual void initializeCommandSystem() override;

		virtual GPUBufferHandle _drvCreateGPUBuffer( const GPUBufferCreateInfo & pCreateInfo ) override final;
		virtual SamplerHandle _drvCreateSampler( const SamplerCreateInfo & pCreateInfo ) override final;
		virtual ShaderHandle _drvCreateShader( const ShaderCreateInfo & pCreateInfo ) override final;
		virtual TextureHandle _drvCreateTexture( const TextureCreateInfo & pCreateInfo ) override final;

	private:
		DX11GraphicsPipelineStateDescriptorCache _descriptorCache;
	};

} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_DX11_GPU_DEVICE_H__
