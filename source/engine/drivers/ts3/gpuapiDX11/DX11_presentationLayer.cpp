
#include "DX11_presentationLayer.h"
#include "DX11_commandList.h"
#include "DX11_coreAPIProxy.h"
#include "DX11_gpuDevice.h"
#include <ts3/gpuapi/commandContext.h>

namespace ts3
{
namespace gpuapi
{

	DX11ScreenPresentationLayer::DX11ScreenPresentationLayer( DX11GPUDevice & pDevice,
                                                              system::WindowHandle pSysWindow,
                                                              ComPtr<IDXGISwapChain1> pDXGISwapChain1 )
	: DXScreenPresentationLayer( pDevice, pSysWindow, std::move( pDXGISwapChain1 ) )
	, mD3D11Device1( pDevice.mD3D11Device1 )
	{}

	DX11ScreenPresentationLayer::~DX11ScreenPresentationLayer() = default;

	DX11ScreenPresentationLayerHandle DX11ScreenPresentationLayer::create( DX11GPUDevice & pDevice, const DX11PresentationLayerCreateInfo & pCreateInfo )
	{
		auto sysWindow = createSysWindow( pDevice, pCreateInfo );
		ts3DebugAssert( sysWindow );

		auto dxgiSwapChain = DX11CoreAPIProxy::createD3D11SwapChainForSystemWindow( pDevice, sysWindow.get() );
		ts3DebugAssert( dxgiSwapChain );

		auto presentationLayer = createGPUAPIObject<DX11ScreenPresentationLayer>( pDevice, sysWindow, std::move( dxgiSwapChain ) );

		return presentationLayer;
	}

	void DX11ScreenPresentationLayer::bindRenderTarget( CommandContext * pCmdContext )
	{
		auto * dx11CommandList = pCmdContext->mCommandList->queryInterface<DX11CommandList>();
		auto * backBufferRTView = getBackBufferRTView();
		auto * backBufferDSView = getBackBufferDSView();
		dx11CommandList->mD3D11DeviceContext1->OMSetRenderTargets( 1, &backBufferRTView, backBufferDSView );
	}

	void DX11ScreenPresentationLayer::invalidateRenderTarget( CommandContext * pCmdContext )
	{
	}

	void DX11ScreenPresentationLayer::present()
	{
		mDXGISwapChain1->Present( 1, 0 );
	}

	ID3D11RenderTargetView * DX11ScreenPresentationLayer::getBackBufferRTView()
	{
		if( !_backBufferRTView )
		{
			_createBackBufferResources();
		}
		return _backBufferRTView.Get();
	}

	ID3D11DepthStencilView * DX11ScreenPresentationLayer::getBackBufferDSView()
	{
		if( !_backBufferRTView )
		{
			_createBackBufferResources();
		}
		return _backBufferDSView.Get();
	}

	bool DX11ScreenPresentationLayer::_createBackBufferResources()
	{
		ComPtr<ID3D11Texture2D> backBufferRTTexture;
		auto hResult = mDXGISwapChain1->GetBuffer( 0, IID_PPV_ARGS( &backBufferRTTexture ) );
		if( FAILED( hResult ) )
		{
			return false;
		}

		D3D11_TEXTURE2D_DESC backBufferDSTextureDesc;
		backBufferRTTexture->GetDesc( & backBufferDSTextureDesc );
		backBufferDSTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		backBufferDSTextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		backBufferDSTextureDesc.CPUAccessFlags = 0;

		ComPtr<ID3D11Texture2D> backBufferDSTexture;
		hResult = mD3D11Device1->CreateTexture2D( &backBufferDSTextureDesc, nullptr, backBufferDSTexture.GetAddressOf() );
		if( FAILED( hResult ) )
		{
			return false;
		}

		ComPtr<ID3D11RenderTargetView> backBufferRTView;
		hResult = mD3D11Device1->CreateRenderTargetView( backBufferRTTexture.Get(), nullptr, backBufferRTView.GetAddressOf() );
		if( FAILED( hResult ) )
		{
			return false;
		}

		ComPtr<ID3D11DepthStencilView> backBufferDSView;
		hResult = mD3D11Device1->CreateDepthStencilView( backBufferDSTexture.Get(), nullptr, backBufferDSView.GetAddressOf() );
		if( FAILED( hResult ) )
		{
			return false;
		}

		_backBufferDSTexture = backBufferDSTexture;
		_backBufferDSView = backBufferDSView;
		_backBufferRTView = backBufferRTView;

		return true;
	}
	
} /* namespace ts3 */
} /* namespace gpuapi */
