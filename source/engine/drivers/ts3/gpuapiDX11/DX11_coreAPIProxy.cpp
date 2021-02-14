
#include "DX11_coreAPIProxy.h"
#include <ts3/gpuapiDX11/DX11_gpuDevice.h>
#include <ts3/gpuapi/resources/gpuBufferCommon.h>
#include <ts3/gpuapi/resources/samplerCommon.h>
#include <ts3/gpuapi/resources/shaderCommon.h>
#include <ts3/gpuapi/resources/textureCommon.h>
#include <ts3/system/windowSystem.h>
#include <ts3/stdext/mapUtils.h>

namespace ts3::gpuapi
{

	ComPtr<ID3D11Device1> DX11CoreAPIProxy::createD3D11Device( D3D_DRIVER_TYPE pDeviceType, Bitmask<UINT> pCreateFlags )
	{
		Bitmask<UINT> createDeviceFlags = 0;
		if( pCreateFlags.isSet( D3D11_CREATE_DEVICE_DEBUG ) )
		{
			// This one is for creating a debug device, which is able to provide additional diagnostic info.
			createDeviceFlags.set( D3D11_CREATE_DEVICE_DEBUG );
			// This one is for shader debugging capability. Usable when using external diagnostic tools.
			createDeviceFlags.set( D3D11_CREATE_DEVICE_DEBUGGABLE );
		}
		if( pCreateFlags.isSet( D3D11_CREATE_DEVICE_SINGLETHREADED ) )
		{
			createDeviceFlags.set( D3D11_CREATE_DEVICE_SINGLETHREADED );
		}

		const D3D_FEATURE_LEVEL featureLevelArray[] =
		{
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0
		};

		const D3D_FEATURE_LEVEL * requestedFeatureLevelPtr = &( featureLevelArray[0] );
		UINT requestedFeatureLevelCount = static_cast<UINT>( staticArraySize( featureLevelArray ) );

		ComPtr<ID3D11Device> d3d11Device;
		ComPtr<ID3D11DeviceContext> d3d11DeviceContext;
		D3D_FEATURE_LEVEL targetFeatureLevel;

		auto hResult = ::D3D11CreateDevice( nullptr,
		                                    pDeviceType,
		                                    nullptr,
		                                    createDeviceFlags,
		                                    requestedFeatureLevelPtr,
		                                    requestedFeatureLevelCount,
		                                    D3D11_SDK_VERSION,
		                                    d3d11Device.GetAddressOf(),
		                                    &targetFeatureLevel,
		                                    d3d11DeviceContext.GetAddressOf() );

		while( hResult != S_OK )
		{
			// E_INVALIDARG is immediately returned if we request 11.1 runtime on a system which does not support it.
			// If that happens, just stick to 11.0. 11.1 is treated as an extension and its absence is not a deal-breaker.
			if( ( hResult == E_INVALIDARG ) && ( requestedFeatureLevelPtr[0] == D3D_FEATURE_LEVEL_11_1 ) )
			{
				// Advance the feature level pointer to a next value (11_0), decrease number of feature levels by 1.
				++requestedFeatureLevelPtr;
				--requestedFeatureLevelCount;
			}
			// D3D11_CREATE_DEVICE_DEBUGGABLE is one of the reasons requires D3D11SDKLayers.dll to be installed on the
			// system *AND* WDDM 1.2 driver + Direct3D 11.1 runtime (>= Windows 8). If this error occurs, drop the flag.
			else if( ( hResult == DXGI_ERROR_UNSUPPORTED ) && createDeviceFlags.isSet( D3D11_CREATE_DEVICE_DEBUGGABLE ) )
			{
				createDeviceFlags.unset( D3D11_CREATE_DEVICE_DEBUGGABLE );
			}
			// DXGI_ERROR_SDK_COMPONENT_MISSING is returned if D3D11_CREATE_DEVICE_DEBUG is specified and the incorrect
			// version of the debug layer is installed on the system. In such case, we just drop the flag and proceed.
			else if( ( hResult == DXGI_ERROR_SDK_COMPONENT_MISSING ) && createDeviceFlags.isSet( D3D11_CREATE_DEVICE_DEBUG ) )
			{
				createDeviceFlags.unset( D3D11_CREATE_DEVICE_DEBUG );
			}
			// Doesn't seem to be a known case. Treat as a failure and exit the loop.
			else
			{
				break;
			}

			// Try again.
			hResult = ::D3D11CreateDevice( nullptr,
			                               pDeviceType,
			                               nullptr,
			                               createDeviceFlags,
			                               requestedFeatureLevelPtr,
			                               requestedFeatureLevelCount,
			                               D3D11_SDK_VERSION,
			                               d3d11Device.GetAddressOf(),
			                               &targetFeatureLevel,
			                               d3d11DeviceContext.GetAddressOf() );
		}

		if( FAILED( hResult ) )
		{
			return nullptr;
		}

		ComPtr<ID3D11Device1> d3d11Device1 = nullptr;
		hResult = d3d11Device->QueryInterface( IID_PPV_ARGS( &d3d11Device1 ) );
		if( FAILED( hResult ) )
		{
			return nullptr;
		}

		return d3d11Device1;
	}

