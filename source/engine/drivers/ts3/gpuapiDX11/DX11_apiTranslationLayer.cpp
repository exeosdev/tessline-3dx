
#include "DX11_apiTranslationLayer.h"
#include <ts3/gpuapiDX11/DX11_gpuDevice.h>
#include <ts3/gpuapi/resources/gpuBufferCommon.h>
#include <ts3/gpuapi/state/samplerCommon.h>
#include <ts3/gpuapi/resources/shaderCommon.h>
#include <ts3/gpuapi/resources/textureCommon.h>
#include <ts3/system/windowNative.h>
#include <ts3/stdext/stlHelperAlgo.h>

namespace ts3::gpuapi
{

	template <typename TEnum>
	inline const TEnum D3D11_INVALID = static_cast< TEnum >( 0xFFFFFFFF );
	
	template <typename TEnum>
	inline const TEnum D3D11_UNDEFINED = static_cast< TEnum >( 0 );

	ComPtr<ID3D11Device1> atl::createD3D11Device( D3D_DRIVER_TYPE pDeviceType, Bitmask<UINT> pCreateFlags )
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

	ComPtr<ID3D11Debug> atl::queryD3D11DebugInterfaceForD3D11Device( const ComPtr<ID3D11Device1> & pD3D11Device1 )
	{
		ComPtr<ID3D11Debug> d3d11DebugInterface;
		auto hResult = pD3D11Device1->QueryInterface( IID_PPV_ARGS( &d3d11DebugInterface ) );
		if( FAILED( hResult ) )
		{
			return nullptr;
		}

		return d3d11DebugInterface;
	}

	ComPtr<IDXGIFactory2> atl::queryDXGIFactoryForD3D11Device( const ComPtr<ID3D11Device1> & pD3D11Device1 )
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

