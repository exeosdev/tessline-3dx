
#pragma once

#ifndef __TS3DRIVER_GPUAPI_DX_CORE_LAYER_H__
#define __TS3DRIVER_GPUAPI_DX_CORE_LAYER_H__

#include "DX_prerequisites.h"

namespace ts3::gpuapi
{

	struct TextureDimensions;

	using DXGIGetDebugInterfaceType = HRESULT ( WINAPI * )( const GUID &, void ** );

	namespace DXCoreAPIProxy
	{

		DXGIGetDebugInterfaceType loadDXGIDebugLegacyLoader();

		ComPtr<IDXGIDebug> queryDXGIDebugInterface( Bitmask<EGPUDriverConfigFlags> pDriverConfigFlags );

		ComPtr<IDXGIInfoQueue> queryDXGIDebugInfoQueue( Bitmask<EGPUDriverConfigFlags> pDriverConfigFlags );

		uint32 getDXGITextureFormatBPP( DXGI_FORMAT pDXGIFormat );

		const char * getDXShaderTargetStr( DXShaderTarget pShaderTarget );

		uint32 computeDXTextureMemoryByteSize( const TextureDimensions & pTextDimensions, DXGI_FORMAT pFormat );

		Bitmask<UINT> translateDXShaderCompileFlags( Bitmask<uint32> pShaderCreateFlags, bool pDebugDevice );

		DXGI_FORMAT translateDXBaseDataType( EBaseDataType pBaseDataType );

		DXGI_FORMAT translateDXTextureFormat( ETextureFormat pTextureFormat );

		DXGI_FORMAT translateDXVertexAttribFormat( EVertexAttribFormat pVertexAttribFormat );

	};

} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_DX_CORE_LAYER_H__