	ComPtr<ID3D11Debug> DX11CoreAPIProxy::queryD3D11DebugInterfaceForD3D11Device( const ComPtr<ID3D11Device1> & pD3D11Device1 )
	{
		ComPtr<ID3D11Debug> d3d11DebugInterface;
		auto hResult = pD3D11Device1->QueryInterface( IID_PPV_ARGS( &d3d11DebugInterface ) );
		if( FAILED( hResult ) )
		{
			return nullptr;
		}

		return d3d11DebugInterface;
	}

	ComPtr<IDXGIFactory2> DX11CoreAPIProxy::queryDXGIFactoryForD3D11Device( const ComPtr<ID3D11Device1> & pD3D11Device1 )
	{
		ComPtr<IDXGIDevice2> dxgiDevice2;
		auto hResult = pD3D11Device1->QueryInterface( IID_PPV_ARGS( &dxgiDevice2 ) );
		if( FAILED( hResult ) )
		{
			return nullptr;
		}

		ComPtr<IDXGIAdapter> dxgiAdapter;
		hResult = dxgiDevice2->GetParent( IID_PPV_ARGS( &dxgiAdapter ) );
		if( FAILED( hResult ) )
		{
			return nullptr;
		}

		ComPtr<IDXGIFactory2> dxgiFactory2;
		hResult = dxgiAdapter->GetParent( IID_PPV_ARGS( &dxgiFactory2 ) );
		if( FAILED( hResult ) )
		{
			return nullptr;
		}

		return dxgiFactory2;
	}

	ComPtr<IDXGISwapChain1> DX11CoreAPIProxy::createD3D11SwapChainForExfWindow( DX11GPUDevice & pDX11GPUDevice, void * pSysWindow )
	{
		auto * sysWindowPtr = static_cast<SysWindow *>( pSysWindow );
		auto presentationLayerSize = sysWindowPtr->getClientAreaSize();

		DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
		ZeroMemory( &swapChainDesc, sizeof( DXGI_SWAP_CHAIN_DESC1 ) );
		swapChainDesc.Width = presentationLayerSize.x;
		swapChainDesc.Height = presentationLayerSize.y;
		swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 1;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

		auto & dxgiFactory = pDX11GPUDevice.mDXGIFactory2;
		ComPtr<IDXGISwapChain1> dxgiSwapChain1;
		auto hResult = dxgiFactory->CreateSwapChainForHwnd( pDX11GPUDevice.mD3D11Device1.Get(),
		                                                    sysWindowPtr->mNativeData.hwnd,
		                                                    &swapChainDesc,
		                                                    nullptr,
		                                                    nullptr,
		                                                    dxgiSwapChain1.GetAddressOf() );
		if( FAILED( hResult ) )
		{
			return nullptr;
		}

		return dxgiSwapChain1;
	}

