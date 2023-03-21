
#pragma once

#ifndef __TS3DRIVER_GPUAPI_DX11_PREREQUISITES_H__
#define __TS3DRIVER_GPUAPI_DX11_PREREQUISITES_H__

#include <ts3/gpuapiDX/DX_prerequisites.h>

#include <d3d11.h>
#include <d3d11_1.h>
#include <d3d11_2.h>
#include <d3d11sdklayers.h>
#include <d3d11shader.h>
#include <d3d11shadertracing.h>

#if( TS3_BUILD_STATIC )
#  define TS3GX_DX11_API
#  define TS3GX_DX11_CLASS
#  define TS3GX_DX11_OBJ    extern
#else
#  if( TS3_DRIVER_GPUAPI_DX11_BUILD )
#    define TS3GX_DX11_API    TS3_PCL_ATTR_DLL_EXPORT
#    define TS3GX_DX11_CLASS  TS3_PCL_ATTR_DLL_EXPORT
#    define TS3GX_DX11_OBJ    TS3_PCL_ATTR_DLL_EXPORT
#  else
#    define TS3GX_DX11_API    TS3_PCL_ATTR_DLL_IMPORT
#    define TS3GX_DX11_CLASS  TS3_PCL_ATTR_DLL_IMPORT
#    define TS3GX_DX11_OBJ    TS3_PCL_ATTR_DLL_IMPORT
#  endif
#endif

namespace ts3::gpuapi
{

	class GraphicsShaderLinkageImmutableStateSeparable;
	class RenderPassConfigurationImmutableStateDefault;

	class DX11BlendImmutableState;
	class DX11DepthStencilImmutableState;
	class DX11RasterizerImmutableState;
	class DX11IAInputLayoutImmutableState;
	class DX11IAVertexStreamImmutableState;
	class DX11RenderTargetBindingImmutableState;

	ts3GpaDeclareClassHandle( DX11CommandList );
	ts3GpaDeclareClassHandle( DX11CommandSystem );
	ts3GpaDeclareClassHandle( DX11GPUDevice );
	ts3GpaDeclareClassHandle( DX11GPUDriver );
	ts3GpaDeclareClassHandle( DX11PresentationLayer );

	ts3GpaDeclareTypedefHandle( DX11GraphicsShaderLinkageImmutableState, GraphicsShaderLinkageImmutableStateSeparable );
	ts3GpaDeclareTypedefHandle( DX11RenderPassConfigurationImmutableState, RenderPassConfigurationImmutableStateDefault );

} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_DX11_PREREQUISITES_H__
