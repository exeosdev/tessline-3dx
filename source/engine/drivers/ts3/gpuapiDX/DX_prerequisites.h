
#pragma once

#ifndef __TS3DRIVER_GPUAPI_DXCOMMON_PREREQUISITES_H__
#define __TS3DRIVER_GPUAPI_DXCOMMON_PREREQUISITES_H__

#include <ts3/gpuapi/memory/commonGPUMemoryDefs.h>

#include <dxgi.h>
#include <dxgi1_2.h>
#include <dxgidebug.h>

#include <d3dcompiler.h>

#if( TS3_DRIVER_GPUAPI_DX12_BUILD )
#  include <dxgi1_3.h>
#  include <dxgi1_4.h>
#endif

#if( TS3_BUILD_STATIC )
#  define TS3GX_DXCOMMON_API
#  define TS3GX_DXCOMMON_CLASS
#  define TS3GX_DXCOMMON_OBJ    extern
#else
#  if( TS3_DRIVER_GPUAPI_DXCOMMON_BUILD )
#    define TS3GX_DXCOMMON_API    TS3_PCL_ATTR_DLL_EXPORT
#    define TS3GX_DXCOMMON_CLASS  TS3_PCL_ATTR_DLL_EXPORT
#    define TS3GX_DXCOMMON_OBJ    TS3_PCL_ATTR_DLL_EXPORT
#  else
#    define TS3GX_DXCOMMON_API    TS3_PCL_ATTR_DLL_IMPORT
#    define TS3GX_DXCOMMON_CLASS  TS3_PCL_ATTR_DLL_IMPORT
#    define TS3GX_DXCOMMON_OBJ    TS3_PCL_ATTR_DLL_IMPORT
#  endif
#endif

#include "prerequisites/DX_commonShaderDefs.h"
#include "prerequisites/DX_errorUtils.h"

namespace ts3
{
namespace gpuapi
{

	ts3DeclareClassHandle( DXGPUDevice );
	ts3DeclareClassHandle( DXGPUDriver );
	ts3DeclareClassHandle( DXPresentationLayer );

} /* namespace ts3 */
} /* namespace gpuapi */

#endif // __TS3DRIVER_GPUAPI_DXCOMMON_PREREQUISITES_H__
