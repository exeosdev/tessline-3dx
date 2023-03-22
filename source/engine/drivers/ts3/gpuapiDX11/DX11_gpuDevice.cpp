
#include "DX11_gpuDevice.h"
#include "DX11_gpuDriver.h"
#include "DX11_apiTranslationLayer.h"
#include "DX11_commandList.h"
#include "DX11_commandSystem.h"
#include "resources/DX11_gpuBuffer.h"
#include "resources/DX11_sampler.h"
#include "resources/DX11_shader.h"
#include "resources/DX11_texture.h"
#include "state/DX11_pipelineStateObject.h"

namespace ts3::gpuapi
{

	DX11GPUDevice::DX11GPUDevice(
			DX11GPUDriver & pDriver,
			ComPtr<ID3D11Device1> pD3D11Device1,
			ComPtr<ID3D11Debug> pD3D11Debug )
	: DXGPUDevice( pDriver, atl::queryDXGIFactoryForD3D11Device( pD3D11Device1 ) )
	, mD3D11Device1( std::move( pD3D11Device1 ) )
	, mD3D11DebugInterface( std::move( pD3D11Debug ) )
	, _immutableStateFactoryDX11( *this )
	, _immutableStateCache( _immutableStateFactoryDX11 )
	{
		setImmutableStateCache( _immutableStateCache );
	}

	DX11GPUDevice::~DX11GPUDevice() = default;

	DX11GPUDeviceHandle DX11GPUDevice::create( DX11GPUDriver & pDriver, const DX11GPUDeviceCreateInfo & pCreateInfo )
	{
		auto driverConfigFlags = pDriver.getConfigFlags();
		auto deviceCreateFlags = atl::translateDX11GPUDeviceCreateFlags( driverConfigFlags );

		D3D_DRIVER_TYPE deviceDriverType = D3D_DRIVER_TYPE_HARDWARE;
		if( driverConfigFlags.isSet( E_GPU_DRIVER_CONFIG_FLAG_USE_REFERENCE_DRIVER_BIT ) )
		{
			deviceDriverType = D3D_DRIVER_TYPE_REFERENCE;
		}

		auto d3d11Device1 = atl::createD3D11Device( deviceDriverType, deviceCreateFlags );
		ts3DebugAssert( d3d11Device1 );

		auto d3d11DebugInterface = atl::queryD3D11DebugInterfaceForD3D11Device( d3d11Device1 );
		ts3DebugAssert( d3d11DebugInterface );

		auto dx11GPUDevice = createGPUAPIObject<DX11GPUDevice>( pDriver, d3d11Device1, d3d11DebugInterface );

		return dx11GPUDevice;
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
	    auto dx11Texture = DX11Texture::createDefault( *this, pCreateInfo );
	    ts3DebugAssert( dx11Texture );
	    return dx11Texture;
	}

	RenderTargetTextureHandle DX11GPUDevice::_drvCreateRenderTargetTexture(
			const RenderTargetTextureCreateInfo & pCreateInfo )
	{
		auto dx11RenderTargetTexture = DX11Texture::createForRenderTarget( *this, pCreateInfo );
		ts3DebugAssert( dx11RenderTargetTexture );
		return dx11RenderTargetTexture;
	}

	GraphicsPipelineStateObjectHandle DX11GPUDevice::_drvCreateGraphicsPipelineStateObject(
			const GraphicsPipelineStateObjectCreateInfo & pCreateInfo )
	{
		auto dx11GraphicsPSO = DX11GraphicsPipelineStateObject::create( *this, pCreateInfo );
		ts3DebugAssert( dx11GraphicsPSO );
		return dx11GraphicsPSO;
	}

} // namespace ts3::gpuapi
