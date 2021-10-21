
#pragma once

#ifndef __TS3DRIVER_GPUAPI_DX11_CORE_LAYER_H__
#define __TS3DRIVER_GPUAPI_DX11_CORE_LAYER_H__

#include "DX11_prerequisites.h"
#include <ts3/gpuapiDX/DX_coreAPIProxy.h>

namespace ts3::gpuapi
{

	using resource_flags_value_t = uint32 ;

	enum GPUDriverConfigFlags : uint32;
	enum ERenderTargetAttachmentFlags : uint32;

	enum class EBlendFactor : enum_default_value_t;
	enum class EBlendOp : enum_default_value_t;
	enum class EBlendRenderTargetWriteMask : enum_default_value_t;
	enum class ECompFunc : enum_default_value_t;
	enum class ECullMode : enum_default_value_t;
	enum class EDepthWriteMask : enum_default_value_t;
	enum class GPUBufferType : enum_default_value_t;
	enum class EPrimitiveFillMode : enum_default_value_t;
	enum class EPrimitiveTopology : enum_default_value_t;
	enum class EShaderType : uint32;
	enum class EStencilOp : enum_default_value_t;
	enum class ETextureAddressMode : enum_default_value_t;
	enum class ETextureCubeMapFace : size_t;
	enum class ETextureDimensionClass : enum_default_value_t;
	enum class ETextureFilter : enum_default_value_t;
	enum class ETextureFormat : gpu_pixel_format_value_t;
	enum class ETextureMipMode : enum_default_value_t;
	enum class ETriangleVerticesOrder : enum_default_value_t;

	namespace DX11CoreAPIProxy
	{

		using namespace DXCoreAPIProxy;

		ComPtr<ID3D11Device1> createD3D11Device( D3D_DRIVER_TYPE pDeviceType, Bitmask<UINT> pCreateFlags );

		ComPtr<ID3D11Debug> queryD3D11DebugInterfaceForD3D11Device( const ComPtr<ID3D11Device1> & pD3D11Device1 );

		ComPtr<IDXGIFactory2> queryDXGIFactoryForD3D11Device( const ComPtr<ID3D11Device1> & pD3D11Device1 );

		ComPtr<IDXGISwapChain1> createD3D11SwapChainForSystemWindow( DX11GPUDevice & pDX11GPUDevice,
		                                                          void * pSysWindow /* It must be a ts3::system::Window */ );

		UINT translateDX11GPUDeviceCreateFlags( Bitmask<GPUDriverConfigFlags> pDriverConfigFlags );

		D3D11_BLEND translateDX11BlendFactor( EBlendFactor pBlendFactor );

		D3D11_BLEND_OP translateDX11BlendOp( EBlendOp pBlendOp );

		D3D11_BLEND_OP translateDX11BlendOp( EBlendOp pBlendOp );

		UINT8 translateDX11BlendRenderTargetWriteMask( EBlendRenderTargetWriteMask pRenderTargetWriteMask );

		Bitmask<D3D11_CLEAR_FLAG> translateDX11ClearDSFlags( Bitmask<ERenderTargetAttachmentFlags> pAttachmentMask );

		D3D11_COMPARISON_FUNC translateDX11CompFunc( ECompFunc pCompFunc );

		D3D11_CULL_MODE translateDX11CullMode( ECullMode pCullMode );

		D3D11_DEPTH_WRITE_MASK translateDX11DepthWriteMask( EDepthWriteMask pDepthWriteMask );

		UINT translateDX11BufferBindFlags( Bitmask<resource_flags_value_t> pBufferFlags );

		D3D11_MAP translateDX11BufferMapFlags( EGPUMemoryMapMode pMapMode, Bitmask<EGPUMemoryFlags> pMemoryFlags );

		D3D11_FILL_MODE translateDX11PrimitiveFillMode( EPrimitiveFillMode pFillMode );

		D3D11_PRIMITIVE_TOPOLOGY translateDX11PrimitiveTopology( EPrimitiveTopology pTopology );

		D3D11_SHADER_TYPE translateDX11EShaderType( EShaderType pShaderType );

		D3D11_STENCIL_OP translateDX11StencilOp( EStencilOp pStencilOp );

		D3D11_TEXTURE_ADDRESS_MODE translateDX11ETextureAddressMode( ETextureAddressMode pAddressMode );

		UINT translateDX11ETextureBindFlags( Bitmask<resource_flags_value_t> pTextureFlags );

		D3D11_FILTER translateDX11ETextureFilter( ETextureFilter magFilter, ETextureFilter minFilter, ETextureMipMode mipMode, uint32 anisotropyLevel );

	};

}

#endif // __TS3DRIVER_GPUAPI_DX11_CORE_LAYER_H__