	UINT DX11CoreAPIProxy::translateDX11GPUDeviceCreateFlags( Bitmask<GPUDriverConfigFlags> pDriverConfigFlags )
	{
		Bitmask<UINT> deviceCreateFlags = 0;
		if( pDriverConfigFlags.isSet( GPU_DRIVER_CONFIG_FLAG_ENABLE_DEBUG_LAYER_BIT ) )
		{
			deviceCreateFlags.set( D3D11_CREATE_DEVICE_DEBUG );
		}
		if( pDriverConfigFlags.isSet( GPU_DRIVER_CONFIG_FLAG_ENABLE_SHADER_DEBUG_INFO_BIT ) )
		{
			deviceCreateFlags.set( D3D11_CREATE_DEVICE_DEBUGGABLE );
		}
		if( pDriverConfigFlags.isSet( GPU_DRIVER_CONFIG_FLAG_DISABLE_MULTI_THREAD_ACCESS_BIT ) )
		{
			deviceCreateFlags.set( D3D11_CREATE_DEVICE_SINGLETHREADED );
		}
		return deviceCreateFlags;
	}

	D3D11_BLEND DX11CoreAPIProxy::translateDX11BlendFactor( EBlendFactor pBlendFactor )
	{
		static const D3D11_BLEND blendFactorArray[] =
		{
			D3D11_BLEND_ZERO,
			D3D11_BLEND_ONE,
			D3D11_BLEND_BLEND_FACTOR,
			D3D11_BLEND_INV_BLEND_FACTOR,
			D3D11_BLEND_SRC_COLOR,
			D3D11_BLEND_SRC_ALPHA,
			D3D11_BLEND_DEST_COLOR,
			D3D11_BLEND_DEST_ALPHA,
			D3D11_BLEND_INV_SRC_COLOR,
			D3D11_BLEND_INV_SRC_ALPHA,
			D3D11_BLEND_INV_DEST_COLOR,
			D3D11_BLEND_INV_DEST_ALPHA,
		};
		return ts3::staticArrayElement( blendFactorArray, pBlendFactor );
	}

	D3D11_BLEND_OP DX11CoreAPIProxy::translateDX11BlendOp( EBlendOp pBlendOp )
	{
		static const D3D11_BLEND_OP blendOpArray[] =
		{
			D3D11_BLEND_OP_ADD,
			D3D11_BLEND_OP_MIN,
			D3D11_BLEND_OP_MAX,
			D3D11_BLEND_OP_SUBTRACT,
			D3D11_BLEND_OP_REV_SUBTRACT
		};
		return ts3::staticArrayElement( blendOpArray, pBlendOp );
	}

	UINT8 DX11CoreAPIProxy::translateDX11BlendRenderTargetWriteMask( EBlendRenderTargetWriteMask pRenderTargetWriteMask )
	{
		if( pRenderTargetWriteMask == EBlendRenderTargetWriteMask::All )
		{
			return D3D11_COLOR_WRITE_ENABLE_ALL;
		}
		else
		{
			return 0;
		}
	}

	Bitmask<D3D11_CLEAR_FLAG> DX11CoreAPIProxy::translateDX11ClearDSFlags( Bitmask<ERenderTargetAttachmentFlags> pAttachmentMask )
	{
		Bitmask<D3D11_CLEAR_FLAG> d3d11ClearDSFlags = 0;
		if( pAttachmentMask.isSet( E_RENDER_TARGET_ATTACHMENT_FLAG_DEPTH_BIT ) )
		{
			d3d11ClearDSFlags.set( D3D11_CLEAR_DEPTH );
		}
		if( pAttachmentMask.isSet( E_RENDER_TARGET_ATTACHMENT_FLAG_STENCIL_BIT ) )
		{
			d3d11ClearDSFlags.set( D3D11_CLEAR_STENCIL );
		}
		return d3d11ClearDSFlags;
	}

