
#include "DX11_gpuDevice.h"
#include "DX11_gpuDriver.h"
#include "DX11_coreAPIProxy.h"
#include "DX11_commandList.h"
#include "DX11_commandSystem.h"
#include "resources/DX11_gpuBuffer.h"
#include "resources/DX11_sampler.h"
#include "resources/DX11_shader.h"
#include "resources/DX11_texture.h"
#include "state/DX11_pipelineStateObject.h"
#include "state/DX11_renderTargetStateObject.h"
#include "state/DX11_vertexStreamStateObject.h"
#include <ts3/gpuapi/resources/gpuBuffer.h>
#include <ts3/gpuapi/resources/shader.h>
#include <ts3/gpuapi/resources/texture.h>

namespace ts3::gpuapi
{

	DX11GPUDevice::DX11GPUDevice( DX11GPUDriver & pDriver,
	                              ComPtr<ID3D11Device1> pD3D11Device1,
	                              ComPtr<ID3D11Debug> pD3D11Debug )
	: DXGPUDevice( pDriver, DX11CoreAPIProxy::queryDXGIFactoryForD3D11Device( pD3D11Device1 ) )
	, mD3D11Device1( std::move( pD3D11Device1 ) )
	, mD3D11DebugInterface( std::move( pD3D11Debug ) )
	{}

	DX11GPUDevice::~DX11GPUDevice() = default;

	DX11GPUDeviceHandle DX11GPUDevice::create( DX11GPUDriver & pDriver, const DX11GPUDeviceCreateInfo & pCreateInfo )
	{
		auto driverConfigFlags = pDriver.getConfigFlags();
		auto deviceCreateFlags = DX11CoreAPIProxy::translateDX11GPUDeviceCreateFlags( driverConfigFlags );

		D3D_DRIVER_TYPE deviceDriverType = D3D_DRIVER_TYPE_HARDWARE;
		if( driverConfigFlags.isSet( GPU_DRIVER_CONFIG_FLAG_USE_REFERENCE_DRIVER_BIT ) )
		{
			deviceDriverType = D3D_DRIVER_TYPE_REFERENCE;
		}

		auto d3d11Device1 = DX11CoreAPIProxy::createD3D11Device( deviceDriverType, deviceCreateFlags );
		ts3DebugAssert( d3d11Device1 );

		auto d3d11DebugInterface = DX11CoreAPIProxy::queryD3D11DebugInterfaceForD3D11Device( d3d11Device1 );
		ts3DebugAssert( d3d11DebugInterface );

		auto dx11GPUDevice = createGPUAPIObject<DX11GPUDevice>( pDriver, d3d11Device1, d3d11DebugInterface );

		dx11GPUDevice->initializeCommandSystem();

		return dx11GPUDevice;
	}

	GraphicsPipelineStateObjectHandle DX11GPUDevice::createGraphicsPipelineStateObject( const GraphicsPipelineStateObjectCreateInfo & pCreateInfo )
	{
		auto graphicsPipelineStateObject = DX11GraphicsPipelineStateObject::create( *this, pCreateInfo );
		ts3DebugAssert( graphicsPipelineStateObject );
		return graphicsPipelineStateObject;
	}

	VertexStreamStateObjectHandle DX11GPUDevice::createVertexStreamStateObject( const VertexStreamStateObjectCreateInfo & pCreateInfo )
	{
		auto vertexStreamStateObject = DX11VertexStreamStateObject::create( *this, pCreateInfo );
		ts3DebugAssert( vertexStreamStateObject );
		return vertexStreamStateObject;
	}

	RenderTargetStateObjectHandle DX11GPUDevice::createRenderTargetStateObject( const RenderTargetStateObjectCreateInfo & pCreateInfo )
	{
		auto renderTargetStateObject = DX11RenderTargetStateObject::create( *this, pCreateInfo );
		ts3DebugAssert( renderTargetStateObject );
		return renderTargetStateObject;
	}

