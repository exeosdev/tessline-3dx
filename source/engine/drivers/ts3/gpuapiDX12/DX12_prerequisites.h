
#pragma once

#ifndef __TS3DRIVER_GPUAPI_DX12_PREREQUISITES_H__
#define __TS3DRIVER_GPUAPI_DX12_PREREQUISITES_H__

#include <ts3/gpuapiDX/DX_prerequisites.h>

#include <dxgi1_3.h>
#include <dxgi1_4.h>

#include <d3d12.h>
#include <d3d12sdklayers.h>
#include <d3d12shader.h>

#if( TS3_BUILD_STATIC )
#  define TS3GX_DX12_API
#  define TS3GX_DX12_CLASS
#  define TS3GX_DX12_OBJ    extern
#else
#  if( TS3_DRIVER_GPUAPI_DX12_BUILD )
#    define TS3GX_DX12_API    TS3_PCL_ATTR_DLL_EXPORT
#    define TS3GX_DX12_CLASS  TS3_PCL_ATTR_DLL_EXPORT
#    define TS3GX_DX12_OBJ    TS3_PCL_ATTR_DLL_EXPORT
#  else
#    define TS3GX_DX12_API    TS3_PCL_ATTR_DLL_IMPORT
#    define TS3GX_DX12_CLASS  TS3_PCL_ATTR_DLL_IMPORT
#    define TS3GX_DX12_OBJ    TS3_PCL_ATTR_DLL_IMPORT
#  endif
#endif

#include "prerequisites/DX12_coreDefs.h"

namespace ts3::gpuapi
{

	ts3DeclareClassHandle( DX12CommandContext );
	ts3DeclareClassHandle( DX12GPUCmdManager );
	ts3DeclareClassHandle( DX12GPUDevice );
	ts3DeclareClassHandle( DX12GPUDriver );
	ts3DeclareClassHandle( DX12PresentationLayer );

}

#endif // __TS3DRIVER_GPUAPI_DX12_PREREQUISITES_H__
