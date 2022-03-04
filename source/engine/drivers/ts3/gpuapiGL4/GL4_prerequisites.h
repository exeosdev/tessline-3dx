
#pragma once

#ifndef __TS3DRIVER_GPUAPI_GL4_PREREQUISITES_H__
#define __TS3DRIVER_GPUAPI_GL4_PREREQUISITES_H__

#include <ts3/gpuapiGL/GL_prerequisites.h>

#if( TS3_BUILD_STATIC )
#  define TS3GX_GL4_API
#  define TS3GX_GL4_CLASS
#  define TS3GX_GL4_OBJ    extern
#else
#  if( TS3_DRIVER_GPUAPI_GL4_BUILD )
#    define TS3GX_GL4_API    TS3_PCL_ATTR_DLL_EXPORT
#    define TS3GX_GL4_CLASS  TS3_PCL_ATTR_DLL_EXPORT
#    define TS3GX_GL4_OBJ    TS3_PCL_ATTR_DLL_EXPORT
#  else
#    define TS3GX_GL4_API    TS3_PCL_ATTR_DLL_IMPORT
#    define TS3GX_GL4_CLASS  TS3_PCL_ATTR_DLL_IMPORT
#    define TS3GX_GL4_OBJ    TS3_PCL_ATTR_DLL_IMPORT
#  endif
#endif

#define TS3GX_GL4_USE_V1CORE_API 1

namespace ts3
{
namespace gpuapi
{

	ts3DeclareClassHandle( GL4GPUDevice );
	ts3DeclareClassHandle( GL4GPUDriver );

} /* namespace ts3 */
} /* namespace gpuapi */

#endif // __TS3DRIVER_GPUAPI_GL4_PREREQUISITES_H__