	UINT DX11CoreAPIProxy::translateDX11BufferBindFlags( Bitmask<gpu_resource_flags_value_t> pBufferFlags )
	{
		Bitmask<UINT> d3d11BindFlags = 0;
		if( pBufferFlags.isSet( E_GPU_BUFFER_BIND_FLAG_CONSTANT_BUFFER_BIT ) )
		{
			d3d11BindFlags.set( D3D11_BIND_CONSTANT_BUFFER );
		}
		if( pBufferFlags.isSet( E_GPU_BUFFER_BIND_FLAG_VERTEX_BUFFER_BIT ) )
		{
			d3d11BindFlags.set( D3D11_BIND_VERTEX_BUFFER );
		}
		if( pBufferFlags.isSet( E_GPU_BUFFER_BIND_FLAG_INDEX_BUFFER_BIT ) )
		{
			d3d11BindFlags.set( D3D11_BIND_INDEX_BUFFER );
		}
		if( pBufferFlags.isSet( E_GPU_BUFFER_BIND_FLAG_SHADER_INPUT_BUFFER_BIT ) )
		{
			d3d11BindFlags.set( D3D11_BIND_SHADER_RESOURCE );
		}
		if( pBufferFlags.isSet( E_GPU_BUFFER_BIND_FLAG_SHADER_UAV_BUFFER_BIT ) )
		{
			d3d11BindFlags.set( D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS );
		}
		if( pBufferFlags.isSet( E_GPU_BUFFER_BIND_FLAG_STREAM_OUTPUT_BUFFER_BIT ) )
		{
			d3d11BindFlags.set( D3D11_BIND_STREAM_OUTPUT );
		}
		return d3d11BindFlags;
	}

	D3D11_MAP DX11CoreAPIProxy::translateDX11BufferMapFlags( EMemoryMapMode pMapMode, Bitmask<EMemoryFlags> /* pMemoryFlags */ )
	{
		static const std::unordered_map<EMemoryMapMode, D3D11_MAP> mapModeMap =
		{
			{ EMemoryMapMode::ReadOnly        , D3D11_MAP_READ               },
			{ EMemoryMapMode::ReadWrite       , D3D11_MAP_READ_WRITE         },
			{ EMemoryMapMode::WriteDefault    , D3D11_MAP_WRITE              },
			{ EMemoryMapMode::WriteInvalidate , D3D11_MAP_WRITE_DISCARD      },
			{ EMemoryMapMode::WriteAppend     , D3D11_MAP_WRITE_NO_OVERWRITE },
		};
		return ts3::getMapValueOrDefault( mapModeMap, pMapMode, static_cast<D3D11_MAP>( 0 ) );
	}

	D3D11_COMPARISON_FUNC DX11CoreAPIProxy::translateDX11CompFunc( ECompFunc pCompFunc )
	{
		static const D3D11_COMPARISON_FUNC compFuncArray[] =
		{
			D3D11_COMPARISON_NEVER,
			D3D11_COMPARISON_ALWAYS,
			D3D11_COMPARISON_EQUAL,
			D3D11_COMPARISON_NOT_EQUAL,
			D3D11_COMPARISON_GREATER,
			D3D11_COMPARISON_GREATER_EQUAL,
			D3D11_COMPARISON_LESS,
			D3D11_COMPARISON_LESS_EQUAL,
		};
		return ts3::staticArrayElement( compFuncArray, pCompFunc );
	}

	D3D11_CULL_MODE DX11CoreAPIProxy::translateDX11CullMode( ECullMode pCullMode )
	{
		static const D3D11_CULL_MODE cullModeArray[] =
		{
			D3D11_CULL_NONE,
			D3D11_CULL_BACK,
			D3D11_CULL_FRONT,
		};
		return ts3::staticArrayElement( cullModeArray, pCullMode );
	}

	D3D11_DEPTH_WRITE_MASK DX11CoreAPIProxy::translateDX11DepthWriteMask( EDepthWriteMask pDepthWriteMask )
	{
		if( pDepthWriteMask == EDepthWriteMask::All )
		{
			return D3D11_DEPTH_WRITE_MASK_ALL;
		}
		else
		{
			return D3D11_DEPTH_WRITE_MASK_ZERO;
		}
	}

	D3D11_FILL_MODE DX11CoreAPIProxy::translateDX11PrimitiveFillMode( EPrimitiveFillMode pFillMode )
	{
		static const D3D11_FILL_MODE fillModeArray[] =
		{
			D3D11_FILL_SOLID,
			D3D11_FILL_WIREFRAME,
		};
		return ts3::staticArrayElement( fillModeArray, pFillMode );
	}

