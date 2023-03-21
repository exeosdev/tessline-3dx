
#pragma once

#ifndef __TS3DRIVER_GPUAPI_DXCOMMON_API_TRANSLATION_LAYER_H__
#define __TS3DRIVER_GPUAPI_DXCOMMON_API_TRANSLATION_LAYER_H__

#include "DX_prerequisites.h"

namespace ts3::gpuapi
{

	struct TextureDimensions;

    using resource_flags_value_t = uint32;

    enum EGPUDriverConfigFlags : uint32;
    enum ERenderTargetAttachmentFlags : uint32;
    enum ETextureCubeMapFace : uint32;

	using DXGIGetDebugInterfaceType = HRESULT ( WINAPI * )( const GUID &, void ** );

	namespace atl
	{

		DXGIGetDebugInterfaceType loadDXGIDebugLegacyLoader();

		ComPtr<IDXGIDebug> queryDXGIDebugInterface( Bitmask<EGPUDriverConfigFlags> pDriverConfigFlags );

		ComPtr<IDXGIInfoQueue> queryDXGIDebugInfoQueue( Bitmask<EGPUDriverConfigFlags> pDriverConfigFlags );

		uint32 getDXGITextureFormatBPP( DXGI_FORMAT pDXGIFormat );

		const char * getDXShaderTargetStr( DXShaderTarget pShaderTarget );

		uint32 computeDXTextureMemoryByteSize( const TextureDimensions & pTextDimensions, DXGI_FORMAT pFormat );

		Bitmask<UINT> translateShaderCompileFlagsDX( Bitmask<uint32> pShaderCreateFlags, bool pDebugDevice );

		DXGI_FORMAT translateBaseDataTypeDX( EBaseDataType pBaseDataType );

		DXGI_FORMAT translateTextureFormatDX( ETextureFormat pTextureFormat );

		ETextureFormat translateTextureFormatInvDX( DXGI_FORMAT pDXGIFormat );

		DXGI_FORMAT translateVertexAttribFormatDX( EVertexAttribFormat pVertexAttribFormat );

	};

} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_DXCOMMON_API_TRANSLATION_LAYER_H__
