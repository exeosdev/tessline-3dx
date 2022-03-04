
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

namespace ts3
{
namespace gpuapi
{

	ts3DeclareClassHandle( DX11CommandList );
	ts3DeclareClassHandle( DX11CommandSystem );
	ts3DeclareClassHandle( DX11GPUDevice );
	ts3DeclareClassHandle( DX11GPUDriver );
	ts3DeclareClassHandle( DX11PresentationLayer );

} /* namespace ts3 */
} /* namespace gpuapi */

#endif // __TS3DRIVER_GPUAPI_DX11_PREREQUISITES_H__