	D3D11_PRIMITIVE_TOPOLOGY DX11CoreAPIProxy::translateDX11PrimitiveTopology( EPrimitiveTopology pTopology )
	{
		static const D3D11_PRIMITIVE_TOPOLOGY primitiveTopologyArray[] =
		{
			D3D11_PRIMITIVE_TOPOLOGY_POINTLIST,
			D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
			D3D11_PRIMITIVE_TOPOLOGY_LINELIST_ADJ,
			D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP,
			D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ,
			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ,
			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ,
			D3D11_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST
		};
		return ts3::staticArrayElement( primitiveTopologyArray, pTopology );
	}

	D3D11_SHADER_TYPE DX11CoreAPIProxy::translateDX11EShaderType( EShaderType pShaderType )
	{
		static const D3D11_SHADER_TYPE shaderTypeArray[] =
		{
			D3D11_VERTEX_SHADER,
			D3D11_HULL_SHADER,
			D3D11_DOMAIN_SHADER,
			D3D11_GEOMETRY_SHADER,
			D3D11_PIXEL_SHADER,
			D3D11_COMPUTE_SHADER,
		};
		return ts3::staticArrayElement( shaderTypeArray, pShaderType );
	}

	D3D11_STENCIL_OP DX11CoreAPIProxy::translateDX11StencilOp( EStencilOp pStencilOp )
	{
		static const D3D11_STENCIL_OP stencilOpArray[] =
		{
			D3D11_STENCIL_OP_ZERO,
			D3D11_STENCIL_OP_KEEP,
			D3D11_STENCIL_OP_REPLACE,
			D3D11_STENCIL_OP_INCR,
			D3D11_STENCIL_OP_INCR_SAT,
			D3D11_STENCIL_OP_DECR,
			D3D11_STENCIL_OP_DECR_SAT,
			D3D11_STENCIL_OP_INVERT,
		};
		return ts3::staticArrayElement( stencilOpArray, pStencilOp );
	}

	D3D11_TEXTURE_ADDRESS_MODE DX11CoreAPIProxy::translateDX11ETextureAddressMode( ETextureAddressMode pAddressMode )
	{
		static const D3D11_TEXTURE_ADDRESS_MODE textureAddressModeArray[] =
		{
			D3D11_TEXTURE_ADDRESS_BORDER,
			D3D11_TEXTURE_ADDRESS_CLAMP,
			D3D11_TEXTURE_ADDRESS_MIRROR,
			D3D11_TEXTURE_ADDRESS_MIRROR_ONCE,
			D3D11_TEXTURE_ADDRESS_WRAP,
		};
		return ts3::staticArrayElement( textureAddressModeArray, pAddressMode );
	}

	UINT DX11CoreAPIProxy::translateDX11ETextureBindFlags( Bitmask<gpu_resource_flags_value_t> pTextureFlags )
	{
		Bitmask<UINT> d3d11BindFlags = 0;
		if( pTextureFlags.isSet( E_GPU_RESOURCE_USAGE_FLAG_SHADER_INPUT_BIT ) )
		{
			d3d11BindFlags.set( D3D11_BIND_SHADER_RESOURCE );
		}
		if( pTextureFlags.isSet( E_GPU_RESOURCE_USAGE_FLAG_RENDER_TARGET_COLOR_BIT ) )
		{
			d3d11BindFlags.set( D3D11_BIND_RENDER_TARGET );
		}
		if( pTextureFlags.isSet( E_GPU_RESOURCE_USAGE_FLAG_RENDER_TARGET_DEPTH_STENCIL_BIT ) )
		{
			d3d11BindFlags.set( D3D11_BIND_DEPTH_STENCIL );
		}
		return d3d11BindFlags;
	}

	D3D11_FILTER DX11CoreAPIProxy::translateDX11ETextureFilter( ETextureFilter magFilter, ETextureFilter minFilter, ETextureMipMode mipMode, uint32 anisotropyLevel )
	{
		return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	}

}