	ComPtr<IDXGISwapChain1> atl::createD3D11SwapChainForSystemWindow( DX11GPUDevice & pDX11GPUDevice, void * pSysWindow )
	{
	    auto * sysWindowPtr = static_cast<system::Window *>( pSysWindow )->queryInterface<system::Win32Window>();
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

	UINT atl::translateDX11GPUDeviceCreateFlags( Bitmask<EGPUDriverConfigFlags> pDriverConfigFlags )
	{
		Bitmask<UINT> deviceCreateFlags = 0;
		if( pDriverConfigFlags.isSet( E_GPU_DRIVER_CONFIG_FLAG_ENABLE_DEBUG_LAYER_BIT ) )
		{
			deviceCreateFlags.set( D3D11_CREATE_DEVICE_DEBUG );
		}
		if( pDriverConfigFlags.isSet( E_GPU_DRIVER_CONFIG_FLAG_ENABLE_SHADER_DEBUG_INFO_BIT ) )
		{
			deviceCreateFlags.set( D3D11_CREATE_DEVICE_DEBUGGABLE );
		}
		if( pDriverConfigFlags.isSet( E_GPU_DRIVER_CONFIG_FLAG_DISABLE_MULTI_THREAD_ACCESS_BIT ) )
		{
			deviceCreateFlags.set( D3D11_CREATE_DEVICE_SINGLETHREADED );
		}
		return deviceCreateFlags;
	}

	D3D11_BLEND atl::translateDX11BlendFactor( EBlendFactor pBlendFactor )
	{
		switch( pBlendFactor )
		{
			ts3CaseReturn( EBlendFactor::Undefined   , D3D11_UNDEFINED<D3D11_BLEND> );
			ts3CaseReturn( EBlendFactor::Zero        , D3D11_BLEND_ZERO );
			ts3CaseReturn( EBlendFactor::One         , D3D11_BLEND_ONE );
			ts3CaseReturn( EBlendFactor::Const       , D3D11_BLEND_BLEND_FACTOR );
			ts3CaseReturn( EBlendFactor::ConstInv    , D3D11_BLEND_INV_BLEND_FACTOR );
			ts3CaseReturn( EBlendFactor::SrcColor    , D3D11_BLEND_SRC_COLOR );
			ts3CaseReturn( EBlendFactor::SrcAlpha    , D3D11_BLEND_SRC_ALPHA );
			ts3CaseReturn( EBlendFactor::DstColor    , D3D11_BLEND_DEST_COLOR );
			ts3CaseReturn( EBlendFactor::DstAlpha    , D3D11_BLEND_DEST_ALPHA );
			ts3CaseReturn( EBlendFactor::SrcColorInv , D3D11_BLEND_INV_SRC_COLOR );
			ts3CaseReturn( EBlendFactor::SrcAlphaInv , D3D11_BLEND_INV_SRC_ALPHA );
			ts3CaseReturn( EBlendFactor::DstColorInv , D3D11_BLEND_INV_DEST_COLOR );
			ts3CaseReturn( EBlendFactor::DstAlphaInv , D3D11_BLEND_INV_DEST_ALPHA );
		};
		return D3D11_INVALID<D3D11_BLEND>;
	}

	D3D11_BLEND_OP atl::translateDX11BlendOp( EBlendOp pBlendOp )
	{
		switch( pBlendOp )
		{
			ts3CaseReturn( EBlendOp::Undefined   , D3D11_UNDEFINED<D3D11_BLEND_OP> );
			ts3CaseReturn( EBlendOp::Add         , D3D11_BLEND_OP_ADD );
			ts3CaseReturn( EBlendOp::Min         , D3D11_BLEND_OP_MIN );
			ts3CaseReturn( EBlendOp::Max         , D3D11_BLEND_OP_MAX );
			ts3CaseReturn( EBlendOp::Subtract    , D3D11_BLEND_OP_SUBTRACT );
			ts3CaseReturn( EBlendOp::SubtractRev , D3D11_BLEND_OP_REV_SUBTRACT );
		};
		return D3D11_INVALID<D3D11_BLEND_OP>;
	}

	UINT8 atl::translateDX11BlendRenderTargetWriteMask( Bitmask<EBlendWriteMaskFlags> pWriteMask )
	{
		auto d3d11WriteMask = makeBitmask<UINT8>( 0 );

		if( pWriteMask.isSet( E_BLEND_WRITE_MASK_CHANNEL_RED ) )
		{
			d3d11WriteMask.set( D3D11_COLOR_WRITE_ENABLE_RED );
		}
		if( pWriteMask.isSet( E_BLEND_WRITE_MASK_CHANNEL_GREEN ) )
		{
			d3d11WriteMask.set( D3D11_COLOR_WRITE_ENABLE_GREEN );
		}
		if( pWriteMask.isSet( E_BLEND_WRITE_MASK_CHANNEL_BLUE ) )
		{
			d3d11WriteMask.set( D3D11_COLOR_WRITE_ENABLE_BLUE );
		}
		if( pWriteMask.isSet( E_BLEND_WRITE_MASK_CHANNEL_ALPHA ) )
		{
			d3d11WriteMask.set( D3D11_COLOR_WRITE_ENABLE_ALPHA );
		}

		return d3d11WriteMask;
	}

	Bitmask<D3D11_CLEAR_FLAG> atl::translateDX11RTClearDepthStencilFlags( Bitmask<ERenderTargetBufferFlags> pClearFlags )
	{
		Bitmask<D3D11_CLEAR_FLAG> d3d11ClearDSFlags = 0;
		if( pClearFlags.isSet( E_RENDER_TARGET_BUFFER_FLAG_DEPTH_BIT ) )
		{
			d3d11ClearDSFlags.set( D3D11_CLEAR_DEPTH );
		}
		if( pClearFlags.isSet( E_RENDER_TARGET_BUFFER_FLAG_STENCIL_BIT ) )
		{
			d3d11ClearDSFlags.set( D3D11_CLEAR_STENCIL );
		}
		return d3d11ClearDSFlags;
	}

	UINT atl::translateDX11BufferBindFlags( Bitmask<resource_flags_value_t> pBufferFlags )
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

	D3D11_MAP atl::translateDX11BufferMapFlags( EGPUMemoryMapMode pMapMode, Bitmask<EGPUMemoryFlags> /* pMemoryFlags */ )
	{
		switch( pMapMode )
		{
			ts3CaseReturn( EGPUMemoryMapMode::ReadOnly        , D3D11_MAP_READ               );
			ts3CaseReturn( EGPUMemoryMapMode::ReadWrite       , D3D11_MAP_READ_WRITE         );
			ts3CaseReturn( EGPUMemoryMapMode::WriteDefault    , D3D11_MAP_WRITE              );
			ts3CaseReturn( EGPUMemoryMapMode::WriteInvalidate , D3D11_MAP_WRITE_DISCARD      );
			ts3CaseReturn( EGPUMemoryMapMode::WriteAppend     , D3D11_MAP_WRITE_NO_OVERWRITE );
		};
		return static_cast<D3D11_MAP>( 0 );
	}

	D3D11_COMPARISON_FUNC atl::translateDX11CompFunc( ECompFunc pCompFunc )
	{
		switch( pCompFunc )
		{
			ts3CaseReturn( ECompFunc::Undefined    , D3D11_UNDEFINED<D3D11_COMPARISON_FUNC> );
			ts3CaseReturn( ECompFunc::Never        , D3D11_COMPARISON_NEVER );
			ts3CaseReturn( ECompFunc::Always       , D3D11_COMPARISON_ALWAYS );
			ts3CaseReturn( ECompFunc::Equal        , D3D11_COMPARISON_EQUAL );
			ts3CaseReturn( ECompFunc::NotEqual     , D3D11_COMPARISON_NOT_EQUAL );
			ts3CaseReturn( ECompFunc::Greater      , D3D11_COMPARISON_GREATER );
			ts3CaseReturn( ECompFunc::GreaterEqual , D3D11_COMPARISON_GREATER_EQUAL );
			ts3CaseReturn( ECompFunc::Less         , D3D11_COMPARISON_LESS );
			ts3CaseReturn( ECompFunc::LessEqual    , D3D11_COMPARISON_LESS_EQUAL );
		};
		return D3D11_INVALID<D3D11_COMPARISON_FUNC>;
	}

	D3D11_CULL_MODE atl::translateDX11CullMode( ECullMode pCullMode )
	{
		switch( pCullMode )
		{
			ts3CaseReturn( ECullMode::Undefined  , D3D11_UNDEFINED<D3D11_CULL_MODE> );
			ts3CaseReturn( ECullMode::None       , D3D11_CULL_NONE );
			ts3CaseReturn( ECullMode::Back       , D3D11_CULL_BACK );
			ts3CaseReturn( ECullMode::Front      , D3D11_CULL_FRONT );
		};
		return D3D11_INVALID<D3D11_CULL_MODE>;
	}

	D3D11_DEPTH_WRITE_MASK atl::translateDX11DepthWriteMask( EDepthWriteMask pDepthWriteMask )
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

	D3D11_FILL_MODE atl::translateDX11PrimitiveFillMode( EPrimitiveFillMode pFillMode )
	{
		switch( pFillMode )
		{
			ts3CaseReturn( EPrimitiveFillMode::Undefined , D3D11_UNDEFINED<D3D11_FILL_MODE> );
			ts3CaseReturn( EPrimitiveFillMode::Solid     , D3D11_FILL_SOLID );
			ts3CaseReturn( EPrimitiveFillMode::Wireframe , D3D11_FILL_WIREFRAME );
		};
		return D3D11_INVALID<D3D11_FILL_MODE>;
	}

	D3D11_PRIMITIVE_TOPOLOGY atl::translateDX11PrimitiveTopology( EPrimitiveTopology pTopology )
	{
		switch( pTopology )
		{
			ts3CaseReturn( EPrimitiveTopology::Undefined        , D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED );
			ts3CaseReturn( EPrimitiveTopology::PointList        , D3D11_PRIMITIVE_TOPOLOGY_POINTLIST );
			ts3CaseReturn( EPrimitiveTopology::LineList         , D3D11_PRIMITIVE_TOPOLOGY_LINELIST );
			ts3CaseReturn( EPrimitiveTopology::LineListAdj      , D3D11_PRIMITIVE_TOPOLOGY_LINELIST_ADJ );
			ts3CaseReturn( EPrimitiveTopology::LineStrip        , D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP );
			ts3CaseReturn( EPrimitiveTopology::LineStripAdj     , D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ );
			ts3CaseReturn( EPrimitiveTopology::TriangleList     , D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
			ts3CaseReturn( EPrimitiveTopology::TriangleListAdj  , D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ );
			ts3CaseReturn( EPrimitiveTopology::TriangleStrip    , D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
			ts3CaseReturn( EPrimitiveTopology::TriangleStripAdj , D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ );
			ts3CaseReturn( EPrimitiveTopology::TesselationPatch , D3D11_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST );
		};
		return D3D11_INVALID<D3D11_PRIMITIVE_TOPOLOGY>;
	}

	D3D11_SHADER_TYPE atl::translateDX11EShaderType( EShaderType pShaderType )
	{
		switch( pShaderType )
		{
			ts3CaseReturn( EShaderType::Unknown    , D3D11_UNDEFINED<D3D11_SHADER_TYPE> );
			ts3CaseReturn( EShaderType::GSVertex   , D3D11_VERTEX_SHADER );
			ts3CaseReturn( EShaderType::GSHull     , D3D11_HULL_SHADER );
			ts3CaseReturn( EShaderType::GSDomain   , D3D11_DOMAIN_SHADER );
			ts3CaseReturn( EShaderType::GSGeometry , D3D11_GEOMETRY_SHADER );
			ts3CaseReturn( EShaderType::GSPixel    , D3D11_PIXEL_SHADER );
			ts3CaseReturn( EShaderType::CSCompute  , D3D11_COMPUTE_SHADER );
		}
		return D3D11_INVALID<D3D11_SHADER_TYPE>;
	}

	D3D11_STENCIL_OP atl::translateDX11StencilOp( EStencilOp pStencilOp )
	{
		switch( pStencilOp )
		{
			ts3CaseReturn( EStencilOp::Undefined , D3D11_UNDEFINED<D3D11_STENCIL_OP> );
			ts3CaseReturn( EStencilOp::Zero      , D3D11_STENCIL_OP_ZERO );
			ts3CaseReturn( EStencilOp::Keep      , D3D11_STENCIL_OP_KEEP );
			ts3CaseReturn( EStencilOp::Replace   , D3D11_STENCIL_OP_REPLACE );
			ts3CaseReturn( EStencilOp::IncrClamp , D3D11_STENCIL_OP_INCR );
			ts3CaseReturn( EStencilOp::IncrWrap  , D3D11_STENCIL_OP_INCR_SAT );
			ts3CaseReturn( EStencilOp::DecrClamp , D3D11_STENCIL_OP_DECR );
			ts3CaseReturn( EStencilOp::DecrWrap  , D3D11_STENCIL_OP_DECR_SAT );
			ts3CaseReturn( EStencilOp::Invert    , D3D11_STENCIL_OP_INVERT );
		};
		return D3D11_INVALID<D3D11_STENCIL_OP>;
	}

	D3D11_TEXTURE_ADDRESS_MODE atl::translateDX11ETextureAddressMode( ETextureAddressMode pAddressMode )
	{
		switch( pAddressMode )
		{
			ts3CaseReturn( ETextureAddressMode::Undefined         , D3D11_UNDEFINED<D3D11_TEXTURE_ADDRESS_MODE> );
			ts3CaseReturn( ETextureAddressMode::ClampToEdge       , D3D11_TEXTURE_ADDRESS_CLAMP );
			ts3CaseReturn( ETextureAddressMode::MirrorRepeat      , D3D11_TEXTURE_ADDRESS_MIRROR );
			ts3CaseReturn( ETextureAddressMode::Repeat            , D3D11_TEXTURE_ADDRESS_WRAP );
			ts3CaseReturn( ETextureAddressMode::ClampToColor      , D3D11_TEXTURE_ADDRESS_BORDER );
			ts3CaseReturn( ETextureAddressMode::MirrorClampToEdge , D3D11_TEXTURE_ADDRESS_MIRROR_ONCE );
		};
		return D3D11_UNDEFINED<D3D11_TEXTURE_ADDRESS_MODE>;
	}

	UINT atl::translateDX11ETextureBindFlags( Bitmask<resource_flags_value_t> pTextureFlags )
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
		if( pTextureFlags.isSetAnyOf( E_GPU_RESOURCE_USAGE_MASK_RENDER_TARGET_DEPTH_STENCIL ) )
		{
			d3d11BindFlags.set( D3D11_BIND_DEPTH_STENCIL );
		}
		return d3d11BindFlags;
	}

	D3D11_FILTER atl::translateDX11ETextureFilter( ETextureFilter magFilter, ETextureFilter minFilter, ETextureMipMode mipMode, uint32 anisotropyLevel )
	{
		return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	}

} // namespace ts3::gpuapi