	void DX11GPUDevice::waitForCommandSync( CommandSync & pCommandSync )
	{
		if( pCommandSync )
		{
			auto * dx11CommandSyncData = static_cast<DX11CommandSyncData *>( pCommandSync.syncData );

			UINT64 syncQueryData = FALSE;
			HRESULT hResult = dx11CommandSyncData->d3d11DeviceContext1->GetData( dx11CommandSyncData->d3d11SyncQuery, &syncQueryData, sizeof( UINT64 ), 0 );

			while( ( hResult != S_OK ) && ( syncQueryData != TRUE ) )
			{
				std::this_thread::yield();
				hResult = dx11CommandSyncData->d3d11DeviceContext1->GetData( dx11CommandSyncData->d3d11SyncQuery, &syncQueryData, sizeof( UINT64 ), 0 );
			}

			releaseDX11CommandSyncData( pCommandSync.syncData );
			pCommandSync.syncData = nullptr;
		}
	}

	const DX11BlendStateDescriptor & DX11GPUDevice::getBlendDescriptor( pipeline_state_descriptor_id_t pDescriptorID ) const
	{
		return _descriptorCache.getBlendDescriptor( pDescriptorID );
	}

	const DX11DepthStencilStateDescriptor & DX11GPUDevice::getDepthStencilDescriptor( pipeline_state_descriptor_id_t pDescriptorID ) const
	{
		return _descriptorCache.getDepthStencilDescriptor( pDescriptorID );
	}

	const DX11RasterizerStateDescriptor & DX11GPUDevice::getRasterizerDescriptor( pipeline_state_descriptor_id_t pDescriptorID ) const
	{
		return _descriptorCache.getRasterizerDescriptor( pDescriptorID );
	}

	const DX11VertexInputFormatStateDescriptor & DX11GPUDevice::getVertexInputFormatDescriptor( pipeline_state_descriptor_id_t pDescriptorID ) const
	{
		return _descriptorCache.getVertexInputFormatDescriptor( pDescriptorID );
	}

	pipeline_state_descriptor_id_t DX11GPUDevice::createBlendDescriptor( const BlendConfigDesc & pConfigDesc )
	{
		return _descriptorCache.createBlendDescriptor( pConfigDesc, *this );
	}

	pipeline_state_descriptor_id_t DX11GPUDevice::createDepthStencilDescriptor( const DepthStencilConfigDesc & pConfigDesc )
	{
		return _descriptorCache.createDepthStencilDescriptor( pConfigDesc, *this );
	}

	pipeline_state_descriptor_id_t DX11GPUDevice::createRasterizerDescriptor( const RasterizerConfigDesc & pConfigDesc )
	{
		return _descriptorCache.createRasterizerDescriptor( pConfigDesc, *this );
	}

	pipeline_state_descriptor_id_t DX11GPUDevice::createVertexInputFormatDescriptor( const VertexInputFormatDesc & pInputFormatDesc, Shader & pVertexShader )
	{
		return _descriptorCache.createVertexInputFormatDescriptor( pInputFormatDesc, *this, pVertexShader );
	}

	DX11GraphicsPipelineStateDescriptorCache & DX11GPUDevice::getDescriptorCache()
	{
		return _descriptorCache;
	}

	void DX11GPUDevice::initializeCommandSystem()
	{
	    ts3DebugAssert( !_commandSystem );
	    _commandSystem = createGPUAPIObject<DX11CommandSystem>( *this );
	}

	GPUBufferHandle DX11GPUDevice::_drvCreateGPUBuffer( const GPUBufferCreateInfo & pCreateInfo )
	{
	    auto dx11Buffer = DX11GPUBuffer::create( *this, pCreateInfo );
	    ts3DebugAssert( dx11Buffer );
	    return dx11Buffer;
	}

	SamplerHandle DX11GPUDevice::_drvCreateSampler( const SamplerCreateInfo & pCreateInfo )
	{
	    auto dx11Sampler = DX11Sampler::create( *this, pCreateInfo );
	    ts3DebugAssert( dx11Sampler );
	    return dx11Sampler;
	}

	ShaderHandle DX11GPUDevice::_drvCreateShader( const ShaderCreateInfo & pCreateInfo )
	{
	    auto dx11Shader = DX11Shader::create( *this, pCreateInfo );
	    ts3DebugAssert( dx11Shader );
	    return dx11Shader;
	}

	TextureHandle DX11GPUDevice::_drvCreateTexture( const TextureCreateInfo & pCreateInfo )
	{
	    auto dx11Texture = DX11Texture::create( *this, pCreateInfo );
	    ts3DebugAssert( dx11Texture );
	    return dx11Texture;
	}

}
